// src/core/d2win/d2win_178b0.cpp -- 1.13c D2Win.dll+0x178B0, control flag bit 2 (masked, not normalised)
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile. Prior art: D2MOO 1.10f
// reference/D2MOO/source/D2Win/ (control type numbering and header layout only).
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

// 1.13c D2Win+0x178B0 (ordinal 10102)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_178B0(uint8_t* ctl) {
    if (!ctl) fatal(0x349);
    return kBm2 & rd32(ctl, 8);
}
