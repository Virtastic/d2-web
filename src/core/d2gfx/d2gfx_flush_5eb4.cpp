// src/core/d2gfx/d2gfx_flush_5eb4.cpp -- 1.13c D2Gfx.dll __flush (93 B), reimplemented from
// build-wasm/ghidra-d2gfx/decomp/0x5EB4.c (alpha lane). MSVC 2003 Release CRT: the FILE object is the
// 32-bit VS2003 layout (ptr, cnt, base, flag, file, ...; d2client_batch_0312a.cpp). The __write callee
// (0x3ED2) is a host seam after the retail arguments; it returns the byte count or -1.
#include <cstdint>
#include <emscripten.h>

struct D2GfxFile {
    uint32_t ptr;       // 0x00 _ptr
    int32_t  cnt;       // 0x04 _cnt
    uint32_t base;      // 0x08 _base
    int32_t  flag;      // 0x0C _flag
    int32_t  file;      // 0x10 _file
};

struct D2GfxFlushEnv {
    int32_t (*write_)(int32_t fh, const void* buf, uint32_t count);  // 0x3ED2 __write
};

// 93 B. __flush(FILE*). Only a write stream ((byte)flag & 3 == 2) with a buffer (flag & 0x108) does
// anything: n = ptr - base, and when positive it hands [base, ptr) to __write. A full write clears the
// read/write bit 0x2 when flag's sign bit (0x80) is set; a short write sets the error bit 0x20 and
// returns -1. Every path then zeroes cnt and rewinds ptr to base. Returns 0 otherwise.
// 1.13c D2Gfx+0x05EB4
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2gfx_5eb4(D2GfxFile* file, const D2GfxFlushEnv* env) {
    int32_t rc = 0;
    if (((uint8_t)file->flag & 3) == 2 && (file->flag & 0x108) != 0) {
        uint32_t n = file->ptr - file->base;
        if ((int32_t)n > 0) {
            int32_t w = env->write_(file->file, (const void*)(uintptr_t)file->base, n);
            if ((uint32_t)w == n) {
                if ((int8_t)file->flag < 0) file->flag &= ~2;
            } else {
                file->flag |= 0x20;
                rc = -1;
            }
        }
    }
    file->cnt = 0;
    file->ptr = file->base;
    return rc;
}
