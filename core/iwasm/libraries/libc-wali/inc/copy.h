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

#ifndef WALI_COPY_H
#define WALI_COPY_H

#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/epoll.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <signal.h>
#include <setjmp.h>

#include "wali.h"
#include "exports.h"
#include "../interpreter/sigtable.h"

// ASM restorer function '__libc_restore_rt'.
extern void
__libc_restore_rt();

/* This is the structure used for the rt_sigaction syscall on supported archs */
struct k_sigaction {
    void (*handler)(int);
    unsigned long flags;
    void (*restorer)(void);
    unsigned mask[2];
};

/* Copy pselect6 sigmask structure */
void *
copy_pselect6_sigmask(long *sm_struct, wasm_exec_env_t exec_env, WasmMemAddr wasm_psel_sm);

/* Copy iovec structure */
struct iovec*
copy_iovec(struct iovec *native_iov, wasm_exec_env_t exec_env, WasmMemAddr wasm_iov, int iovcnt);

/* Copy epoll_event structure */
struct epoll_event *
copy_epoll_event(struct epoll_event *native_epoll, wasm_exec_env_t exec_env, WasmMemAddr wasm_epoll);

void
copy2wasm_epoll_event(wasm_exec_env_t exec_env, WasmMemAddr wasm_epoll, struct epoll_event *native_epoll);

/* Copy msghdr structure */
struct msghdr *
copy_msghdr(struct msghdr *msg, wasm_exec_env_t exec_env, WasmMemAddr wasm_msghdr);

/* Copy sigaction back to WASM */
void
copy2wasm_old_ksigaction(int signo, Addr wasm_act, struct k_sigaction *act);

/* Copy sigaction to native: Function pointers are padded */
struct k_sigaction *
copy_ksigaction(wasm_exec_env_t exec_env, Addr wasm_act,
                struct k_sigaction *act, void (*common_handler)(int),
                WasmFuncPtr *target_wasm_funcptr, char *debug_str);

/* Copy sigstack structure */
stack_t *
copy_sigstack(stack_t *ss, wasm_exec_env_t exec_env, WasmMemAddr wasm_sigstack);

/* Copy native sigstack back to Wasm */
void
copy2wasm_sigstack(wasm_exec_env_t exec_env, WasmMemAddr wasm_ss, stack_t *ss);

/* Copy array of strings (strings are not malloced) */
char **
copy_stringarr(wasm_exec_env_t exec_env, Addr wasm_arr);

/** Architecture-specific copies **/
#if __has_include("copy_arch.h")
#include "copy_arch.h"
#endif

#endif
