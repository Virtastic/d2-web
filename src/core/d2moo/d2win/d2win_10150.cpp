// src/core/d2win/d2win_10150.cpp -- 1.13c D2Win.dll+0x10150, image (type 2): set the cell file [1] and field [0x12]
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile. Prior art: D2MOO 1.10f
// reference/D2MOO/source/D2Win/ (control type numbering and header layout only).
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

// 1.13c D2Win+0x10150 (ordinal 10058)
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_10150(int32_t* ctl, int32_t a, int32_t b) {
    if (ctl[0] != 2) fatal(0x5F);
    ctl[1] = a;
    ctl[0x12] = b;
    return 1;
}
