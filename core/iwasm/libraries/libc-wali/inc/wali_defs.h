#ifndef _WALI_DEFS_H_
#define _WALI_DEFS_H_

/** Architecture **/
#ifndef __riscv64__
#if __riscv
#if __riscv_xlen == 64
#define __riscv64__ 1
#endif
#endif
#endif

#if !__x86_64__ && !__aarch64__ && !__riscv64__
#error \
    "Unsupported architecture for WALI -- Only supports [x86_64, aarch64, riscv64]"
#endif

/** Memory defines **/
#define WASM_PAGESIZE 65536

/** Signal defines **/
#define WASM_SIG_DFL (0)
#define WASM_SIG_ERR (-1)
#define WASM_SIG_IGN (-2)

#define SIG_MEM_PROF 37
#define SIG_SYSCALL_PROF 38
#define SIG_WASM_THREAD_TERM 39

extern bool invoked_wali;
extern int wali_app_argc;
extern char **wali_app_argv;
extern char *wali_app_env_file;

#endif
