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

long newfstatat_impl(wasm_exec_env_t exec_env, int32_t dirfd, BufPtr pathname, WasmMemAddr statbuf, int32_t flags) {
#if __x86_64__
    return __syscall4(SYS_newfstatat, dirfd, bp_as_native(pathname), MADDR(statbuf), flags);
#elif __aarch64__ || __riscv64__
    Addr wasm_stat = MADDR(statbuf);
    struct stat sb;
    long retval = __syscall4(SYS_newfstatat, dirfd, bp_as_native(pathname), &sb, flags);
    copy2wasm_stat_struct(exec_env, wasm_stat, &sb);
    return retval;
#endif
}

long ppoll_impl(wasm_exec_env_t exec_env, WasmMemAddr fds, uint64_t nfds, BufPtr tmo_p, WasmMemAddr sigmask, uint32_t sigsetsize) {
    return __syscall5(SYS_ppoll, MADDR(fds), nfds, bp_as_native(tmo_p), MADDR(sigmask), sigsetsize);
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
            return __syscall3(SYS_fcntl, fd, cmd, MADDR(arg));
        default:
            return __syscall3(SYS_fcntl, fd, cmd, arg);
    }
}

long openat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t flags, int32_t mode) {
    // security check
    if (strncmp((char *)MADDR(pathname), "/proc/self/mem", 15) == 0) {
        printf("Unpermitted attempt to open /proc/self/mem.");
        return -1;
    }
#if __aarch64__
    return __syscall4(SYS_openat, dirfd, MADDR(pathname), swap_open_flags(flags), mode);
#else
    return __syscall4(SYS_openat, dirfd, MADDR(pathname), flags, mode);
#endif
}

long mkdirat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t mode) {
    return __syscall3(SYS_mkdirat, dirfd, MADDR(pathname), mode);
}

long fchownat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t owner, int32_t group, int32_t flags) {
    return __syscall5(SYS_fchownat, dirfd, MADDR(pathname), owner, group, flags);
}

long unlinkat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t flags) {
    return __syscall3(SYS_unlinkat, dirfd, MADDR(pathname), flags);
}

long linkat_impl(wasm_exec_env_t exec_env, int32_t olddirfd, WasmMemAddr oldpath, int32_t newdirfd, WasmMemAddr newpath, int32_t flags) {
    return __syscall5(SYS_linkat, olddirfd, MADDR(oldpath), newdirfd, MADDR(newpath), flags);
}

long symlinkat_impl(wasm_exec_env_t exec_env, WasmMemAddr target, int32_t newdirfd, WasmMemAddr linkpath) {
    return __syscall3(SYS_symlinkat, MADDR(target), newdirfd, MADDR(linkpath));
}

long readlinkat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, WasmMemAddr buf, uint32_t bufsiz) {
    return __syscall4(SYS_readlinkat, dirfd, MADDR(pathname), MADDR(buf), bufsiz);
}

long fchmodat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t mode, int32_t flags) {
    return __syscall4(SYS_fchmodat, dirfd, MADDR(pathname), mode, flags);
}

long faccessat_impl(wasm_exec_env_t exec_env, int32_t dirfd, WasmMemAddr pathname, int32_t mode, int32_t flags) {
    return __syscall4(SYS_faccessat, dirfd, MADDR(pathname), mode, flags);
}

long pselect6_impl(wasm_exec_env_t exec_env, int32_t nfds, WasmMemAddr readfds, WasmMemAddr writefds, WasmMemAddr exceptfds, WasmMemAddr timeout, WasmMemAddr sigmask) {
    VB("pselect args | nfds: %ld, readfds: %ld, writefds: %ld, exceptfds: %ld, timeout: %ld, sigmask: %ld",
       nfds, readfds, writefds, exceptfds, timeout, sigmask);
    Addr wasm_psel_sm = MADDR(sigmask);
    long sm_struct[2];
    long *sm_struct_ptr =
        copy_pselect6_sigmask(exec_env, wasm_psel_sm, sm_struct);
    return __syscall6(SYS_pselect6, nfds, MADDR(readfds), MADDR(writefds), MADDR(exceptfds),
                      MADDR(timeout), sm_struct_ptr);
}

long eventfd2_impl(wasm_exec_env_t exec_env, int32_t initval, int32_t flags) {
    // TODO: Support all flags for all ISAs
    MIS_SC(eventfd2);
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
    return __syscall2(SYS_pipe2, MADDR(pipefd), swap_open_flags(flags));
#else
    return __syscall2(SYS_pipe2, MADDR(pipefd), flags);
#endif
}

long renameat2_impl(wasm_exec_env_t exec_env, int32_t olddirfd, WasmMemAddr oldpath, int32_t newdirfd, WasmMemAddr newpath, int32_t flags) {
    return __syscall5(SYS_renameat2, olddirfd, MADDR(oldpath), newdirfd, MADDR(newpath), flags);
}
