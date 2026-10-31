// src/core/d2win/d2win_eba0.cpp -- 1.13c D2Win.dll+0x0EBA0, progress bar (type 10): write field [0x10]
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile. Prior art: D2MOO 1.10f
// reference/D2MOO/source/D2Win/ (control type numbering and header layout only).
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

// 1.13c D2Win+0x0EBA0 (ordinal 10063)
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_EBA0(int32_t* ctl, int32_t v) {
    if (ctl[0] != 10) fatal(0x3A);
    ctl[0x10] = v;
    return 1;
}
