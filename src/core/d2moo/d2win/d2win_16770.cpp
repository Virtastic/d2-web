// src/core/d2win/d2win_16770.cpp -- 1.13c D2Win.dll+0x16770, list (type 7): index of the current item, or -1
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile. Prior art: D2MOO 1.10f
// reference/D2MOO/source/D2Win/ (control type numbering and header layout only).
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

// 1.13c D2Win+0x16770 (ordinal 10039)
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_16770(int32_t* ctl) {
    if (ctl[0] != 7) fatal(0xD2);
    uint32_t it = (uint32_t)ctl[0x19];
    if (!it) return -1;
    int32_t i = 0;
    while ((uint32_t)ctl[0x1A] != it) {
        it = rd32(ptr(it), 0x220);
        ++i;
        if (!it) return -1;
    }
    return i;
}
