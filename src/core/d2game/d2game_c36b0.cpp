// D2Game+0xC36B0 -- ACT2Q3_UnitIterate_SetRewardPending: ECX = this, EDX = param_2, always returns 0. Written from the 1.13c disassembly; callees are the D2Seam.
// this+0x6d slot picks a unit from the 0xA4C0 table; this+0x10f4 holds a list head, its first node chain (+0xF4) is searched for type 10; node+0x18+0x98 is compared
// with 0xA556 of the 0xA478 unit; unless 0xA568 reports 1 for (unit,0xb,0) or (unit,0xb,0xf), 0xA57A sets (unit,0xb,0xd) and (unit,0xb,1).
#include "d2game_seam4e.h"
// 1.13c D2Game+0xC36B0
extern "C" uint32_t d2game_c36b0(uint32_t self, uint32_t p2, const D2Seam* e)
{
    const uint32_t tbl = e->call(0xA4C0, 0, 0, 0, 0, 0, p2);
    const uint32_t unit = d2rd32(tbl + 0x10 + (uint32_t)d2rd8(self + 0x6D) * 4);
    const uint32_t other = e->call(0xA478, 0, 0, 0, 0, 0, p2);
    uint32_t node = 0;
    const uint32_t hd = d2rd32(self + 0x10F4);
    if (hd) {
        for (uint32_t n = d2rd32(hd); n; n = d2rd32(n + 0xF4))
            if (d2rd32(n) == 10) { node = n; break; }
    }
    if (!other || !node) return 0;
    const uint32_t st = d2rd32(node + 0x18);
    const uint32_t id = e->call(0xA556, 0, 0, 0, 0, 0, other);
    if (d2rd32(st + 0x98) != id) return 0;
    if (e->call(0xA568, 0, 0, 0, 0, 0, unit, 0xB, 0) == 1) return 0;
    if (e->call(0xA568, 0, 0, 0, 0, 0, unit, 0xB, 0xF) == 1) return 0;
    e->call(0xA57A, 0, 0, 0, 0, 0, unit, 0xB, 0xD);
    e->call(0xA57A, 0, 0, 0, 0, 0, unit, 0xB, 1);
    return 0;
}
