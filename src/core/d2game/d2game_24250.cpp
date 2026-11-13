// 1.13c D2Game+0x24250: skill 0x42 timer start. Fastcall ECX = game, EDX = unit, two stack words (the first is a gate, the second never read; ret 8).
// A zero gate does nothing. Otherwise the unit seed pair steps (lo*0x6AC690C5 + hi) and a low two bits of 0 end it. The count n = stat 0xC / 5 + 1
// (at least 1) and the skill table's row 0x42 (dt+0xB98, 0x23C each; count dt+0xBA0 must exceed 0x42) give a level through 0xAC9A(unit, row[+0x64], 0x42, n),
// clamped to 1..0x28, which goes in EAX to 0xA06F0(game, 3, unit, n, callback 0x24030).
#include "d2game_seam4e.h"
extern "C" void d2g_24250(uint32_t game, uint32_t unit, uint32_t gate, uint32_t dt, const D2Seam* e)
{
    if (!gate) return;
    const uint64_t m = (uint64_t)d2rd32(unit + 0x20) * 0x6AC690C5u + d2rd32(unit + 0x24);
    d2wr32(unit + 0x20, (uint32_t)m);
    d2wr32(unit + 0x24, (uint32_t)(m >> 32));
    if (((uint32_t)m & 3) < 1) return;
    const int32_t v = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0xC, 0);
    int32_t n = v / 5 + 1;
    if (n < 1) n = 1;
    if ((int32_t)d2rd32(dt + 0xBA0) <= 0x42) return;
    const uint32_t row = d2rd32(dt + 0xB98) + 0x9378;
    if (!row) return;
    int32_t lvl = (int32_t)e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(row + 0x64), 0x42, (uint32_t)n);
    if (lvl <= 1) lvl = 1;
    else if (lvl >= 0x28) lvl = 0x28;
    e->call(0xA06F0, (uint32_t)lvl, 0, 0, 0, 0, game, 3, unit, (uint32_t)n, 0xB2000000u | 0x24030u);
}
