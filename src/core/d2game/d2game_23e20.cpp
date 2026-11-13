// 1.13c D2Game+0x23e20: ECX = game, EDX = unit; two ignored stack words. Wake-or-stagger step for a unit. First the unit's state list
// (+0xDC, next at +0x24) is walked and every node whose first byte is 2 goes to 0x8E6F0 (EAX = node, EDI = game; the next link is read
// first). Unless unit[0xC6] bit 0 is set, a type 0 unit must be in mode 0 or 0x11 and a type 1 unit in mode 0 or 0xC (unit[0x10]), or the step ends.
// It also ends when #AD60(unit) is nonzero, a monster whose record (+0x14) has bit 8 at +0x16, or #A514(unit) == 2. Then the room
// #A478(unit, #1100, #1130, 1, 0x3C01) goes to #B348: with a nonzero low word 0x8EA80 (ECX = game, EDX = unit; 7, game[0xA8] + 10 * int16
// row[0x56 + 2 * byte game[0x6D]] + 1, 0, 0, 0) ends it. Otherwise a monster whose +0x28 data has +0x14 >= 2 ends it, and so do #ADC0(unit, 0) == 0 or
// #A514 != 0; else 0xF2380 (EAX = 8, ECX = unit, EBX = game; 0x125, 0, 0, 0), clears unit[0xC4] & 0xFBFDFFFF, bumps the +0x28 data's +0x14,
// walks the list again and ends with 0x8EA80 (2, game[0xA8] + 0x33, 0, 0, 0).
#include "d2game_f1pos.h"
static void f1Walk(uint32_t game, uint32_t unit, const D2Seam* e)
{
    uint32_t n = d2rd32(unit + 0xDC);
    while (n) {
        const uint32_t next = d2rd32(n + 0x24);
        if (d2rd8(n) == 2) { e->call(0x8E6F0, n); e->edi(0x8E6F0, game); }
        n = next;
    }
}
static inline uint32_t f1Data28(uint32_t unit)
{
    if (!unit || d2rd32(unit) != 1) return 0;
    const uint32_t p = d2rd32(unit + 0x14);
    return p ? d2rd32(p + 0x28) : 0;
}
extern "C" void d2g_23e20(uint32_t game, uint32_t unit, uint32_t dt, const D2Seam* e)
{
    uint32_t row = 0;
    if (unit) {
        const int32_t cls = (int32_t)d2rd32(unit + 4);
        if (cls >= 0 && cls < (int32_t)d2rd32(dt + 0xA80)) row = d2rd32(dt + 0xA78) + (uint32_t)cls * 0x1A8;
    }
    if (unit) {
        f1Walk(game, unit, e);
        if (!(d2rd8(unit + 0xC6) & 1)) {
            const uint32_t t = d2rd32(unit), m = d2rd32(unit + 0x10);
            if (t == 0 && m != 0 && m != 0x11) return;
            if (t == 1 && m != 0 && m != 0xC) return;
        }
    }
    if (e->call(0xAD60, 0, 0, 0, 0, 0, unit)) return;
    if (unit && d2rd32(unit) == 1) { const uint32_t p = d2rd32(unit + 0x14); if (p && (d2rd8(p + 0x16) & 8)) return; }
    if (e->call(0xA514, 0, 0, 0, 0, 0, unit) == 2) return;
    const uint32_t y = e->call(0x1130, 0, unit);
    const uint32_t x = e->call(0x1100, 0, unit);
    const uint32_t room = e->call(0xA478, 0, 0, 0, 0, 0, unit, x, y, 1, 0x3C01);
    if ((uint16_t)e->call(0xB348, 0, 0, 0, 0, 0, room)) {
        const int32_t sel = (int16_t)d2rd16(row + 0x56 + 2 * d2rd8(game + 0x6D));
        e->call(0x8EA80, 0, game, unit, 0, 0, 7, d2rd32(game + 0xA8) + (uint32_t)(sel * 10) + 1, 0, 0, 0);
        return;
    }
    if (const uint32_t q = f1Data28(unit)) if ((int32_t)d2rd32(q + 0x14) >= 2) return;
    if (!e->call(0xADC0, 0, 0, 0, 0, 0, unit, 0)) return;
    if (e->call(0xA514, 0, 0, 0, 0, 0, unit)) return;
    e->call(0xF2380, 8, unit, 0, game, 0, 0x125, 0, 0, 0);
    if (unit) {
        d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) & 0xFBFDFFFFu);
        const uint32_t q = f1Data28(unit);
        if (q) d2wr32(q + 0x14, d2rd32(q + 0x14) + 1);
        f1Walk(game, unit, e);
    }
    e->call(0x8EA80, 0, game, unit, 0, 0, 2, d2rd32(game + 0xA8) + 0x33, 0, 0, 0);
}
