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

#include "wali.h"
#include "copy.h"
#include "../interpreter/wasm_runtime.h"
#include "thread_manager.h"

#include "syscall.h"

/* For startup environment */
int wali_app_argc;
char **wali_app_argv;
char *wali_app_env_file;
bool invoked_wali;

/* For process exit functionality */
int64 proc_exit_primary_tid = -1;
bool proc_exit_invoked = false;

/* For WALI syscall stats */
#define MAX_SYSCALLS 500
typedef struct {
    int64_t vt_count;
    int64_t vt_time;
    int64_t nt_count;
    int64_t nt_time;
} sysmetric_t;

static pthread_mutex_t metrics_lock = PTHREAD_MUTEX_INITIALIZER;
sysmetric_t syscall_metrics[MAX_SYSCALLS] = { { 0 } };

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
uint32_t BASE_MEMSIZE = 0;
uint32_t THREAD_ID = 0; // unused atm

/* strace */
int strace = -1;             // -1=no, 0=print to stdout, 1=print to file
FILE *strace_logfile = NULL; // test

inline void
gettime(struct timespec *ts)
{
    clock_gettime(CLOCK_MONOTONIC_RAW, ts);
}
inline void
gettimethread(struct timespec *ts)
{
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, ts);
}
inline int64_t
timediff(struct timespec *tstart, struct timespec *tend)
{
    int64_t timed =
        ((int64_t)tend->tv_sec - (int64_t)tstart->tv_sec) * 1000000000ull
        + ((int64_t)tend->tv_nsec - (int64_t)tstart->tv_nsec);
    return timed;
}

/** Miscellaneous Callbacks **/
wasm_module_inst_t main_mod_inst = NULL;

void
wali_memory_profile_dump(int signo)
{
    wasm_runtime_dump_mem_consumption(
        wasm_runtime_get_exec_env_singleton(main_mod_inst));
}
void
wali_syscall_profile_dump(int signo)
{
    printf("Save WALI syscall metrics\n");
    pthread_mutex_lock(&metrics_lock);
    FILE *f = fopen("wali_syscalls.profile", "w");
    for (int i = 0; i < (MAX_SYSCALLS - 1); i++) {
        fprintf(f, "%ld:%ld/%ld,", syscall_metrics[i].vt_count,
                syscall_metrics[i].nt_time, syscall_metrics[i].vt_time);
    }
    fprintf(f, "%d:%d/%d", 0, 0, 0);
    pthread_mutex_unlock(&metrics_lock);
}

/* Dummy callback to be invoked after termination flags are set
 * for all threads in process */
void
wali_terminate_process_sighandler(int signo)
{
    VB("WALI termination handler called by process %d", getpid());
}
/** **/

/* Startup init */
void
wali_init_native(wasm_module_inst_t module_inst)
{
    if (sem_init(&tid_sem, 0, 0)) {
        perror("sem_init");
    }

    main_mod_inst = module_inst;
    is_multithreaded = false;
    proc_exit_invoked = false;

    // Register signals for profiling / termination
    struct sigaction act = { 0 };
#if WASM_ENABLE_MEMORY_PROFILING
    act.sa_handler = wali_memory_profile_dump;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIG_MEM_PROF, &act, NULL) == -1) {
        perror("Could not install WALI memory prof signal\n");
        exit(1);
    }
#endif
#if WALI_ENABLE_SYSCALL_PROFILE
    act.sa_handler = wali_syscall_profile_dump;
    if (sigaction(SIG_SYSCALL_PROF, &act, NULL) == -1) {
        perror("Could not install WALI syscall prof signal\n");
        exit(1);
    }
#endif

    act.sa_handler = wali_terminate_process_sighandler;
    if (sigaction(SIG_WASM_THREAD_TERM, &act, NULL) == -1) {
        perror("Could not install WALI termination signal\n");
        exit(1);
    }

    NATIVE_PAGESIZE = sysconf(_SC_PAGE_SIZE);
    MMAP_PAGELEN = 0;
    WASM_PAGELEN = 0;
    WASM_TO_NATIVE_PAGE = WASM_PAGESIZE / NATIVE_PAGESIZE;
    // Set in mmap
    BASE_MEMSIZE = 0;
    THREAD_ID = 1;
}

/** Helper methods **/
static uint32_t
get_current_memory_size(wasm_exec_env_t exec_env)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    wasm_function_inst_t memorysize_fn =
        wasm_runtime_lookup_function(module_inst, "__wasm_memory_size");
    uint32_t cur_wasm_pages[1];
    uint32_t mem_size = 0;
    if (memorysize_fn
        && wasm_runtime_call_wasm(exec_env, memorysize_fn, 0, cur_wasm_pages)) {
        // Success
        VB("Used \'__wasm_memory_size\' export for size query");
        mem_size = cur_wasm_pages[0] * WASM_PAGESIZE;
    }
    else {
        // Failure: Fallback to internal implementation
        mem_size = wasm_runtime_get_memory_size(get_module_inst(exec_env));
    }
    return mem_size;
}

/* CURRENTLY UNUSED: WAMR internal API for memory.grow performs
 * additional OS protection when growing memory which may interfere with
 * any mmap specifications */
__attribute__((used)) static void
grow_memory_size(wasm_exec_env_t exec_env, uint32_t inc_wasm_pages)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    wasm_function_inst_t memorygrow_fn =
        wasm_runtime_lookup_function(module_inst, "__wasm_memory_grow");
    uint32_t prev_wasm_pages[1] = { inc_wasm_pages };
    if (memorygrow_fn
        && wasm_runtime_call_wasm(exec_env, memorygrow_fn, 1,
                                  prev_wasm_pages)) {
        // Success
        VB("Used \'__wasm_memory_grow\' export for grow query");
    }
    else {
        // Failure: Fallback to internal implementation
        wasm_enlarge_memory((WASMModuleInstance *)module_inst, inc_wasm_pages,
                            true);
    }
}

/* Get page aligned address after memory to mmap; since base is mapped it's
 * already aligned, and memory data size is a multiple of 64kB but rounding
 * added for safety */
#define PA_ALIGN_MMAP_ADDR()                                                \
    ({                                                                      \
        Addr base = BASE_ADDR();                                            \
        Addr punalign =                                                     \
            base                                                            \
            + wasm_runtime_get_base_memory_size(get_module_inst(exec_env)); \
        long pageoff = (long)(punalign) & (NATIVE_PAGESIZE - 1);            \
        Addr palign = punalign - pageoff;                                   \
        if (pageoff) {                                                      \
            palign += NATIVE_PAGESIZE;                                      \
        }                                                                   \
        palign;                                                             \
    })

static __thread volatile int64_t nsys_exectime = 0;
#if WALI_ENABLE_SYSCALL_PROFILE
int64_t total_wali_time = 0;
#if WALI_ENABLE_NATIVE_SYSCALL_PROFILE
int64_t total_native_time = 0;
#define NATIVE_TIME(code)                               \
    ({                                                  \
        struct timespec nt_tstart = { 0, 0 };           \
        struct timespec nt_tend = { 0, 0 };             \
        gettime(&nt_tstart);                            \
        long rv = code;                                 \
        gettime(&nt_tend);                              \
        nsys_exectime = timediff(&nt_tstart, &nt_tend); \
        rv;                                             \
    })
#else /* WALI_ENABLE_NATIVE_SYSCALL_PROFILE = 0 */
#define NATIVE_TIME(code) code
#endif
#else /* WALI_ENABLE_SYSCALL_PROFILE = 0 */
#define NATIVE_TIME(code) code
#endif

#define __syscall0(n) NATIVE_TIME(__syscall0(n))
#define __syscall1(n, a1) NATIVE_TIME(__syscall1(n, (long)a1))
#define __syscall2(n, a1, a2) NATIVE_TIME(__syscall2(n, (long)a1, (long)a2))
#define __syscall3(n, a1, a2, a3) \
    NATIVE_TIME(__syscall3(n, (long)a1, (long)a2, (long)a3))
#define __syscall4(n, a1, a2, a3, a4) \
    NATIVE_TIME(__syscall4(n, (long)a1, (long)a2, (long)a3, (long)a4))
#define __syscall5(n, a1, a2, a3, a4, a5) \
    NATIVE_TIME(__syscall5(n, (long)a1, (long)a2, (long)a3, (long)a4, (long)a5))
#define __syscall6(n, a1, a2, a3, a4, a5, a6)                         \
    NATIVE_TIME(__syscall6(n, (long)a1, (long)a2, (long)a3, (long)a4, \
                           (long)a5, (long)a6))

#define PC(f) LOG_VERBOSE("[%d] WALI: | " #f, gettid())

#if WALI_ENABLE_SYSCALL_PROFILE

#define SC(nr, f)                         \
    int scno = nr;                        \
    struct timespec vt_tstart = { 0, 0 }; \
    gettime(&vt_tstart);

#define RETURN(v, syscall, num_args, ...)                          \
    {                                                              \
        long frv = v;                                              \
        struct timespec vt_tend = { 0, 0 };                        \
        gettime(&vt_tend);                                         \
        int64_t virtsys_exectime = timediff(&vt_tstart, &vt_tend); \
        pthread_mutex_lock(&metrics_lock);                         \
        syscall_metrics[scno].vt_time +=                           \
            ((virtsys_exectime - syscall_metrics[scno].vt_time)    \
             / (syscall_metrics[scno].vt_count + 1));              \
        syscall_metrics[scno].vt_count++;                          \
        syscall_metrics[scno].nt_time +=                           \
            ((nsys_exectime - syscall_metrics[scno].nt_time)       \
             / (syscall_metrics[scno].nt_count + 1));              \
        syscall_metrics[scno].nt_count++;                          \
        total_native_time += nsys_exectime;                        \
        total_wali_time += (virtsys_exectime - nsys_exectime);     \
        pthread_mutex_unlock(&metrics_lock);                       \
        if (strace == 0 || strace == 1) {                          \
            strace_print(frv, syscall, num_args, __VA_ARGS__);     \
        }                                                          \
        return frv;                                                \
    }

#define FIN_TIME()                                                 \
    {                                                              \
        struct timespec vt_tend = { 0, 0 };                        \
        gettime(&vt_tend);                                         \
        int64_t virtsys_exectime = timediff(&vt_tstart, &vt_tend); \
        pthread_mutex_lock(&metrics_lock);                         \
        syscall_metrics[scno].vt_time +=                           \
            ((virtsys_exectime - syscall_metrics[scno].vt_time)    \
             / (syscall_metrics[scno].vt_count + 1));              \
        syscall_metrics[scno].vt_count++;                          \
        syscall_metrics[scno].nt_time +=                           \
            ((nsys_exectime - syscall_metrics[scno].nt_time)       \
             / (syscall_metrics[scno].nt_count + 1));              \
        syscall_metrics[scno].nt_count++;                          \
        pthread_mutex_unlock(&metrics_lock);                       \
    }

