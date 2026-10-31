// src/core/d2win/d2win_11400.cpp -- 1.13c D2Win.dll+0x11400, smack (type 9): set the position and its copy
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile. Prior art: D2MOO 1.10f
// reference/D2MOO/source/D2Win/ (control type numbering and header layout only).
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

// 1.13c D2Win+0x11400 (ordinal 10116)
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_11400(int32_t* ctl, int32_t x, int32_t y) {
    if (ctl[0] != 9) fatal(0x79);
    ctl[3] = x;
    ctl[4] = y;
    ctl[0x52] = x;
    ctl[0x53] = y;
    return 1;
}
