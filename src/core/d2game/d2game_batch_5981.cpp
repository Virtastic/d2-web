// src/core/d2game/d2game_batch_5981.cpp -- 1.13c D2Game.dll CRT row __fflush_lk, transcribed from the
// cached decompile (build-wasm/ghidra-d2game/decomp/0x5981.c). Stage A batch lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector.
//
// Host model: host stdio owns the byte buffer; the retail fields that host stdio cannot represent
// (_flag and _file) stay explicit in the wrapper. Retail pointers/offsets remain 32-bit, even in wasm64.
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <emscripten.h>
#include "d2game_crt_commit.h"

struct D2GameCrtFile32 {
    uint32_t ptr;       // _ptr, retail offset 0x00
    int32_t  cnt;       // _cnt, retail offset 0x04
    uint32_t base;      // _base, retail offset 0x08
    int32_t  flag;      // _flag, retail offset 0x0C
    int32_t  file;      // _file, retail offset 0x10
    int32_t  charbuf;   // _charbuf, retail offset 0x14
    int32_t  bufsiz;    // _bufsiz, retail offset 0x18
    uint32_t tmpfname;  // _tmpfname, retail offset 0x1C
};
static_assert(sizeof(D2GameCrtFile32) == 0x20);
static_assert(offsetof(D2GameCrtFile32, flag) == 0x0C);
static_assert(offsetof(D2GameCrtFile32, file) == 0x10);

// Probe-facing host wrapper: hostStream is a libc FILE*, while retailFile carries the explicit
// 32-bit MSVC _iobuf fields needed by this function. Do not reinterpret glibc's FILE layout as MSVC.
struct D2GameFflushLkStream {
    FILE* hostStream;
    D2GameCrtFile32* retailFile;
};

// 0x5924 __flush (src/core/d2game/d2game_batch_7de3.cpp).
extern "C" int32_t d2_fn_d2game_5924(FILE* stream);

// 1.13c D2Game+0x05981 (__fflush_lk): __flush(stream) first; any failure is -1. Otherwise a stream
// flagged 0x4000 commits its descriptor (_file) through __commit (0x8B19). The seam carries the CRT
// handle table and explicit FlushFileBuffers/GetLastError callbacks used by that existing implementation.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_5981(D2GameFflushLkStream* stream,
                                               const D2GameCrtCommitSeams* commitSeams) {
    if (d2_fn_d2game_5924(stream->hostStream) != 0) return -1;
    if ((stream->retailFile->flag & 0x4000) != 0) {
        return d2_fn_d2game_8B19(stream->retailFile->file, commitSeams) != 0 ? -1 : 0;
    }
    return 0;
}
