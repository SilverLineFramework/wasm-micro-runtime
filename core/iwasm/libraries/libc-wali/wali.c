/*
  MIT License

  Copyright (c) [2023] [Arjun Ramesh]

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <linux/prctl.h>

#include "wali.h"
#include "copy.h"
#include "../interpreter/wasm_runtime.h"
#include "thread_manager.h"

#include "syscall.h"
#include "impl.h"

/* For startup environment */
int wali_app_argc;
char **wali_app_argv;
char *wali_app_env_file;
bool invoked_wali;

/* For process exit functionality */
int64 proc_exit_primary_tid = -1;
bool proc_exit_invoked = false;

/* For exit code handling */
static bool is_multithreaded = false;

/* For thread cloning TID synchronization */
static pthread_mutex_t clone_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mmap_lock = PTHREAD_MUTEX_INITIALIZER;
static sem_t tid_sem;

volatile int signalled_tid = -1;

/* WALI State */
uint32_t NATIVE_PAGESIZE = 0;
int MMAP_PAGELEN = 0;
int WASM_PAGELEN = 0;
int WASM_TO_NATIVE_PAGE = 0;
uint32_t THREAD_ID = 0; // unused atm

/* strace */
int strace = -1;             // -1=no, 0=print to stdout, 1=print to file
FILE *strace_logfile = NULL; // test

/* Dummy callback to be invoked after termination flags are set
 * for all threads in process */
void
wali_terminate_process_sighandler(int signo)
{
    VERB("WALI termination handler called by process %d", getpid());
}
/** **/

/* Startup init */
// Global initialization for WALI -- called once per process
void wali_global_init() {
    if (sem_init(&tid_sem, 0, 0)) {
        perror("sem_init");
    }

    // Register signals for termination
    struct sigaction act = { 0 };
    act.sa_handler = wali_terminate_process_sighandler;
    if (sigaction(SIG_WASM_THREAD_TERM, &act, NULL) == -1) {
        perror("Could not install WALI termination signal\n");
        exit(1);
    }

}

// Instance-specific state initialization for WALI -- called once per module instance
void wali_instance_init(wasm_module_inst_t module_inst) {
    is_multithreaded = false;
    proc_exit_invoked = false;

    NATIVE_PAGESIZE = sysconf(_SC_PAGE_SIZE);
    MMAP_PAGELEN = 0;
    WASM_PAGELEN = 0;
    WASM_TO_NATIVE_PAGE = WASM_PAGESIZE / NATIVE_PAGESIZE;
    THREAD_ID = 1;
}

// TODO: Factor this out
void wali_init_native() {
    wali_global_init();
    wali_instance_init(NULL);
}


/* Get page aligned address after memory to mmap; since base is mapped it's
 * already aligned, and memory data size is a multiple of 64kB but rounding
 * added for safety */
Addr align_mmap_addr(wasm_exec_env_t exec_env) {
    Addr base = wasm_linear_memory_base(exec_env);
    Addr punalign =
        base + wasm_runtime_get_base_memory_size(get_module_inst(exec_env));
    long pageoff = (long)(punalign) & (NATIVE_PAGESIZE - 1);
    Addr palign = punalign - pageoff;
    if (pageoff) {
        palign += NATIVE_PAGESIZE;
    }
    return palign;
}

/* This implicitly checks for process exits, use carefully.
  We can return -1 within if process exit since CHECK_SUSPEND will
  trigger before any future call into WALI */
#define SC_SKEL(sc, ret)                                     \
    {                                                 \
        LOG_VERBOSE(SCSTR(sc)); \
        if (proc_exit_invoked) {                      \
            wali_thread_exit(exec_env, 0);            \
            ret;                                \
        }                                             \
    }

#define SC(sc) SC_SKEL(sc, return -1)
#define SC_VOID(sc) SC_SKEL(sc, return)

#define RETURN(v, sc, num_args, ...)                      \
    {                                                          \
        long sc_result = v;                                          \
        if (proc_exit_invoked) {                               \
            wali_thread_exit(exec_env, 0);                     \
        }                                                      \
        if (strace == 0 || strace == 1) {                      \
            strace_print(sc_result, sc, num_args, __VA_ARGS__); \
        }                                                      \
        return sc_result;                                     \
    }

/***** WALI Methods *******/
// strace helper
void
strace_print(long syscall_res, char *syscall_name, int num_args, ...)
{
    va_list args;
    va_start(args, num_args);
    long argv[6];
    for (int i = 0; i < num_args; i++) {
        argv[i] = va_arg(args, long);
    }
    va_end(args);
    int strace_fd = fileno(strace_logfile);
    if (strace == 1 || strace == 0) {
        unsigned int offset = 0;
        char buf[2024];
        offset = sprintf(buf, "%d, %d: ", getpid(), gettid());
        offset += sprintf(&buf[offset], "%s(", syscall_name);
        for (int i = 0; i < num_args; i++) {
            if (i == num_args - 1) {
                offset += sprintf(&buf[offset], "%lld", (long long int)argv[i]);
            }
            else {
                offset +=
                    sprintf(&buf[offset], "%lld, ", (long long int)argv[i]);
            }
        }
        offset +=
            sprintf(&buf[offset], ") = %lld \n", (long long int)syscall_res);
        int num_written_bytes = write(strace_fd, buf, offset);
        (void)num_written_bytes;
    }
}


void
wali_thread_exit(wasm_exec_env_t exec_env, long v)
{
    VERB("Exiting thread...");
    /* Have to use cancel thread as opposed to exit thread
     * so that it is caught after native functions (WALI) returns */
    wasm_cluster_cancel_thread(exec_env);
}



// 0
long
wali_syscall_read(wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr buf, uint32_t count)
{
    SC(read);
    RETURN(__syscall3(SYS_read, fd, addr_wasm2native(exec_env, buf), count), "read", 3, fd, buf, count);
}

// 1
long
wali_syscall_write(wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr buf, uint32_t count)
{
    SC(write);
    RETURN(__syscall3(SYS_write, fd, addr_wasm2native(exec_env, buf), count), "write", 3, fd, buf, count);
}

// 2
long
wali_syscall_open(wasm_exec_env_t exec_env, WasmMemAddr pathname, int32_t flags, int32_t mode)
{
    SC(open);
#if __x86_64__
    RETURN(__syscall3(SYS_open, addr_wasm2native(exec_env, pathname), flags, mode), "open", 3, pathname, flags, mode);
#elif __aarch64__ || __riscv64__
    RETURN(openat_impl(exec_env, AT_FDCWD, pathname, flags, mode), "open", 3, pathname,
           flags, mode);
#endif
}

// 3
long
wali_syscall_close(wasm_exec_env_t exec_env, int32_t fd)
{
    SC(close);
    RETURN(__syscall1(SYS_close, fd), "close", 1, fd);
}

// 4
long
wali_syscall_stat(wasm_exec_env_t exec_env, WasmMemAddr pathname, WasmMemAddr statbuf)
{
    SC(stat);
#if __x86_64__
    RETURN(__syscall2(SYS_stat, addr_wasm2native(exec_env, pathname), addr_wasm2native(exec_env, statbuf)), "stat", 2, pathname, statbuf);
#elif __aarch64__ || __riscv64__
    RETURN(newfstatat_impl(exec_env, AT_FDCWD, wasm_bp(exec_env, pathname), statbuf, 0), "stat", 2,
           pathname, statbuf);
#endif
}

// 5
long
wali_syscall_fstat(wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr statbuf)
{
    SC(fstat);
#if __x86_64__
    RETURN(__syscall2(SYS_fstat, fd, addr_wasm2native(exec_env, statbuf)), "fstat", 2, fd, statbuf);
#elif __aarch64__ || __riscv64__
    int ret = newfstatat_impl(exec_env, fd, native_bp(exec_env, &""), statbuf, AT_EMPTY_PATH);
    RETURN(ret, "fstat", 2, fd, statbuf);
#endif
}

// 6
long
wali_syscall_lstat(wasm_exec_env_t exec_env, WasmMemAddr pathname, WasmMemAddr statbuf)
{
    SC(lstat);
#if __x86_64__
    RETURN(__syscall2(SYS_lstat, addr_wasm2native(exec_env, pathname), addr_wasm2native(exec_env, statbuf)), "lstat", 2, pathname, statbuf);
#elif __aarch64__ || __riscv64__
    RETURN(newfstatat_impl(exec_env, AT_FDCWD, wasm_bp(exec_env, pathname), statbuf,
                           AT_SYMLINK_NOFOLLOW),
           "lstat", 2, pathname, statbuf);
#endif
}

// 7
long
wali_syscall_poll(wasm_exec_env_t exec_env, WasmMemAddr fds, uint64_t nfds, int32_t timeout)
{
    SC(poll);
#if __x86_64__
    RETURN(__syscall3(SYS_poll, addr_wasm2native(exec_env, fds), nfds, timeout), "poll", 3, fds, nfds, timeout);
#elif __aarch64__ || __riscv64__
    struct timespec* tmo_p = ((timeout >= 0) ? &((struct timespec){ .tv_sec = timeout / 1000, .tv_nsec = (timeout % 1000) * 1000000 }) 
        : 0);
    long ret = ppoll_impl(exec_env, fds, nfds, native_bp(exec_env, tmo_p), 0, _NSIG / 8);
    RETURN(ret, "poll", 3, fds, nfds, timeout);
#endif
}

// 8
long
wali_syscall_lseek(wasm_exec_env_t exec_env, int32_t fd, int64_t offset, int32_t whence)
{
    SC(lseek);
    RETURN(__syscall3(SYS_lseek, fd, offset, whence), "lseek", 3, fd, offset, whence);
}

// 9
long
wali_syscall_mmap(wasm_exec_env_t exec_env, WasmMemAddr addr, uint32_t length, int32_t prot, int32_t flags, int32_t fd, int64_t offset)
{
    SC(mmap);
    VERB("mmap args | addr: %ld, length: 0x%x, prot: %ld, flags: %ld, fd: %ld, offset: %ld | "
       "MMAP_PAGELEN: %d",
       addr, length, prot, flags, fd, offset, MMAP_PAGELEN);
    wasm_module_inst_t module_inst = get_module_inst(exec_env);

    pthread_mutex_lock(&mmap_lock);
    Addr base_addr = wasm_linear_memory_base(exec_env);
    Addr pa_aligned_addr = align_mmap_addr(exec_env);
    Addr mmap_addr = pa_aligned_addr + MMAP_PAGELEN * NATIVE_PAGESIZE;

    /* Get current memory size */
    uint32_t mem_size = get_current_memory_size(exec_env);
    VERB("Mem Base: %p | Mem End: %p | Mem Size: 0x%x | Mmap Addr: %p", base_addr,
       base_addr + mem_size, mem_size, mmap_addr);

    /* Check if wasm memory needs to be expanded and it is safe */
    int inc_wasm_pages = 0;
    int num_pages = ((length + NATIVE_PAGESIZE - 1) / NATIVE_PAGESIZE);
    int extended_mmap_pagelen = MMAP_PAGELEN + num_pages;
    if (extended_mmap_pagelen > WASM_PAGELEN * WASM_TO_NATIVE_PAGE) {
        int new_wasm_pagelen =
            ((extended_mmap_pagelen + WASM_TO_NATIVE_PAGE - 1)
             / WASM_TO_NATIVE_PAGE);
        inc_wasm_pages = new_wasm_pagelen - WASM_PAGELEN;
        if (!wasm_can_enlarge_memory((WASMModuleInstance *)module_inst,
                                     inc_wasm_pages)) {
            FATAL_SC(mmap, "Out of memory!\n");
            goto mmap_fail;
        }
    }

    Addr mem_addr =
        (Addr)__syscall6(SYS_mmap, mmap_addr, length, prot, MAP_FIXED | flags, fd, offset);
    /* Mmap syscall returns -ERRNO */
    if (((long)mem_addr < 0) && ((long)mem_addr > -4096)) {
        FATAL_SC(mmap, "Failed to mmap!\n");
        goto mmap_fail;
    }
    /* On successful mmap */
    else {
        MMAP_PAGELEN += num_pages;
        /* Expand wasm memory if needed */
        if (inc_wasm_pages) {
            wasm_enlarge_memory((WASMModuleInstance *)module_inst,
                                inc_wasm_pages, true);
            WASM_PAGELEN += inc_wasm_pages;
        }
    }
    long retval = addr_native2wasm(exec_env, mem_addr);
    VERB("New MMAP Pagelen: %d", MMAP_PAGELEN);
    pthread_mutex_unlock(&mmap_lock);
    VERB("Ret Addr: 0x%x\n", retval);
    RETURN(retval, "mmap", 6, addr, length, prot, flags, fd, offset);

mmap_fail:
    pthread_mutex_unlock(&mmap_lock);
    RETURN((long)MAP_FAILED, "mmap", 6, addr, length, prot, flags, fd, offset);
}

