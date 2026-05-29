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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "wali.h"
#include "copy.h"
#include "syscall.h"
#include "impl.h"

// BufPtr methods
BufPtr wasm_bp(wasm_exec_env_t exec_env, WasmMemAddr bp) {
    return (BufPtr) { .val = { .wasm_ptr = bp }, .ctx = WasmPtr, .env = exec_env };
}
BufPtr native_bp(wasm_exec_env_t exec_env, Addr bp) {
    return (BufPtr) { .val = { .ptr = bp }, .ctx = NativePtr, .env = exec_env };
}
long bp_as_native(BufPtr bp) {
    return (long) ((bp.ctx == WasmPtr) ? addr_wasm2native(bp.env, bp.val.wasm_ptr) : bp.val.ptr);
}
WasmMemAddr bp_as_wasm(BufPtr bp) {
    return (bp.ctx == WasmPtr) ? bp.val.wasm_ptr : addr_native2wasm(bp.env, bp.val.ptr);
}


long newfstatat_impl(wasm_exec_env_t exec_env, int32_t dirfd, BufPtr pathname, WasmMemAddr statbuf, int32_t flags) {
#if __x86_64__
    return __syscall4(SYS_newfstatat, dirfd, bp_as_native(pathname), addr_wasm2native(exec_env, statbuf), flags);
#elif __aarch64__ || __riscv64__
    Addr wasm_stat = addr_wasm2native(exec_env, statbuf);
    struct stat sb;
    long retval = __syscall4(SYS_newfstatat, dirfd, bp_as_native(pathname), &sb, flags);
    copy2wasm_stat_struct(exec_env, wasm_stat, &sb);
    return retval;
#endif
}

long ppoll_impl(wasm_exec_env_t exec_env, WasmMemAddr fds, uint64_t nfds, BufPtr tmo_p, WasmMemAddr sigmask, uint32_t sigsetsize) {
    return __syscall5(SYS_ppoll, addr_wasm2native(exec_env, fds), nfds, bp_as_native(tmo_p), addr_wasm2native(exec_env, sigmask), sigsetsize);
}

long fcntl_impl(wasm_exec_env_t exec_env, int32_t fd, int32_t cmd, uint64_t arg) {
    /* Swap open flags only on F_GETFL and F_SETFL mode for aarch64 */
    switch (cmd) {
#if __aarch64__
        case F_GETFL:
            return swap_open_flags(__syscall3(SYS_fcntl, fd, cmd, arg));
        case F_SETFL:
            return __syscall3(SYS_fcntl, fd, cmd, swap_open_flags(arg));
#endif
        case F_GETLK:
        case F_SETLK:
        case F_GETOWN_EX:
        case F_SETOWN_EX:
            return __syscall3(SYS_fcntl, fd, cmd, addr_wasm2native(exec_env, arg));
        default:
            return __syscall3(SYS_fcntl, fd, cmd, arg);
    }
}

long openat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t flags, int32_t mode) {
    // security check
    if (strncmp((char *)addr_wasm2native(exec_env, pathname), "/proc/self/mem", 15) == 0) {
        printf("Unpermitted attempt to open /proc/self/mem.");
        return -1;
    }
#if __aarch64__
    return __syscall4(SYS_openat, dirfd, addr_wasm2native(exec_env, pathname), swap_open_flags(flags), mode);
#else
    return __syscall4(SYS_openat, dirfd, addr_wasm2native(exec_env, pathname), flags, mode);
#endif
}

long mkdirat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t mode) {
    return __syscall3(SYS_mkdirat, dirfd, addr_wasm2native(exec_env, pathname), mode);
}

long fchownat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t owner, int32_t group, int32_t flags) {
    return __syscall5(SYS_fchownat, dirfd, addr_wasm2native(exec_env, pathname), owner, group, flags);
}

long unlinkat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t flags) {
    return __syscall3(SYS_unlinkat, dirfd, addr_wasm2native(exec_env, pathname), flags);
}

long linkat_impl(wasm_exec_env_t exec_env, int32_t olddirfd, WasmMemAddr oldpath, int32_t newdirfd, WasmMemAddr newpath, int32_t flags) {
    return __syscall5(SYS_linkat, olddirfd, addr_wasm2native(exec_env, oldpath), newdirfd, addr_wasm2native(exec_env, newpath), flags);
}

long symlinkat_impl(wasm_exec_env_t exec_env, WasmMemAddr target, int32_t newdirfd, WasmMemAddr linkpath) {
    return __syscall3(SYS_symlinkat, addr_wasm2native(exec_env, target), newdirfd, addr_wasm2native(exec_env, linkpath));
}

long readlinkat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, WasmMemAddr buf, uint32_t bufsiz) {
    return __syscall4(SYS_readlinkat, dirfd, addr_wasm2native(exec_env, pathname), addr_wasm2native(exec_env, buf), bufsiz);
}

long fchmodat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t mode, int32_t flags) {
    return __syscall4(SYS_fchmodat, dirfd, addr_wasm2native(exec_env, pathname), mode, flags);
}

long faccessat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t mode, int32_t flags) {
    return __syscall4(SYS_faccessat, dirfd, addr_wasm2native(exec_env, pathname), mode, flags);
}

long pselect6_impl(wasm_exec_env_t exec_env, int32_t nfds, WasmMemAddr readfds, WasmMemAddr writefds, WasmMemAddr exceptfds, WasmMemAddr timeout, WasmMemAddr sigmask) {
    VERB("pselect args | nfds: %ld, readfds: %ld, writefds: %ld, exceptfds: %ld, timeout: %ld, sigmask: %ld",
       nfds, readfds, writefds, exceptfds, timeout, sigmask);
    long *sm_struct_ptr = copy_pselect6_sigmask((long[]){0, 0}, exec_env, sigmask);
    return __syscall6(SYS_pselect6, nfds, addr_wasm2native(exec_env, readfds), addr_wasm2native(exec_env, writefds), addr_wasm2native(exec_env, exceptfds),
                      addr_wasm2native(exec_env, timeout), sm_struct_ptr);
}

long eventfd2_impl(wasm_exec_env_t exec_env, int32_t initval, int32_t flags) {
    // TODO: Support all flags for all ISAs
    ERR_SC(eventfd2, "Only supports basic flags for now; so many be incorrect in some cases...");
    return __syscall2(SYS_eventfd2, initval, flags);
}

long dup3_impl(wasm_exec_env_t exec_env, int32_t oldfd, int32_t newfd, int32_t flags) {
#if __aarch64__
    return __syscall3(SYS_dup3, oldfd, newfd, swap_open_flags(flags));
#else
    return __syscall3(SYS_dup3, oldfd, newfd, flags);
#endif
}

long pipe2_impl(wasm_exec_env_t exec_env, WasmMemAddr pipefd, int32_t flags) {
#if __aarch64__
    return __syscall2(SYS_pipe2, addr_wasm2native(exec_env, pipefd), swap_open_flags(flags));
#else
    return __syscall2(SYS_pipe2, addr_wasm2native(exec_env, pipefd), flags);
#endif
}

long renameat2_impl(wasm_exec_env_t exec_env, int32_t olddirfd, WasmMemAddr oldpath, int32_t newdirfd, WasmMemAddr newpath, int32_t flags) {
    return __syscall5(SYS_renameat2, olddirfd, addr_wasm2native(exec_env, oldpath), newdirfd, addr_wasm2native(exec_env, newpath), flags);
}
