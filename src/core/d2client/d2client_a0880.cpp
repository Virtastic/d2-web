// EDI = unit or null, stack arg 0 = value (ret 4).
// Allocates a 4-kind record for the unit (type/id, or 6/-1 without one), links it and sets field 0x44 to the value.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xA0880
extern "C" void d2c_a0880(uint32_t unit, uint32_t v, const cl::Env& e) {
    const uint32_t ty = unit ? e.R(unit) : 6u, id = unit ? e.R(unit + 0xc) : 0xffffffffu;
    const uint32_t w[5] = { 0, 4, 0, ty, id };
    const uint32_t s = e.H(0xc442, 0, 4, 0, ty, id);                // 11013 (5 stack words)
    if (e.blk) e.blk(e.ctx, 0xc442, (uint32_t)(uintptr_t)w, 5);
    if (!s) return;
    e.H(0xc36a, unit, s, 1);                                        // 10807
    e.H(0xc562, s, 0x44, v, 0);                                     // 10188
    e.H(0xc5c8, unit, 0xB2000000u | 0xd44b4u, 0x9e4);               // 10819 (source-file string, line)
}
