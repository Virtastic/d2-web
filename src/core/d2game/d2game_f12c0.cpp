// D2Game 0xF12C0: place a dropped item near a unit, up to ten tries. Stack: the unit (ret 4).
// Thunks (stdcall): 0xA478(unit) gives r0 (0 returns 0); the unit position as in 0xF1C30; 0xAD00(unit) gives r1; 0xB3DE(path) gives r2.
// Each try steps the unit seed (+0x20, 64-bit, lo*0x6AC690C5 + hi) twice: x' = x - 0x14 + (lo1 % 0x28), y' = y - 0x14 + (lo2 % 0x28);
// 0xB348(r0, x', y', r1, r2) refuses with a nonzero low word. The first accepted spot runs 0xAB2C(path, x', y') and returns 1; ten refusals return 0.
#include "d2game_s15.h"

// 1.13c D2Game+0xF12C0
extern "C" uint32_t d2game_f12c0(uint32_t unit, const D2Seam* sm)
{
    const D2Seam& s = *sm;
    const uint32_t r0 = s.call(0xA478, 0, 0, 0, 0, 0, unit);
    if (!r0) return 0;
    const uint32_t x = s15_pos(s, unit, false), y = s15_pos(s, unit, true);
    const uint32_t seed = unit + 0x20;
    const uint32_t r1 = s.call(0xAD00, 0, 0, 0, 0, 0, unit);
    const uint32_t path = d2rd32(unit + 0x2C);
    const uint32_t r2 = s.call(0xB3DE, 0, 0, 0, 0, 0, path);
    for (uint32_t n = 0; n < 10; n++) {
        uint64_t p = (uint64_t)d2rd32(seed) * 0x6AC690C5u + d2rd32(seed + 4);
        const uint32_t lo1 = (uint32_t)p, hi1 = (uint32_t)(p >> 32);
        const uint32_t ox = x + lo1 % 0x28u - 0x14u;
        p = (uint64_t)lo1 * 0x6AC690C5u + hi1;
        const uint32_t lo2 = (uint32_t)p;
        d2wr32(seed, lo2); d2wr32(seed + 4, (uint32_t)(p >> 32));
        const uint32_t oy = y + lo2 % 0x28u - 0x14u;
        const uint32_t w[5] = { r0, ox, oy, r1, r2 };
        const uint32_t rv = s.call(0xB348, 0, 0, 0, 0, 0, w[0], w[1], w[2], w[3], w[4]);
        s.blk(0xB348, w, 5);
        if ((rv & 0xFFFF) == 0) { s.call(0xAB2C, 0, 0, 0, 0, 0, path, ox, oy); return 1; }
    }
    return 0;
}
