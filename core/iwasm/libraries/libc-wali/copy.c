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

typedef struct {
    wasm_exec_env_t env;
    // A native pointer to a native object
    Addr ptr;
    // A native pointer to a Wasm object
    Addr wasm_ptr;
    // Initial values of the above, for validating sizes
    Addr initial_ptr;
    Addr initial_wasm_ptr;
} CopyCtx;

CopyCtx ctx(wasm_exec_env_t env, Addr ptr, WasmMemAddr wasm_ptr) {
    Addr wasm_ptr_addr = addr_wasm2native(env, wasm_ptr);
    return (CopyCtx) { .env = env, .ptr = ptr, 
        .wasm_ptr = wasm_ptr_addr, .initial_ptr = ptr, 
        .initial_wasm_ptr = wasm_ptr_addr };
}

// A simple Wasm pointer increment when no copying is needed (e.g. for padding/unsupported fields)
void cp_skip_wasm(CopyCtx *ctx, size_t wasm_field_size) {
    ctx->wasm_ptr += wasm_field_size;
}

// A simple native pointer increment when no copying is needed (e.g. for padding/unsupported fields)
void cp_skip_native(CopyCtx *ctx, size_t native_field_size) {
    ctx->ptr += native_field_size;
}

void assert_cp_size(CopyCtx *ctx, size_t target_native_size, size_t target_wasm_size) {
    assert((size_t)(ctx->ptr - ctx->initial_ptr) == target_native_size);
    assert((size_t)(ctx->wasm_ptr - ctx->initial_wasm_ptr) == target_wasm_size);
}

// A wasm-to-native memory copy when the fields being copied match up
void cp_w2n(CopyCtx *ctx, size_t field_size) {
    memcpy(ctx->ptr, ctx->wasm_ptr, field_size);
    ctx->wasm_ptr += field_size;
    ctx->ptr += field_size;
}

// A wasm-to-native memory copy for fields with different sizes, with optional sign-extension
// Returns the original wasm field pointer.
void* cp_w2n_ext(CopyCtx *ctx, size_t native_field_size, size_t wasm_field_size, bool sext) {
    assert(native_field_size >= wasm_field_size);
    size_t common = wasm_field_size;
    memcpy(ctx->ptr, ctx->wasm_ptr, common);
    // Sign-extend: on LE, the MSB lives in the highest-address byte we just wrote.
    uint8_t fill = (sext && (((uint8_t*)ctx->ptr)[common - 1] & 0x80)) ? 0xFF : 0x00;
    memset(ctx->ptr + common, fill, native_field_size - common);
    void* ret = ctx->wasm_ptr;
    ctx->wasm_ptr += wasm_field_size;
    ctx->ptr += native_field_size;
    return ret;
}

// A wasm-to-native memory copy specialized for pointer/address fields
void cp_w2n_ptr(CopyCtx *ctx) {
    WasmMemAddr w;
    memcpy(&w, ctx->wasm_ptr, sizeof w);
    Addr native = addr_wasm2native(ctx->env, w);
    memcpy(ctx->ptr, &native, sizeof native);
    ctx->wasm_ptr += sizeof w;
    ctx->ptr += sizeof native;
}

// A wasm-to-native memory copy for pointer stored in a non-standard field size
void cp_w2n_ptr_align(CopyCtx *ctx, size_t wasm_field_size) {
    WasmMemAddr w;
    memcpy(&w, ctx->wasm_ptr, sizeof w);
    Addr native = addr_wasm2native(ctx->env, w);
    memcpy(ctx->ptr, &native, sizeof native);
    ctx->wasm_ptr += wasm_field_size;
    ctx->ptr += sizeof native;
}

// A native-to-wasm memory copy for fields with different sizes
void cp_n2w_ext(CopyCtx *ctx, size_t wasm_field_size, size_t native_field_size) {
    assert(native_field_size >= wasm_field_size);
    size_t common = wasm_field_size;
    memcpy(ctx->wasm_ptr, ctx->ptr, common);
    ctx->wasm_ptr += wasm_field_size;
    ctx->ptr += native_field_size;
}

// A native-to-wasm memory copy
void cp_n2w(CopyCtx *ctx, size_t field_size) {
    memcpy(ctx->wasm_ptr, ctx->ptr, field_size);
    ctx->ptr += field_size;
    ctx->wasm_ptr += field_size;
}

// A native-to-wasm memory copy specialized for pointer/address fields
void cp_n2w_ptr(CopyCtx *ctx) {
    Addr native;
    memcpy(&native, ctx->ptr, sizeof native);
    WasmMemAddr w = addr_native2wasm(ctx->env, native);
    memcpy(ctx->wasm_ptr, &w, sizeof w);
    ctx->ptr += sizeof native;
    ctx->wasm_ptr += sizeof w;
}



