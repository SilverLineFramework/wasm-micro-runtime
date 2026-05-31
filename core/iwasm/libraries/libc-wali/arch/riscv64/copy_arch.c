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

#include "copy_arch.h"

/* Copy for differing `struct stat` */
void
copy2wasm_stat_struct(wasm_exec_env_t exec_env, WasmMemAddr wasm_stat,
                      struct stat *native_stat)
{

    if (native_stat == NULL) {
        return;
    }
    CopyCtx cc = ctx(exec_env, native_stat, wasm_stat);
    
    cp_n2w(&cc, sizeof(uint64_t)); // st_dev
    cp_n2w(&cc, sizeof(uint64_t)); // st_ino

    // nlink_t: 64-bit in Wasm, but 32-bit in native
    uint64_t* wasm_nlink = (uint64_t*)cp_skip_wasm(&cc, sizeof(uint64_t)); // st_nlink (wasm) 

    cp_n2w(&cc, sizeof(uint32_t)); // st_mode
    *wasm_nlink = *(uint32_t*)cp_skip_native(&cc, sizeof(uint32_t)); // st_nlink (native; write2wasm)

    cp_n2w(&cc, sizeof(uint32_t)); // st_uid
    cp_n2w(&cc, sizeof(uint32_t)); // st_gid

    cp_skip_wasm(&cc, sizeof(uint32_t)); // pad0 (wasm)

    cp_n2w(&cc, sizeof(uint64_t)); // st_rdev 
    cp_skip_native(&cc, sizeof(uint64_t)); // pad (native; skip)

    cp_n2w(&cc, sizeof(uint64_t)); // st_size

    // st_blksize: 64-bit in Wasm, but 32-bit in native
    *(uint64_t*)cp_skip_wasm(&cc, sizeof(uint64_t)) = *(uint32_t*) cp_skip_native(&cc, sizeof(uint32_t));
    cp_skip_native(&cc, sizeof(uint32_t)); // pad (native; skip)

    cp_n2w(&cc, sizeof(uint64_t)); // st_blocks
    cp_n2w(&cc, sizeof(struct timespec)); // st_atim
    cp_n2w(&cc, sizeof(struct timespec)); // st_mtim
    cp_n2w(&cc, sizeof(struct timespec)); // st_ctim

    // unused
    cp_skip_wasm(&cc, sizeof(uint64_t[3]));
    cp_skip_native(&cc, sizeof(unsigned[2]));

    //WR_FIELD(wasm_stat, n_stat->st_dev, uint64_t);
    //WR_FIELD(wasm_stat, n_stat->st_ino, uint64_t);

    //long nlink = n_stat->st_nlink;
    //WR_FIELD(wasm_stat, nlink, uint64_t);
    //WR_FIELD(wasm_stat, n_stat->st_mode, uint32_t);

    //WR_FIELD(wasm_stat, n_stat->st_uid, uint32_t);
    //WR_FIELD(wasm_stat, n_stat->st_gid, uint32_t);
    //int pad = 0;
    //WR_FIELD(wasm_stat, pad, uint32_t); // Pad
    //WR_FIELD(wasm_stat, n_stat->st_rdev, uint64_t);
    //WR_FIELD(wasm_stat, n_stat->st_size, uint64_t);

    //long blksize = n_stat->st_blksize;
    //WR_FIELD(wasm_stat, blksize, uint64_t);
    //WR_FIELD(wasm_stat, n_stat->st_blocks, uint64_t);

    //WR_FIELD(wasm_stat, n_stat->st_atim, struct timespec);
    //WR_FIELD(wasm_stat, n_stat->st_mtim, struct timespec);
    //WR_FIELD(wasm_stat, n_stat->st_ctim, struct timespec);
    assert_cp_size(&cc, sizeof(struct stat), 144);
}