// 10
long
wali_syscall_mprotect(wasm_exec_env_t exec_env, WasmMemAddr addr, uint32_t len, int32_t prot)
{
    SC(mprotect);
    RETURN(__syscall3(SYS_mprotect, addr_wasm2native(exec_env, addr), len, prot), "mprotect", 3, addr, len,
           prot);
}

// 11
long
wali_syscall_munmap(wasm_exec_env_t exec_env, WasmMemAddr addr, uint32_t len)
{
    SC(munmap);
    pthread_mutex_lock(&mmap_lock);
    Addr mmap_addr = addr_wasm2native(exec_env, addr);
    Addr mmap_addr_end = (Addr)(mmap_addr + len);
    /* Reclaim some mmap space if end region is unmapped */
    Addr pa_aligned_addr = align_mmap_addr(exec_env);
    int end_page = (mmap_addr_end - pa_aligned_addr + NATIVE_PAGESIZE - 1)
                   / NATIVE_PAGESIZE;
    VERB("End page: %d | MMAP_PAGELEN: %d", end_page, MMAP_PAGELEN);
    if (end_page == MMAP_PAGELEN) {
        MMAP_PAGELEN -= ((len + NATIVE_PAGESIZE - 1) / NATIVE_PAGESIZE);
        VERB("End page unmapped | New MMAP_PAGELEN: %d", MMAP_PAGELEN);
    }
    pthread_mutex_unlock(&mmap_lock);
    RETURN(__syscall2(SYS_munmap, mmap_addr, len), "munmap", 2, addr, len);
}

// 12
long
wali_syscall_brk(wasm_exec_env_t exec_env, WasmMemAddr addr)
{
    SC(brk);
    VERB("brk syscall is a NOP in WASM");
    RETURN(0, "brk", 1, addr);
}

void
sa_handler_wali(int signo)
{
    /* Mark pending signal */
    pthread_mutex_lock(&sigpending_mut);
    wali_sigpending |= ((uint64_t)1 << signo);
    pthread_mutex_unlock(&sigpending_mut);
}
// 13
long
wali_syscall_rt_sigaction(wasm_exec_env_t exec_env, int32_t signum, WasmMemAddr act, WasmMemAddr oldact, uint32_t sigsetsize)
{
    SC(rt_sigaction);
    VERB("rt_sigaction args | signum: %ld, act: %ld, oldact: %ld, sigsetsize: %ld", signum, act, oldact,
       sigsetsize);
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    int signo = signum;
    Addr wasm_act = addr_wasm2native(exec_env, act);
    Addr wasm_oldact = addr_wasm2native(exec_env, oldact);
    struct k_sigaction native_act = { 0 };
    struct k_sigaction native_oldact = { 0 };

    /* Block signal manipulation while setting up synchronized wali table */
    pthread_mutex_lock(&sigtable_mut);
    WasmFuncPtr target_wasm_funcptr = 0;
    char sigtype[30];

    /* Prepare for native signal syscall */
    struct k_sigaction *act_pt =
        copy_ksigaction(exec_env, wasm_act, &native_act, sa_handler_wali,
                        &target_wasm_funcptr, sigtype);
    struct k_sigaction *oldact_pt = wasm_oldact ? &native_oldact : NULL;
    long retval = __syscall4(SYS_rt_sigaction, signum, act_pt, oldact_pt, sigsetsize);

    VERB("Signal Registration -- \'%s\'(%d) | Sigtype: %s", strsignal(signum), signo,
       sigtype);

    /* Register virtual signal in WALI sigtable
     * ---------------------------------------------------------------
     * | Handler value | WALI sigtable (set)     | Old Action (get)  |
     * ---------------------------------------------------------------
     * | SIG_DFL       | No register             | WASM_SIG_DFL      |
     * | SIG_IGN       | No register             | WASM_SIG_IGN      |
     * | SIG_ERR       |     -                   | WASM_SIG_ERR      |
     * | WasmFuncPtr     | Table[WasmFuncPtr]        | Table[WasmFuncPtr]  |
     * ---------------------------------------------------------------
     * */
    if (!retval && (signo < NSIG)) {
        /* Save old sigaction to WASM */
        if (oldact_pt) {
            copy2wasm_old_ksigaction(signo, wasm_oldact, oldact_pt);
        }
        /* Set WALI table */
        if (act_pt && (act_pt->handler != SIG_DFL)
            && (act_pt->handler != SIG_IGN) && (act_pt->handler != SIG_ERR)) {
            wasm_function_inst_t target_wasm_handler =
                wasm_runtime_get_indirect_function(module_inst, 0,
                                                   target_wasm_funcptr);
            uint32_t old_fn_idx = wali_sigtable[signo].function
                                      ? wasm_runtime_get_function_idx(module_inst, wali_sigtable[signo].function)
                                      : 0;
            uint32_t new_fn_idx =
                target_wasm_handler ? wasm_runtime_get_function_idx(module_inst, target_wasm_handler) : 0;
            VERB("Replacing target handler: Fn[%u] -> Fn[%u]\n", old_fn_idx, new_fn_idx);
            wasm_func_free(exec_env, wali_sigtable[signo].function);
            wali_sigtable[signo].function = target_wasm_handler;
            wali_sigtable[signo].func_table_idx = target_wasm_funcptr;
            wali_sigtable[signo].func_idx = new_fn_idx;
        }
    }
    /* Reset block signals */
    pthread_mutex_unlock(&sigtable_mut);
    RETURN(retval, "rt_sigaction", 4, signum, act, oldact, sigsetsize);
}

// 14
long
wali_syscall_rt_sigprocmask(wasm_exec_env_t exec_env, int32_t how, WasmMemAddr set, WasmMemAddr oldset, uint32_t sigsetsize)
{
    SC(rt_sigprocmask);
    RETURN(__syscall4(SYS_rt_sigprocmask, how, addr_wasm2native(exec_env, set), addr_wasm2native(exec_env, oldset), sigsetsize),
           "rt_sigprocmask", 4, how, set, oldset, sigsetsize);
}

// 15: Never directly called; __libc_restore_rt is called by OS
long
wali_syscall_rt_sigreturn(wasm_exec_env_t exec_env, int64_t unused)
{
    SC(rt_sigreturn);
    FATAL_SC(rt_sigreturn, "should never be called by the user!");
    RETURN(-1, "rt_sigreturn", 1, unused);
}

// 16
long
wali_syscall_ioctl(wasm_exec_env_t exec_env, int32_t fd, int32_t request, WasmMemAddr argp)
{
    SC(ioctl);
    RETURN(__syscall3(SYS_ioctl, fd, request, addr_wasm2native(exec_env, argp)), "ioctl", 3, fd, request, argp);
}

// 17
long
wali_syscall_pread64(wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr buf, uint32_t count, int64_t offset)
{
    SC(pread64);
    RETURN(__syscall4(SYS_pread64, fd, addr_wasm2native(exec_env, buf), count, offset), "pread64", 4, fd, buf,
           count, offset);
}

// 18
long
wali_syscall_pwrite64(wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr buf, uint32_t count, int64_t offset)
{
    SC(pwrite64);
    RETURN(__syscall4(SYS_pwrite64, fd, addr_wasm2native(exec_env, buf), count, offset), "pwrite64", 4, fd,
           buf, count, offset);
}

// 19
long
wali_syscall_readv(wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr iov, int32_t iovcnt)
{
    SC(readv);
    struct iovec *native_iov = copy_iovec(malloc(iovcnt * sizeof(struct iovec)), exec_env, iov, iovcnt);
    long retval = __syscall3(SYS_readv, fd, native_iov, iovcnt);
    free(native_iov);
    RETURN(retval, "readv", 3, fd, iov, iovcnt);
}

// 20
long
wali_syscall_writev(wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr iov, int32_t iovcnt)
{
    SC(writev);
    struct iovec *native_iov = copy_iovec(malloc(iovcnt * sizeof(struct iovec)), exec_env, iov, iovcnt);
    long retval = __syscall3(SYS_writev, fd, native_iov, iovcnt);
    free(native_iov);
    RETURN(retval, "writev", 3, fd, iov, iovcnt);
}

// 21
long
wali_syscall_access(wasm_exec_env_t exec_env, WasmMemAddr pathname, int32_t mode)
{
    SC(access);
#if __x86_64__
    RETURN(__syscall2(SYS_access, addr_wasm2native(exec_env, pathname), mode), "access", 2, pathname, mode);
#elif __aarch64__ || __riscv64__
    RETURN(faccessat_impl(exec_env, AT_FDCWD, pathname, mode, 0), "access", 2,
           pathname, mode);
#endif
}

// 22
long
wali_syscall_pipe(wasm_exec_env_t exec_env, WasmMemAddr pipefd)
{
    SC(pipe);
#if __x86_64__
    RETURN(__syscall1(SYS_pipe, addr_wasm2native(exec_env, pipefd)), "pipe", 1, pipefd);
#elif __aarch64__ || __riscv64__
    RETURN(pipe2_impl(exec_env, pipefd, 0), "pipe", 1, pipefd);
#endif
}

// 23
long
wali_syscall_select(wasm_exec_env_t exec_env, int32_t nfds, WasmMemAddr readfds, WasmMemAddr writefds, WasmMemAddr exceptfds, WasmMemAddr timeout)
{
    SC(select);
#if __x86_64__
    RETURN(
        __syscall5(SYS_select, nfds, addr_wasm2native(exec_env, readfds), addr_wasm2native(exec_env, writefds), addr_wasm2native(exec_env, exceptfds), addr_wasm2native(exec_env, timeout)),
        "select", 5, nfds, readfds, writefds, exceptfds, timeout);
#elif __aarch64__ || __riscv64__
    RETURN(pselect6_impl(exec_env, nfds, readfds, writefds, exceptfds, timeout,
                         (long)((long[]){ 0, _NSIG / 8 })),
           "select", 5, nfds, readfds, writefds, exceptfds, timeout);
#endif
}

// 24
long
wali_syscall_sched_yield(wasm_exec_env_t exec_env)
{
    SC(sched_yield);
    RETURN(__syscall0(SYS_sched_yield), "sched_yield", 0, 0);
}

