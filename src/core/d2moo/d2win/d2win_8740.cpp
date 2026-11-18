// src/core/d2win/d2win_8740.cpp -- 1.13c D2Win.dll+0x08740, list box (type 4): its scrollbar child's +0x48 field
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile. Prior art: D2MOO 1.10f
// reference/D2MOO/source/D2Win/ (control type numbering and header layout only).
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

// 1.13c D2Win+0x08740 (ordinal 10019)
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_8740(int32_t* ctl) {
    if (!ctl) fatal(0x578);
    if (ctl[0] != 4) fatal(0x579);
    int32_t* sb = (int32_t*)ptr((uint32_t)ctl[0x24]);
    if (sb[0] != 5) fatal(0x4B);
    return sb[0x12];
}
