#ifndef WALI_H
#define WALI_H

#include "wasm_export.h"
#include "bh_platform.h"
#include "aot_export.h"
#include "wali_defs.h"

#if !__x86_64__ && !__aarch64__ && !__riscv64__
#error \
    "Unsupported architecture for WALI -- Only supports [x86_64, aarch64, riscv64]"
#endif

/** Logging: Disable when profiling **/
#if WALI_ENABLE_SYSCALL_PROFILE
#define VB(fmt, ...)
#define WARN(fmt, ...)
#define ERR(fmt, ...)
#else
#define VB(fmt, ...) LOG_VERBOSE("[%d] WALI: " fmt, gettid(), ##__VA_ARGS__)
#define WARN(fmt, ...) LOG_WARNING("[%d] WALI: " fmt, gettid(), ##__VA_ARGS__)
#define ERR(fmt, ...) LOG_ERROR("[%d] WALI: " fmt, gettid(), ##__VA_ARGS__)
#endif

/** Memory defines/translations **/
typedef uint8_t *Addr;
typedef uint32_t FuncPtr_t;

#define BASE_ADDR() \
    ({ (Addr) wasm_runtime_addr_app_to_native(get_module_inst(exec_env), 0); })

#define MADDR(wasm_addr)                                                  \
    ({                                                                    \
        Addr addr = wasm_addr ? (Addr)wasm_runtime_addr_app_to_native(    \
                                    get_module_inst(exec_env), wasm_addr) \
                              : NULL;                                     \
        if (addr == NULL) {                                               \
        }                                                                 \
        addr;                                                             \
    })

#define WADDR(mem_addr) \
    ({ wasm_runtime_addr_native_to_app(get_module_inst(exec_env), mem_addr); })
/** **/

/** Function translations **/
#define FUNC_IDX(func) ({ wasm_runtime_get_function_idx(module_inst, func); })

/* Needs to be called only for AoT when using wasm_runtime_get_indirect_function
 */
#define FUNC_FREE(func)                                                       \
    {                                                                         \
        if (func                                                              \
            && (get_module_inst(exec_env)->module_type == Wasm_Module_AoT)) { \
            wasm_runtime_free(func);                                          \
        }                                                                     \
    }

/** Some internal structs for syscalls **/

/* This is the structure used for the rt_sigaction syscall on most archs,
 * but it can be overridden by a file with the same name in the top-level
 * arch dir for a given arch, if necessary. */
struct k_sigaction {
    void (*handler)(int);
    unsigned long flags;
    void (*restorer)(void);
    unsigned mask[2];
};

/* Setjmp/longjmp with signal handling */
typedef unsigned long __libc_jmp_buf_internal[8];

typedef struct __libc_jmp_buf_tag {
    __libc_jmp_buf_internal __jb;
    unsigned long __fl;
    unsigned long __ss[128 / sizeof(long)];
} __libc_jmp_buf[1];

typedef __libc_jmp_buf __libc_sigjmp_buf;


// Flag to indicate whether a pointer is a Wasm memory address or a native memory address
typedef enum {
    WasmPtr = 0,
    NativePtr = 1
} PtrCtx;

// Type capturing pointer along with its context
typedef struct {
    long val;
    PtrCtx ctx;
} BufPtr;

/** **/

void
wali_thread_exit(wasm_exec_env_t exec_env, long v);