// 25
long
wali_syscall_mremap(wasm_exec_env_t exec_env, WasmMemAddr old_address, uint32_t old_size, uint32_t new_size, int32_t flags, WasmMemAddr new_address)
{
    SC(mremap);
    VERB("mremap args | old_address: %ld, old_size: 0x%x, new_size: 0x%x, flags: %ld, new_address: %ld | "
       "MMAP_PAGELEN: %d",
       old_address, old_size, new_size, flags, new_address, MMAP_PAGELEN);
    wasm_module_inst_t module_inst = get_module_inst(exec_env);

    /* Remap pages to the end of the wasm memory, like mmap */
    pthread_mutex_lock(&mmap_lock);
    Addr base_addr = wasm_linear_memory_base(exec_env);
    Addr pa_aligned_addr = align_mmap_addr(exec_env);
    Addr mmap_addr = pa_aligned_addr + MMAP_PAGELEN * NATIVE_PAGESIZE;

    uint32 mem_size = get_current_memory_size(exec_env);
    VERB("Mem Base: %p | Mem End: %p | Mem Size: 0x%x | Mmap Addr: %p", base_addr,
       base_addr + mem_size, mem_size, mmap_addr);

    /* Check if wasm memory needs to be expanded and it is safe */
    int inc_wasm_pages = 0;
    int num_pages = ((new_size + NATIVE_PAGESIZE - 1) / NATIVE_PAGESIZE);
    int extended_mmap_pagelen = MMAP_PAGELEN + num_pages;
    if (extended_mmap_pagelen > WASM_PAGELEN * WASM_TO_NATIVE_PAGE) {
        int new_wasm_pagelen =
            ((extended_mmap_pagelen + WASM_TO_NATIVE_PAGE - 1)
             / WASM_TO_NATIVE_PAGE);
        inc_wasm_pages = new_wasm_pagelen - WASM_PAGELEN;
        if (!wasm_can_enlarge_memory((WASMModuleInstance *)module_inst,
                                     inc_wasm_pages)) {
            FATAL_SC(mremap, "Out of memory!\n");
            goto mremap_fail;
        }
    }

    Addr mem_addr = (Addr)__syscall5(SYS_mremap, addr_wasm2native(exec_env, old_address), old_size, new_size,
                                     MREMAP_MAYMOVE | MREMAP_FIXED, mmap_addr);
    VERB("Mem Addr: %p\n", mem_addr);
    /* Sometimes mremap returns -9 instead of MAP_FAILED? */
    if ((mem_addr == MAP_FAILED) || (mem_addr == (void *)(-9))) {
        FATAL_SC(mremap, "Failed to mremap!\n");
        goto mremap_fail;
    }
    /* On success */
    else {
        MMAP_PAGELEN += num_pages;
        /* Expand wasm memory if needed */
        if (inc_wasm_pages) {
            wasm_enlarge_memory((WASMModuleInstance *)module_inst,
                                inc_wasm_pages, true);
            WASM_PAGELEN += inc_wasm_pages;
        }
    }
    long retval = addr_native2wasm(exec_env, mem_addr);
    VERB("New MMAP Pagelen: %d\n", MMAP_PAGELEN);
    pthread_mutex_unlock(&mmap_lock);
    VERB("Ret Addr: 0x%x", retval);
    RETURN(retval, "mremap", 5, old_address, old_size, new_size, flags, new_address);

mremap_fail:
    pthread_mutex_unlock(&mmap_lock);
    RETURN((long)MAP_FAILED, "mremap", 5, old_address, old_size, new_size, flags, new_address);
}

// 26
long
wali_syscall_msync(wasm_exec_env_t exec_env, WasmMemAddr addr, uint32_t length, int32_t flags)
{
    SC(msync);
    RETURN(__syscall3(SYS_msync, addr_wasm2native(exec_env, addr), length, flags), "msync", 3, addr, length, flags);
}

// 28
long
wali_syscall_madvise(wasm_exec_env_t exec_env, WasmMemAddr addr, uint32_t length, int32_t advice)
{
    SC(madvise);
    RETURN(__syscall3(SYS_madvise, addr_wasm2native(exec_env, addr), length, advice), "madvise", 3, addr, length,
           advice);
}

// 32
long
wali_syscall_dup(wasm_exec_env_t exec_env, int32_t oldfd)
{
    SC(dup);
    RETURN(__syscall1(SYS_dup, oldfd), "dup", 1, oldfd);
}

// 33
long
wali_syscall_dup2(wasm_exec_env_t exec_env, int32_t oldfd, int32_t newfd)
{
    SC(dup2);
#if __x86_64__
    RETURN(__syscall2(SYS_dup2, oldfd, newfd), "dup2", 2, oldfd, newfd);
#elif __aarch64__ || __riscv64__
    /* Dup2 returns newfd while dup3 throws error, handle with case below */
    if (oldfd == newfd) {
        long r = fcntl_impl(exec_env, oldfd, F_GETFD, 0);
        RETURN((r >= 0) ? newfd : r, "dup2", 2, oldfd, newfd);
    }
    else {
        RETURN(dup3_impl(exec_env, oldfd, newfd, 0), "dup2", 2, oldfd, newfd);
    }
#endif
}

// 35
long
wali_syscall_nanosleep(wasm_exec_env_t exec_env, WasmMemAddr req, WasmMemAddr rem)
{
    SC(nanosleep);
    RETURN(__syscall2(SYS_nanosleep, addr_wasm2native(exec_env, req), addr_wasm2native(exec_env, rem)), "nanosleep", 2, req,
           rem);
}

// 38
long
wali_syscall_setitimer(wasm_exec_env_t exec_env, int32_t which, WasmMemAddr new_value, WasmMemAddr old_value)
{
    SC(setitimer);
    RETURN(__syscall3(SYS_setitimer, which, addr_wasm2native(exec_env, new_value), addr_wasm2native(exec_env, old_value)), "setitimer", 3,
           which, new_value, old_value);
}

// 39
long
wali_syscall_getpid(wasm_exec_env_t exec_env)
{
    SC(getpid);
    RETURN(__syscall0(SYS_getpid), "getpid", 0, 0);
}

// 41
long
wali_syscall_socket(wasm_exec_env_t exec_env, int32_t domain, int32_t type, int32_t protocol)
{
    SC(socket);
    RETURN(__syscall3(SYS_socket, domain, type, protocol), "socket", 3, domain, type, protocol);
}

// 42
long
wali_syscall_connect(wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr addr, uint32_t addrlen)
{
    SC(connect);
    RETURN(__syscall3(SYS_connect, sockfd, addr_wasm2native(exec_env, addr), addrlen), "connect", 3, sockfd, addr,
           addrlen);
}

// 43
long
wali_syscall_accept(wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr addr, WasmMemAddr addrlen)
{
    SC(accept);
    RETURN(__syscall3(SYS_accept, sockfd, addr_wasm2native(exec_env, addr), addr_wasm2native(exec_env, addrlen)), "accept", 3, sockfd,
           addr, addrlen);
}

// 44
long
wali_syscall_sendto(wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr buf, uint32_t len, int32_t flags, WasmMemAddr dest_addr, uint32_t addrlen)
{
    SC(sendto);
    RETURN(__syscall6(SYS_sendto, sockfd, addr_wasm2native(exec_env, buf), len, flags, addr_wasm2native(exec_env, dest_addr), addrlen),
           "sendto", 6, sockfd, buf, len, flags, dest_addr, addrlen);
}

// 45
long
wali_syscall_recvfrom(wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr buf, uint32_t len, int32_t flags, WasmMemAddr src_addr, WasmMemAddr addrlen)
{
    SC(recvfrom);
    RETURN(
        __syscall6(SYS_recvfrom, sockfd, addr_wasm2native(exec_env, buf), len, flags, addr_wasm2native(exec_env, src_addr), addr_wasm2native(exec_env, addrlen)),
        "recvfrom", 6, sockfd, buf, len, flags, src_addr, addrlen);
}

// 46
long
wali_syscall_sendmsg(wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr msg, int32_t flags)
{
    SC(sendmsg);
    struct msghdr *native_msghdr = copy_msghdr(malloc(sizeof(struct msghdr)), exec_env, msg);
    long retval = __syscall3(SYS_sendmsg, sockfd, native_msghdr, flags);
    free(native_msghdr);
    RETURN(retval, "sendmsg", 3, sockfd, msg, flags);
}

// 47
long
wali_syscall_recvmsg(wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr msg, int32_t flags)
{
    SC(recvmsg);
    struct msghdr *native_msghdr = copy_msghdr(malloc(sizeof(struct msghdr)), exec_env, msg);
    long retval = __syscall3(SYS_recvmsg, sockfd, native_msghdr, flags);
    free(native_msghdr);
    RETURN(retval, "recvmsg", 3, sockfd, msg, flags);
}

// 48
long
wali_syscall_shutdown(wasm_exec_env_t exec_env, int32_t sockfd, int32_t how)
{
    SC(shutdown);
    RETURN(__syscall2(SYS_shutdown, sockfd, how), "shutdown", 2, sockfd, how);
}

// 49
long
wali_syscall_bind(wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr addr, uint32_t addrlen)
{
    SC(bind);
    RETURN(__syscall3(SYS_bind, sockfd, addr_wasm2native(exec_env, addr), addrlen), "bind", 3, sockfd, addr, addrlen);
}

// 50
long
wali_syscall_listen(wasm_exec_env_t exec_env, int32_t sockfd, int32_t backlog)
{
    SC(listen);
    RETURN(__syscall2(SYS_listen, sockfd, backlog), "listen", 2, sockfd, backlog);
}

// 51
long
wali_syscall_getsockname(wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr addr, WasmMemAddr addrlen)
{
    SC(getsockname);
    RETURN(__syscall3(SYS_getsockname, sockfd, addr_wasm2native(exec_env, addr), addr_wasm2native(exec_env, addrlen)), "getsockname",
           3, sockfd, addr, addrlen);
}

// 52
long
wali_syscall_getpeername(wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr addr, WasmMemAddr addrlen)
{
    SC(getpeername);
    RETURN(__syscall3(SYS_getpeername, sockfd, addr_wasm2native(exec_env, addr), addr_wasm2native(exec_env, addrlen)), "getpeername",
           3, sockfd, addr, addrlen);
}

// 53
long
wali_syscall_socketpair(wasm_exec_env_t exec_env, int32_t domain, int32_t type, int32_t protocol, WasmMemAddr sv)
{
    SC(socketpair);
    RETURN(__syscall4(SYS_socketpair, domain, type, protocol, addr_wasm2native(exec_env, sv)), "socketpair", 4,
           domain, type, protocol, sv);
}

// 54
long
wali_syscall_setsockopt(wasm_exec_env_t exec_env, int32_t sockfd, int32_t level, int32_t optname, WasmMemAddr optval, uint32_t optlen)
{
    SC(setsockopt);
    RETURN(__syscall5(SYS_setsockopt, sockfd, level, optname, addr_wasm2native(exec_env, optval), optlen), "setsockopt",
           5, sockfd, level, optname, optval, optlen);
}

// 55
long
wali_syscall_getsockopt(wasm_exec_env_t exec_env, int32_t sockfd, int32_t level, int32_t optname, WasmMemAddr optval, WasmMemAddr optlen)
{
    SC(getsockopt);
    RETURN(__syscall5(SYS_getsockopt, sockfd, level, optname, addr_wasm2native(exec_env, optval), addr_wasm2native(exec_env, optlen)),
           "getsockopt", 5, sockfd, level, optname, optval, optlen);
}

