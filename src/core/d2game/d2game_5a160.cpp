// 1.13c D2Game+0x5A160 (ACT5_HasTokenItem1B2): EAX = unit or 0, ECX = game-side record (+0x6A kind byte, +0x88 owner word), stack (fallback), ret 4.
// Unit = EAX; when 0 and (record+0x6A == 3 and record+0x88 != 0) it is 0x4AD10(0), and the fallback when that is 0 too. Room = #10331(unit); none gives 0.
// #10383(room, &array, &count) lists the room's units; each unit's list at +0x74 (chained at +0xE8) is searched for a record {+0 == 1, +4 == 0x1B2,
// +0x10 == 0xC}; a hit returns 1, otherwise 0. Callees are the seam; the array and count are the callee's out-locals.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_5a160(uint32_t inEax, uint32_t self, uint32_t param1, const D2Seam* e)
{
    uint32_t u = inEax;
    if (!u && (d2rd8(self + 0x6A) != 3 || !d2rd32(self + 0x88) || !(u = e->call(0x4AD10, 0, 0, 0, 0, 0, 0)))) u = param1;
    const uint32_t room = e->call(0xA478, 0, 0, 0, 0, 0, u);
    if (!room) return 0;
    volatile uint32_t arr = 0, cnt = 0;
    e->call(0xA55C, 0, 0, 0, 0, 0, room, (uint32_t)(uintptr_t)&arr, (uint32_t)(uintptr_t)&cnt);
    for (uint32_t i = 0; i < cnt; i++)
        for (uint32_t p = d2rd32(d2rd32(arr + 4 * i) + 0x74); p; p = d2rd32(p + 0xE8))
            if (d2rd32(p) == 1 && d2rd32(p + 4) == 0x1B2 && d2rd32(p + 0x10) == 0xC) return 1;
    return 0;
}
