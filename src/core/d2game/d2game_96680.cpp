// 1.13c D2Game+0x96680: eax = unit, esi = type, ebx = guid, edi = info (plain ret). q = [0xA4C0(unit) +0x44]; type in [0, dt+0xBF0) picks the row [q] + 12*type whose list
// (node +4 guid, +0x14 next) gets the 12 bytes at info stored at every matching node's +8.
#include "d2game_seam4e.h"
extern "C" void d2game_96680(uint32_t unit, uint32_t type, uint32_t guid, uint32_t info, uint32_t dt, const D2Seam* e)
{
    const uint32_t r = e->call(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t q = d2rd32(r + 0x44);
    if (!q || (int32_t)type < 0 || (int32_t)type >= (int32_t)d2rd32(dt + 0xBF0)) return;
    const uint32_t row = d2rd32(q) + type * 12;
    if (!row) return;
    for (uint32_t n = d2rd32(row); n; n = d2rd32(n + 0x14))
        if (d2rd32(n + 4) == guid) { d2wr32(n + 8, d2rd32(info)); d2wr32(n + 0xC, d2rd32(info + 4)); d2wr32(n + 0x10, d2rd32(info + 8)); }
}