// 57
long
wali_syscall_fork(wasm_exec_env_t exec_env)
{
    SC(fork);
#if __x86_64__
    RETURN(__syscall0(SYS_fork), "fork", 0, 0);
#elif __aarch64__ || __riscv64__
    RETURN(__syscall2(SYS_clone, SIGCHLD, 0), "fork", 0, 0);
#endif
}

void
create_pass_env_file(char **envp)
{
    char filename[100];
    sprintf(filename, "/tmp/wali_env.%d", getpid());
    FILE *fp = fopen(filename, "w");
    for (char **e = envp; *e; e++) {
        fprintf(fp, "%s\n", *e);
    }
    fclose(fp);
}
// 59
long
wali_syscall_execve(wasm_exec_env_t exec_env, WasmMemAddr pathname, WasmMemAddr argv, WasmMemAddr envp)
{
    SC(execve);
    VERB("Execve string: %s\n", addr_wasm2native(exec_env, pathname));
    uint32_t num_argv = 0;
    char **native_argv = NULL;
    if (arr_len_nullterm(exec_env, argv, &num_argv)) {
        native_argv = copy_stringarr(malloc((num_argv + 1) * sizeof(char*)), exec_env, argv, num_argv);
    }
    char **argpt = native_argv;
    int i = 0;
    while (*argpt != NULL) {
        VERB("Argv[%d] : %s\n", i, *argpt);
        argpt++;
        i++;
    }
    uint32_t num_envp = 0;
    char **native_envp = NULL;
    if (arr_len_nullterm(exec_env, envp, &num_envp)) {
        native_envp = copy_stringarr(malloc((num_envp + 1) * sizeof(char*)), exec_env, envp, num_envp);
    }
    /* For child WALI processes: Pass env through temporary file-descriptor that
     * is read on init For child native processes: envp is passed through the
     * syscall invocation */
    if (native_envp) {
        create_pass_env_file(native_envp);
    }
    long retval = __syscall3(SYS_execve, addr_wasm2native(exec_env, pathname), native_argv, native_envp);
    free(native_argv);
    free(native_envp);
    RETURN(retval, "execve", 3, pathname, argv, envp);
}

// 60 
long
wali_syscall_exit(wasm_exec_env_t exec_env, int32_t status)
{
    SC(exit);
    if (is_multithreaded) {
        WARN_SC(exit,
              "Program detected as multithreaded; exiting current thread but "
              "cannot guarantee process-level exit code generation. It is "
              "recommended to use exit_group instead");
        wali_thread_exit(exec_env, status);
    }
    else {
        WARN_SC(exit, "Program detected as single-threaded; invoking proc_exit "
                    "to exit program");
        wali_proc_exit(exec_env, status);
    }
    RETURN(0, "exit", 1, status);
}

// 61
long
wali_syscall_wait4(wasm_exec_env_t exec_env, int32_t pid, WasmMemAddr wstatus, int32_t options, WasmMemAddr rusage)
{
    SC(wait4);
    RETURN(__syscall4(SYS_wait4, pid, addr_wasm2native(exec_env, wstatus), options, addr_wasm2native(exec_env, rusage)), "wait4", 4, pid,
           wstatus, options, rusage);
}

// 62
long
wali_syscall_kill(wasm_exec_env_t exec_env, int32_t pid, int32_t sig)
{
    SC(kill);
    RETURN(__syscall2(SYS_kill, pid, sig), "kill", 2, pid, sig);
}

// 63
long
wali_syscall_uname(wasm_exec_env_t exec_env, WasmMemAddr buf)
{
    SC(uname);
    RETURN(__syscall1(SYS_uname, addr_wasm2native(exec_env, buf)), "uname", 1, buf);
}

// 72
long
wali_syscall_fcntl(wasm_exec_env_t exec_env, int32_t fd, int32_t cmd, uint64_t arg)
{
    SC(fcntl);
    RETURN(fcntl_impl(exec_env, fd, cmd, arg), "fcntl", 3, fd, cmd, arg);
}

// 73
long
wali_syscall_flock(wasm_exec_env_t exec_env, int32_t fd, int32_t operation)
{
    SC(flock);
    RETURN(__syscall2(SYS_flock, fd, operation), "flock", 2, fd, operation);
}

// 74
long
wali_syscall_fsync(wasm_exec_env_t exec_env, int32_t fd)
{
    SC(fsync);
    RETURN(__syscall1(SYS_fsync, fd), "fsync", 1, fd);
}

// 75
long
wali_syscall_fdatasync(wasm_exec_env_t exec_env, int32_t fd)
{
    SC(fdatasync);
    RETURN(__syscall1(SYS_fdatasync, fd), "fdatasync", 1, fd);
}

// 77
long
wali_syscall_ftruncate(wasm_exec_env_t exec_env, int32_t fd, int64_t length)
{
    SC(ftruncate);
    RETURN(__syscall2(SYS_ftruncate, fd, length), "ftruncate", 2, fd, length);
}

// 79
long
wali_syscall_getcwd(wasm_exec_env_t exec_env, WasmMemAddr buf, uint32_t size)
{
    SC(getcwd);
    RETURN(__syscall2(SYS_getcwd, addr_wasm2native(exec_env, buf), size), "getcwd", 2, buf, size);
}

// 80
long
wali_syscall_chdir(wasm_exec_env_t exec_env, WasmMemAddr path)
{
    SC(chdir);
    RETURN(__syscall1(SYS_chdir, addr_wasm2native(exec_env, path)), "chdir", 1, path);
}

// 81
long
wali_syscall_fchdir(wasm_exec_env_t exec_env, int32_t fd)
{
    SC(fchdir);
    RETURN(__syscall1(SYS_fchdir, fd), "fchdir", 1, fd);
}

// 82
long
wali_syscall_rename(wasm_exec_env_t exec_env, WasmMemAddr oldpath, WasmMemAddr newpath)
{
    SC(rename);
#if __x86_64__
    RETURN(__syscall2(SYS_rename, addr_wasm2native(exec_env, oldpath), addr_wasm2native(exec_env, newpath)), "rename", 2, oldpath, newpath);
#elif __aarch64__ || __riscv64__
    RETURN(renameat2_impl(exec_env, AT_FDCWD, oldpath, AT_FDCWD, newpath, 0),
           "rename", 2, oldpath, newpath);
#endif
}

// 83
long
wali_syscall_mkdir(wasm_exec_env_t exec_env, WasmMemAddr pathname, int32_t mode)
{
    SC(mkdir);
#if __x86_64__
    RETURN(__syscall2(SYS_mkdir, addr_wasm2native(exec_env, pathname), mode), "mkdir", 2, pathname, mode);
#elif __aarch64__ || __riscv64__
    RETURN(mkdirat_impl(exec_env, AT_FDCWD, pathname, mode), "mkdir", 2, pathname,
           mode);
#endif
}

// 84
long
wali_syscall_rmdir(wasm_exec_env_t exec_env, WasmMemAddr pathname)
{
    SC(rmdir);
#if __x86_64__
    RETURN(__syscall1(SYS_rmdir, addr_wasm2native(exec_env, pathname)), "rmdir", 1, pathname);
#elif __aarch64__ || __riscv64__
    RETURN(unlinkat_impl(exec_env, AT_FDCWD, pathname, AT_REMOVEDIR), "rmdir",
           1, pathname);
#endif
}

// 86
long
wali_syscall_link(wasm_exec_env_t exec_env, WasmMemAddr oldpath, WasmMemAddr newpath)
{
    SC(link);
#if __x86_64__
    RETURN(__syscall2(SYS_link, addr_wasm2native(exec_env, oldpath), addr_wasm2native(exec_env, newpath)), "link", 2, oldpath, newpath);
#elif __aarch64__ || __riscv64__
    RETURN(linkat_impl(exec_env, AT_FDCWD, oldpath, AT_FDCWD, newpath, 0), "link",
           2, oldpath, newpath);
#endif
}

// 87
long
wali_syscall_unlink(wasm_exec_env_t exec_env, WasmMemAddr pathname)
{
    SC(unlink);
#if __x86_64__
    RETURN(__syscall1(SYS_unlink, addr_wasm2native(exec_env, pathname)), "unlink", 1, pathname);
#elif __aarch64__ || __riscv64__
    RETURN(unlinkat_impl(exec_env, AT_FDCWD, pathname, 0), "unlink", 1, pathname);
#endif
}

// 88
long
wali_syscall_symlink(wasm_exec_env_t exec_env, WasmMemAddr target, WasmMemAddr linkpath)
{
    SC(symlink);
#if __x86_64__
    RETURN(__syscall2(SYS_symlink, addr_wasm2native(exec_env, target), addr_wasm2native(exec_env, linkpath)), "symlink", 2, target, linkpath);
#elif __aarch64__ || __riscv64__
    RETURN(symlinkat_impl(exec_env, target, AT_FDCWD, linkpath), "symlink", 2, target,
           linkpath);
#endif
}

// 89
long
wali_syscall_readlink(wasm_exec_env_t exec_env, WasmMemAddr pathname, WasmMemAddr buf, uint32_t bufsiz)
{
    SC(readlink);
#if __x86_64__
    RETURN(__syscall3(SYS_readlink, addr_wasm2native(exec_env, pathname), addr_wasm2native(exec_env, buf), bufsiz), "readlink", 3,
           pathname, buf, bufsiz);
#elif __aarch64__ || __riscv64__
    RETURN(readlinkat_impl(exec_env, AT_FDCWD, pathname, buf, bufsiz), "readlink",
           3, pathname, buf, bufsiz);
#endif
}

// 90
long
wali_syscall_chmod(wasm_exec_env_t exec_env, WasmMemAddr pathname, int32_t mode)
{
    SC(chmod);
#if __x86_64__
    RETURN(__syscall2(SYS_chmod, addr_wasm2native(exec_env, pathname), mode), "chmod", 2, pathname, mode);
#elif __aarch64__ || __riscv64__
    RETURN(fchmodat_impl(exec_env, AT_FDCWD, pathname, mode, 0), "chmod", 2, pathname,
           mode);
#endif
}

// 91
long
wali_syscall_fchmod(wasm_exec_env_t exec_env, int32_t fd, int32_t mode)
{
    SC(fchmod);
    RETURN(__syscall2(SYS_fchmod, fd, mode), "fchmod", 2, fd, mode);
}

// 92
long
wali_syscall_chown(wasm_exec_env_t exec_env, WasmMemAddr pathname, int32_t owner, int32_t group)
{
    SC(chown);
#if __x86_64__
    RETURN(__syscall3(SYS_chown, addr_wasm2native(exec_env, pathname), owner, group), "chown", 3, pathname, owner, group);
#elif __aarch64__ || __riscv64__
    RETURN(fchownat_impl(exec_env, AT_FDCWD, pathname, owner, group, 0), "chown", 3,
           pathname, owner, group);
#endif
}

// 93
long
wali_syscall_fchown(wasm_exec_env_t exec_env, int32_t fd, int32_t owner, int32_t group)
{
    SC(fchown);
    RETURN(__syscall3(SYS_fchown, fd, owner, group), "fchown", 3, fd, owner, group);
}

// 95
long
wali_syscall_umask(wasm_exec_env_t exec_env, int32_t mask)
{
    SC(umask);
    RETURN(__syscall1(SYS_umask, mask), "umask", 1, mask);
}

// 96
long
wali_syscall_gettimeofday(wasm_exec_env_t exec_env, WasmMemAddr tv, WasmMemAddr tz)
{
    SC(gettimeofday);
    RETURN(__syscall2(SYS_gettimeofday, addr_wasm2native(exec_env, tv), addr_wasm2native(exec_env, tz)), "gettimeofday",
           2, tv, tz);
}

