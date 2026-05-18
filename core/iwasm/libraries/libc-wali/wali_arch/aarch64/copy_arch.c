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

extern inline int
swap_bits(int val, int b1pos, int b2pos)
{
    int b1 = (val >> b1pos) & 1;
    int b2 = (val >> b2pos) & 1;
    int x = b1 ^ b2;
    x = ((x << b1pos) | (x << b2pos));
    return val ^ x;
}
/* aarch64 swaps O_DIRECTORY <-> O_DIRECT
 *    and O_NOFOLLOW <-> O_LARGEFILE */
extern inline int
swap_open_flags(int open_flags)
{
    int odirectory_shf = __builtin_ctz(O_DIRECTORY);
    int odirect_shf = __builtin_ctz(O_DIRECT);
    int olargefile_shf = __builtin_ctz(O_LARGEFILE);
    int onofollow_shf = __builtin_ctz(O_NOFOLLOW);
    int one_swap = swap_bits(open_flags, odirectory_shf, odirect_shf);
    int result = swap_bits(one_swap, olargefile_shf, onofollow_shf);
    return result;
}

#endif
