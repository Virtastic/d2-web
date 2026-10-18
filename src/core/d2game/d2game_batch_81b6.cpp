// src/core/d2game/d2game_batch_81b6.cpp -- 1.13c D2Game.dll row 0x081B6 __getbuf, from the cached
// decompile (build-wasm/ghidra-d2game/decomp/0x81B6.c). Alpha lane (OBJECTIVE v8.3): implementation +
// marker only, no spec, no vector.
//
// Host model follows src/core/d2game/d2game_batch_3e70.cpp: FILE is the 32-bit VS2003 layout, and the
// one callee (_malloc, 0x5F78) is still an UNMAPPED row, so it is a seam pointer after the retail arg.
#include <cstdint>
#include <emscripten.h>

// MSVC 2003 32-bit FILE; only the fields __getbuf touches.
struct CrtFile {
    uint32_t _ptr;      // 0x00
    int32_t  _cnt;      // 0x04
    uint32_t _base;     // 0x08
    int32_t  _flag;     // 0x0C
    int32_t  _file;     // 0x10
    int32_t  _charbuf;  // 0x14
    int32_t  _bufsiz;   // 0x18
    uint32_t _tmpfname; // 0x1C
};

struct D2GameCrtGetbufEnv {
    void* (*malloc)(uint32_t);      // 0x5F78 _malloc
};

// 0x6FD2FEE8: the CRT stream-buffer counter, bumped here and in __openfile (0x4CFD). Never read here.
static uint32_t g_getbufAllocs;

// 1.13c D2Game+0x081B6
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_81b6(CrtFile* f, const D2GameCrtGetbufEnv* env) {
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