// 97
long
wali_syscall_getrlimit(wasm_exec_env_t exec_env, int32_t resource, WasmMemAddr rlim)
{
    SC(getrlimit);
    RETURN(__syscall2(SYS_getrlimit, resource, addr_wasm2native(exec_env, rlim)), "getrlimit", 2, resource, rlim);
}

// 98
long
wali_syscall_getrusage(wasm_exec_env_t exec_env, int32_t who, WasmMemAddr usage)
{
    SC(getrusage);
    RETURN(__syscall2(SYS_getrusage, who, addr_wasm2native(exec_env, usage)), "getrusage", 2, who, usage);
}

// 99
long
wali_syscall_sysinfo(wasm_exec_env_t exec_env, WasmMemAddr info)
{
    SC(sysinfo);
    RETURN(__syscall1(SYS_sysinfo, addr_wasm2native(exec_env, info)), "sysinfo", 1, info);
}

// 102
long
wali_syscall_getuid(wasm_exec_env_t exec_env)
{
    SC(getuid);
    RETURN(__syscall0(SYS_getuid), "getuid", 0, 0);
}

// 104
long
wali_syscall_getgid(wasm_exec_env_t exec_env)
{
    SC(getgid);
    RETURN(__syscall0(SYS_getgid), "getgid", 0, 0);
}

// 105
long
wali_syscall_setuid(wasm_exec_env_t exec_env, int32_t uid)
{
    SC(setuid);
    RETURN(__syscall1(SYS_setuid, uid), "setuid", 1, uid);
}

// 106
long
wali_syscall_setgid(wasm_exec_env_t exec_env, int32_t gid)
{
    SC(setgid);
    RETURN(__syscall1(SYS_setgid, gid), "setgid", 1, gid);
}

// 107
long
wali_syscall_geteuid(wasm_exec_env_t exec_env)
{
    SC(geteuid);
    RETURN(__syscall0(SYS_geteuid), "geteuid", 0, 0);
}

// 108
long
wali_syscall_getegid(wasm_exec_env_t exec_env)
{
    SC(getegid);
    RETURN(__syscall0(SYS_getegid), "getegid", 0, 0);
}

// 109
long
wali_syscall_setpgid(wasm_exec_env_t exec_env, int32_t pid, int32_t pgid)
{
    SC(setpgid);
    RETURN(__syscall2(SYS_setpgid, pid, pgid), "setpgid", 2, pid, pgid);
}

// 110
long
wali_syscall_getppid(wasm_exec_env_t exec_env)
{
    SC(getppid);
    RETURN(__syscall0(SYS_getppid), "getppid", 0, 0);
}

// 112
long
wali_syscall_setsid(wasm_exec_env_t exec_env)
{
    SC(setsid);
    RETURN(__syscall0(SYS_setsid), "setsid", 0, 0);
}

// 113
long
wali_syscall_setreuid(wasm_exec_env_t exec_env, int32_t ruid, int32_t euid)
{
    SC(setreuid);
    RETURN(__syscall2(SYS_setreuid, ruid, euid), "setreuid", 2, ruid, euid);
}

// 114
long
wali_syscall_setregid(wasm_exec_env_t exec_env, int32_t rgid, int32_t egid)
{
    SC(setregid);
    RETURN(__syscall2(SYS_setregid, rgid, egid), "setregid", 2, rgid, egid);
}

// 115
long
wali_syscall_getgroups(wasm_exec_env_t exec_env, uint32_t size, WasmMemAddr list)
{
    SC(getgroups);
    RETURN(__syscall2(SYS_getgroups, size, addr_wasm2native(exec_env, list)), "getgroups", 2, size, list);
}

// 116
long
wali_syscall_setgroups(wasm_exec_env_t exec_env, uint32_t size, WasmMemAddr list)
{
    SC(setgroups);
    RETURN(__syscall2(SYS_setgroups, size, addr_wasm2native(exec_env, list)), "setgroups", 2, size, list);
}

// 117
long
wali_syscall_setresuid(wasm_exec_env_t exec_env, int32_t ruid, int32_t euid, int32_t suid)
{
    SC(setresuid);
    RETURN(__syscall3(SYS_setresuid, ruid, euid, suid), "setresuid", 3, ruid, euid, suid);
}

// 119
long
wali_syscall_setresgid(wasm_exec_env_t exec_env, int32_t rgid, int32_t egid, int32_t sgid)
{
    SC(setresgid);
    RETURN(__syscall3(SYS_setresgid, rgid, egid, sgid), "setresgid", 3, rgid, egid, sgid);
}

// 121
long
wali_syscall_getpgid(wasm_exec_env_t exec_env, int32_t pid)
{
    SC(getpgid);
    RETURN(__syscall1(SYS_getpgid, pid), "getpgid", 1, pid);
}

// 124
long
wali_syscall_getsid(wasm_exec_env_t exec_env, int32_t pid)
{
    SC(getsid);
    RETURN(__syscall1(SYS_getsid, pid), "getsid", 1, pid);
}

// 127
long
wali_syscall_rt_sigpending(wasm_exec_env_t exec_env, WasmMemAddr set, uint32_t sigsetsize)
{
    SC(rt_sigpending);
    RETURN(__syscall2(SYS_rt_sigpending, addr_wasm2native(exec_env, set), sigsetsize), "rt_sigpending", 2, set,
           sigsetsize);
}

// 130
long
wali_syscall_rt_sigsuspend(wasm_exec_env_t exec_env, WasmMemAddr mask, uint32_t sigsetsize)
{
    SC(rt_sigsuspend);
    RETURN(__syscall2(SYS_rt_sigsuspend, addr_wasm2native(exec_env, mask), sigsetsize), "rt_sigsuspend", 2, mask,
           sigsetsize);
}

// 131
long
wali_syscall_sigaltstack(wasm_exec_env_t exec_env, WasmMemAddr ss, WasmMemAddr old_ss)
{
    SC(sigaltstack);

    stack_t *ss_ptr = copy_sigstack(&(stack_t){0}, exec_env, ss);
    // This doesn't need to copy the fields in, just needs the pointer
    stack_t *old_ss_ptr = copy_sigstack(&(stack_t){0}, exec_env, old_ss);
    long retval = __syscall2(SYS_sigaltstack, ss_ptr, old_ss_ptr);
    // Reconstruct the old stack returned
    copy2wasm_sigstack(exec_env, old_ss, old_ss_ptr);

    RETURN(retval, "sigaltstack", 2, ss, old_ss);
}

// 137
long
wali_syscall_statfs(wasm_exec_env_t exec_env, WasmMemAddr path, WasmMemAddr buf)
{
    SC(statfs);
    RETURN(__syscall2(SYS_statfs, addr_wasm2native(exec_env, path), addr_wasm2native(exec_env, buf)), "statfs", 2, path, buf);
}

// 138
long
wali_syscall_fstatfs(wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr buf)
{
    SC(fstatfs);
    RETURN(__syscall2(SYS_fstatfs, fd, addr_wasm2native(exec_env, buf)), "fstatfs", 2, fd, buf);
}

// 157
long
wali_syscall_prctl(wasm_exec_env_t exec_env, int32_t option, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5)
{
    SC(prctl);
    long retval = -1;
    switch(option) {
        case PR_GET_NAME:
        case PR_SET_NAME:
            retval = __syscall2(SYS_prctl, option, addr_wasm2native(exec_env, arg2));
            break;
        default:
            WARN_SC(prctl, "Unsupported option: %ld", option);
            retval = -1;
            break;
    }
    RETURN(retval, "prctl", 5, option, arg2, arg3, arg4, arg5);
}

// 160
long
wali_syscall_setrlimit(wasm_exec_env_t exec_env, int32_t resource, WasmMemAddr rlim)
{
    SC(setrlimit);
    RETURN(__syscall2(SYS_setrlimit, resource, addr_wasm2native(exec_env, rlim)), "setrlimit", 2, resource, rlim);
}

// 161
long
wali_syscall_chroot(wasm_exec_env_t exec_env, WasmMemAddr path)
{
    SC(chroot);
    RETURN(__syscall1(SYS_chroot, addr_wasm2native(exec_env, path)), "chroot", 1, path);
}

// 186
long
wali_syscall_gettid(wasm_exec_env_t exec_env)
{
    SC(gettid);
    RETURN(__syscall0(SYS_gettid), "gettid", 0, 0);
}

// 200
long
wali_syscall_tkill(wasm_exec_env_t exec_env, int32_t tid, int32_t sig)
{
    SC(tkill);
    RETURN(__syscall2(SYS_tkill, tid, sig), "tkill", 2, tid, sig);
}

// 202
long
wali_syscall_futex(wasm_exec_env_t exec_env, WasmMemAddr uaddr, int32_t futex_op, int32_t val, WasmMemAddr timeout, WasmMemAddr uaddr2, int32_t val3)
{
    SC(futex);
    RETURN(__syscall6(SYS_futex, addr_wasm2native(exec_env, uaddr), futex_op, val, addr_wasm2native(exec_env, timeout), addr_wasm2native(exec_env, uaddr2), val3),
           "futex", 6, uaddr, futex_op, val, timeout, uaddr2, val3);
}

// 204
long
wali_syscall_sched_getaffinity(wasm_exec_env_t exec_env, int32_t pid, uint32_t cpusetsize, WasmMemAddr mask)
{
    SC(sched_getaffinity);
    RETURN(__syscall3(SYS_sched_getaffinity, pid, cpusetsize, addr_wasm2native(exec_env, mask)),
           "sched_getaffinity", 3, pid, cpusetsize, mask);
}

// 217
long
wali_syscall_getdents64(wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr dirp, int32_t count)
{
    SC(getdents64);
    RETURN(__syscall3(SYS_getdents64, fd, addr_wasm2native(exec_env, dirp), count), "getdents64", 3, fd,
           dirp, count);
}

// 218
long
wali_syscall_set_tid_address(wasm_exec_env_t exec_env, WasmMemAddr tidptr)
{
    SC(set_tid_address);
    RETURN(__syscall1(SYS_set_tid_address, addr_wasm2native(exec_env, tidptr)), "set_tid_address", 1,
           tidptr);
}

// 221 TODO
long
wali_syscall_fadvise(wasm_exec_env_t exec_env, int32_t fd, int64_t offset, int64_t len, int32_t advice)
{
    SC(fadvise);
    MIS_SC(fadvise);
    RETURN(__syscall4(SYS_fadvise64, fd, offset, len, advice), "fadvise", 4, fd, offset, len,
           advice);
}

// 228
long
wali_syscall_clock_gettime(wasm_exec_env_t exec_env, int32_t clockid, WasmMemAddr tp)
{
    SC(clock_gettime);
    RETURN(__syscall2(SYS_clock_gettime, clockid, addr_wasm2native(exec_env, tp)), "clock_gettime", 2, clockid,
           tp);
}

// 229
long
wali_syscall_clock_getres(wasm_exec_env_t exec_env, int32_t clockid, WasmMemAddr res)
{
    SC(clock_getres);
    RETURN(__syscall2(SYS_clock_getres, clockid, addr_wasm2native(exec_env, res)), "clock_getres", 2, clockid,
           res);
}

// 230
long
wali_syscall_clock_nanosleep(wasm_exec_env_t exec_env, int32_t clockid, int32_t flags, WasmMemAddr request, WasmMemAddr remain)
{
    SC(clock_nanosleep);
    RETURN(__syscall4(SYS_clock_nanosleep, clockid, flags, addr_wasm2native(exec_env, request), addr_wasm2native(exec_env, remain)),
           "clock_nanosleep", 4, clockid, flags, request, remain);
}

