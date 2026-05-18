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

#include "copy.h"

/* Copy pselect6 sigmask structure */
void *
copy_pselect6_sigmask(wasm_exec_env_t exec_env, Addr wasm_psel_sm,
                      long *sm_struct)
{
    /* Libc stores the address in a long (64-bit). Cannot use RD_FIELD_ADDR
     * since it reads 32-bit values */
    if (wasm_psel_sm == NULL) {
      return NULL;
    }
    long sigmask_addr = RD_FIELD(wasm_psel_sm, long);
    sm_struct[0] = (long)addr_wasm2native(exec_env, sigmask_addr);
    sm_struct[1] = RD_FIELD(wasm_psel_sm, long);
    return sm_struct;
}

/* Copy iovec structure */
struct iovec *
copy_iovec(wasm_exec_env_t exec_env, Addr wasm_iov, int iov_cnt)
{
    if (wasm_iov == NULL) {
        return NULL;
    }
    struct iovec *new_iov =
        (struct iovec *)malloc(iov_cnt * sizeof(struct iovec));
    for (int i = 0; i < iov_cnt; i++) {
        new_iov[i].iov_base = RD_FIELD_ADDR(wasm_iov);
        new_iov[i].iov_len = RD_FIELD(wasm_iov, int32_t);
    }
    return new_iov;
}

/* Copy epoll_event structure */
struct epoll_event *
copy_epoll_event(wasm_exec_env_t exec_env, Addr wasm_epoll,
                 struct epoll_event *n_epoll)
{
    if (wasm_epoll == NULL) {
        return NULL;
    }
    n_epoll->events = RD_FIELD(wasm_epoll, uint32_t);
    n_epoll->data.u64 = RD_FIELD(wasm_epoll, uint64_t);
    return n_epoll;
}

void
copy2wasm_epoll_event(wasm_exec_env_t exec_env, Addr wasm_epoll,
                      struct epoll_event *n_epoll)
{
    if (n_epoll == NULL) {
        return;
    }
    WR_FIELD(wasm_epoll, n_epoll->events, uint32_t);
    WR_FIELD(wasm_epoll, n_epoll->data.u64, uint64_t);
    return;
}

/* Copy msghdr structure */
struct msghdr *
copy_msghdr(wasm_exec_env_t exec_env, Addr wasm_msghdr)
{
    if (wasm_msghdr == NULL) {
        return NULL;
    }
    struct msghdr *msg = (struct msghdr *)malloc(sizeof(struct msghdr));
    msg->msg_name = RD_FIELD_ADDR(wasm_msghdr);
    msg->msg_namelen = RD_FIELD(wasm_msghdr, unsigned);

    Addr wasm_iov = RD_FIELD_ADDR(wasm_msghdr);
    msg->msg_iovlen = RD_FIELD(wasm_msghdr, int);

    RD_FIELD(wasm_msghdr, int); // pad1

    msg->msg_control = RD_FIELD_ADDR(wasm_msghdr);
    msg->msg_controllen = RD_FIELD(wasm_msghdr, unsigned);

    RD_FIELD(wasm_msghdr, int); // pad2
    msg->msg_flags = RD_FIELD(wasm_msghdr, int);

    msg->msg_iov = copy_iovec(exec_env, wasm_iov, msg->msg_iovlen);
    return msg;
}

/* Copy sigaction back to WASM */
void
copy2wasm_old_ksigaction(int signo, Addr wasm_act, struct k_sigaction *act)
{
    WasmFuncPtr old_wasm_funcptr;
    if (act->handler == SIG_DFL) {
        old_wasm_funcptr = WASM_SIG_DFL;
    }
    else if (act->handler == SIG_IGN) {
        old_wasm_funcptr = WASM_SIG_IGN;
    }
    else if (act->handler == SIG_ERR) {
        old_wasm_funcptr = WASM_SIG_ERR;
    }
    else {
        old_wasm_funcptr = wali_sigtable[signo].func_table_idx;
        VERB("Save old sigaction handler -- Tbl[%d]", old_wasm_funcptr);
    }
    WR_FIELD(wasm_act, old_wasm_funcptr, WasmFuncPtr);
    WR_FIELD(wasm_act, act->flags, unsigned long);
    WR_FIELD(wasm_act, act->restorer, WasmFuncPtr);
    WR_FIELD_ARRAY(wasm_act, act->mask, unsigned, 2);
}

