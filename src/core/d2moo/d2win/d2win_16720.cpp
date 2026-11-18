// src/core/d2win/d2win_16720.cpp -- 1.13c D2Win.dll+0x16720, list (type 7): the idx-th item or 0
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile. Prior art: D2MOO 1.10f
// reference/D2MOO/source/D2Win/ (control type numbering and header layout only).
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

// 1.13c D2Win+0x16720 (ordinal 10046): items chain from [0x19] through item+0x220.
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t* d2win_sub_16720(int32_t* ctl, int32_t idx) {
    if (ctl[0] != 7) fatal(0xE5);
    uint8_t* it = ptr((uint32_t)ctl[0x19]);
    if (!it) return 0;
    for (int32_t i = 0; i != idx; ++i) {
        it = ptr(rd32(it, 0x220));
        if (!it) return 0;
    }
    return it;
}