// 231
long
wali_syscall_exit_group(wasm_exec_env_t exec_env, int32_t status)
{
    SC(exit_group);
    wali_proc_exit(exec_env, status);
    RETURN(-1, "exit_group", 1, status);
}

// 233
long
wali_syscall_epoll_ctl(wasm_exec_env_t exec_env, int32_t epfd, int32_t op, int32_t fd, WasmMemAddr event)
{
    SC(epoll_ctl);
    struct epoll_event *nev = copy_epoll_event(&(struct epoll_event){ 0 }, exec_env, event);
    RETURN(__syscall4(SYS_epoll_ctl, epfd, op, fd, nev), "epoll_ctl", 4, epfd, op,
           fd, event);
}

// 257
long
wali_syscall_openat(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t flags, int32_t mode)
{
    SC(openat);
    RETURN(openat_impl(exec_env, dirfd, pathname, flags, mode), "openat", 4, dirfd, pathname, flags, mode);
}

// 258
long
wali_syscall_mkdirat(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t mode)
{
    SC(mkdirat);
    RETURN(mkdirat_impl(exec_env, dirfd, pathname, mode), "mkdirat", 3, dirfd, pathname, mode);
}

// 260
long
wali_syscall_fchownat(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t owner, int32_t group, int32_t flags)
{
    SC(fchownat);
    RETURN(fchownat_impl(exec_env, dirfd, pathname, owner, group, flags), "fchownat", 5,
           dirfd, pathname, owner, group, flags);
}

// 262
long
wali_syscall_newfstatat(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, WasmMemAddr statbuf, int32_t flags)
{
    SC(newfstatat);
    int ret = newfstatat_impl(exec_env, dirfd, wasm_bp(exec_env, pathname), statbuf, flags);
    RETURN(ret, "newfstatat", 4, dirfd, pathname, statbuf, flags);
}

// 263
long
wali_syscall_unlinkat(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t flags)
{
    SC(unlinkat);
    RETURN(unlinkat_impl(exec_env, dirfd, pathname, flags), "unlinkat", 3, dirfd, pathname, flags);
}

// 265
long
wali_syscall_linkat(wasm_exec_env_t exec_env, int32_t olddirfd, WasmMemAddr oldpath, int32_t newdirfd, WasmMemAddr newpath, int32_t flags)
{
    SC(linkat);
    RETURN(linkat_impl(exec_env, olddirfd, oldpath, newdirfd, newpath, flags), "linkat",
           5, olddirfd, oldpath, newdirfd, newpath, flags);
}

// 266
long
wali_syscall_symlinkat(wasm_exec_env_t exec_env, WasmMemAddr target, int32_t newdirfd, WasmMemAddr linkpath)
{
    SC(symlinkat);
    RETURN(symlinkat_impl(exec_env, target, newdirfd, linkpath), "symlinkat", 3,
           target, newdirfd, linkpath);
}

// 267
long
wali_syscall_readlinkat(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, WasmMemAddr buf, uint32_t bufsiz)
{
    SC(readlinkat);
    RETURN(readlinkat_impl(exec_env, dirfd, pathname, buf, bufsiz),
           "readlinkat", 4, dirfd, pathname, buf, bufsiz);
}

// 268
long
wali_syscall_fchmodat(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t mode, int32_t flags)
{
    SC(fchmodat);
    RETURN(fchmodat_impl(exec_env, dirfd, pathname, mode, flags), "fchmodat", 4, dirfd,
           pathname, mode, flags);
}

// 269
long
wali_syscall_faccessat(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t mode, int32_t flags)
{
    SC(faccessat);
    RETURN(faccessat_impl(exec_env, dirfd, pathname, mode, flags), "faccessat", 4, dirfd,
           pathname, mode, flags);
}

// 270
long
wali_syscall_pselect6(wasm_exec_env_t exec_env, int32_t nfds, WasmMemAddr readfds, WasmMemAddr writefds, WasmMemAddr exceptfds, WasmMemAddr timeout, WasmMemAddr sigmask)
{
    SC(pselect6);
    RETURN(pselect6_impl(exec_env, nfds, readfds, writefds, exceptfds, timeout, sigmask),
           "pselect6", 6, nfds, readfds, writefds, exceptfds, timeout, sigmask);
}

// 271
long
wali_syscall_ppoll(wasm_exec_env_t exec_env, WasmMemAddr fds, uint64_t nfds, WasmMemAddr tmo_p, WasmMemAddr sigmask, uint32_t sigsetsize)
{
    SC(ppoll);
    long ret = ppoll_impl(exec_env, fds, nfds, wasm_bp(exec_env, tmo_p), sigmask, sigsetsize);
    RETURN(ret, "ppoll", 5, fds, nfds, tmo_p, sigmask, sigsetsize);
}

// 280
long
wali_syscall_utimensat(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, WasmMemAddr times, int32_t flags)
{
    SC(utimensat);
    RETURN(__syscall4(SYS_utimensat, dirfd, addr_wasm2native(exec_env, pathname), addr_wasm2native(exec_env, times), flags), "utimensat",
           4, dirfd, pathname, times, flags);
}

// 281
long
wali_syscall_epoll_pwait(wasm_exec_env_t exec_env, int32_t epfd, WasmMemAddr events, int32_t maxevents, int32_t timeout, WasmMemAddr sigmask, uint32_t sigsetsize)
{
    SC(epoll_pwait);
    struct epoll_event *nev = copy_epoll_event(&(struct epoll_event){ 0 }, exec_env, events);
    long retval = __syscall6(SYS_epoll_pwait, epfd, nev, maxevents, timeout, addr_wasm2native(exec_env, sigmask), sigsetsize);
    copy2wasm_epoll_event(exec_env, events, nev);
    RETURN(retval, "epoll_pwait", 6, epfd, events, maxevents, timeout, sigmask, sigsetsize);
}

// 284
long
wali_syscall_eventfd(wasm_exec_env_t exec_env, int32_t initval)
{
    SC(eventfd);
#if __x86_64__
    RETURN(__syscall1(SYS_eventfd, initval), "eventfd", 1, initval);
#elif __aarch64__ || __riscv64__
    RETURN(eventfd2_impl(exec_env, initval, 0), "eventfd", 1, initval);
#endif
}

// 288
long
wali_syscall_accept4(wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr addr, WasmMemAddr addrlen, int32_t flags)
{
    SC(accept4);
    RETURN(__syscall4(SYS_accept4, sockfd, addr_wasm2native(exec_env, addr), addr_wasm2native(exec_env, addrlen), flags), "accept4", 4,
           sockfd, addr, addrlen, flags);
}

// 290
long
wali_syscall_eventfd2(wasm_exec_env_t exec_env, int32_t initval, int32_t flags)
{
    SC(eventfd2);
    RETURN(eventfd2_impl(exec_env, initval, flags), "eventfd2", 2, initval, flags);
}

// 291
long
wali_syscall_epoll_create1(wasm_exec_env_t exec_env, int32_t flags)
{
    SC(epoll_create1);
    RETURN(__syscall1(SYS_epoll_create1, flags), "epoll_create1", 1, flags);
}

// 292
long
wali_syscall_dup3(wasm_exec_env_t exec_env, int32_t oldfd, int32_t newfd, int32_t flags)
{
    SC(dup3);
    RETURN(dup3_impl(exec_env, oldfd, newfd, flags), "dup3", 3, oldfd, newfd, flags);
}

// 293
long
wali_syscall_pipe2(wasm_exec_env_t exec_env, WasmMemAddr pipefd, int32_t flags)
{
    SC(pipe2);
    RETURN(pipe2_impl(exec_env, pipefd, flags), "pipe2", 2, pipefd, flags);
}

// 302
long
wali_syscall_prlimit64(wasm_exec_env_t exec_env, int32_t pid, int32_t resource, WasmMemAddr new_limit, WasmMemAddr old_limit)
{
    SC(prlimit64);
    RETURN(__syscall4(SYS_prlimit64, pid, resource, addr_wasm2native(exec_env, new_limit), addr_wasm2native(exec_env, old_limit)), "prlimit64",
           4, pid, resource, new_limit, old_limit);
}

// 316
long
wali_syscall_renameat2(wasm_exec_env_t exec_env, int32_t olddirfd, WasmMemAddr oldpath, int32_t newdirfd, WasmMemAddr newpath, int32_t flags)
{
    SC(renameat2);
    RETURN(renameat2_impl(exec_env, olddirfd, oldpath, newdirfd, newpath, flags),
           "renameat2", 5, olddirfd, oldpath, newdirfd, newpath, flags);
}

// 318
long
wali_syscall_getrandom(wasm_exec_env_t exec_env, WasmMemAddr buf, uint32_t buflen, int32_t flags)
{
    SC(getrandom);
    RETURN(__syscall3(SYS_getrandom, addr_wasm2native(exec_env, buf), buflen, flags), "getrandom", 3, buf, buflen,
           flags);
}

// 332
long
wali_syscall_statx(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t flags, uint32_t mask, WasmMemAddr statxbuf)
{
    SC(statx);
    RETURN(__syscall5(SYS_statx, dirfd, addr_wasm2native(exec_env, pathname), flags, mask, addr_wasm2native(exec_env, statxbuf)), "statx", 5,
           dirfd, pathname, flags, mask, statxbuf);
}

// 439
long
wali_syscall_faccessat2(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t mode, int32_t flags)
{
    SC(faccessat2);
    RETURN(__syscall4(439, dirfd, addr_wasm2native(exec_env, pathname), mode, flags), "faccessat2", 4, dirfd, pathname, mode,
           flags);
}

/***** Non-syscall methods *****/
int
wali_sigsetjmp(wasm_exec_env_t exec_env, WasmMemAddr sigjmp_buf, int32_t savesigs)
{
    WARN_SC(sigsetjmp, "Currently a NOP; `longjmp` will terminate the process");
    return 0;
}

int
wali_setjmp(wasm_exec_env_t exec_env, WasmMemAddr env)
{
    WARN_SC(setjmp, "Currently a NOP; `longjmp` will terminate the process");
    return 0;
}

void
wali_longjmp(wasm_exec_env_t exec_env, WasmMemAddr env, int32_t val)
{
    FATAL_SC(siglongjmp, "terminates process immediately...");
    wali_proc_exit(exec_env, 1);
    /* Should not reach here */
    exit(-1);
}

/***** Startup *****/
static bool init_called = false;
static bool deinit_called = false;

int
wali_init(wasm_exec_env_t exec_env)
{
    SC(__init);
    if (init_called) {
        ERR_SC(__init, "should only be invoked once!");
        return 1;
    }
    init_called = true;
    // WAMR engine interfacing
    invoked_wali = true;
    return 0;
}

int
wali_deinit(wasm_exec_env_t exec_env)
{
    SC(__deinit);
    if (deinit_called) {
        ERR_SC(__deinit, "should only be invoked once!");
        return 1;
    }
    deinit_called = true;
    return 0;
}

void
wali_proc_exit(wasm_exec_env_t exec_env, int32_t status)
{
    // Don't use SC since it polls proc_exit_invoked, which is set in this function
    LOG_VERBOSE(SCSTR(__proc_exit));
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    WALIContext *wali_ctx = wasm_runtime_get_wali_ctx(module_inst);
    /* if wali_deinit is invoked, main ended successfully, do
     * not set exception */
    if (!deinit_called || status) {
        VERB("WALI process exit called prematurely");
        wasm_runtime_set_exception(module_inst, "wali proc exit");
    }
    else {
        VERB("Main ended successfully");
    }
    wali_ctx->exit_code = status;
    proc_exit_primary_tid = gettid();
    proc_exit_invoked = true;
}