#else /* WALI_ENABLE_SYSCALL_PROFILE = 0 */

/* We can return -1 within if process exit since CHECK_SUSPEND will trigger
 * before any future call into WALI */
#define SC(nr, f)                                     \
    {                                                 \
        LOG_VERBOSE("[%d] WALI: SC | " #f, gettid()); \
        if (proc_exit_invoked) {                      \
            wali_thread_exit(exec_env, 0);            \
            return -1;                                \
        }                                             \
    }

#define RETURN(v, syscall, num_args, ...)                      \
    {                                                          \
        long res = v;                                          \
        if (proc_exit_invoked) {                               \
            wali_thread_exit(exec_env, 0);                     \
        }                                                      \
        if (strace == 0 || strace == 1) {                      \
            strace_print(res, syscall, num_args, __VA_ARGS__); \
        }                                                      \
        return res;                                            \
    }

#endif /* end of WALI_ENABLE_SYSCALL_PROFILE */

#define ERRSC(f, ...)                                               \
    {                                                               \
        LOG_ERROR("[%d] WALI: SC \"" #f                             \
                  "\" not implemented correctly yet! " __VA_ARGS__, \
                  gettid());                                        \
    }
#define FATALSC(f, ...)                                                 \
    {                                                                   \
        LOG_FATAL("[%d] WALI: SC \"" #f "\" fatal error! " __VA_ARGS__, \
                  gettid());                                            \
    }
#define MISSC(f, ...)                                            \
    {                                                            \
        LOG_FATAL("[%d] WALI: SC \"" #f                          \
                  "\" fatal error! No such syscall on platform", \
                  gettid());                                     \
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

// 0
long
wali_syscall_read(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(0, read);
    RETURN(__syscall3(SYS_read, a1, MADDR(a2), a3), "read", 3, a1, a2, a3);
}

// 1
long
wali_syscall_write(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(1, write);
    RETURN(__syscall3(SYS_write, a1, MADDR(a2), a3), "write", 3, a1, a2, a3);
}

// 2
long
wali_syscall_open(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(2, open);
#if __x86_64__
    RETURN(__syscall3(SYS_open, MADDR(a1), a2, a3), "open", 3, a1, a2, a3);
#elif __aarch64__ || __riscv64__
    RETURN(wali_syscall_openat(exec_env, AT_FDCWD, a1, a2, a3), "open", 3, a1,
           a2, a3);
#endif
}

// 3
long
wali_syscall_close(wasm_exec_env_t exec_env, long a1)
{
    SC(3, close);
    RETURN(__syscall1(SYS_close, a1), "close", 1, a1);
}

// 4
long
wali_syscall_stat(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(4, stat);
#if __x86_64__
    RETURN(__syscall2(SYS_stat, MADDR(a1), MADDR(a2)), "stat", 2, a1, a2);
#elif __aarch64__ || __riscv64__
    RETURN(wali_syscall_fstatat(exec_env, AT_FDCWD, a1, a2, 0), "stat", 2, a1,
           a2);
#endif
}

// 5
long
wali_syscall_fstat(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(5, fstat);
    RETURN(__syscall2(SYS_fstat, a1, MADDR(a2)), "fstat", 2, a1, a2);
}

// 6
long
wali_syscall_lstat(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(6, lstat);
#if __x86_64__
    RETURN(__syscall2(SYS_lstat, MADDR(a1), MADDR(a2)), "lstat", 2, a1, a2);
#elif __aarch64__ || __riscv64__
    RETURN(
        wali_syscall_fstatat(exec_env, AT_FDCWD, a1, a2, AT_SYMLINK_NOFOLLOW),
        "lstat", 2, a1, a2);
#endif
}

#define CONV_TIME_TO_TS(x)                                              \
    ((x >= 0) ? &((struct timespec){ .tv_sec = x / 1000,                \
                                     .tv_nsec = (x % 1000) * 1000000 }) \
              : 0)
// 7
long
wali_syscall_poll(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(7, poll);
#if __x86_64__
    RETURN(__syscall3(SYS_poll, MADDR(a1), a2, a3), "poll", 3, a1, a2, a3);
#elif __aarch64__ || __riscv64__
    RETURN(wali_syscall_ppoll_aliased(exec_env, a1, a2,
                                      (long)CONV_TIME_TO_TS(a3), 0, _NSIG / 8),
           "poll", 3, a1, a2, a3);
#endif
}

// 8
long
wali_syscall_lseek(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(8, lseek);
    RETURN(__syscall3(SYS_lseek, a1, a2, a3), "lseek", 3, a1, a2, a3);
}

// 9
long
wali_syscall_mmap(wasm_exec_env_t exec_env, long a1, long a2, long a3, long a4,
                  long a5, long a6)
{
    SC(9, mmap);
    VB("mmap args | a1: %ld, a2: 0x%x, a3: %ld, a4: %ld, a5: %ld, a6: %ld | "
       "MMAP_PAGELEN: %d",
       a1, a2, a3, a4, a5, a6, MMAP_PAGELEN);
    wasm_module_inst_t module_inst = get_module_inst(exec_env);

    pthread_mutex_lock(&mmap_lock);
    Addr base_addr = BASE_ADDR();
    Addr pa_aligned_addr = PA_ALIGN_MMAP_ADDR();
    Addr mmap_addr = pa_aligned_addr + MMAP_PAGELEN * NATIVE_PAGESIZE;

    /* Get current memory size */
    uint32_t mem_size = get_current_memory_size(exec_env);
    VB("Mem Base: %p | Mem End: %p | Mem Size: 0x%x | Mmap Addr: %p", base_addr,
       base_addr + mem_size, mem_size, mmap_addr);

    /* Check if wasm memory needs to be expanded and it is safe */
    int inc_wasm_pages = 0;
    int num_pages = ((a2 + NATIVE_PAGESIZE - 1) / NATIVE_PAGESIZE);
    int extended_mmap_pagelen = MMAP_PAGELEN + num_pages;
    if (extended_mmap_pagelen > WASM_PAGELEN * WASM_TO_NATIVE_PAGE) {
        int new_wasm_pagelen =
            ((extended_mmap_pagelen + WASM_TO_NATIVE_PAGE - 1)
             / WASM_TO_NATIVE_PAGE);
        inc_wasm_pages = new_wasm_pagelen - WASM_PAGELEN;
        if (!wasm_can_enlarge_memory((WASMModuleInstance *)module_inst,
                                     inc_wasm_pages)) {
            FATALSC(mmap, "Out of memory!\n");
            goto mmap_fail;
        }
    }

    Addr mem_addr =
        (Addr)__syscall6(SYS_mmap, mmap_addr, a2, a3, MAP_FIXED | a4, a5, a6);
    /* Sometimes mmap returns -9 instead of MAP_FAILED? */
    if ((mem_addr == MAP_FAILED) || (mem_addr == (void *)(-9))) {
        FATALSC(mmap, "Failed to mmap!\n");
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
    long retval = WADDR(mem_addr);
    VB("New MMAP Pagelen: %d", MMAP_PAGELEN);
    pthread_mutex_unlock(&mmap_lock);
    VB("Ret Addr: 0x%x\n", retval);
    RETURN(retval, "mmap", 6, a1, a2, a3, a4, a5, a6);

mmap_fail:
    pthread_mutex_unlock(&mmap_lock);
    RETURN((long)MAP_FAILED, "mmap", 6, a1, a2, a3, a4, a5, a6);
}

// 10
long
wali_syscall_mprotect(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(10, mprotect);
    RETURN(__syscall3(SYS_mprotect, MADDR(a1), a2, a3), "mprotect", 3, a1, a2,
           a3);
}

// 11
long
wali_syscall_munmap(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(11, munmap);
    pthread_mutex_lock(&mmap_lock);
    Addr mmap_addr = MADDR(a1);
    Addr mmap_addr_end = (Addr)(mmap_addr + a2);
    /* Reclaim some mmap space if end region is unmapped */
    Addr pa_aligned_addr = PA_ALIGN_MMAP_ADDR();
    int end_page = (mmap_addr_end - pa_aligned_addr + NATIVE_PAGESIZE - 1)
                   / NATIVE_PAGESIZE;
    VB("End page: %d | MMAP_PAGELEN: %d", end_page, MMAP_PAGELEN);
    if (end_page == MMAP_PAGELEN) {
        MMAP_PAGELEN -= ((a2 + NATIVE_PAGESIZE - 1) / NATIVE_PAGESIZE);
        VB("End page unmapped | New MMAP_PAGELEN: %d", MMAP_PAGELEN);
    }
    pthread_mutex_unlock(&mmap_lock);
    RETURN(__syscall2(SYS_munmap, mmap_addr, a2), "munmap", 2, a1, a2);
}

// 12
long
wali_syscall_brk(wasm_exec_env_t exec_env, long a1)
{
    SC(12, brk);
    VB("brk syscall is a NOP in WASM");
    RETURN(0, "brk", 1, a1);
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
wali_syscall_rt_sigaction(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                          long a4)
{
    SC(13, rt_sigaction);
    VB("rt_sigaction args | a1: %ld, a2: %ld, a3: %ld, a4: %ld", a1, a2, a3,
       a4);
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    int signo = a1;
    Addr wasm_act = MADDR(a2);
    Addr wasm_oldact = MADDR(a3);
    struct k_sigaction act = { 0 };
    struct k_sigaction oldact = { 0 };

    /* Block signal manipulation while setting up synchronized wali table */
    pthread_mutex_lock(&sigtable_mut);
    FuncPtr_t target_wasm_funcptr = 0;
    char sigtype[30];

    /* Prepare for native signal syscall */
    struct k_sigaction *act_pt =
        copy_ksigaction(exec_env, wasm_act, &act, sa_handler_wali,
                        &target_wasm_funcptr, sigtype);
    struct k_sigaction *oldact_pt = wasm_oldact ? &oldact : NULL;
    long retval = __syscall4(SYS_rt_sigaction, a1, act_pt, oldact_pt, a4);

    VB("Signal Registration -- \'%s\'(%d) | Sigtype: %s", strsignal(a1), signo,
       sigtype);

    /* Register virtual signal in WALI sigtable
     * ---------------------------------------------------------------
     * | Handler value | WALI sigtable (set)     | Old Action (get)  |
     * ---------------------------------------------------------------
     * | SIG_DFL       | No register             | WASM_SIG_DFL      |
     * | SIG_IGN       | No register             | WASM_SIG_IGN      |
     * | SIG_ERR       |     -                   | WASM_SIG_ERR      |
     * | FuncPtr_t     | Table[FuncPtr_t]        | Table[FuncPtr_t]  |
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
                                      ? FUNC_IDX(wali_sigtable[signo].function)
                                      : 0;
            uint32_t new_fn_idx =
                target_wasm_handler ? FUNC_IDX(target_wasm_handler) : 0;
            VB("Replacing target handler: Fn[%u] -> Fn[%u]\n", old_fn_idx,
               new_fn_idx);
            FUNC_FREE(wali_sigtable[signo].function);
            wali_sigtable[signo].function = target_wasm_handler;
            wali_sigtable[signo].func_table_idx = target_wasm_funcptr;
            wali_sigtable[signo].func_idx = new_fn_idx;
        }
    }
    /* Reset block signals */
    pthread_mutex_unlock(&sigtable_mut);
    RETURN(retval, "rt_sigaction", 4, a1, a2, a3, a4);
}

// 14
long
wali_syscall_rt_sigprocmask(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                            long a4)
{
    SC(14, rt_sigprocmask);
    RETURN(__syscall4(SYS_rt_sigprocmask, a1, MADDR(a2), MADDR(a3), a4),
           "rt_sigprocmask", 4, a1, a2, a3, a4);
}

// 15: Never directly called; __libc_restore_rt is called by OS
long
wali_syscall_rt_sigreturn(wasm_exec_env_t exec_env, long a1)
{
    SC(15, rt_sigreturn);
    ERRSC(rt_sigreturn, "rt_sigreturn should never be called by the user!");
    RETURN(-1, "rt_sigreturn", 1, a1);
}

// 16
long
wali_syscall_ioctl(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(16, ioctl);
    RETURN(__syscall3(SYS_ioctl, a1, a2, MADDR(a3)), "ioctl", 3, a1, a2, a3);
}

// 17
long
wali_syscall_pread64(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                     long a4)
{
    SC(17, pread64);
    RETURN(__syscall4(SYS_pread64, a1, MADDR(a2), a3, a4), "pread64", 4, a1, a2,
           a3, a4);
}

// 18
long
wali_syscall_pwrite64(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                      long a4)
{
    SC(18, pwrite64);
    RETURN(__syscall4(SYS_pwrite64, a1, MADDR(a2), a3, a4), "pwrite64", 4, a1,
           a2, a3, a4);
}

// 19
long
wali_syscall_readv(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(19, readv);
    Addr wasm_iov = MADDR(a2);
    int iov_cnt = a3;

    struct iovec *native_iov = copy_iovec(exec_env, wasm_iov, iov_cnt);
    long retval = __syscall3(SYS_readv, a1, native_iov, a3);
    free(native_iov);

    RETURN(retval, "readv", 3, a1, a2, a3);
}

// 20
long
wali_syscall_writev(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(20, writev);
    Addr wasm_iov = MADDR(a2);
    int iov_cnt = a3;

    struct iovec *native_iov = copy_iovec(exec_env, wasm_iov, iov_cnt);
    long retval = __syscall3(SYS_writev, a1, native_iov, a3);
    free(native_iov);
    RETURN(retval, "writev", 3, a1, a2, a3);
}

// 21
long
wali_syscall_access(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(21, access);
#if __x86_64__
    RETURN(__syscall2(SYS_access, MADDR(a1), a2), "access", 2, a1, a2);
#elif __aarch64__ || __riscv64__
    RETURN(wali_syscall_faccessat(exec_env, AT_FDCWD, a1, a2, 0), "access", 2,
           a1, a2);
#endif
}

// 22
long
wali_syscall_pipe(wasm_exec_env_t exec_env, long a1)
{
    SC(22, pipe);
#if __x86_64__
    RETURN(__syscall1(SYS_pipe, MADDR(a1)), "pipe", 1, a1);
#elif __aarch64__ || __riscv64__
    RETURN(wali_syscall_pipe2(exec_env, a1, 0), "pipe", 1, a1);
#endif
}

// 23
long
wali_syscall_select(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                    long a4, long a5)
{
    SC(23, select);
#if __x86_64__
    RETURN(
        __syscall5(SYS_select, a1, MADDR(a2), MADDR(a3), MADDR(a4), MADDR(a5)),
        "select", 5, a1, a2, a3, a4, a5);
#elif __aarch64__ || __riscv64__
    RETURN(wali_syscall_pselect6(exec_env, a1, a2, a3, a4, a5,
                                 (long)((long[]){ 0, _NSIG / 8 })),
           "select", 5, a1, a2, a3, a4, a5);
#endif
}

// 24
long
wali_syscall_sched_yield(wasm_exec_env_t exec_env)
{
    SC(24, sched_yield);
    RETURN(__syscall0(SYS_sched_yield), "sched_yield", 0, 0);
}

// 25
long
wali_syscall_mremap(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                    long a4, long a5)
{
    SC(25, mremap);
    VB("mremap args | a1: %ld, a2: 0x%x, a3: 0x%x, a4: %ld, a5: %ld | "
       "MMAP_PAGELEN: %d",
       a1, a2, a3, a4, a5, MMAP_PAGELEN);
    wasm_module_inst_t module_inst = get_module_inst(exec_env);

    /* Remap pages to the end of the wasm memory, like mmap */
    pthread_mutex_lock(&mmap_lock);
    Addr base_addr = BASE_ADDR();
    Addr pa_aligned_addr = PA_ALIGN_MMAP_ADDR();
    Addr mmap_addr = pa_aligned_addr + MMAP_PAGELEN * NATIVE_PAGESIZE;

    uint32 mem_size = get_current_memory_size(exec_env);
    VB("Mem Base: %p | Mem End: %p | Mem Size: 0x%x | Mmap Addr: %p", base_addr,
       base_addr + mem_size, mem_size, mmap_addr);

    /* Check if wasm memory needs to be expanded and it is safe */
    int inc_wasm_pages = 0;
    int num_pages = ((a3 + NATIVE_PAGESIZE - 1) / NATIVE_PAGESIZE);
    int extended_mmap_pagelen = MMAP_PAGELEN + num_pages;
    if (extended_mmap_pagelen > WASM_PAGELEN * WASM_TO_NATIVE_PAGE) {
        int new_wasm_pagelen =
            ((extended_mmap_pagelen + WASM_TO_NATIVE_PAGE - 1)
             / WASM_TO_NATIVE_PAGE);
        inc_wasm_pages = new_wasm_pagelen - WASM_PAGELEN;
        if (!wasm_can_enlarge_memory((WASMModuleInstance *)module_inst,
                                     inc_wasm_pages)) {
            FATALSC(mremap, "Out of memory!\n");
            goto mremap_fail;
        }
    }

    Addr mem_addr = (Addr)__syscall5(SYS_mremap, MADDR(a1), a2, a3,
                                     MREMAP_MAYMOVE | MREMAP_FIXED, mmap_addr);
    VB("Mem Addr: %p\n", mem_addr);
    /* Sometimes mremap returns -9 instead of MAP_FAILED? */
    if ((mem_addr == MAP_FAILED) || (mem_addr == (void *)(-9))) {
        FATALSC(mremap, "Failed to mremap!\n");
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
    long retval = WADDR(mem_addr);
    VB("New MMAP Pagelen: %d\n", MMAP_PAGELEN);
    pthread_mutex_unlock(&mmap_lock);
    VB("Ret Addr: 0x%x", retval);
    RETURN(retval, "mremap", 5, a1, a2, a3, a4, a5);

mremap_fail:
    pthread_mutex_unlock(&mmap_lock);
    RETURN((long)MAP_FAILED, "mremap", 5, a1, a2, a3, a4, a5);
}

// 26
long
wali_syscall_msync(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(26, msync);
    RETURN(__syscall3(SYS_msync, MADDR(a1), a2, a3), "msync", 3, a1, a2, a3);
}

// 28
long
wali_syscall_madvise(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(28, madvise);
    RETURN(__syscall3(SYS_madvise, MADDR(a1), a2, a3), "madvise", 3, a1, a2,
           a3);
}

// 32
long
wali_syscall_dup(wasm_exec_env_t exec_env, long a1)
{
    SC(32, dup);
    RETURN(__syscall1(SYS_dup, a1), "dup", 1, a1);
}

// 33
long
wali_syscall_dup2(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(33, dup2);
#if __x86_64__
    RETURN(__syscall2(SYS_dup2, a1, a2), "dup2", 2, a1, a2);
#elif __aarch64__ || __riscv64__
    /* Dup2 returns newfd while dup3 throws error, handle with case below */
    if (a1 == a2) {
        long r = wali_syscall_fcntl(exec_env, a1, F_GETFD, 0);
        RETURN((r >= 0) ? a2 : r, "dup2", 2, a1, a2);
    }
    else {
        RETURN(wali_syscall_dup3(exec_env, a1, a2, 0), "dup2", 2, a1, a2);
    }
#endif
}

// 35
long
wali_syscall_nanosleep(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(35, nanosleep);
    RETURN(__syscall2(SYS_nanosleep, MADDR(a1), MADDR(a2)), "nanosleep", 2, a1,
           a2);
}

// 37
long
wali_syscall_alarm(wasm_exec_env_t exec_env, long a1)
{
    SC(37, alarm);
#if __x86_64__
    RETURN(__syscall1(SYS_alarm, a1), "alarm", 1, a1);
#elif __aarch64__ || __riscv64__
    MISSC(alarm);
    wali_proc_exit(exec_env, 1);
#endif
}

// 38
long
wali_syscall_setitimer(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(38, setitimer);
    RETURN(__syscall3(SYS_setitimer, a1, MADDR(a2), MADDR(a3)), "setitimer", 3,
           a1, a2, a3);
}

// 39
long
wali_syscall_getpid(wasm_exec_env_t exec_env)
{
    SC(39, getpid);
    RETURN(__syscall0(SYS_getpid), "getpid", 0, 0);
}

// 41
long
wali_syscall_socket(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(41, socket);
    RETURN(__syscall3(SYS_socket, a1, a2, a3), "socket", 3, a1, a2, a3);
}

// 42
long
wali_syscall_connect(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(42, connect);
    RETURN(__syscall3(SYS_connect, a1, MADDR(a2), a3), "connect", 3, a1, a2,
           a3);
}

// 43
long
wali_syscall_accept(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(43, accept);
    RETURN(__syscall3(SYS_accept, a1, MADDR(a2), MADDR(a3)), "accept", 3, a1,
           a2, a3);
}

// 44
long
wali_syscall_sendto(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                    long a4, long a5, long a6)
{
    SC(44, sendto);
    RETURN(__syscall6(SYS_sendto, a1, MADDR(a2), a3, a4, MADDR(a5), a6),
           "sendto", 6, a1, a2, a3, a4, a5, a6);
}

// 45
long
wali_syscall_recvfrom(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                      long a4, long a5, long a6)
{
    SC(45, recvfrom);
    RETURN(
        __syscall6(SYS_recvfrom, a1, MADDR(a2), a3, a4, MADDR(a5), MADDR(a6)),
        "recvfrom", 6, a1, a2, a3, a4, a5, a6);
}

// 46
long
wali_syscall_sendmsg(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(46, sendmsg);
    Addr wasm_msghdr = MADDR(a2);
    struct msghdr *native_msghdr = copy_msghdr(exec_env, wasm_msghdr);
    long retval = __syscall3(SYS_sendmsg, a1, native_msghdr, a3);
    free(native_msghdr);
    RETURN(retval, "sendmsg", 3, a1, a2, a3);
}

// 47
long
wali_syscall_recvmsg(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(47, recvmsg);
    Addr wasm_msghdr = MADDR(a2);
    struct msghdr *native_msghdr = copy_msghdr(exec_env, wasm_msghdr);
    long retval = __syscall3(SYS_recvmsg, a1, native_msghdr, a3);
    free(native_msghdr);
    RETURN(retval, "recvmsg", 3, a1, a2, a3);
}

// 48
long
wali_syscall_shutdown(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(48, shutdown);
    RETURN(__syscall2(SYS_shutdown, a1, a2), "shutdown", 2, a1, a2);
}

// 49
long
wali_syscall_bind(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(49, bind);
    RETURN(__syscall3(SYS_bind, a1, MADDR(a2), a3), "bind", 3, a1, a2, a3);
}

// 50
long
wali_syscall_listen(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(50, listen);
    RETURN(__syscall2(SYS_listen, a1, a2), "listen", 2, a1, a2);
}

// 51
long
wali_syscall_getsockname(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(51, getsockname);
    RETURN(__syscall3(SYS_getsockname, a1, MADDR(a2), MADDR(a3)), "getsockname",
           3, a1, a2, a3);
}

// 52
long
wali_syscall_getpeername(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(52, getpeername);
    RETURN(__syscall3(SYS_getpeername, a1, MADDR(a2), MADDR(a3)), "getpeername",
           3, a1, a2, a3);
}

// 53
long
wali_syscall_socketpair(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                        long a4)
{
    SC(53, socketpair);
    RETURN(__syscall4(SYS_socketpair, a1, a2, a3, MADDR(a4)), "socketpair", 4,
           a1, a2, a3, a4);
}

// 54
long
wali_syscall_setsockopt(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                        long a4, long a5)
{
    SC(54, setsockopt);
    RETURN(__syscall5(SYS_setsockopt, a1, a2, a3, MADDR(a4), a5), "setsockopt",
           5, a1, a2, a3, a4, a5);
}

// 55
long
wali_syscall_getsockopt(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                        long a4, long a5)
{
    SC(55, getsockopt);
    RETURN(__syscall5(SYS_getsockopt, a1, a2, a3, MADDR(a4), MADDR(a5)),
           "getsockopt", 5, a1, a2, a3, a4, a5);
}

// 57
long
wali_syscall_fork(wasm_exec_env_t exec_env)
{
    SC(57, fork);
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
wali_syscall_execve(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(59, execve);
    VB("Execve string: %s\n", MADDR(a1));
    char **argv = copy_stringarr(exec_env, MADDR(a2));
    char **argpt = argv;
    int i = 0;
    while (*argpt != NULL) {
        VB("Argv[%d] : %s\n", i, *argpt);
        argpt++;
        i++;
    }
    char **envp = copy_stringarr(exec_env, MADDR(a3));
    /* For child WALI processes: Pass env through temporary file-descriptor that
     * is read on init For child native processes: envp is passed through the
     * syscall invocation */
    if (envp) {
        create_pass_env_file(envp);
    }
    long retval = __syscall3(SYS_execve, MADDR(a1), argv, envp);
    free(argv);
    free(envp);
    RETURN(retval, "execve", 3, a1, a2, a3);
}

// 60 TODO
long
wali_syscall_exit(wasm_exec_env_t exec_env, long a1)
{
    SC(60, exit);
    if (is_multithreaded) {
        ERRSC(exit,
              "Program detected as multithreaded; exiting current thread but "
              "cannot guarantee process-level exit code generation. It is "
              "recommended to use exit_group instead");
        wali_thread_exit(exec_env, a1);
    }
    else {
        ERRSC(exit, "Program detected as single-threaded; invoking proc_exit "
                    "to exit program");
        wali_proc_exit(exec_env, a1);
    }
    RETURN(0, "exit", 1, a1);
}

// 61
long
wali_syscall_wait4(wasm_exec_env_t exec_env, long a1, long a2, long a3, long a4)
{
    SC(61, wait4);
    RETURN(__syscall4(SYS_wait4, a1, MADDR(a2), a3, MADDR(a4)), "wait4", 4, a1,
           a2, a3, a4);
}

// 62
long
wali_syscall_kill(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(62, kill);
    RETURN(__syscall2(SYS_kill, a1, a2), "kill", 2, a1, a2);
}

// 63
long
wali_syscall_uname(wasm_exec_env_t exec_env, long a1)
{
    SC(63, uname);
    RETURN(__syscall1(SYS_uname, MADDR(a1)), "uname", 1, a1);
}

// 72
long
wali_syscall_fcntl(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(72, fcntl);
    /* Swap open flags only on F_GETFL and F_SETFL mode for aarch64 */
    switch (a2) {
#if __aarch64__
        case F_GETFL:
            RETURN(swap_open_flags(__syscall3(SYS_fcntl, a1, a2, a3)), "fcntl",
                   3, a1, a2, a3);
        case F_SETFL:
            RETURN(__syscall3(SYS_fcntl, a1, a2, swap_open_flags(a3)), "fcntl",
                   3, a1, a2, a3);
#endif
        case F_GETLK:
        case F_SETLK:
        case F_GETOWN_EX:
        case F_SETOWN_EX:
            RETURN(__syscall3(SYS_fcntl, a1, a2, MADDR(a3)), "fcntl", 3, a1, a2,
                   a3);
        default:
            RETURN(__syscall3(SYS_fcntl, a1, a2, a3), "fcntl", 3, a1, a2, a3);
    }
}

// 73
long
wali_syscall_flock(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(73, flock);
    RETURN(__syscall2(SYS_flock, a1, a2), "flock", 2, a1, a2);
}

// 74
long
wali_syscall_fsync(wasm_exec_env_t exec_env, long a1)
{
    SC(74, fsync);
    RETURN(__syscall1(SYS_fsync, a1), "fsync", 1, a1);
}

// 75
long
wali_syscall_fdatasync(wasm_exec_env_t exec_env, long a1)
{
    SC(75, fdatasync);
    RETURN(__syscall1(SYS_fdatasync, a1), "fdatasync", 1, a1);
}

// 77
long
wali_syscall_ftruncate(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(77, ftruncate);
    RETURN(__syscall2(SYS_ftruncate, a1, a2), "ftruncate", 2, a1, a2);
}

// 78
long
wali_syscall_getdents(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(78, getdents);
    FATALSC(getdents, "Not going to support this legacy call; use getdents64");
#if __x86_64__
    RETURN(__syscall3(SYS_getdents, a1, MADDR(a2), a3), "getdents", 3, a1, a2,
           a3);
#elif __aarch64__ || __riscv64__
    RETURN(-1, "getdents", 3, a1, a2, a3);
#endif
}

// 79
long
wali_syscall_getcwd(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(79, getcwd);
    RETURN(__syscall2(SYS_getcwd, MADDR(a1), a2), "getcwd", 2, a1, a2);
}

// 80
long
wali_syscall_chdir(wasm_exec_env_t exec_env, long a1)
{
    SC(80, chdir);
    RETURN(__syscall1(SYS_chdir, MADDR(a1)), "chdir", 1, a1);
}

// 81
long
wali_syscall_fchdir(wasm_exec_env_t exec_env, long a1)
{
    SC(81, fchdir);
    RETURN(__syscall1(SYS_fchdir, a1), "fchdir", 1, a1);
}

// 82
long
wali_syscall_rename(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(82, rename);
#if __x86_64__
    RETURN(__syscall2(SYS_rename, MADDR(a1), MADDR(a2)), "rename", 2, a1, a2);
#elif __aarch64__ || __riscv64__
    RETURN(wali_syscall_renameat2(exec_env, AT_FDCWD, a1, AT_FDCWD, a2, 0),
           "rename", 2, a1, a2);
#endif
}

// 83
long
wali_syscall_mkdir(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(83, mkdir);
#if __x86_64__
    RETURN(__syscall2(SYS_mkdir, MADDR(a1), a2), "mkdir", 2, a1, a2);
#elif __aarch64__ || __riscv64__
    RETURN(wali_syscall_mkdirat(exec_env, AT_FDCWD, a1, a2), "mkdir", 2, a1,
           a2);
#endif
}

// 84
long
wali_syscall_rmdir(wasm_exec_env_t exec_env, long a1)
{
    SC(84, rmdir);
#if __x86_64__
    RETURN(__syscall1(SYS_rmdir, MADDR(a1)), "rmdir", 1, a1);
#elif __aarch64__ || __riscv64__
    RETURN(wali_syscall_unlinkat(exec_env, AT_FDCWD, a1, AT_REMOVEDIR), "rmdir",
           1, a1);
#endif
}

// 86
long
wali_syscall_link(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(86, link);
#if __x86_64__
    RETURN(__syscall2(SYS_link, MADDR(a1), MADDR(a2)), "link", 2, a1, a2);
#elif __aarch64__ || __riscv64__
    RETURN(wali_syscall_linkat(exec_env, AT_FDCWD, a1, AT_FDCWD, a2, 0), "link",
           2, a1, a2);
#endif
}

// 87
long
wali_syscall_unlink(wasm_exec_env_t exec_env, long a1)
{
    SC(87, unlink);
#if __x86_64__
    RETURN(__syscall1(SYS_unlink, MADDR(a1)), "unlink", 1, a1);
#elif __aarch64__ || __riscv64__
    RETURN(wali_syscall_unlinkat(exec_env, AT_FDCWD, a1, 0), "unlink", 1, a1);
#endif
}

// 88
long
wali_syscall_symlink(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(88, symlink);
#if __x86_64__
    RETURN(__syscall2(SYS_symlink, MADDR(a1), MADDR(a2)), "symlink", 2, a1, a2);
#elif __aarch64__ || __riscv64__
    RETURN(wali_syscall_symlinkat(exec_env, a1, AT_FDCWD, a2), "symlink", 2, a1,
           a2);
#endif
}

// 89
long
wali_syscall_readlink(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(89, readlink);
#if __x86_64__
    RETURN(__syscall3(SYS_readlink, MADDR(a1), MADDR(a2), a3), "readlink", 3,
           a1, a2, a3);
#elif __aarch64__ || __riscv64__
    RETURN(wali_syscall_readlinkat(exec_env, AT_FDCWD, a1, a2, a3), "readlink",
           3, a1, a2, a3);
#endif
}

// 90
long
wali_syscall_chmod(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(90, chmod);
#if __x86_64__
    RETURN(__syscall2(SYS_chmod, MADDR(a1), a2), "chmod", 2, a1, a2);
#elif __aarch64__ || __riscv64__
    RETURN(wali_syscall_fchmodat(exec_env, AT_FDCWD, a1, a2, 0), "chmod", 2, a1,
           a2);
#endif
}

// 91
long
wali_syscall_fchmod(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(91, fchmod);
    RETURN(__syscall2(SYS_fchmod, a1, a2), "fchmod", 2, a1, a2);
}

// 92
long
wali_syscall_chown(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(92, chown);
#if __x86_64__
    RETURN(__syscall3(SYS_chown, MADDR(a1), a2, a3), "chown", 3, a1, a2, a3);
#elif __aarch64__ || __riscv64__
    RETURN(wali_syscall_fchownat(exec_env, AT_FDCWD, a1, a2, a3, 0), "chown", 3,
           a1, a2, a3);
#endif
}

// 93
long
wali_syscall_fchown(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(93, fchown);
    RETURN(__syscall3(SYS_fchown, a1, a2, a3), "fchown", 3, a1, a2, a3);
}

// 95
long
wali_syscall_umask(wasm_exec_env_t exec_env, long a1)
{
    SC(95, umask);
    RETURN(__syscall1(SYS_umask, a1), "umask", 1, a1);
}

// 97
long
wali_syscall_getrlimit(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(97, getrlimit);
    RETURN(__syscall2(SYS_getrlimit, a1, MADDR(a2)), "getrlimit", 2, a1, a2);
}

// 98
long
wali_syscall_getrusage(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(98, getrusage);
    RETURN(__syscall2(SYS_getrusage, a1, MADDR(a2)), "getrusage", 2, a1, a2);
}

// 99
long
wali_syscall_sysinfo(wasm_exec_env_t exec_env, long a1)
{
    SC(99, sysinfo);
    RETURN(__syscall1(SYS_sysinfo, MADDR(a1)), "sysinfo", 1, a1);
}

// 102
long
wali_syscall_getuid(wasm_exec_env_t exec_env)
{
    SC(102, getuid);
    RETURN(__syscall0(SYS_getuid), "getuid", 0, 0);
}

// 104
long
wali_syscall_getgid(wasm_exec_env_t exec_env)
{
    SC(104, getgid);
    RETURN(__syscall0(SYS_getgid), "getgid", 0, 0);
}

// 105
long
wali_syscall_setuid(wasm_exec_env_t exec_env, long a1)
{
    SC(105, setuid);
    RETURN(__syscall1(SYS_setuid, a1), "setuid", 1, a1);
}

// 106
long
wali_syscall_setgid(wasm_exec_env_t exec_env, long a1)
{
    SC(106, setgid);
    RETURN(__syscall1(SYS_setgid, a1), "setgid", 1, a1);
}

// 107
long
wali_syscall_geteuid(wasm_exec_env_t exec_env)
{
    SC(107, geteuid);
    RETURN(__syscall0(SYS_geteuid), "geteuid", 0, 0);
}

// 108
long
wali_syscall_getegid(wasm_exec_env_t exec_env)
{
    SC(108, getegid);
    RETURN(__syscall0(SYS_getegid), "getegid", 0, 0);
}

// 109
long
wali_syscall_setpgid(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(109, setpgid);
    RETURN(__syscall2(SYS_setpgid, a1, a2), "setpgid", 2, a1, a2);
}

// 110
long
wali_syscall_getppid(wasm_exec_env_t exec_env)
{
    SC(110, getppid);
    RETURN(__syscall0(SYS_getppid), "getppid", 0, 0);
}

// 112
long
wali_syscall_setsid(wasm_exec_env_t exec_env)
{
    SC(112, setsid);
    RETURN(__syscall0(SYS_setsid), "setsid", 0, 0);
}

// 113
long
wali_syscall_setreuid(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(113, setreuid);
    RETURN(__syscall2(SYS_setreuid, a1, a2), "setreuid", 2, a1, a2);
}

// 114
long
wali_syscall_setregid(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(114, setregid);
    RETURN(__syscall2(SYS_setregid, a1, a2), "setregid", 2, a1, a2);
}

// 115
long
wali_syscall_getgroups(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(115, getgroups);
    RETURN(__syscall2(SYS_getgroups, a1, MADDR(a2)), "getgroups", 2, a1, a2);
}

// 116
long
wali_syscall_setgroups(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(116, setgroups);
    RETURN(__syscall2(SYS_setgroups, a1, MADDR(a2)), "setgroups", 2, a1, a2);
}

// 117
long
wali_syscall_setresuid(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(117, setresuid);
    RETURN(__syscall3(SYS_setresuid, a1, a2, a3), "setresuid", 3, a1, a2, a3);
}

// 119
long
wali_syscall_setresgid(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(119, setresgid);
    RETURN(__syscall3(SYS_setresgid, a1, a2, a3), "setresgid", 3, a1, a2, a3);
}

// 121
long
wali_syscall_getpgid(wasm_exec_env_t exec_env, long a1)
{
    SC(121, getpgid);
    RETURN(__syscall1(SYS_getpgid, a1), "getpgid", 1, a1);
}

// 124
long
wali_syscall_getsid(wasm_exec_env_t exec_env, long a1)
{
    SC(124, getsid);
    RETURN(__syscall1(SYS_getsid, a1), "getsid", 1, a1);
}

// 127
long
wali_syscall_rt_sigpending(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(127, rt_sigpending);
    RETURN(__syscall2(SYS_rt_sigpending, MADDR(a1), a2), "rt_sigpending", 2, a1,
           a2);
}

// 130
long
wali_syscall_rt_sigsuspend(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(130, rt_sigsuspend);
    RETURN(__syscall2(SYS_rt_sigsuspend, MADDR(a1), a2), "rt_sigsuspend", 2, a1,
           a2);
}

// 131
long
wali_syscall_sigaltstack(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(131, sigaltstack);
    Addr wasm_ss = MADDR(a1), wasm_old_ss = MADDR(a2);

    stack_t ss = { 0 }, old_ss = { 0 };
    stack_t *ss_ptr = copy_sigstack(exec_env, wasm_ss, &ss);
    stack_t *old_ss_ptr = copy_sigstack(exec_env, wasm_old_ss, &old_ss);

    RETURN(__syscall2(SYS_sigaltstack, ss_ptr, old_ss_ptr), "sigaltstack", 2,
           a1, a2);
}

// 132
long
wali_syscall_utime(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(132, utime);
    FATALSC(utime, "Obsolete -- Use \'utimesnsat\' instead");
#if __x86_64__
#elif __aarch64__ || __riscv64__
#endif
    RETURN(-1, "utime", 2, a1, a2);
}

// 137
long
wali_syscall_statfs(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(137, statfs);
    RETURN(__syscall2(SYS_statfs, MADDR(a1), MADDR(a2)), "statfs", 2, a1, a2);
}

// 138
long
wali_syscall_fstatfs(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(138, fstatfs);
    RETURN(__syscall2(SYS_fstatfs, a1, MADDR(a2)), "fstatfs", 2, a1, a2);
}

// 157
long
wali_syscall_prctl(wasm_exec_env_t exec_env, long a1, long a2, long a3, long a4,
                   long a5)
{
    SC(157, prctl);
    RETURN(__syscall5(SYS_prctl, a1, a2, a3, a4, a5), "prctl", 5, a1, a2, a3,
           a4, a5);
}

// 160
long
wali_syscall_setrlimit(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(160, setrlimit);
    RETURN(__syscall2(SYS_setrlimit, a1, MADDR(a2)), "setrlimit", 2, a1, a2);
}

// 161
long
wali_syscall_chroot(wasm_exec_env_t exec_env, long a1)
{
    SC(161, chroot);
    RETURN(__syscall1(SYS_chroot, MADDR(a1)), "chroot", 1, a1);
}

// 186
long
wali_syscall_gettid(wasm_exec_env_t exec_env)
{
    SC(186, gettid);
    RETURN(__syscall0(SYS_gettid), "gettid", 0, 0);
}

// 200
long
wali_syscall_tkill(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(200, tkill);
    RETURN(__syscall2(SYS_tkill, a1, a2), "tkill", 2, a1, a2);
}

// 202
long
wali_syscall_futex(wasm_exec_env_t exec_env, long a1, long a2, long a3, long a4,
                   long a5, long a6)
{
    SC(202, futex);
    RETURN(__syscall6(SYS_futex, MADDR(a1), a2, a3, MADDR(a4), MADDR(a5), a6),
           "futex", 6, a1, a2, a3, a4, a5, a6);
}

// 204
long
wali_syscall_sched_getaffinity(wasm_exec_env_t exec_env, long a1, long a2,
                               long a3)
{
    SC(204, sched_getaffinity);
    RETURN(__syscall3(SYS_sched_getaffinity, a1, a2, MADDR(a3)),
           "sched_getaffinity", 3, a1, a2, a3);
}

// 217
long
wali_syscall_getdents64(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(217, getdents64);
    RETURN(__syscall3(SYS_getdents64, a1, MADDR(a2), a3), "getdents64", 3, a1,
           a2, a3);
}

// 218
long
wali_syscall_set_tid_address(wasm_exec_env_t exec_env, long a1)
{
    SC(218, set_tid_address);
    RETURN(__syscall1(SYS_set_tid_address, MADDR(a1)), "set_tid_address", 1,
           a1);
}

// 221 TODO
long
wali_syscall_fadvise(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                     long a4)
{
    SC(221, fadvise);
    ERRSC(fadvise);
    RETURN(__syscall4(SYS_fadvise64, a1, a2, a3, a4), "fadvise", 4, a1, a2, a3,
           a4);
}

// 228
long
wali_syscall_clock_gettime(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(228, clock_gettime);
    RETURN(__syscall2(SYS_clock_gettime, a1, MADDR(a2)), "clock_gettime", 2, a1,
           a2);
}

// 229
long
wali_syscall_clock_getres(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(229, clock_getres);
    RETURN(__syscall2(SYS_clock_getres, a1, MADDR(a2)), "clock_getres", 2, a1,
           a2);
}

// 230
long
wali_syscall_clock_nanosleep(wasm_exec_env_t exec_env, long a1, long a2,
                             long a3, long a4)
{
    SC(230, clock_nanosleep);
    RETURN(__syscall4(SYS_clock_nanosleep, a1, a2, MADDR(a3), MADDR(a4)),
           "clock_nanosleep", 4, a1, a2, a3, a4);
}

// 231
long
wali_syscall_exit_group(wasm_exec_env_t exec_env, long a1)
{
    SC(231, exit_group);
    wali_proc_exit(exec_env, a1);
    RETURN(-1, "exit_group", 1, a1);
}

// 233
long
wali_syscall_epoll_ctl(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                       long a4)
{
    SC(233, epoll_ctl);
    struct epoll_event *nev =
        copy_epoll_event(exec_env, MADDR(a4), &(struct epoll_event){ 0 });
    RETURN(__syscall4(SYS_epoll_ctl, a1, a2, a3, nev), "epoll_ctl", 4, a1, a2,
           a3, a4);
}

// 257
long
wali_syscall_openat(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                    long a4)
{
    SC(257, openat);
    // security check
    if (strncmp((char *)MADDR(a2), "/proc/self/mem", 15) == 0) {
        printf("Unpermitted attempt to open /proc/self/mem.");
        RETURN(-1, "openat", 4, a1, a2, a3, a4);
    }
#if __aarch64__
    RETURN(__syscall4(SYS_openat, a1, MADDR(a2), swap_open_flags(a3), a4),
           "openat", 4, a1, a2, a3, a4);
#else
    RETURN(__syscall4(SYS_openat, a1, MADDR(a2), a3, a4), "openat", 4, a1, a2,
           a3, a4);
#endif
}

// 258
long
wali_syscall_mkdirat(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(258, mkdirat);
    RETURN(__syscall3(SYS_mkdirat, a1, MADDR(a2), a3), "mkdirat", 3, a1, a2,
           a3);
}

// 260
long
wali_syscall_fchownat(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                      long a4, long a5)
{
    SC(260, fchownat);
    RETURN(__syscall5(SYS_fchownat, a1, MADDR(a2), a3, a4, a5), "fchownat", 5,
           a1, a2, a3, a4, a5);
}

// 262
long
wali_syscall_fstatat(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                     long a4)
{
    SC(262, fstatat);
#if __x86_64__
    RETURN(__syscall4(SYS_newfstatat, a1, MADDR(a2), MADDR(a3), a4), "fstatat",
           4, a1, a2, a3, a4);
#elif __aarch64__ || __riscv64__
    Addr wasm_stat = MADDR(a3);
    struct stat sb;
    long retval = __syscall4(SYS_newfstatat, a1, MADDR(a2), &sb, a4);
    copy2wasm_stat_struct(exec_env, wasm_stat, &sb);
    RETURN(retval, "fstatat", 4, a1, a2, a3, a4);
#endif
}

// 263
long
wali_syscall_unlinkat(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(263, unlinkat);
    RETURN(__syscall3(SYS_unlinkat, a1, MADDR(a2), a3), "unlinkat", 3, a1, a2,
           a3);
}

// 265
long
wali_syscall_linkat(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                    long a4, long a5)
{
    SC(265, linkat);
    RETURN(__syscall5(SYS_linkat, a1, MADDR(a2), a3, MADDR(a4), a5), "linkat",
           5, a1, a2, a3, a4, a5);
}

// 266
long
wali_syscall_symlinkat(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(266, symlinkat);
    RETURN(__syscall3(SYS_symlinkat, MADDR(a1), a2, MADDR(a3)), "symlinkat", 3,
           a1, a2, a3);
}

// 267
long
wali_syscall_readlinkat(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                        long a4)
{
    SC(267, readlinkat);
    RETURN(__syscall4(SYS_readlinkat, a1, MADDR(a2), MADDR(a3), a4),
           "readlinkat", 4, a1, a2, a3, a4);
}

// 268
long
wali_syscall_fchmodat(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                      long a4)
{
    SC(268, fchmodat);
    RETURN(__syscall4(SYS_fchmodat, a1, MADDR(a2), a3, a4), "fchmodat", 4, a1,
           a2, a3, a4);
}

// 269
long
wali_syscall_faccessat(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                       long a4)
{
    SC(269, faccessat);
    RETURN(__syscall4(SYS_faccessat, a1, MADDR(a2), a3, a4), "faccessat", 4, a1,
           a2, a3, a4);
}

// 270
long
wali_syscall_pselect6(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                      long a4, long a5, long a6)
{
    SC(270, pselect6);
    VB("pselect args | a1: %ld, a2: %ld, a3: %ld, a4: %ld, a5: %ld, a6: %ld",
       a1, a2, a3, a4, a5, a6);
    Addr wasm_psel_sm = MADDR(a6);
    long sm_struct[2];
    long *sm_struct_ptr =
        copy_pselect6_sigmask(exec_env, wasm_psel_sm, sm_struct);
    RETURN(__syscall6(SYS_pselect6, a1, MADDR(a2), MADDR(a3), MADDR(a4),
                      MADDR(a5), sm_struct_ptr),
           "pselect6", 6, a1, a2, a3, a4, a5, a6);
}

// 271
long
wali_syscall_ppoll(wasm_exec_env_t exec_env, long a1, long a2, long a3, long a4,
                   long a5)
{
    SC(271, ppoll);
    RETURN(__syscall5(SYS_ppoll, MADDR(a1), a2, MADDR(a3), MADDR(a4), a5),
           "ppoll", 5, a1, a2, a3, a4, a5);
}
/* Since poll needs a time conversion on pointer, need to use a different alias
 * call */
long
wali_syscall_ppoll_aliased(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                           long a4, long a5)
{
    SC(271, ppoll - alias);
    RETURN(__syscall5(SYS_ppoll, MADDR(a1), a2, a3, MADDR(a4), a5),
           "ppoll_aliased", 5, a1, a2, a3, a4, a5);
}

// 280
long
wali_syscall_utimensat(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                       long a4)
{
    SC(280, utimensat);
    RETURN(__syscall4(SYS_utimensat, a1, MADDR(a2), MADDR(a3), a4), "utimensat",
           4, a1, a2, a3, a4);
}

// 281
long
wali_syscall_epoll_pwait(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                         long a4, long a5, long a6)
{
    SC(281, epoll_pwait);
    Addr wasm_epoll = MADDR(a2);
    struct epoll_event *nev =
        copy_epoll_event(exec_env, wasm_epoll, &(struct epoll_event){ 0 });
    long retval = __syscall6(SYS_epoll_pwait, a1, nev, a3, a4, MADDR(a5), a6);
    copy2wasm_epoll_event(exec_env, wasm_epoll, nev);
    RETURN(retval, "epoll_pwait", 6, a1, a2, a3, a4, a5, a6);
}

// 284
long
wali_syscall_eventfd(wasm_exec_env_t exec_env, long a1)
{
    SC(284, eventfd);
#if __x86_64__
    RETURN(__syscall1(SYS_eventfd, a1), "eventfd", 1, a1);
#elif __aarch64__ || __riscv64__
    RETURN(wali_syscall_eventfd2(exec_env, a1, 0), "eventfd", 1, a1);
#endif
}

// 288
long
wali_syscall_accept4(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                     long a4)
{
    SC(288, accept4);
    RETURN(__syscall4(SYS_accept4, a1, MADDR(a2), MADDR(a3), a4), "accept4", 4,
           a1, a2, a3, a4);
}

// 290 TODO
long
wali_syscall_eventfd2(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(290, eventfd2);
    ERRSC(eventfd2);
    RETURN(__syscall2(SYS_eventfd2, a1, a2), "eventfd2", 2, a1, a2);
}

// 291
long
wali_syscall_epoll_create1(wasm_exec_env_t exec_env, long a1)
{
    SC(291, epoll_create1);
    RETURN(__syscall1(SYS_epoll_create1, a1), "epoll_create1", 1, a1);
}

// 292
long
wali_syscall_dup3(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(292, dup3);
#if __aarch64__
    RETURN(__syscall3(SYS_dup3, a1, a2, swap_open_flags(a3)), "dup3", 3, a1, a2,
           a3);
#else
    RETURN(__syscall3(SYS_dup3, a1, a2, a3), "dup3", 3, a1, a2, a3);
#endif
}

// 293
long
wali_syscall_pipe2(wasm_exec_env_t exec_env, long a1, long a2)
{
    SC(293, pipe2);
#if __aarch64__
    RETURN(__syscall2(SYS_pipe2, MADDR(a1), swap_open_flags(a2)), "pipe2", 2,
           a1, a2);
#else
    RETURN(__syscall2(SYS_pipe2, MADDR(a1), a2), "pipe2", 2, a1, a2);
#endif
}

// 302
long
wali_syscall_prlimit64(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                       long a4)
{
    SC(302, prlimit64);
    RETURN(__syscall4(SYS_prlimit64, a1, a2, MADDR(a3), MADDR(a4)), "prlimit64",
           4, a1, a2, a3, a4);
}

// 316
long
wali_syscall_renameat2(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                       long a4, long a5)
{
    SC(316, renameat2);
    RETURN(__syscall5(SYS_renameat2, a1, MADDR(a2), a3, MADDR(a4), a5),
           "renameat2", 5, a1, a2, a3, a4, a5);
}

// 318
long
wali_syscall_getrandom(wasm_exec_env_t exec_env, long a1, long a2, long a3)
{
    SC(318, getrandom);
    RETURN(__syscall3(SYS_getrandom, MADDR(a1), a2, a3), "getrandom", 3, a1, a2,
           a3);
}

// 332
long
wali_syscall_statx(wasm_exec_env_t exec_env, long a1, long a2, long a3, long a4,
                   long a5)
{
    SC(332, statx);
    RETURN(__syscall5(SYS_statx, a1, MADDR(a2), a3, a4, MADDR(a5)), "statx", 5,
           a1, a2, a3, a4, a5);
}

// 439
long
wali_syscall_faccessat2(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                        long a4)
{
    SC(439, faccessat2);
    RETURN(__syscall4(439, a1, MADDR(a2), a3, a4), "faccessat2", 4, a1, a2, a3,
           a4);
}

/***** Non-syscall methods *****/
int
wali_sigsetjmp(wasm_exec_env_t exec_env, int sigjmp_buf_addr, int savesigs)
{
    PC(sigsetjmp);
    Addr wasm_sigjmp_buf = MADDR(sigjmp_buf_addr);
    struct __libc_jmp_buf_tag *jmpenv = copy_jmp_buf(exec_env, wasm_sigjmp_buf);
    int retval = __libc_sigsetjmp_asm(jmpenv, savesigs);
    ERRSC(sigsetjmp, "Unsupported in WALI right now, continuing execution with "
                     "flag on siglongjmp");
    if (retval == 0) {
        copy2wasm_jmp_buf(exec_env, wasm_sigjmp_buf, jmpenv);
        free(jmpenv);
    }
    return retval;
}

int
wali_setjmp(wasm_exec_env_t exec_env, int jmp_buf_addr)
{
    PC(setjmp);
    ERRSC(setjmp, "Unsupported in WALI right now, continuing execution with "
                  "flag on longjmp");
    return 0;
}

_Noreturn void
wali_siglongjmp(wasm_exec_env_t exec_env, int sigjmp_buf_addr, int val)
{
    PC(siglongjmp);
    // struct __libc_jmp_buf_tag* env = copy_jmp_buf(exec_env,
    // MADDR(sigjmp_buf_addr));
    FATALSC(siglongjmp, "Not supported in WALI yet, exiting code...");
    wali_proc_exit(exec_env, 1);
    // __libc_siglongjmp(env, val);
    /* Should not reach here */
    exit(-1);
}

/***** Startup *****/
static bool init_called = false;
static bool deinit_called = false;

void
wali_init(wasm_exec_env_t exec_env)
{
    PC(__init);
    if (init_called) {
        ERR("__init has already been invoked once for the instance; do not re-invoke");
    }
    init_called = true;
    // WAMR engine interfacing
    invoked_wali = true;
}

void
wali_deinit(wasm_exec_env_t exec_env)
{
    PC(__deinit);
    if (deinit_called) {
        ERR("__deinit has already been invoked once for the instance; do not re-invoke");
    }
    deinit_called = true;
}

void
wali_proc_exit(wasm_exec_env_t exec_env, long v)
{
    PC(proc_exit);
#if WALI_ENABLE_SYSCALL_PROFILE
    wali_syscall_profile_dump(0);
#endif
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    WALIContext *wali_ctx = wasm_runtime_get_wali_ctx(module_inst);
    /* if wali_deinit is invoked, main ended successfully, do
     * not set exception */
    if (!deinit_called || v) {
        VB("WALI process exit called prematurely");
        wasm_runtime_set_exception(module_inst, "wali proc exit");
    }
    else {
        VB("Main ended successfully");
    }
    wali_ctx->exit_code = v;
    proc_exit_primary_tid = gettid();
    proc_exit_invoked = true;
}

void
wali_thread_exit(wasm_exec_env_t exec_env, long v)
{
    PC(thread_exit);
    /* Have to use cancel thread as opposed to exit thread
     * so that it is caught after native functions (WALI) returns */
    wasm_cluster_cancel_thread(exec_env);
}

int
wali_cl_get_argc(wasm_exec_env_t exec_env)
{
    PC(cl_get_argc);
    return wali_app_argc;
}

int
wali_cl_get_argv_len(wasm_exec_env_t exec_env, int arg_idx)
{
    PC(cl_get_argc_len);
    return strlen(wali_app_argv[arg_idx]);
}

int
wali_cl_copy_argv(wasm_exec_env_t exec_env, int argv_addr, int arg_idx)
{
    PC(cl_copy_argv);
    Addr argv = MADDR(argv_addr);
    strcpy((char *)argv, wali_app_argv[arg_idx]);
    return 0;
}

int
wali_get_init_envfile(wasm_exec_env_t exec_env, int faddr, int fsize)
{
    PC(get_init_envfile);
    Addr fbuf = MADDR(faddr);

    /* Check for passthrough env from an execve call */
    char pass_filename[100];
    sprintf(pass_filename, "/tmp/wali_env.%d", getpid());
    int execve_invoked = !access(pass_filename, R_OK);

    char *envfile = execve_invoked ? pass_filename : wali_app_env_file;

    if (!envfile) {
        ERR("No WALI environment file provided\n");
        return 0;
    }

    if ((int)(strlen(envfile) + 1) > fsize) {
        ERR("WALI env initialization filepath too large (max length: %d)."
            "Defaulting to NULL\n",
            fsize);
        ((char *)fbuf)[0] = 0;
    }
    else {
        strcpy((char *)fbuf, envfile);
        ERR("WALI init env file: \'%s\'\n", fbuf);
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

    VB("Dispatcher | Child TID: %d\n", wasm_argv[0]);
    /* Send parent our TID */
    signalled_tid = tid;
    if (sem_post(&tid_sem)) {
        perror("sem_post");
    }

    if (!wasm_runtime_call_wasm(exec_env, thread_arg->start_fn, 2, wasm_argv)) {
        /* Exception has already been spread during throwing */
    }

    VB("================ Thread [%d] exiting ==============\n", gettid());
    // Cleanup
    wasm_runtime_free(thread_arg);
    exec_env->thread_arg = NULL;

    return NULL;
}

int
wali_wasm_thread_spawn(wasm_exec_env_t exec_env, int setup_fnptr, int arg_wasm)
{
    SC(56, wasm_thread_spawn(clone));
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
        wasm_runtime_get_indirect_function(module_inst, 0, setup_fnptr);

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
        FATALSC(wasm_thread_spawn, "Runtime args allocation failed");
        goto thread_spawn_fail;
    }

    thread_start_arg->tid = thread_id = THREAD_ID++;

    thread_start_arg->start_fn = setup_wasm_fn;
    thread_start_arg->arg = arg_wasm;
    /** **/

    /** Create and dispatch the thread (language-independent: currently just C);
     * Thread ID of the created thread is sent back to parent */
    volatile int child_tid = -1;
    pthread_mutex_lock(&clone_lock);
    ret =
        wasm_cluster_create_thread(exec_env, new_module_inst, false, 0, 0,
                                   wali_dispatch_thread_libc, thread_start_arg);
    if (ret != 0) {
        FATALSC(wasm_thread_spawn, "Failed to spawn a new thread");
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
        FATALSC(wasm_thread_spawn, "TID signalling error");
        goto thread_spawn_fail_post_clone;
    }

    child_tid = signalled_tid;
    VB("Parent of Dispatcher | Child TID: %d\n", child_tid);
    pthread_mutex_unlock(&clone_lock);

    FUNC_FREE(setup_wasm_fn);

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

/* Native WALI Symbols */
#define NSYMBOL(symbol, fn, sign) { #symbol, (void *)fn, sign, NULL }

static NativeSymbol wali_native_symbols[] = {
    // Syscalls
    NSYMBOL(SYS_read, wali_syscall_read, "(iii)I"),
    NSYMBOL(SYS_write, wali_syscall_write, "(iii)I"),
    NSYMBOL(SYS_open, wali_syscall_open, "(iii)I"),
    NSYMBOL(SYS_close, wali_syscall_close, "(i)I"),
    NSYMBOL(SYS_stat, wali_syscall_stat, "(ii)I"),
    NSYMBOL(SYS_fstat, wali_syscall_fstat, "(ii)I"),
    NSYMBOL(SYS_lstat, wali_syscall_lstat, "(ii)I"),
    NSYMBOL(SYS_poll, wali_syscall_poll, "(iii)I"),
    NSYMBOL(SYS_lseek, wali_syscall_lseek, "(iIi)I"),
    NSYMBOL(SYS_mmap, wali_syscall_mmap, "(iiiiiI)I"),
    NSYMBOL(SYS_mprotect, wali_syscall_mprotect, "(iii)I"),
    NSYMBOL(SYS_munmap, wali_syscall_munmap, "(ii)I"),
    NSYMBOL(SYS_brk, wali_syscall_brk, "(i)I"),
    NSYMBOL(SYS_rt_sigaction, wali_syscall_rt_sigaction, "(iiii)I"),
    NSYMBOL(SYS_rt_sigprocmask, wali_syscall_rt_sigprocmask, "(iiii)I"),
    NSYMBOL(SYS_rt_sigreturn, wali_syscall_rt_sigreturn, "(I)I"),
    NSYMBOL(SYS_ioctl, wali_syscall_ioctl, "(iii)I"),
    NSYMBOL(SYS_pread64, wali_syscall_pread64, "(iiiI)I"),
    NSYMBOL(SYS_pwrite64, wali_syscall_pwrite64, "(iiiI)I"),
    NSYMBOL(SYS_readv, wali_syscall_readv, "(iii)I"),
    NSYMBOL(SYS_writev, wali_syscall_writev, "(iii)I"),
    NSYMBOL(SYS_access, wali_syscall_access, "(ii)I"),
    NSYMBOL(SYS_pipe, wali_syscall_pipe, "(i)I"),
    NSYMBOL(SYS_select, wali_syscall_select, "(iiiii)I"),
    NSYMBOL(SYS_sched_yield, wali_syscall_sched_yield, "()I"),
    NSYMBOL(SYS_mremap, wali_syscall_mremap, "(iiiii)I"),
    NSYMBOL(SYS_msync, wali_syscall_msync, "(iii)I"),
    NSYMBOL(SYS_madvise, wali_syscall_madvise, "(iii)I"),
    NSYMBOL(SYS_dup, wali_syscall_dup, "(i)I"),
    NSYMBOL(SYS_dup2, wali_syscall_dup2, "(ii)I"),
    NSYMBOL(SYS_nanosleep, wali_syscall_nanosleep, "(ii)I"),
    NSYMBOL(SYS_alarm, wali_syscall_alarm, "(i)I"),
    NSYMBOL(SYS_setitimer, wali_syscall_setitimer, "(iii)I"),
    NSYMBOL(SYS_getpid, wali_syscall_getpid, "()I"),
    NSYMBOL(SYS_socket, wali_syscall_socket, "(iii)I"),
    NSYMBOL(SYS_connect, wali_syscall_connect, "(iii)I"),
    NSYMBOL(SYS_accept, wali_syscall_accept, "(iii)I"),
    NSYMBOL(SYS_sendto, wali_syscall_sendto, "(iiiiii)I"),
    NSYMBOL(SYS_recvfrom, wali_syscall_recvfrom, "(iiiiii)I"),
    NSYMBOL(SYS_sendmsg, wali_syscall_sendmsg, "(iii)I"),
    NSYMBOL(SYS_recvmsg, wali_syscall_recvmsg, "(iii)I"),
    NSYMBOL(SYS_shutdown, wali_syscall_shutdown, "(ii)I"),
    NSYMBOL(SYS_bind, wali_syscall_bind, "(iii)I"),
    NSYMBOL(SYS_listen, wali_syscall_listen, "(ii)I"),
    NSYMBOL(SYS_getsockname, wali_syscall_getsockname, "(iii)I"),
    NSYMBOL(SYS_getpeername, wali_syscall_getpeername, "(iii)I"),
    NSYMBOL(SYS_socketpair, wali_syscall_socketpair, "(iiii)I"),
    NSYMBOL(SYS_setsockopt, wali_syscall_setsockopt, "(iiiii)I"),
    NSYMBOL(SYS_getsockopt, wali_syscall_getsockopt, "(iiiii)I"),
    NSYMBOL(SYS_fork, wali_syscall_fork, "()I"),
    NSYMBOL(SYS_execve, wali_syscall_execve, "(iii)I"),
    NSYMBOL(SYS_exit, wali_syscall_exit, "(i)I"),
    NSYMBOL(SYS_wait4, wali_syscall_wait4, "(iiii)I"),
    NSYMBOL(SYS_kill, wali_syscall_kill, "(ii)I"),
    NSYMBOL(SYS_uname, wali_syscall_uname, "(i)I"),
    NSYMBOL(SYS_fcntl, wali_syscall_fcntl, "(iii)I"),
    NSYMBOL(SYS_flock, wali_syscall_flock, "(ii)I"),
    NSYMBOL(SYS_fsync, wali_syscall_fsync, "(i)I"),
    NSYMBOL(SYS_fdatasync, wali_syscall_fdatasync, "(i)I"),
    NSYMBOL(SYS_ftruncate, wali_syscall_ftruncate, "(iI)I"),
    NSYMBOL(SYS_getdents, wali_syscall_getdents, "(iii)I"),
    NSYMBOL(SYS_getcwd, wali_syscall_getcwd, "(ii)I"),
    NSYMBOL(SYS_chdir, wali_syscall_chdir, "(i)I"),
    NSYMBOL(SYS_fchdir, wali_syscall_fchdir, "(i)I"),
    NSYMBOL(SYS_rename, wali_syscall_rename, "(ii)I"),
    NSYMBOL(SYS_mkdir, wali_syscall_mkdir, "(ii)I"),
    NSYMBOL(SYS_rmdir, wali_syscall_rmdir, "(i)I"),
    NSYMBOL(SYS_link, wali_syscall_link, "(ii)I"),
    NSYMBOL(SYS_unlink, wali_syscall_unlink, "(i)I"),
    NSYMBOL(SYS_symlink, wali_syscall_symlink, "(ii)I"),
    NSYMBOL(SYS_readlink, wali_syscall_readlink, "(iii)I"),
    NSYMBOL(SYS_chmod, wali_syscall_chmod, "(ii)I"),
    NSYMBOL(SYS_fchmod, wali_syscall_fchmod, "(ii)I"),
    NSYMBOL(SYS_chown, wali_syscall_chown, "(iii)I"),
    NSYMBOL(SYS_fchown, wali_syscall_fchown, "(iii)I"),
    NSYMBOL(SYS_umask, wali_syscall_umask, "(i)I"),
    NSYMBOL(SYS_getrlimit, wali_syscall_getrlimit, "(ii)I"),
    NSYMBOL(SYS_getrusage, wali_syscall_getrusage, "(ii)I"),
    NSYMBOL(SYS_sysinfo, wali_syscall_sysinfo, "(i)I"),
    NSYMBOL(SYS_getuid, wali_syscall_getuid, "()I"),
    NSYMBOL(SYS_getgid, wali_syscall_getgid, "()I"),
    NSYMBOL(SYS_setuid, wali_syscall_setuid, "(i)I"),
    NSYMBOL(SYS_setgid, wali_syscall_setgid, "(i)I"),
    NSYMBOL(SYS_geteuid, wali_syscall_geteuid, "()I"),
    NSYMBOL(SYS_getegid, wali_syscall_getegid, "()I"),
    NSYMBOL(SYS_setpgid, wali_syscall_setpgid, "(ii)I"),
    NSYMBOL(SYS_getppid, wali_syscall_getppid, "()I"),
    NSYMBOL(SYS_setsid, wali_syscall_setsid, "()I"),
    NSYMBOL(SYS_setreuid, wali_syscall_setreuid, "(ii)I"),
    NSYMBOL(SYS_setregid, wali_syscall_setregid, "(ii)I"),
    NSYMBOL(SYS_getgroups, wali_syscall_getgroups, "(ii)I"),
    NSYMBOL(SYS_setgroups, wali_syscall_setgroups, "(ii)I"),
    NSYMBOL(SYS_setresuid, wali_syscall_setresuid, "(iii)I"),
    NSYMBOL(SYS_setresgid, wali_syscall_setresgid, "(iii)I"),
    NSYMBOL(SYS_getpgid, wali_syscall_getpgid, "(i)I"),
    NSYMBOL(SYS_getsid, wali_syscall_getsid, "(i)I"),
    NSYMBOL(SYS_rt_sigpending, wali_syscall_rt_sigpending, "(ii)I"),
    NSYMBOL(SYS_rt_sigsuspend, wali_syscall_rt_sigsuspend, "(ii)I"),
    NSYMBOL(SYS_sigaltstack, wali_syscall_sigaltstack, "(ii)I"),
    NSYMBOL(SYS_utime, wali_syscall_utime, "(ii)I"),
    NSYMBOL(SYS_statfs, wali_syscall_statfs, "(ii)I"),
    NSYMBOL(SYS_fstatfs, wali_syscall_fstatfs, "(ii)I"),
    NSYMBOL(SYS_prctl, wali_syscall_prctl, "(iIIII)I"),
    NSYMBOL(SYS_setrlimit, wali_syscall_setrlimit, "(ii)I"),
    NSYMBOL(SYS_chroot, wali_syscall_chroot, "(i)I"),
    NSYMBOL(SYS_gettid, wali_syscall_gettid, "()I"),
    NSYMBOL(SYS_tkill, wali_syscall_tkill, "(ii)I"),
    NSYMBOL(SYS_futex, wali_syscall_futex, "(iiiiii)I"),
    NSYMBOL(SYS_sched_getaffinity, wali_syscall_sched_getaffinity, "(iii)I"),
    NSYMBOL(SYS_getdents64, wali_syscall_getdents64, "(iii)I"),
    NSYMBOL(SYS_set_tid_address, wali_syscall_set_tid_address, "(i)I"),
    NSYMBOL(SYS_fadvise, wali_syscall_fadvise, "(iIIi)I"),
    NSYMBOL(SYS_clock_gettime, wali_syscall_clock_gettime, "(ii)I"),
    NSYMBOL(SYS_clock_getres, wali_syscall_clock_getres, "(ii)I"),
    NSYMBOL(SYS_clock_nanosleep, wali_syscall_clock_nanosleep, "(iiii)I"),
    NSYMBOL(SYS_exit_group, wali_syscall_exit_group, "(i)I"),
    NSYMBOL(SYS_epoll_ctl, wali_syscall_epoll_ctl, "(iiii)I"),
    NSYMBOL(SYS_openat, wali_syscall_openat, "(iiii)I"),
    NSYMBOL(SYS_mkdirat, wali_syscall_mkdirat, "(iii)I"),
    NSYMBOL(SYS_fchownat, wali_syscall_fchownat, "(iiiii)I"),
    NSYMBOL(SYS_fstatat, wali_syscall_fstatat, "(iiii)I"),
    NSYMBOL(SYS_unlinkat, wali_syscall_unlinkat, "(iii)I"),
    NSYMBOL(SYS_linkat, wali_syscall_linkat, "(iiiii)I"),
    NSYMBOL(SYS_symlinkat, wali_syscall_symlinkat, "(iii)I"),
    NSYMBOL(SYS_readlinkat, wali_syscall_readlinkat, "(iiii)I"),
    NSYMBOL(SYS_fchmodat, wali_syscall_fchmodat, "(iiii)I"),
    NSYMBOL(SYS_faccessat, wali_syscall_faccessat, "(iiii)I"),
    NSYMBOL(SYS_pselect6, wali_syscall_pselect6, "(iiiiii)I"),
    NSYMBOL(SYS_ppoll, wali_syscall_ppoll, "(iiiii)I"),
    NSYMBOL(SYS_utimensat, wali_syscall_utimensat, "(iiii)I"),
    NSYMBOL(SYS_epoll_pwait, wali_syscall_epoll_pwait, "(iiiiii)I"),
    NSYMBOL(SYS_eventfd, wali_syscall_eventfd, "(i)I"),
    NSYMBOL(SYS_accept4, wali_syscall_accept4, "(iiii)I"),
    NSYMBOL(SYS_eventfd2, wali_syscall_eventfd2, "(ii)I"),
    NSYMBOL(SYS_epoll_create1, wali_syscall_epoll_create1, "(i)I"),
    NSYMBOL(SYS_dup3, wali_syscall_dup3, "(iii)I"),
    NSYMBOL(SYS_pipe2, wali_syscall_pipe2, "(ii)I"),
    NSYMBOL(SYS_prlimit64, wali_syscall_prlimit64, "(iiii)I"),
    NSYMBOL(SYS_renameat2, wali_syscall_renameat2, "(iiiii)I"),
    NSYMBOL(SYS_getrandom, wali_syscall_getrandom, "(iii)I"),
    NSYMBOL(SYS_statx, wali_syscall_statx, "(iiiii)I"),
    NSYMBOL(SYS_faccessat2, wali_syscall_faccessat2, "(iiii)I"),

    /* Libc imports */
    // Threads
    // thread_spawn is the substitute for syscall(clone)
    NSYMBOL(__wasm_thread_spawn, wali_wasm_thread_spawn, "(ii)i"),

    // Startup
    NSYMBOL(__init, wali_init, "()"),
    NSYMBOL(__deinit, wali_deinit, "()"),
    NSYMBOL(__proc_exit, wali_proc_exit, "(i)"),
    NSYMBOL(__cl_get_argc, wali_cl_get_argc, "()i"),
    NSYMBOL(__cl_get_argv_len, wali_cl_get_argv_len, "(i)i"),
    NSYMBOL(__cl_copy_argv, wali_cl_copy_argv, "(ii)i"),
    NSYMBOL(__get_init_envfile, wali_get_init_envfile, "(ii)i"),

    // Signal
    NSYMBOL(sigsetjmp, wali_sigsetjmp, "(ii)i"),
    NSYMBOL(setjmp, wali_setjmp, "(i)i"),
    NSYMBOL(longjmp, wali_siglongjmp, "(ii)"),

};

uint32
get_libc_wali_export_apis(NativeSymbol **p_libc_wali_apis)
{
    *p_libc_wali_apis = wali_native_symbols;
    return sizeof(wali_native_symbols) / sizeof(NativeSymbol);
}
