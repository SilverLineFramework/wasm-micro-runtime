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

/** Memory Copy Macros **/
#define WR_FIELD(wptr, val, ty)         \
    ({                                  \
        memcpy(wptr, &val, sizeof(ty)); \
        wptr += sizeof(ty);             \
    })

#define WR_FIELD_ADDR(wptr, nptr)              \
    ({                                         \
        uint32_t wasm_addr = addr_native2wasm(exec_env, nptr);      \
        if (!wasm_addr) {                      \
            VERB("NULL Wasm Address generated"); \
        }                                      \
        WR_FIELD(wptr, wasm_addr, uint32_t);   \
    })

#define WR_FIELD_ARRAY(wptr, narr, ty, num)   \
    ({                                        \
        memcpy(wptr, narr, sizeof(ty) * num); \
        wptr += (sizeof(ty) * num);           \
    })

#define RD_FIELD(ptr, ty)              \
    ({                                 \
        ty val;                        \
        memcpy(&val, ptr, sizeof(ty)); \
        ptr += sizeof(ty);             \
        val;                           \
    })

#define RD_FIELD_ADDR(ptr)                        \
    ({                                            \
        uint32_t field = RD_FIELD(ptr, uint32_t); \
        addr_wasm2native(exec_env, field);                             \
    })

#define RD_FIELD_ARRAY(dest, ptr, ty, num)    \
    ({                                        \
        memcpy(&dest, ptr, sizeof(ty) * num); \
        ptr += (sizeof(ty) * num);            \
    })
/** **/

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
copy_pselect6_sigmask(wasm_exec_env_t exec_env, Addr wasm_psel_sm,
                      long *sm_struct);

/* Copy iovec structure */
struct iovec *
copy_iovec(wasm_exec_env_t exec_env, Addr wasm_iov, int iov_cnt);

/* Copy epoll_event structure */
struct epoll_event *
copy_epoll_event(wasm_exec_env_t exec_env, Addr wasm_epoll,
                 struct epoll_event *n_epoll);

void
copy2wasm_epoll_event(wasm_exec_env_t exec_env, Addr wasm_epoll,
                      struct epoll_event *n_epoll);

/* Copy msghdr structure */
struct msghdr *
copy_msghdr(wasm_exec_env_t exec_env, Addr wasm_msghdr);

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
copy_sigstack(wasm_exec_env_t exec_env, Addr wasm_sigstack, stack_t *ss);

/* Copy native sigstack back to Wasm */
void
copy2wasm_sigstack(wasm_exec_env_t exec_env, Addr wasm_sigstack, stack_t *ss);

/* Copy array of strings (strings are not malloced) */
char **
copy_stringarr(wasm_exec_env_t exec_env, Addr wasm_arr);

/** Architecture-specific copies **/
#if __has_include("copy_arch.h")
#include "copy_arch.h"
#endif

#endif