unsigned int
wali_cl_get_argc(wasm_exec_env_t exec_env)
{
    SC(__cl_get_argc);
    return wali_app_argc;
}

unsigned int
wali_cl_get_argv_len(wasm_exec_env_t exec_env, uint32_t arg_index)
{
    SC(__cl_get_argv_len);
    return strlen(wali_app_argv[arg_index]);
}

int
wali_cl_copy_argv(wasm_exec_env_t exec_env, WasmMemAddr argbuf, uint32_t arg_index)
{
    SC(__cl_copy_argv);
    Addr argv = addr_wasm2native(exec_env, argbuf);
    strcpy((char *)argv, wali_app_argv[arg_index]);
    return 0;
}

int
wali_get_init_envfile(wasm_exec_env_t exec_env, WasmMemAddr pathbuf, uint32_t bufsize)
{
    SC(__get_init_envfile);
    Addr fbuf = addr_wasm2native(exec_env, pathbuf);

    /* Check for passthrough env from an execve call */
    char pass_filename[100];
    sprintf(pass_filename, "/tmp/wali_env.%d", getpid());
    int execve_invoked = !access(pass_filename, R_OK);

    char *envfile = execve_invoked ? pass_filename : wali_app_env_file;

    if (!envfile) {
        WARN_SC(__get_init_envfile, "No environment file provided\n");
        return 0;
    }

    if ((uint64_t)(strlen(envfile) + 1) > bufsize) {
        ERR_SC(__get_init_envfile, "Env initialization filepath too large (max length: %d)."
            "Defaulting to NULL\n",
            bufsize);
        ((char *)fbuf)[0] = 0;
    }
    else {
        strcpy((char *)fbuf, envfile);
        VERB("Env file: \'%s\'\n", fbuf);
    }
    return 1;
}

/***** Threads *****/
typedef struct {
    /* Initial function */
    wasm_function_inst_t start_fn;
    /* Wasm address for args */
    int arg;
    /* Wasm Thread ID */
    int tid;
} WasmThreadStartArg;

/* Thread dispatcher function calls into WASM  */
static void *
wali_dispatch_thread_libc(void *exec_env_ptr)
{
    wasm_exec_env_t exec_env = (wasm_exec_env_t)exec_env_ptr;
    WasmThreadStartArg *thread_arg = (WasmThreadStartArg *)exec_env->thread_arg;

    wasm_exec_env_set_thread_info(exec_env);
    int tid = gettid();
    /* Libc start fn: (int thread_id, void *arg) */
    uint32_t wasm_argv[2];
    // Dispatcher is part of child thread; can get tid using syscall
    wasm_argv[0] = tid; // thread_arg->tid;
    wasm_argv[1] = thread_arg->arg;

    VERB("Dispatcher | Child TID: %d\n", wasm_argv[0]);
    /* Send parent our TID */
    signalled_tid = tid;
    if (sem_post(&tid_sem)) {
        perror("sem_post");
    }

    if (!wasm_runtime_call_wasm(exec_env, thread_arg->start_fn, 2, wasm_argv)) {
        /* Exception has already been spread during throwing */
    }

    VERB("================ Thread [%d] exiting ==============\n", gettid());
    // Cleanup
    wasm_runtime_free(thread_arg);
    exec_env->thread_arg = NULL;

    return NULL;
}

int
wali_wasm_thread_spawn(wasm_exec_env_t exec_env, WasmFuncPtr wasm_start_fn, WasmMemAddr args)
{
    SC(__wasm_thread_spawn);
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    wasm_module_t module = wasm_runtime_get_module(module_inst);
    bh_assert(module);
    bh_assert(module_inst);

    wasm_module_inst_t new_module_inst = NULL;
    WasmThreadStartArg *thread_start_arg = NULL;
    uint32_t stack_size = 8192;
    int thread_id = -1;
    int ret = -1;

    /* Table 0 is only supported currently */
    wasm_function_inst_t setup_wasm_fn =
        wasm_runtime_get_indirect_function(module_inst, 0, wasm_start_fn);

    stack_size = ((WASMModuleInstance *)module_inst)->default_wasm_stack_size;

    /* New module instance -- custom data, import function registration, etc. */
    if (!(new_module_inst = wasm_runtime_instantiate_internal(
              module, module_inst, exec_env, stack_size, 0, 0, NULL, 0)))
        return -1;

    wasm_runtime_set_custom_data_internal(
        new_module_inst, wasm_runtime_get_custom_data(module_inst));

    if (!(wasm_cluster_dup_c_api_imports(new_module_inst, module_inst)))
        goto thread_spawn_fail;
    /** **/

    /** Setup args to pass to startup dispatcher **/
    if (!(thread_start_arg = wasm_runtime_malloc(sizeof(WasmThreadStartArg)))) {
        FATAL_SC(wasm_thread_spawn, "Runtime args allocation failed");
        goto thread_spawn_fail;
    }

    thread_start_arg->tid = thread_id = THREAD_ID++;

    thread_start_arg->start_fn = setup_wasm_fn;
    thread_start_arg->arg = args;
    /** **/

    /** Create and dispatch the thread (language-independent: currently just C);
     * Thread ID of the created thread is sent back to parent */
    volatile int child_tid = -1;
    pthread_mutex_lock(&clone_lock);
    ret =
        wasm_cluster_create_thread(exec_env, new_module_inst, false, 0, 0,
                                   wali_dispatch_thread_libc, thread_start_arg);
    if (ret != 0) {
        FATAL_SC(wasm_thread_spawn, "Failed to spawn a new thread");
        goto thread_spawn_fail_post_clone;
    }

    /* Mark current program as multithreaded (relevant for SYS_exit) */
    is_multithreaded = true;
    /* Get the thread-id of spawned child. Wait for timeout (5 sec) for signal
     */
    struct timespec dtime;
    if (clock_gettime(CLOCK_REALTIME, &dtime) == -1) {
        perror("clock_gettime");
        goto thread_spawn_fail_post_clone;
    }
    dtime.tv_sec += 5;
    if (sem_timedwait(&tid_sem, &dtime)) {
        perror("sem_timedwait");
        FATAL_SC(wasm_thread_spawn, "TID signalling error");
        goto thread_spawn_fail_post_clone;
    }

    child_tid = signalled_tid;
    VERB("Parent of Dispatcher | Child TID: %d\n", child_tid);
    pthread_mutex_unlock(&clone_lock);

    wasm_func_free(exec_env, setup_wasm_fn);

    RETURN(child_tid, 0, 0, 0);

thread_spawn_fail_post_clone:
    pthread_mutex_unlock(&clone_lock);
thread_spawn_fail:
    if (new_module_inst)
        wasm_runtime_deinstantiate_internal(new_module_inst, true);
    if (thread_start_arg)
        wasm_runtime_free(thread_start_arg);

    RETURN(-1, 0, 0, 0);
}

