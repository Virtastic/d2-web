// src/core/d2common/d2common_getbuf_7374.cpp -- 1.13c D2Common.dll row 0x07374 __getbuf. Alpha lane: no decompile
// was available (no Ghidra project), so the body is the D2Game 0x081B6 copy of the same VS2003 CRT leaf.
// GUESS: identical body; counter address and _malloc seam (D2Common 0x05F7F) unverified.
#include <cstdint>
#include <emscripten.h>

// MSVC 2003 32-bit FILE; only the fields __getbuf touches.
struct D2CommonGetbufFile {
    uint32_t _ptr;      // 0x00
    int32_t  _cnt;      // 0x04
    uint32_t _base;     // 0x08
    int32_t  _flag;     // 0x0C
    int32_t  _file;     // 0x10
    int32_t  _charbuf;  // 0x14
    int32_t  _bufsiz;   // 0x18
    uint32_t _tmpfname; // 0x1C
};

struct D2CommonGetbufEnv {
    void* (*malloc)(uint32_t);      // 0x5F7F _malloc
};

static uint32_t g_getbufAllocs;

// 1.13c D2Common+0x07374
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_7374(D2CommonGetbufFile* f, const D2CommonGetbufEnv* env) {
    ++g_getbufAllocs;
    f->_base = (uint32_t)(uintptr_t)env->malloc(0x1000);
    if (f->_base == 0) {
        f->_flag |= 4;                              // _IONBF
        f->_base = (uint32_t)(uintptr_t)&f->_charbuf;
        f->_bufsiz = 2;
    } else {
        f->_flag |= 8;                              // _IOMYBUF
        f->_bufsiz = 0x1000;
    }
    f->_cnt = 0;
    f->_ptr = f->_base;
}
