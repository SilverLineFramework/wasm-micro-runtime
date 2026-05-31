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

#include "exports.h"
#include "../interpreter/wasm_runtime.h"

Addr wasm_linear_memory_base(wasm_exec_env_t exec_env) {
    return wasm_runtime_addr_app_to_native(get_module_inst(exec_env), 0);
}

WasmMemAddr addr_native2wasm(wasm_exec_env_t exec_env, void* native_addr) {
    if (native_addr == NULL) {
        return 0;
    } else {
        return wasm_runtime_addr_native_to_app(get_module_inst(exec_env), native_addr);
    }
}

Addr addr_wasm2native(wasm_exec_env_t exec_env, WasmMemAddr wasm_addr) {
    if (wasm_addr == 0) {
        return NULL;
    } else {
        return wasm_runtime_addr_app_to_native(get_module_inst(exec_env), wasm_addr);
    }
}

void wasm_func_free(wasm_exec_env_t exec_env, wasm_function_inst_t func) {
    if (func && (get_module_inst(exec_env)->module_type == Wasm_Module_AoT)) {
        /* Needs to be called only for AoT when using wasm_runtime_get_indirect_function */
        wasm_runtime_free(func);
    }
}

uint32_t get_current_memory_size(wasm_exec_env_t exec_env) {
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    wasm_function_inst_t memorysize_fn =
        wasm_runtime_lookup_function(module_inst, "__wasm_memory_size");
    uint32_t cur_wasm_pages[1];
    uint32_t mem_size = 0;
    if (memorysize_fn
        && wasm_runtime_call_wasm(exec_env, memorysize_fn, 0, cur_wasm_pages)) {
        // Success
        VERB("Used \'__wasm_memory_size\' export for size query");
        mem_size = cur_wasm_pages[0] * WASM_PAGESIZE;
    }
    else {
        // Failure: Fallback to internal implementation
        mem_size = wasm_runtime_get_memory_size(get_module_inst(exec_env));
    }
    return mem_size;
}