/* Engine Bindings for WALI */
#define NSYMBOL(symbol, fn, sign) { #symbol, (void *)fn, sign, NULL }
static NativeSymbol wali_native_symbols[] = {
	/* Syscalls */
	NSYMBOL (             SYS_read,              wali_syscall_read,     "(iii)I" ),
	NSYMBOL (            SYS_write,             wali_syscall_write,     "(iii)I" ),
	NSYMBOL (             SYS_open,              wali_syscall_open,     "(iii)I" ),
	NSYMBOL (            SYS_close,             wali_syscall_close,       "(i)I" ),
	NSYMBOL (             SYS_stat,              wali_syscall_stat,      "(ii)I" ),
	NSYMBOL (            SYS_fstat,             wali_syscall_fstat,      "(ii)I" ),
	NSYMBOL (            SYS_lstat,             wali_syscall_lstat,      "(ii)I" ),
	NSYMBOL (             SYS_poll,              wali_syscall_poll,     "(iIi)I" ),
	NSYMBOL (            SYS_lseek,             wali_syscall_lseek,     "(iIi)I" ),
	NSYMBOL (             SYS_mmap,              wali_syscall_mmap,  "(iiiiiI)I" ),
	NSYMBOL (         SYS_mprotect,          wali_syscall_mprotect,     "(iii)I" ),
	NSYMBOL (           SYS_munmap,            wali_syscall_munmap,      "(ii)I" ),
	NSYMBOL (              SYS_brk,               wali_syscall_brk,       "(i)I" ),
	NSYMBOL (     SYS_rt_sigaction,      wali_syscall_rt_sigaction,    "(iiii)I" ),
	NSYMBOL (   SYS_rt_sigprocmask,    wali_syscall_rt_sigprocmask,    "(iiii)I" ),
	NSYMBOL (     SYS_rt_sigreturn,      wali_syscall_rt_sigreturn,       "(I)I" ),
	NSYMBOL (            SYS_ioctl,             wali_syscall_ioctl,     "(iii)I" ),
	NSYMBOL (          SYS_pread64,           wali_syscall_pread64,    "(iiiI)I" ),
	NSYMBOL (         SYS_pwrite64,          wali_syscall_pwrite64,    "(iiiI)I" ),
	NSYMBOL (            SYS_readv,             wali_syscall_readv,     "(iii)I" ),
	NSYMBOL (           SYS_writev,            wali_syscall_writev,     "(iii)I" ),
	NSYMBOL (           SYS_access,            wali_syscall_access,      "(ii)I" ),
	NSYMBOL (             SYS_pipe,              wali_syscall_pipe,       "(i)I" ),
	NSYMBOL (           SYS_select,            wali_syscall_select,   "(iiiii)I" ),
	NSYMBOL (      SYS_sched_yield,       wali_syscall_sched_yield,        "()I" ),
	NSYMBOL (           SYS_mremap,            wali_syscall_mremap,   "(iiiii)I" ),
	NSYMBOL (            SYS_msync,             wali_syscall_msync,     "(iii)I" ),
	NSYMBOL (          SYS_madvise,           wali_syscall_madvise,     "(iii)I" ),
	NSYMBOL (              SYS_dup,               wali_syscall_dup,       "(i)I" ),
	NSYMBOL (             SYS_dup2,              wali_syscall_dup2,      "(ii)I" ),
	NSYMBOL (        SYS_nanosleep,         wali_syscall_nanosleep,      "(ii)I" ),
	NSYMBOL (        SYS_setitimer,         wali_syscall_setitimer,     "(iii)I" ),
	NSYMBOL (           SYS_getpid,            wali_syscall_getpid,        "()I" ),
	NSYMBOL (           SYS_socket,            wali_syscall_socket,     "(iii)I" ),
	NSYMBOL (          SYS_connect,           wali_syscall_connect,     "(iii)I" ),
	NSYMBOL (           SYS_accept,            wali_syscall_accept,     "(iii)I" ),
	NSYMBOL (           SYS_sendto,            wali_syscall_sendto,  "(iiiiii)I" ),
	NSYMBOL (         SYS_recvfrom,          wali_syscall_recvfrom,  "(iiiiii)I" ),
	NSYMBOL (          SYS_sendmsg,           wali_syscall_sendmsg,     "(iii)I" ),
	NSYMBOL (          SYS_recvmsg,           wali_syscall_recvmsg,     "(iii)I" ),
	NSYMBOL (         SYS_shutdown,          wali_syscall_shutdown,      "(ii)I" ),
	NSYMBOL (             SYS_bind,              wali_syscall_bind,     "(iii)I" ),
	NSYMBOL (           SYS_listen,            wali_syscall_listen,      "(ii)I" ),
	NSYMBOL (      SYS_getsockname,       wali_syscall_getsockname,     "(iii)I" ),
	NSYMBOL (      SYS_getpeername,       wali_syscall_getpeername,     "(iii)I" ),
	NSYMBOL (       SYS_socketpair,        wali_syscall_socketpair,    "(iiii)I" ),
	NSYMBOL (       SYS_setsockopt,        wali_syscall_setsockopt,   "(iiiii)I" ),
	NSYMBOL (       SYS_getsockopt,        wali_syscall_getsockopt,   "(iiiii)I" ),
	NSYMBOL (             SYS_fork,              wali_syscall_fork,        "()I" ),
	NSYMBOL (           SYS_execve,            wali_syscall_execve,     "(iii)I" ),
	NSYMBOL (             SYS_exit,              wali_syscall_exit,       "(i)I" ),
	NSYMBOL (            SYS_wait4,             wali_syscall_wait4,    "(iiii)I" ),
	NSYMBOL (             SYS_kill,              wali_syscall_kill,      "(ii)I" ),
	NSYMBOL (            SYS_uname,             wali_syscall_uname,       "(i)I" ),
	NSYMBOL (            SYS_fcntl,             wali_syscall_fcntl,     "(iiI)I" ),
	NSYMBOL (            SYS_flock,             wali_syscall_flock,      "(ii)I" ),
	NSYMBOL (            SYS_fsync,             wali_syscall_fsync,       "(i)I" ),
	NSYMBOL (        SYS_fdatasync,         wali_syscall_fdatasync,       "(i)I" ),
	NSYMBOL (        SYS_ftruncate,         wali_syscall_ftruncate,      "(iI)I" ),
	NSYMBOL (           SYS_getcwd,            wali_syscall_getcwd,      "(ii)I" ),
	NSYMBOL (            SYS_chdir,             wali_syscall_chdir,       "(i)I" ),
	NSYMBOL (           SYS_fchdir,            wali_syscall_fchdir,       "(i)I" ),
	NSYMBOL (           SYS_rename,            wali_syscall_rename,      "(ii)I" ),
	NSYMBOL (            SYS_mkdir,             wali_syscall_mkdir,      "(ii)I" ),
	NSYMBOL (            SYS_rmdir,             wali_syscall_rmdir,       "(i)I" ),
	NSYMBOL (             SYS_link,              wali_syscall_link,      "(ii)I" ),
	NSYMBOL (           SYS_unlink,            wali_syscall_unlink,       "(i)I" ),
	NSYMBOL (          SYS_symlink,           wali_syscall_symlink,      "(ii)I" ),
	NSYMBOL (         SYS_readlink,          wali_syscall_readlink,     "(iii)I" ),
	NSYMBOL (            SYS_chmod,             wali_syscall_chmod,      "(ii)I" ),
	NSYMBOL (           SYS_fchmod,            wali_syscall_fchmod,      "(ii)I" ),
	NSYMBOL (            SYS_chown,             wali_syscall_chown,     "(iii)I" ),
	NSYMBOL (           SYS_fchown,            wali_syscall_fchown,     "(iii)I" ),
	NSYMBOL (            SYS_umask,             wali_syscall_umask,       "(i)I" ),
	NSYMBOL (     SYS_gettimeofday,      wali_syscall_gettimeofday,      "(ii)I" ),
	NSYMBOL (        SYS_getrlimit,         wali_syscall_getrlimit,      "(ii)I" ),
	NSYMBOL (        SYS_getrusage,         wali_syscall_getrusage,      "(ii)I" ),
	NSYMBOL (          SYS_sysinfo,           wali_syscall_sysinfo,       "(i)I" ),
	NSYMBOL (           SYS_getuid,            wali_syscall_getuid,        "()I" ),
	NSYMBOL (           SYS_getgid,            wali_syscall_getgid,        "()I" ),
	NSYMBOL (           SYS_setuid,            wali_syscall_setuid,       "(i)I" ),
	NSYMBOL (           SYS_setgid,            wali_syscall_setgid,       "(i)I" ),
	NSYMBOL (          SYS_geteuid,           wali_syscall_geteuid,        "()I" ),
	NSYMBOL (          SYS_getegid,           wali_syscall_getegid,        "()I" ),
	NSYMBOL (          SYS_setpgid,           wali_syscall_setpgid,      "(ii)I" ),
	NSYMBOL (          SYS_getppid,           wali_syscall_getppid,        "()I" ),
	NSYMBOL (           SYS_setsid,            wali_syscall_setsid,        "()I" ),
	NSYMBOL (         SYS_setreuid,          wali_syscall_setreuid,      "(ii)I" ),
	NSYMBOL (         SYS_setregid,          wali_syscall_setregid,      "(ii)I" ),
	NSYMBOL (        SYS_getgroups,         wali_syscall_getgroups,      "(ii)I" ),
	NSYMBOL (        SYS_setgroups,         wali_syscall_setgroups,      "(ii)I" ),
	NSYMBOL (        SYS_setresuid,         wali_syscall_setresuid,     "(iii)I" ),
	NSYMBOL (        SYS_setresgid,         wali_syscall_setresgid,     "(iii)I" ),
	NSYMBOL (          SYS_getpgid,           wali_syscall_getpgid,       "(i)I" ),
	NSYMBOL (           SYS_getsid,            wali_syscall_getsid,       "(i)I" ),
	NSYMBOL (    SYS_rt_sigpending,     wali_syscall_rt_sigpending,      "(ii)I" ),
	NSYMBOL (    SYS_rt_sigsuspend,     wali_syscall_rt_sigsuspend,      "(ii)I" ),
	NSYMBOL (      SYS_sigaltstack,       wali_syscall_sigaltstack,      "(ii)I" ),
	NSYMBOL (           SYS_statfs,            wali_syscall_statfs,      "(ii)I" ),
	NSYMBOL (          SYS_fstatfs,           wali_syscall_fstatfs,      "(ii)I" ),
	NSYMBOL (            SYS_prctl,             wali_syscall_prctl,   "(iIIII)I" ),
	NSYMBOL (        SYS_setrlimit,         wali_syscall_setrlimit,      "(ii)I" ),
	NSYMBOL (           SYS_chroot,            wali_syscall_chroot,       "(i)I" ),
	NSYMBOL (           SYS_gettid,            wali_syscall_gettid,        "()I" ),
	NSYMBOL (            SYS_tkill,             wali_syscall_tkill,      "(ii)I" ),
	NSYMBOL (            SYS_futex,             wali_syscall_futex,  "(iiiiii)I" ),
	NSYMBOL ( SYS_sched_getaffinity, wali_syscall_sched_getaffinity,     "(iii)I" ),
	NSYMBOL (       SYS_getdents64,        wali_syscall_getdents64,     "(iii)I" ),
	NSYMBOL (  SYS_set_tid_address,   wali_syscall_set_tid_address,       "(i)I" ),
	NSYMBOL (          SYS_fadvise,           wali_syscall_fadvise,    "(iIIi)I" ),
	NSYMBOL (    SYS_clock_gettime,     wali_syscall_clock_gettime,      "(ii)I" ),
	NSYMBOL (     SYS_clock_getres,      wali_syscall_clock_getres,      "(ii)I" ),
	NSYMBOL (  SYS_clock_nanosleep,   wali_syscall_clock_nanosleep,    "(iiii)I" ),
	NSYMBOL (       SYS_exit_group,        wali_syscall_exit_group,       "(i)I" ),
	NSYMBOL (        SYS_epoll_ctl,         wali_syscall_epoll_ctl,    "(iiii)I" ),
	NSYMBOL (           SYS_openat,            wali_syscall_openat,    "(iiii)I" ),
	NSYMBOL (          SYS_mkdirat,           wali_syscall_mkdirat,     "(iii)I" ),
	NSYMBOL (         SYS_fchownat,          wali_syscall_fchownat,   "(iiiii)I" ),
	NSYMBOL (       SYS_newfstatat,        wali_syscall_newfstatat,    "(iiii)I" ),
	NSYMBOL (         SYS_unlinkat,          wali_syscall_unlinkat,     "(iii)I" ),
	NSYMBOL (           SYS_linkat,            wali_syscall_linkat,   "(iiiii)I" ),
	NSYMBOL (        SYS_symlinkat,         wali_syscall_symlinkat,     "(iii)I" ),
	NSYMBOL (       SYS_readlinkat,        wali_syscall_readlinkat,    "(iiii)I" ),
	NSYMBOL (         SYS_fchmodat,          wali_syscall_fchmodat,    "(iiii)I" ),
	NSYMBOL (        SYS_faccessat,         wali_syscall_faccessat,    "(iiii)I" ),
	NSYMBOL (         SYS_pselect6,          wali_syscall_pselect6,  "(iiiiii)I" ),
	NSYMBOL (            SYS_ppoll,             wali_syscall_ppoll,   "(iIiii)I" ),
	NSYMBOL (        SYS_utimensat,         wali_syscall_utimensat,    "(iiii)I" ),
	NSYMBOL (      SYS_epoll_pwait,       wali_syscall_epoll_pwait,  "(iiiiii)I" ),
	NSYMBOL (          SYS_eventfd,           wali_syscall_eventfd,       "(i)I" ),
	NSYMBOL (          SYS_accept4,           wali_syscall_accept4,    "(iiii)I" ),
	NSYMBOL (         SYS_eventfd2,          wali_syscall_eventfd2,      "(ii)I" ),
	NSYMBOL (    SYS_epoll_create1,     wali_syscall_epoll_create1,       "(i)I" ),
	NSYMBOL (             SYS_dup3,              wali_syscall_dup3,     "(iii)I" ),
	NSYMBOL (            SYS_pipe2,             wali_syscall_pipe2,      "(ii)I" ),
	NSYMBOL (        SYS_prlimit64,         wali_syscall_prlimit64,    "(iiii)I" ),
	NSYMBOL (        SYS_renameat2,         wali_syscall_renameat2,   "(iiiii)I" ),
	NSYMBOL (        SYS_getrandom,         wali_syscall_getrandom,     "(iii)I" ),
	NSYMBOL (            SYS_statx,             wali_syscall_statx,   "(iiiii)I" ),
	NSYMBOL (       SYS_faccessat2,        wali_syscall_faccessat2,    "(iiii)I" ),

	/* Auxiliary calls */
	NSYMBOL (               __init,                      wali_init,        "()i" ),
	NSYMBOL (             __deinit,                    wali_deinit,        "()i" ),
	NSYMBOL (          __proc_exit,                 wali_proc_exit,        "(i)" ),
	NSYMBOL (        __cl_get_argc,               wali_cl_get_argc,        "()i" ),
	NSYMBOL (    __cl_get_argv_len,           wali_cl_get_argv_len,       "(i)i" ),
	NSYMBOL (       __cl_copy_argv,              wali_cl_copy_argv,      "(ii)i" ),
	NSYMBOL (   __get_init_envfile,          wali_get_init_envfile,      "(ii)i" ),
	NSYMBOL (  __wasm_thread_spawn,         wali_wasm_thread_spawn,      "(ii)i" ),
	NSYMBOL (            sigsetjmp,                 wali_sigsetjmp,      "(ii)i" ),
	NSYMBOL (              longjmp,                   wali_longjmp,       "(ii)" ),
	NSYMBOL (               setjmp,                    wali_setjmp,       "(i)i" ),
};

uint32
get_libc_wali_export_apis(NativeSymbol **p_libc_wali_apis)
{
    *p_libc_wali_apis = wali_native_symbols;
    return sizeof(wali_native_symbols) / sizeof(NativeSymbol);
}

