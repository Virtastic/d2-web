// src/core/d2win/d2win_9e10.cpp -- 1.13c D2Win.dll+0x09E10, list-box (type 0xC) mouse selection.
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll) and the cached Ghidra decompile.
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

// 1.13c D2Win+0x09E10 (stdcall, ret 4): ev[0] = the list-box control (type 0xC, else fatal 0x10D). Control dwords: [2] flags,
// [3] / [4] x / y origin, [6] bottom, [0x10] first item, [0x11] selected item. The mouse position is in the globals at
// 0x21488 (x) and 0x2148C (y). Cells are 0x78 wide and 0x14 tall, six per row. A control that is not enabled returns 0, as does an
// empty list; otherwise the item at the picked index (walking the list through item+0x34) becomes the selection, or none when
// the list ends first. Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_9E10(int32_t* ev, int32_t mouseX, int32_t mouseY) {
    int32_t* c = (int32_t*)ptr(rd32(ev, 0));
    if (c[0] != 0xC) fatal(0x10D);
    const uint32_t flags = (uint32_t)c[2];
    if (!((kBm2 & flags) && (kBm0 & flags))) return 0;
    uint32_t item = (uint32_t)c[0x10];
    if (item == 0) return 0;
    const int32_t pick = (mouseX - c[3]) / 0x78 + (((c[6] - c[4]) + 4 + mouseY) / 0x14) * 6;
    int32_t i = 0;
    for (;;) {
        if (i == pick) { c[0x11] = (int32_t)item; return 1; }
        item = rd32(ptr(item), 0x34);
        ++i;
        if (item == 0) break;
    }
    c[0x11] = 0;
    return 1;
}
