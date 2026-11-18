// src/core/d2win/d2win_ab50.cpp -- 1.13c D2Win.dll+0x0AB50, scrollbar (type 5): clear the drag state.
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll) and the cached Ghidra decompile.
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

// 1.13c D2Win+0x0AB50 (stdcall, ret 4): ev[0] = the scrollbar control (type 5, else fatal 0x1B0).
// Control dwords [0x10], [0x11], [0x16] are the drag state: all zero returns 0; otherwise all three are cleared and 1 is returned.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_AB50(int32_t* ev) {
    int32_t* c = (int32_t*)ptr(rd32(ev, 0));
    if (c[0] != 5) fatal(0x1B0);
    if (c[0x10] == 0 && c[0x11] == 0 && c[0x16] == 0) return 0;
    c[0x10] = 0;
    c[0x11] = 0;
    c[0x16] = 0;
    return 1;
}
