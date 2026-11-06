// 1.13c D2Client+0x6C3F0: runs 0x6C020 for every list entry a class's flag row selects. EAX = unit (also ESI and EDI inside), one stack word arg0 (ret 4).
// D2Client+0x1B50 (ESI = unit) gives the class index (<0 or >= dt[0xA80] returns); int16 at dt[0xA78] + idx*0x1A8 + 0x18 is the row v (<0 or
// >= dt[0xA98] returns); row = dt[0xA90] + v*0x134 (0 returns). e.gl = the list at +0xD28B0 (15 dwords, count at +0x3C); e.bm = *+0xCEE50, the bit-mask table.
// An entry s runs when byte (bm + (s & 7) * 4) & row byte [0xF0 + (s >> 3)] is nonzero: 0x6C020 with ESI = s, EDI = unit, arg0 (the record logs EDI).
#include "cl_env.h"
extern "C" void d2cl_6c3f0(uint32_t unit, uint32_t arg0, const cl::Env& e) {   // 1.13c D2Client+0x6C3F0
    const int32_t idx = (int32_t)e.H(0x1b50, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit);
    if (idx < 0 || idx >= (int32_t)e.R(e.dt + 0xa80)) return;
    const int32_t v = e.R16s(e.R(e.dt + 0xa78) + (uint32_t)idx * 0x1a8u + 0x18);
    if (v < 0 || v >= (int32_t)e.R(e.dt + 0xa98)) return;
    const uint32_t row = (uint32_t)v * 0x134u + e.R(e.dt + 0xa90);
    if (!row) return;
    for (uint32_t i = 0; i < e.R(e.gl + 0x3c); ++i) {
        const uint32_t s = e.R(e.gl + 4 * i);
        if (e.R8(e.bm + (s & 7) * 4) & e.R8(row + 0xf0 + (s >> 3))) {
            e.H(0x6c020, arg0, 0, 0, 0, 0, 0, 0, 0, 0, s);
            e.Edj(0x6c020, unit);
        }
    }
}
