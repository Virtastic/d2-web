// 1.13c D2Game+0x23570: monster mode roll. Fastcall ECX = game, EDX = unit, two stack words never read (ret 8). A unit whose +0x10 word is
// nonzero, or one 0xA514 answers for, does nothing. Otherwise the unit's state list (+0xDC, next at +0x24) is walked and every node whose
// first byte is 7 goes through 0x8E6F0 (EAX node, EDI game); the seed pair (+0x20/+0x24) steps as lo*0x6AC690C5 + hi and a roll of
// lo mod 100 under the int16 at row[+0x80 + 2 * byte game[+0x6D]] starts 0x8EA80 (game, unit; 7, game[+0xA8] + 10 * row int16 [+0x56 + 2 * byte] + 1, 0, 0, 0).
// The row is the monster table entry (dt+0xA78, 0x1A8 each, count dt+0xA80) of unit[+4]; a missing row is a null base in retail.
#include "d2game_seam4e.h"
extern "C" void d2g_23570(uint32_t game, uint32_t unit, uint32_t dt, const D2Seam* e)
{
    if (unit && d2rd32(unit + 0x10)) return;
    if (e->call(0xA514, 0, 0, 0, 0, 0, unit)) return;
    if (!unit) return;                       // retail reads +0x20 of a null unit here
    uint32_t row = 0;
    const int32_t id = (int32_t)d2rd32(unit + 4);
    if (id >= 0 && id < (int32_t)d2rd32(dt + 0xA80)) row = d2rd32(dt + 0xA78) + (uint32_t)id * 0x1A8;
    for (uint32_t n = d2rd32(unit + 0xDC); n; ) {
        const uint32_t next = d2rd32(n + 0x24);
        if (d2rd8(n) == 7) { e->call(0x8E6F0, n); e->edi(0x8E6F0, game); }
        n = next;
    }
    const uint64_t m = (uint64_t)d2rd32(unit + 0x20) * 0x6AC690C5u + d2rd32(unit + 0x24);
    d2wr32(unit + 0x24, (uint32_t)(m >> 32));
    d2wr32(unit + 0x20, (uint32_t)m);
    const uint32_t roll = (uint32_t)m % 100u;
    const uint32_t k = d2rd8(game + 0x6D);
    if ((int32_t)roll >= (int32_t)(int16_t)d2rd16(row + k * 2 + 0x80)) return;
    const int32_t sk = (int16_t)d2rd16(row + k * 2 + 0x56);
    e->call(0x8EA80, 0, game, unit, 0, 0, 7, d2rd32(game + 0xA8) + (uint32_t)sk * 10 + 1, 0, 0, 0);
}
