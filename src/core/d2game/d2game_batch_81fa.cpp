// src/core/d2game/d2game_batch_81fa.cpp -- 1.13c D2Game.dll row 0x081FA __isatty, from the cached
// decompile (build-wasm/ghidra-d2game/decomp/0x81FA.c). Alpha lane (OBJECTIVE v8.3): implementation +
// marker only, no spec, no vector.
//
// Host model follows src/core/d2game/d2game_crt_lseek.cpp: retail's module globals are the statics below
// at their retail RVA. __isatty has no callees, so there is no seam struct.
#include <cstdint>
#include <emscripten.h>

// 0x6FD3A214 _nfile: number of valid CRT handles; __ioinit (0x60EF) seeds it at startup. Only the alpha
// host seed (the MSVC 2003 CRT's initial 64-handle table) is fixed here; the real value is module state.
static uint32_t g_nfile = 64;
// 0x6FD3A220 _pioinfo: 64 block pointers, each 32 ioinfo records of 0x24 bytes, osfile at record +4
// (bit 0x40 = FDEV, a character device). The blocks are contiguous, so it is modeled flat:
// block[fh>>5][fh&0x1f] == flat[fh].
static uint8_t g_ioinfo[0x400][0x24];

// 1.13c D2Game+0x081fa
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_81fa(int32_t _FileHandle) {
    const uint32_t fh = (uint32_t)_FileHandle;          // the bound test is unsigned
    if (g_nfile <= fh) return 0;
    return g_ioinfo[fh][4] & 0x40;                      // FDEV
}
