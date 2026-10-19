// src/core/d2game/d2game_crt_setmode.cpp -- MSVC 2003 Release CRT __setmode_lk, transcribed from the
// cached Ghidra decompile build-wasm/ghidra-d2game/decomp/0xA131.c. Alpha lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector.
//
// Host model follows src/core/d2game/d2game_crt_free_osfhnd.cpp: 0x6FD3A220 _pioinfo is 32 block
// pointers to 32 ioinfo records of 0x24 bytes each, so fd>>5 selects the block and (fd&0x1f)*0x24 the
// record. ioinfo.osfile is record+4; bit 0x80 there is FTEXT (text translation). The table is
// host-owned and comes in through the seam. The lock the `_lk` suffix names is the caller's (retail
// __setmode takes it before calling); nothing is locked here. Callee 0x4E65 __getptd()->_terrno is a
// seam, as in __write and __free_osfhnd.
#include <cstdint>
#include <emscripten.h>

struct D2GameCrtSetmodeEnv {
    int32_t*  (*errnoPtr)(void);                               // 0x4E65 __getptd()->_terrno
    uint8_t** pioinfo;                                         // 0x6FD3A220 _pioinfo
};

// 1.13c D2Game+0x0A131
// __setmode_lk(fd, mode): ioinfo.osfile's FTEXT bit selects the translation mode. _O_TEXT (0x4000)
// sets FTEXT, _O_BINARY (0x8000) clears it, and any other mode sets errno EINVAL (0x16) and returns -1.
// The return is the previous mode: _O_TEXT when FTEXT was set, _O_BINARY otherwise. The decompile
// indexes _pioinfo and ioinfo without a range check, so neither does this.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_A131(uint32_t fd, int32_t mode, const D2GameCrtSetmodeEnv* s) {
    uint8_t* io = s->pioinfo[(int32_t)fd >> 5] + (fd & 0x1f) * 0x24;
    const int32_t oldmode = (io[4] & 0x80) != 0 ? 0x4000 : 0x8000;   // _O_TEXT : _O_BINARY
    if (mode == 0x8000) {                                           // _O_BINARY: clear FTEXT
        io[4] &= 0x7f;
    } else if (mode == 0x4000) {                                    // _O_TEXT: set FTEXT
        io[4] |= 0x80;
    } else {
        *s->errnoPtr() = 0x16;                                      // EINVAL
        return -1;
    }
    return oldmode;
}
