// src/core/d2win/d2win_abe0.cpp -- 1.13c D2Win.dll+0x0ABE0, scrollbar (type 5): write field [0x14]
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile. Prior art: D2MOO 1.10f
// reference/D2MOO/source/D2Win/ (control type numbering and header layout only).
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

// 1.13c D2Win+0x0ABE0 (ordinal 10081)
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_ABE0(int32_t* ctl, int32_t v) {
    if (ctl[0] != 5) fatal(0xCA);
    ctl[0x14] = v;
}
