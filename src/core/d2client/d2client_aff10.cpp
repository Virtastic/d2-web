// 1.13c D2Client+0xAFF10: spawn request builder (EDX = unit, five stack words). Read off the disassembly: the 0x17-dword request is zeroed,
// [0] = 1, [1] = unit, [4..6] = s0..s2, [0xB] = s3, [0xC] = s4, then 0xB30F0 with ECX = request; nothing happens when the unit is 0.
#include <cstdint>
#include <emscripten.h>
#include "d2client_aff10.h"
extern "C" {
// 1.13c D2Client+0xAFF10
EMSCRIPTEN_KEEPALIVE void d2c_aff10(const D2cAFF10Env* e, uintptr_t unit, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4) {
    if (!unit) return;
    uint32_t l[0x17] = {0};
    l[0] = 1; l[1] = (uint32_t)unit; l[4] = s0; l[5] = s1; l[6] = s2; l[0xb] = s3; l[0xc] = s4;
    e->fB30F0(l);
}
}
