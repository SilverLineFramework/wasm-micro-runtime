#ifndef _WALI_IMPL_H_
#define _WALI_IMPL_H_

#include "wali.h"

/* Syscall macro wrappers — cast all args to long for the underlying inline
 * helpers in wali_arch/<arch>/syscall.h. Self-referential macros: the
 * preprocessor does not recurse, so each call resolves to the actual function.
 * `syscall.h` must be included before these macros are expanded. */
#define __syscall0(n) __syscall0(n)
#define __syscall1(n, a1) __syscall1(n, (long)a1)
#define __syscall2(n, a1, a2) __syscall2(n, (long)a1, (long)a2)
#define __syscall3(n, a1, a2, a3) __syscall3(n, (long)a1, (long)a2, (long)a3)
#define __syscall4(n, a1, a2, a3, a4) \
    __syscall4(n, (long)a1, (long)a2, (long)a3, (long)a4)
#define __syscall5(n, a1, a2, a3, a4, a5) \
    __syscall5(n, (long)a1, (long)a2, (long)a3, (long)a4, (long)a5)
#define __syscall6(n, a1, a2, a3, a4, a5, a6)                         \
    __syscall6(n, (long)a1, (long)a2, (long)a3, (long)a4, \
                           (long)a5, (long)a6)


/** Logging **/
#define SCSTR(sc) "[\033[1;36mwali\033[0m::\033[1;33m" #sc "\033[0m] "
#define WARN_SC(sc, ...) LOG_WARNING(SCSTR(sc) __VA_ARGS__);
#define ERR_SC(sc, ...)  LOG_ERROR(SCSTR(sc) __VA_ARGS__);
#define FATAL_SC(sc, ...) LOG_FATAL(SCSTR(sc) __VA_ARGS__);
#define MIS_SC(sc) FATAL_SC(sc, "Syscall non-existent or unsupported");

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

/* BufPtr methods */
static inline BufPtr wasm_bp(wasm_exec_env_t exec_env, WasmMemAddr bp) {
    return (BufPtr) { .val = (long) bp, .ctx = WasmPtr, .env = exec_env };
}
static inline BufPtr native_bp(wasm_exec_env_t exec_env, void* bp) {
    return (BufPtr) { .val = (long) bp, .ctx = NativePtr, .env = exec_env };
}
static inline long bp_as_native(BufPtr bp) {
    wasm_exec_env_t exec_env = bp.env;
    return (bp.ctx == WasmPtr) ? (long) addr_wasm2native(exec_env, bp.val) : bp.val;
}
static inline WasmMemAddr bp_as_wasm(BufPtr bp) {
    return (bp.ctx == WasmPtr) ? bp.val : addr_native2wasm(bp.env, (void*) bp.val);
}

/* Shared syscall implementations — called by the corresponding
 * `wali_syscall_X` wrapper as well as any aliases that delegate to it. */
long newfstatat_impl(wasm_exec_env_t exec_env, int32_t dirfd, BufPtr pathname, WasmMemAddr statbuf, int32_t flags);
long ppoll_impl(wasm_exec_env_t exec_env, WasmMemAddr fds, uint64_t nfds, BufPtr tmo_p, WasmMemAddr sigmask, uint32_t sigsetsize);
long fcntl_impl(wasm_exec_env_t exec_env, int32_t fd, int32_t cmd, uint64_t arg);
long openat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t flags, int32_t mode);
long mkdirat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t mode);
long fchownat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t owner, int32_t group, int32_t flags);
long unlinkat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t flags);
long linkat_impl(wasm_exec_env_t exec_env, int32_t olddirfd, WasmMemAddr oldpath, int32_t newdirfd, WasmMemAddr newpath, int32_t flags);
long symlinkat_impl(wasm_exec_env_t exec_env, WasmMemAddr target, int32_t newdirfd, WasmMemAddr linkpath);
long readlinkat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, WasmMemAddr buf, uint32_t bufsiz);
long fchmodat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t mode, int32_t flags);
long faccessat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t mode, int32_t flags);
long pselect6_impl(wasm_exec_env_t exec_env, int32_t nfds, WasmMemAddr readfds, WasmMemAddr writefds, WasmMemAddr exceptfds, WasmMemAddr timeout, WasmMemAddr sigmask);
long eventfd2_impl(wasm_exec_env_t exec_env, int32_t initval, int32_t flags);
long dup3_impl(wasm_exec_env_t exec_env, int32_t oldfd, int32_t newfd, int32_t flags);
long pipe2_impl(wasm_exec_env_t exec_env, WasmMemAddr pipefd, int32_t flags);
long renameat2_impl(wasm_exec_env_t exec_env, int32_t olddirfd, WasmMemAddr oldpath, int32_t newdirfd, WasmMemAddr newpath, int32_t flags);

#endif /* WALI_IMPL_H */
