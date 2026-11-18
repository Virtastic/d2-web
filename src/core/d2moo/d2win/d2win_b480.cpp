// src/core/d2win/d2win_b480.cpp -- 1.13c D2Win.dll+0x0B480, read the dword at +0xC4
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile. Prior art: D2MOO 1.10f
// reference/D2MOO/source/D2Win/ (control type numbering and header layout only).
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

// 1.13c D2Win+0x0B480 (ordinal 10007)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_B480(uint8_t* p) {
    if (!p) fatal(0xD41);
    return rd32(p, 0xC4);
}