/** Syscalls **/
long
wali_syscall_read(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_write(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_open(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_close(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_stat(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_fstat(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_lstat(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_poll(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_lseek(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_mmap(wasm_exec_env_t exec_env, long a1, long a2, long a3, long a4,
                  long a5, long a6);
long
wali_syscall_mprotect(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_munmap(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_brk(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_rt_sigaction(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                          long a4);
long
wali_syscall_rt_sigprocmask(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                            long a4);
long
wali_syscall_rt_sigreturn(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_ioctl(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_pread64(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                     long a4);
long
wali_syscall_pwrite64(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                      long a4);
long
wali_syscall_readv(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_writev(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_access(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_pipe(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_select(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                    long a4, long a5);
long
wali_syscall_sched_yield(wasm_exec_env_t exec_env);
long
wali_syscall_mremap(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                    long a4, long a5);
long
wali_syscall_msync(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_madvise(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_dup(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_dup2(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_nanosleep(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_setitimer(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_getpid(wasm_exec_env_t exec_env);
long
wali_syscall_socket(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_connect(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_accept(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_sendto(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                    long a4, long a5, long a6);
long
wali_syscall_recvfrom(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                      long a4, long a5, long a6);
long
wali_syscall_sendmsg(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_recvmsg(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_shutdown(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_bind(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_listen(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_getsockname(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_getpeername(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_socketpair(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                        long a4);
long
wali_syscall_setsockopt(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                        long a4, long a5);
long
wali_syscall_getsockopt(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                        long a4, long a5);
long
wali_syscall_fork(wasm_exec_env_t exec_env);
long
wali_syscall_execve(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_exit(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_wait4(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                   long a4);
long
wali_syscall_kill(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_uname(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_fcntl(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_flock(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_fsync(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_fdatasync(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_ftruncate(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_getcwd(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_chdir(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_fchdir(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_rename(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_mkdir(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_rmdir(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_link(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_unlink(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_symlink(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_readlink(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_chmod(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_fchmod(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_chown(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_fchown(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_umask(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_gettimeofday(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_getrlimit(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_getrusage(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_sysinfo(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_getuid(wasm_exec_env_t exec_env);
long
wali_syscall_getgid(wasm_exec_env_t exec_env);
long
wali_syscall_setuid(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_setgid(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_geteuid(wasm_exec_env_t exec_env);
long
wali_syscall_getegid(wasm_exec_env_t exec_env);
long
wali_syscall_setpgid(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_getppid(wasm_exec_env_t exec_env);
long
wali_syscall_setsid(wasm_exec_env_t exec_env);
long
wali_syscall_setreuid(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_setregid(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_getgroups(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_setgroups(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_setresuid(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_setresgid(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_getpgid(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_getsid(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_rt_sigpending(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_rt_sigsuspend(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_sigaltstack(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_statfs(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_fstatfs(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_prctl(wasm_exec_env_t exec_env, long a1, long a2, long a3, long a4,
                   long a5);
long
wali_syscall_setrlimit(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_chroot(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_gettid(wasm_exec_env_t exec_env);
long
wali_syscall_tkill(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_futex(wasm_exec_env_t exec_env, long a1, long a2, long a3, long a4,
                   long a5, long a6);
long
wali_syscall_sched_getaffinity(wasm_exec_env_t exec_env, long a1, long a2,
                               long a3);
long
wali_syscall_getdents64(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_set_tid_address(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_fadvise(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                     long a4);
long
wali_syscall_clock_gettime(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_clock_getres(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_clock_nanosleep(wasm_exec_env_t exec_env, long a1, long a2,
                             long a3, long a4);
long
wali_syscall_exit_group(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_epoll_ctl(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                       long a4);
long
wali_syscall_openat(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                    long a4);
long
wali_syscall_mkdirat(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_fchownat(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                      long a4, long a5);
long
wali_syscall_newfstatat(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                        long a4);
long
wali_syscall_unlinkat(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_linkat(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                    long a4, long a5);
long
wali_syscall_symlinkat(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_readlinkat(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                        long a4);
long
wali_syscall_fchmodat(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                      long a4);
long
wali_syscall_faccessat(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                       long a4);
long
wali_syscall_pselect6(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                      long a4, long a5, long a6);
long
wali_syscall_ppoll(wasm_exec_env_t exec_env, long a1, long a2, long a3, long a4,
                   long a5);
long
wali_syscall_utimensat(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                       long a4);
long
wali_syscall_epoll_pwait(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                         long a4, long a5, long a6);
long
wali_syscall_eventfd(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_accept4(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                     long a4);
long
wali_syscall_eventfd2(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_epoll_create1(wasm_exec_env_t exec_env, long a1);
long
wali_syscall_dup3(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_pipe2(wasm_exec_env_t exec_env, long a1, long a2);
long
wali_syscall_prlimit64(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                       long a4);
long
wali_syscall_renameat2(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                       long a4, long a5);
long
wali_syscall_getrandom(wasm_exec_env_t exec_env, long a1, long a2, long a3);
long
wali_syscall_statx(wasm_exec_env_t exec_env, long a1, long a2, long a3, long a4,
                   long a5);
long
wali_syscall_faccessat2(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                        long a4);

/** Alias calls **/
long
wali_syscall_ppoll_aliased(wasm_exec_env_t exec_env, long a1, long a2, long a3,
                           long a4, long a5);

/** Auxillary **/
int
wali_sigsetjmp(wasm_exec_env_t exec_env, int sigjmp_buf_addr, int savesigs);
void
wali_siglongjmp(wasm_exec_env_t exec_env, int sigjmp_buf_addr, int val);

/** Startup/Environment **/
void
wali_call_ctors(wasm_exec_env_t exec_env);
void
wali_call_dtors(wasm_exec_env_t exec_env);
void
wali_proc_exit(wasm_exec_env_t exec_env, long v);
int
wali_cl_get_argc(wasm_exec_env_t exec_env);
int
wali_cl_get_argv_len(wasm_exec_env_t exec_env, int arg_idx);
int
wali_cl_copy_argv(wasm_exec_env_t exec_env, int argv_addr, int arg_idx);
int
wali_get_init_envfile(wasm_exec_env_t exec_env, int faddr, int fsize);

/** Threads **/
int
wali_wasm_thread_spawn(wasm_exec_env_t exec_env, int setup_fnptr, int arg_wasm);

#endif