/** Memory Copy Macros **/
#define WR_FIELD(wptr, val, ty)         \
    ({                                  \
        memcpy(wptr, &val, sizeof(ty) ); \
        wptr += sizeof(ty) ;             \
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


/* Copy pselect6 sigmask structure */
void *
copy_pselect6_sigmask(long *sm_struct, wasm_exec_env_t exec_env, WasmMemAddr wasm_psel_sm)
{
    CopyCtx cc = ctx(exec_env, sm_struct, wasm_psel_sm);
    if (cc.wasm_ptr == NULL) {
      return NULL;
    }
    // The sigmask uses 64-bit aligned pointer
    cp_w2n_ptr_align(&cc, sizeof(uintptr_t));
    cp_w2n(&cc, sizeof(long));
    assert_cp_size(&cc, 16, 16);
    return sm_struct;
}

/* Copy iovec structure */
struct iovec*
copy_iovec(struct iovec *native_iov, wasm_exec_env_t exec_env, WasmMemAddr wasm_iov, int iovcnt)
{
    CopyCtx cc = ctx(exec_env, native_iov, wasm_iov);
    if (cc.wasm_ptr == NULL) {
        return NULL;
    }
    for (int i = 0; i < iovcnt; i++) {
        cp_w2n_ptr(&cc); // iov_base
        cp_w2n_ext(&cc, sizeof(size_t), sizeof(uint32_t), false); // iov_len
    }
    assert_cp_size(&cc, sizeof(struct iovec) * iovcnt, 8 * iovcnt);
    return native_iov;
}

/* Copy epoll_event structure */
struct epoll_event *
copy_epoll_event(struct epoll_event *native_epoll, wasm_exec_env_t exec_env, WasmMemAddr wasm_epoll)
{
    CopyCtx cc = ctx(exec_env, native_epoll, wasm_epoll);
    if (cc.wasm_ptr == NULL) {
        return NULL;
    }
#if __x86_64__
    cp_w2n(&cc, sizeof(uint32_t)); // events
#else
    cp_w2n_ext(&cc, sizeof(uint64_t), sizeof(uint32_t), false); // events
#endif
    cp_w2n(&cc, sizeof(uint64_t)); // data
    assert_cp_size(&cc, sizeof(struct epoll_event), 12);
    return native_epoll;
}

void
copy2wasm_epoll_event(wasm_exec_env_t exec_env, WasmMemAddr wasm_epoll, struct epoll_event *native_epoll)
{
    if (native_epoll == NULL) {
        return;
    }
    CopyCtx cc = ctx(exec_env, native_epoll, wasm_epoll);
#if __x86_64__
    cp_n2w(&cc, sizeof(uint32_t)); // events
#else
    cp_n2w_ext(&cc, sizeof(uint32_t), sizeof(uint64_t)); // events
#endif
    cp_n2w(&cc, sizeof(uint64_t)); // data
    return;
}

/* Copy msghdr structure */
struct msghdr *
copy_msghdr(struct msghdr *msg, wasm_exec_env_t exec_env, WasmMemAddr wasm_msghdr)
{
    CopyCtx cc = ctx(exec_env, msg, wasm_msghdr);
    if (cc.wasm_ptr == NULL) {
        return NULL;
    }
    cp_w2n_ptr(&cc); // msg_name
    cp_w2n_ext(&cc, sizeof(uint64_t), sizeof(unsigned), false); // msg_namelen (8-byte align)

    // msg_iov; don't use w2n_ptr since we need to copy
    WasmMemAddr iov = *((WasmMemAddr*)cp_w2n_ext(&cc, sizeof(void*), sizeof(WasmMemAddr), false)); 
    cp_w2n_ext(&cc, sizeof(size_t), sizeof(int), false); // msg_iovlen
    cp_skip_wasm(&cc, sizeof(int)); // pad1

    cp_w2n_ptr(&cc); // msg_control
    cp_w2n_ext(&cc, sizeof(size_t), sizeof(unsigned), false); // msg_controllen (8-byte align)    
    cp_skip_wasm(&cc, sizeof(int)); // pad2
    cp_w2n(&cc, sizeof(int)); // msg_flags
    cp_skip_native(&cc, 4); // Trailing 4-byte padding for 8-byte alignment in native

    msg->msg_iov = copy_iovec(malloc(msg->msg_iovlen * sizeof(struct iovec)), exec_env, iov, msg->msg_iovlen);
    assert_cp_size(&cc, sizeof(struct msghdr), 36);
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
    char **stringarr = (char **)malloc((num_strings + 1) * sizeof(char*));
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

