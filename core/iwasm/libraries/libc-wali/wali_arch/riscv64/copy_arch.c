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

#ifndef WALI_COPY_ARCH_H
#define WALI_COPY_ARCH_H

/* Copy for differing `struct stat` */
extern inline void
copy2wasm_stat_struct(wasm_exec_env_t exec_env, Addr wasm_stat,
                      struct stat *n_stat)
{
    if (n_stat == NULL) {
        return;
    }
    WR_FIELD(wasm_stat, n_stat->st_dev, uint64_t);
    WR_FIELD(wasm_stat, n_stat->st_ino, uint64_t);

    long nlink = n_stat->st_nlink;
    WR_FIELD(wasm_stat, nlink, uint64_t);
    WR_FIELD(wasm_stat, n_stat->st_mode, uint32_t);

    WR_FIELD(wasm_stat, n_stat->st_uid, uint32_t);
    WR_FIELD(wasm_stat, n_stat->st_gid, uint32_t);
    int pad = 0;
    WR_FIELD(wasm_stat, pad, uint32_t); // Pad
    WR_FIELD(wasm_stat, n_stat->st_rdev, uint64_t);
    WR_FIELD(wasm_stat, n_stat->st_size, uint64_t);

    long blksize = n_stat->st_blksize;
    WR_FIELD(wasm_stat, blksize, uint64_t);
    WR_FIELD(wasm_stat, n_stat->st_blocks, uint64_t);

    WR_FIELD(wasm_stat, n_stat->st_atim, struct timespec);
    WR_FIELD(wasm_stat, n_stat->st_mtim, struct timespec);
    WR_FIELD(wasm_stat, n_stat->st_ctim, struct timespec);
}

#endif
