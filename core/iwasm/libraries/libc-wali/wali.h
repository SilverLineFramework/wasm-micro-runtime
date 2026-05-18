#ifndef WALI_H
#define WALI_H

#include "wasm_export.h"
#include "bh_platform.h"
#include "aot_export.h"
#include "wali_defs.h"

/** Logging **/
#define VB(fmt, ...) LOG_VERBOSE("[wali-log] " fmt, ##__VA_ARGS__)

/** Memory defines/translations **/
typedef uint8_t *Addr;
typedef uint32_t FuncPtr_t;


#define BASE_ADDR() \
    ({ (Addr) wasm_runtime_addr_app_to_native(get_module_inst(exec_env), 0); })

#define MADDR(wasm_addr)                                                  \
    ({                                                                    \
        Addr _maddr_n = wasm_addr ? (Addr)wasm_runtime_addr_app_to_native(\
                                    get_module_inst(exec_env), wasm_addr) \
                                  : NULL;                                 \
        if (_maddr_n == NULL) {                                           \
        }                                                                 \
        _maddr_n;                                                         \
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

// Flag to indicate whether a pointer is a Wasm memory address or a native memory address
typedef enum {
    WasmPtr = 0,
    NativePtr = 1
} PtrCtx;

// Type capturing pointer along with its context
typedef struct {
    long val;
    PtrCtx ctx;
	wasm_exec_env_t env;
} BufPtr;

typedef uint32_t WasmMemAddr;
typedef uint32_t WasmTableInternalIdx;

/* Syscalls */
long wali_syscall_read (wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr buf, uint32_t count);
long wali_syscall_write (wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr buf, uint32_t count);
long wali_syscall_open (wasm_exec_env_t exec_env, WasmMemAddr pathname, int32_t flags, int32_t mode);
long wali_syscall_close (wasm_exec_env_t exec_env, int32_t fd);
long wali_syscall_stat (wasm_exec_env_t exec_env, WasmMemAddr pathname, WasmMemAddr statbuf);
long wali_syscall_fstat (wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr statbuf);
long wali_syscall_lstat (wasm_exec_env_t exec_env, WasmMemAddr pathname, WasmMemAddr statbuf);
long wali_syscall_poll (wasm_exec_env_t exec_env, WasmMemAddr fds, uint64_t nfds, int32_t timeout);
long wali_syscall_lseek (wasm_exec_env_t exec_env, int32_t fd, int64_t offset, int32_t whence);
long wali_syscall_mmap (wasm_exec_env_t exec_env, WasmMemAddr addr, uint32_t length, int32_t prot, int32_t flags, int32_t fd, int64_t offset);
long wali_syscall_mprotect (wasm_exec_env_t exec_env, WasmMemAddr addr, uint32_t len, int32_t prot);
long wali_syscall_munmap (wasm_exec_env_t exec_env, WasmMemAddr addr, uint32_t len);
long wali_syscall_brk (wasm_exec_env_t exec_env, WasmMemAddr addr);
long wali_syscall_rt_sigaction (wasm_exec_env_t exec_env, int32_t signum, WasmMemAddr act, WasmMemAddr oldact, uint32_t sigsetsize);
long wali_syscall_rt_sigprocmask (wasm_exec_env_t exec_env, int32_t how, WasmMemAddr set, WasmMemAddr oldset, uint32_t sigsetsize);
long wali_syscall_rt_sigreturn (wasm_exec_env_t exec_env, int64_t unused);
long wali_syscall_ioctl (wasm_exec_env_t exec_env, int32_t fd, int32_t request, WasmMemAddr argp);
long wali_syscall_pread64 (wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr buf, uint32_t count, int64_t offset);
long wali_syscall_pwrite64 (wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr buf, uint32_t count, int64_t offset);
long wali_syscall_readv (wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr iov, int32_t iovcnt);
long wali_syscall_writev (wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr iov, int32_t iovcnt);
long wali_syscall_access (wasm_exec_env_t exec_env, WasmMemAddr pathname, int32_t mode);
long wali_syscall_pipe (wasm_exec_env_t exec_env, WasmMemAddr pipefd);
long wali_syscall_select (wasm_exec_env_t exec_env, int32_t nfds, WasmMemAddr readfds, WasmMemAddr writefds, WasmMemAddr exceptfds, WasmMemAddr timeout);
long wali_syscall_sched_yield (wasm_exec_env_t exec_env);
long wali_syscall_mremap (wasm_exec_env_t exec_env, WasmMemAddr old_address, uint32_t old_size, uint32_t new_size, int32_t flags, WasmMemAddr new_address);
long wali_syscall_msync (wasm_exec_env_t exec_env, WasmMemAddr addr, uint32_t length, int32_t flags);
long wali_syscall_madvise (wasm_exec_env_t exec_env, WasmMemAddr addr, uint32_t length, int32_t advice);
long wali_syscall_dup (wasm_exec_env_t exec_env, int32_t oldfd);
long wali_syscall_dup2 (wasm_exec_env_t exec_env, int32_t oldfd, int32_t newfd);
long wali_syscall_nanosleep (wasm_exec_env_t exec_env, WasmMemAddr req, WasmMemAddr rem);
long wali_syscall_setitimer (wasm_exec_env_t exec_env, int32_t which, WasmMemAddr new_value, WasmMemAddr old_value);
long wali_syscall_getpid (wasm_exec_env_t exec_env);
long wali_syscall_socket (wasm_exec_env_t exec_env, int32_t domain, int32_t type, int32_t protocol);
long wali_syscall_connect (wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr addr, uint32_t addrlen);
long wali_syscall_accept (wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr addr, WasmMemAddr addrlen);
long wali_syscall_sendto (wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr buf, uint32_t len, int32_t flags, WasmMemAddr dest_addr, uint32_t addrlen);
long wali_syscall_recvfrom (wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr buf, uint32_t len, int32_t flags, WasmMemAddr src_addr, WasmMemAddr addrlen);
long wali_syscall_sendmsg (wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr msg, int32_t flags);
long wali_syscall_recvmsg (wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr msg, int32_t flags);
long wali_syscall_shutdown (wasm_exec_env_t exec_env, int32_t sockfd, int32_t how);
long wali_syscall_bind (wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr addr, uint32_t addrlen);
long wali_syscall_listen (wasm_exec_env_t exec_env, int32_t sockfd, int32_t backlog);
long wali_syscall_getsockname (wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr addr, WasmMemAddr addrlen);
long wali_syscall_getpeername (wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr addr, WasmMemAddr addrlen);
long wali_syscall_socketpair (wasm_exec_env_t exec_env, int32_t domain, int32_t type, int32_t protocol, WasmMemAddr sv);
long wali_syscall_setsockopt (wasm_exec_env_t exec_env, int32_t sockfd, int32_t level, int32_t optname, WasmMemAddr optval, uint32_t optlen);
long wali_syscall_getsockopt (wasm_exec_env_t exec_env, int32_t sockfd, int32_t level, int32_t optname, WasmMemAddr optval, WasmMemAddr optlen);
long wali_syscall_fork (wasm_exec_env_t exec_env);
long wali_syscall_execve (wasm_exec_env_t exec_env, WasmMemAddr pathname, WasmMemAddr argv, WasmMemAddr envp);
long wali_syscall_exit (wasm_exec_env_t exec_env, int32_t status);
long wali_syscall_wait4 (wasm_exec_env_t exec_env, int32_t pid, WasmMemAddr wstatus, int32_t options, WasmMemAddr rusage);
long wali_syscall_kill (wasm_exec_env_t exec_env, int32_t pid, int32_t sig);
long wali_syscall_uname (wasm_exec_env_t exec_env, WasmMemAddr buf);
long wali_syscall_fcntl (wasm_exec_env_t exec_env, int32_t fd, int32_t cmd, uint64_t arg);
long wali_syscall_flock (wasm_exec_env_t exec_env, int32_t fd, int32_t operation);
long wali_syscall_fsync (wasm_exec_env_t exec_env, int32_t fd);
long wali_syscall_fdatasync (wasm_exec_env_t exec_env, int32_t fd);
long wali_syscall_ftruncate (wasm_exec_env_t exec_env, int32_t fd, int64_t length);
long wali_syscall_getcwd (wasm_exec_env_t exec_env, WasmMemAddr buf, uint32_t size);
long wali_syscall_chdir (wasm_exec_env_t exec_env, WasmMemAddr path);
long wali_syscall_fchdir (wasm_exec_env_t exec_env, int32_t fd);
long wali_syscall_rename (wasm_exec_env_t exec_env, WasmMemAddr oldpath, WasmMemAddr newpath);
long wali_syscall_mkdir (wasm_exec_env_t exec_env, WasmMemAddr pathname, int32_t mode);
long wali_syscall_rmdir (wasm_exec_env_t exec_env, WasmMemAddr pathname);
long wali_syscall_link (wasm_exec_env_t exec_env, WasmMemAddr oldpath, WasmMemAddr newpath);
long wali_syscall_unlink (wasm_exec_env_t exec_env, WasmMemAddr pathname);
long wali_syscall_symlink (wasm_exec_env_t exec_env, WasmMemAddr target, WasmMemAddr linkpath);
long wali_syscall_readlink (wasm_exec_env_t exec_env, WasmMemAddr pathname, WasmMemAddr buf, uint32_t bufsiz);
long wali_syscall_chmod (wasm_exec_env_t exec_env, WasmMemAddr pathname, int32_t mode);
long wali_syscall_fchmod (wasm_exec_env_t exec_env, int32_t fd, int32_t mode);
long wali_syscall_chown (wasm_exec_env_t exec_env, WasmMemAddr pathname, int32_t owner, int32_t group);
long wali_syscall_fchown (wasm_exec_env_t exec_env, int32_t fd, int32_t owner, int32_t group);
long wali_syscall_umask (wasm_exec_env_t exec_env, int32_t mask);
long wali_syscall_gettimeofday (wasm_exec_env_t exec_env, WasmMemAddr tv, WasmMemAddr tz);
long wali_syscall_getrlimit (wasm_exec_env_t exec_env, int32_t resource, WasmMemAddr rlim);
long wali_syscall_getrusage (wasm_exec_env_t exec_env, int32_t who, WasmMemAddr usage);
long wali_syscall_sysinfo (wasm_exec_env_t exec_env, WasmMemAddr info);
long wali_syscall_getuid (wasm_exec_env_t exec_env);
long wali_syscall_getgid (wasm_exec_env_t exec_env);
long wali_syscall_setuid (wasm_exec_env_t exec_env, int32_t uid);
long wali_syscall_setgid (wasm_exec_env_t exec_env, int32_t gid);
long wali_syscall_geteuid (wasm_exec_env_t exec_env);
long wali_syscall_getegid (wasm_exec_env_t exec_env);
long wali_syscall_setpgid (wasm_exec_env_t exec_env, int32_t pid, int32_t pgid);
long wali_syscall_getppid (wasm_exec_env_t exec_env);
long wali_syscall_setsid (wasm_exec_env_t exec_env);
long wali_syscall_setreuid (wasm_exec_env_t exec_env, int32_t ruid, int32_t euid);
long wali_syscall_setregid (wasm_exec_env_t exec_env, int32_t rgid, int32_t egid);
long wali_syscall_getgroups (wasm_exec_env_t exec_env, uint32_t size, WasmMemAddr list);
long wali_syscall_setgroups (wasm_exec_env_t exec_env, uint32_t size, WasmMemAddr list);
long wali_syscall_setresuid (wasm_exec_env_t exec_env, int32_t ruid, int32_t euid, int32_t suid);
long wali_syscall_setresgid (wasm_exec_env_t exec_env, int32_t rgid, int32_t egid, int32_t sgid);
long wali_syscall_getpgid (wasm_exec_env_t exec_env, int32_t pid);
long wali_syscall_getsid (wasm_exec_env_t exec_env, int32_t pid);
long wali_syscall_rt_sigpending (wasm_exec_env_t exec_env, WasmMemAddr set, uint32_t sigsetsize);
long wali_syscall_rt_sigsuspend (wasm_exec_env_t exec_env, WasmMemAddr mask, uint32_t sigsetsize);
long wali_syscall_sigaltstack (wasm_exec_env_t exec_env, WasmMemAddr ss, WasmMemAddr old_ss);
long wali_syscall_statfs (wasm_exec_env_t exec_env, WasmMemAddr path, WasmMemAddr buf);
long wali_syscall_fstatfs (wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr buf);
long wali_syscall_prctl (wasm_exec_env_t exec_env, int32_t option, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);
long wali_syscall_setrlimit (wasm_exec_env_t exec_env, int32_t resource, WasmMemAddr rlim);
long wali_syscall_chroot (wasm_exec_env_t exec_env, WasmMemAddr path);
long wali_syscall_gettid (wasm_exec_env_t exec_env);
long wali_syscall_tkill (wasm_exec_env_t exec_env, int32_t tid, int32_t sig);
long wali_syscall_futex (wasm_exec_env_t exec_env, WasmMemAddr uaddr, int32_t futex_op, int32_t val, WasmMemAddr timeout, WasmMemAddr uaddr2, int32_t val3);
long wali_syscall_sched_getaffinity (wasm_exec_env_t exec_env, int32_t pid, uint32_t cpusetsize, WasmMemAddr mask);
long wali_syscall_getdents64 (wasm_exec_env_t exec_env, int32_t fd, WasmMemAddr dirp, int32_t count);
long wali_syscall_set_tid_address (wasm_exec_env_t exec_env, WasmMemAddr tidptr);
long wali_syscall_fadvise (wasm_exec_env_t exec_env, int32_t fd, int64_t offset, int64_t len, int32_t advice);
long wali_syscall_clock_gettime (wasm_exec_env_t exec_env, int32_t clockid, WasmMemAddr tp);
long wali_syscall_clock_getres (wasm_exec_env_t exec_env, int32_t clockid, WasmMemAddr res);
long wali_syscall_clock_nanosleep (wasm_exec_env_t exec_env, int32_t clockid, int32_t flags, WasmMemAddr request, WasmMemAddr remain);
long wali_syscall_exit_group (wasm_exec_env_t exec_env, int32_t status);
long wali_syscall_epoll_ctl (wasm_exec_env_t exec_env, int32_t epfd, int32_t op, int32_t fd, WasmMemAddr event);
long wali_syscall_openat (wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t flags, int32_t mode);
long wali_syscall_mkdirat (wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t mode);
long wali_syscall_fchownat (wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t owner, int32_t group, int32_t flags);
long wali_syscall_newfstatat (wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, WasmMemAddr statbuf, int32_t flags);
long wali_syscall_unlinkat (wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t flags);
long wali_syscall_linkat (wasm_exec_env_t exec_env, int32_t olddirfd, WasmMemAddr oldpath, int32_t newdirfd, WasmMemAddr newpath, int32_t flags);
long wali_syscall_symlinkat (wasm_exec_env_t exec_env, WasmMemAddr target, int32_t newdirfd, WasmMemAddr linkpath);
long wali_syscall_readlinkat (wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, WasmMemAddr buf, uint32_t bufsiz);
long wali_syscall_fchmodat (wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t mode, int32_t flags);
long wali_syscall_faccessat (wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t mode, int32_t flags);
long wali_syscall_pselect6 (wasm_exec_env_t exec_env, int32_t nfds, WasmMemAddr readfds, WasmMemAddr writefds, WasmMemAddr exceptfds, WasmMemAddr timeout, WasmMemAddr sigmask);
long wali_syscall_ppoll (wasm_exec_env_t exec_env, WasmMemAddr fds, uint64_t nfds, WasmMemAddr tmo_p, WasmMemAddr sigmask, uint32_t sigsetsize);
long wali_syscall_utimensat (wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, WasmMemAddr times, int32_t flags);
long wali_syscall_epoll_pwait (wasm_exec_env_t exec_env, int32_t epfd, WasmMemAddr events, int32_t maxevents, int32_t timeout, WasmMemAddr sigmask, uint32_t sigsetsize);
long wali_syscall_eventfd (wasm_exec_env_t exec_env, int32_t initval);
long wali_syscall_accept4 (wasm_exec_env_t exec_env, int32_t sockfd, WasmMemAddr addr, WasmMemAddr addrlen, int32_t flags);
long wali_syscall_eventfd2 (wasm_exec_env_t exec_env, int32_t initval, int32_t flags);
long wali_syscall_epoll_create1 (wasm_exec_env_t exec_env, int32_t flags);
long wali_syscall_dup3 (wasm_exec_env_t exec_env, int32_t oldfd, int32_t newfd, int32_t flags);
long wali_syscall_pipe2 (wasm_exec_env_t exec_env, WasmMemAddr pipefd, int32_t flags);
long wali_syscall_prlimit64 (wasm_exec_env_t exec_env, int32_t pid, int32_t resource, WasmMemAddr new_limit, WasmMemAddr old_limit);
long wali_syscall_renameat2 (wasm_exec_env_t exec_env, int32_t olddirfd, WasmMemAddr oldpath, int32_t newdirfd, WasmMemAddr newpath, int32_t flags);
long wali_syscall_getrandom (wasm_exec_env_t exec_env, WasmMemAddr buf, uint32_t buflen, int32_t flags);
long wali_syscall_statx (wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t flags, uint32_t mask, WasmMemAddr statxbuf);
long wali_syscall_faccessat2 (wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t mode, int32_t flags);

/* Auxiliary calls */
int wali_init (wasm_exec_env_t exec_env);
int wali_deinit (wasm_exec_env_t exec_env);
void wali_proc_exit (wasm_exec_env_t exec_env, int32_t status);
unsigned int wali_cl_get_argc (wasm_exec_env_t exec_env);
unsigned int wali_cl_get_argv_len (wasm_exec_env_t exec_env, uint32_t arg_index);
int wali_cl_copy_argv (wasm_exec_env_t exec_env, WasmMemAddr argbuf, uint32_t arg_index);
int wali_get_init_envfile (wasm_exec_env_t exec_env, WasmMemAddr pathbuf, uint32_t bufsize);
int wali_wasm_thread_spawn (wasm_exec_env_t exec_env, WasmTableInternalIdx wasm_start_fn, WasmMemAddr args);
int wali_sigsetjmp (wasm_exec_env_t exec_env, WasmMemAddr sigjmp_buf, int32_t savesigs);
void wali_longjmp (wasm_exec_env_t exec_env, WasmMemAddr env, int32_t val);
int wali_setjmp (wasm_exec_env_t exec_env, WasmMemAddr env);


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

#endif
