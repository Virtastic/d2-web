// src/core/d2win/d2win_ac80.cpp -- 1.13c D2Win.dll+0x0AC80, scrollbar (type 5): read field [0x13]
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile. Prior art: D2MOO 1.10f
// reference/D2MOO/source/D2Win/ (control type numbering and header layout only).
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

// 1.13c D2Win+0x0AC80 (ordinal 10188)
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_AC80(int32_t* ctl) {
    if (ctl[0] != 5) fatal(0x74);
    return ctl[0x13];
}