/* Copy sigaction to native: Function pointers are padded */
struct k_sigaction *
copy_ksigaction(wasm_exec_env_t exec_env, Addr wasm_act,
                struct k_sigaction *act, void (*common_handler)(int),
                WasmFuncPtr *target_wasm_funcptr, char *debug_str)
{
    if (wasm_act == NULL) {
        return NULL;
    }

    WasmFuncPtr wasm_handler_funcptr = RD_FIELD(wasm_act, WasmFuncPtr);
    if (wasm_handler_funcptr == (WasmFuncPtr)(WASM_SIG_DFL)) {
        act->handler = SIG_DFL;
        strcpy(debug_str, "SIG_DFL");
    }
    else if (wasm_handler_funcptr == (WasmFuncPtr)(WASM_SIG_IGN)) {
        act->handler = SIG_IGN;
        strcpy(debug_str, "SIG_IGN");
    }
    else if (wasm_handler_funcptr == (WasmFuncPtr)(WASM_SIG_ERR)) {
        act->handler = SIG_ERR;
        strcpy(debug_str, "SIG_ERR");
    }
    else {
        /* Setup common handler */
        act->handler = common_handler;
        *target_wasm_funcptr = wasm_handler_funcptr;
        strcpy(debug_str, "Wasm SIG");
    }

    act->flags = RD_FIELD(wasm_act, unsigned long);

    RD_FIELD(wasm_act, WasmFuncPtr);
    act->restorer = __libc_restore_rt;

    RD_FIELD_ARRAY(act->mask, wasm_act, unsigned, 2);
    return act;
}

/* Copy sigstack structure */
stack_t *
copy_sigstack(wasm_exec_env_t exec_env, Addr wasm_sigstack, stack_t *ss)
{
    if (!wasm_sigstack) {
        return NULL;
    }
    ss->ss_sp = RD_FIELD_ADDR(wasm_sigstack);
    ss->ss_flags = RD_FIELD(wasm_sigstack, int);
    ss->ss_size = RD_FIELD(wasm_sigstack, uint32_t);
    return ss;
}

/* Copy native sigstack back to Wasm */
void
copy2wasm_sigstack(wasm_exec_env_t exec_env, Addr wasm_sigstack, stack_t *ss)
{
    if (!ss) {
        return;
    }
    WR_FIELD_ADDR(wasm_sigstack, ss->ss_sp);
    WR_FIELD(wasm_sigstack, ss->ss_flags, int);
    WR_FIELD(wasm_sigstack, ss->ss_size, uint32_t);
}

/* Copy array of strings (strings are not malloced) */
char **
copy_stringarr(wasm_exec_env_t exec_env, Addr wasm_arr)
{
    if (!wasm_arr) {
        return NULL;
    }
    int num_strings = 0;
    /* Find num elems */
    Addr arr_it = wasm_arr;
    char *str;
    while ((str = (char *)RD_FIELD_ADDR(arr_it))) {
        num_strings++;
    }
    char **stringarr = (char **)malloc((num_strings + 1) * sizeof(char *));
    for (int i = 0; i < num_strings; i++) {
        stringarr[i] = (char *)RD_FIELD_ADDR(wasm_arr);
    }
    stringarr[num_strings] = NULL;
    return stringarr;
}

/** Architecture-specific copies **/
#if __has_include("copy_arch.c")
#include "copy_arch.c"
#endif

