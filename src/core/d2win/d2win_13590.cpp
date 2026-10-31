// src/core/d2win/d2win_13590.cpp -- 1.13c D2Win.dll+0x13590, is the third argument's first char an ASCII letter
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile. Prior art: D2MOO 1.10f
// reference/D2MOO/source/D2Win/ (control type numbering and header layout only).
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

// 1.13c D2Win+0x13590 (ordinal 10136): the first two arguments are ignored. Signed char compare.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_13590(uint32_t, uint32_t, const char* s) {
    int8_t c = (int8_t)*s;
    if ((c < (int8_t)0x61 || c > (int8_t)0x7A) && (c < (int8_t)0x41 || c > (int8_t)0x5A)) return 0;
    return 1;
}
