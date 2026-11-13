// 1.13c D2Game+0x39670 -- monster-mode hit callback: steps the unit's 64-bit seed (x * 0x6AC690C5 + hi) and, when its low 7 bits are below row +0x78, sets bit 0x40 of event record byte +5
// fastcall: ECX = unit... (edx on entry), stack word 0 unused, stack word 1 = the event record (ret 8)
// Returns EAX at the ret: 0 on a null unit or row, the id on the range exits, the low 7 seed bits when not below the bound, else the record pointer.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f39670(NE& e, uint32_t dt, uint32_t unit, uint32_t rec)
{
    if (!unit) return 0;
    const int32_t id = (int32_t)e.rd32(unit + 4);
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xb6c)) return (uint32_t)id;
    const uint32_t row = e.rd32(dt + 0xb64) + (uint32_t)id * 0x1a4u;
    if (!row) return 0;
    const uint64_t s = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5ull + e.rd32(unit + 0x24);
    e.wr32(unit + 0x20, (uint32_t)s);
    e.wr32(unit + 0x24, (uint32_t)(s >> 32));
    const uint32_t lo7 = (uint32_t)s & 0x7f;
    if ((int32_t)lo7 < (int32_t)e.rd32(row + 0x78)) { e.wr8(rec + 5, e.rd8(rec + 5) | 0x40); return rec; }
    return lo7;
}
}
