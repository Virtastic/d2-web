// src/core/d2win/d2win_8830.cpp -- 1.13c D2Win.dll+0x08830, list box: set or clear the flag bit 2, then forward to the scrollbar child
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile. Prior art: D2MOO 1.10f
// reference/D2MOO/source/D2Win/ (control type numbering and header layout only).
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

// D2Win+0x178E0 (d2win_batch_8640.cpp): set or clear control+8 bit 2.
extern "C" void d2win_sub_178E0(uint8_t* ctl, int32_t on);

// 1.13c D2Win+0x08830 (ordinal 10135). The flag write happens before the type check, as in retail.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_8830(int32_t* ctl, int32_t on) {
    if (!ctl) fatal(0x33B);
    uint32_t f = (uint32_t)ctl[2];
    ctl[2] = (int32_t)(on ? (f | kBm2) : (f & ~kBm2));
    if (ctl[0] != 4) fatal(0x321);
    uint32_t child = (uint32_t)ctl[0x24];
    if (child) d2win_sub_178E0(ptr(child), on);
}
