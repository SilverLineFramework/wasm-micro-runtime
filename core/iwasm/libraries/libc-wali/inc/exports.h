/*
  MIT License

  Copyright (c) [2026] [Arjun Ramesh]

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

#ifndef _WALI_EXPORTS_H_
#define _WALI_EXPORTS_H_

#include "defs.h"
#include "wasm_export.h"

Addr wasm_linear_memory_base(wasm_exec_env_t exec_env);

WasmMemAddr addr_native2wasm(wasm_exec_env_t exec_env, void* native_addr);

Addr addr_wasm2native(wasm_exec_env_t exec_env, WasmMemAddr wasm_addr);

void wasm_func_free(wasm_exec_env_t exec_env, wasm_function_inst_t func);

uint32_t get_current_memory_size(wasm_exec_env_t exec_env);

#endif