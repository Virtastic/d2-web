// D2Game+0xDDAC0 -- timed state 0xC for `amount` frames (the sub_DDAC0 spec). EAX = unit (type 0 / 1 source), ECX = the target unit (its +0x80 record, +0 / +0xC type and id), stack amount (ret 4); the param below named `game` is that ECX unit.
// Written from the 1.13c disassembly. amount <= 0 returns. A type 0 unit hands the amount to 0xDC780(EAX = game, amount); a type 1 unit that fails the
// 0xA68E(unit, 0x36) / 0xA580(0x1220(EAX = class)) / 0x22A80 / 0xA7F6 checks does the same. The rest (unit types other than 0 / 1 are reached only when 0xDAE20 >= 0):
// the unit's data record gd = game[0x80]; 0xDAE20(EAX = unit; gd) >= 0 returns; a type 1 unit divides the amount by the 0xA7BA(gd[0x6d]) record's +0x14;
// ebx = gd[0xA8] + amount; with no pile (A5D4(unit, 1) == 0) one is made (A3CA, A6A0, A5B6, A75A, A60A) else its A790 amount is raised to ebx via A778.
// Then 0x8EA80(ECX = gd, EDX = unit; 0xC, ebx, 0, 0, 0), a walk of the unit's +0xDC list (0x8E6F0 for each node of kind 2, EDI = gd) and 0x8EA80(...; 2, gd[0xA8] + amount + 1, 0, 0, 0).
// A null game with an A5D4 result, a zero divisor, and a non-type-1 unit after DAE20 are fatal (Fog 0xA2DA / 0xA2C2) and not modelled.
#include "d2game_seam4e.h"
// 1.13c D2Game+0xDDAC0
extern "C" void d2game_ddac0(uint32_t unit, uint32_t game, uint32_t amount, const D2Seam* e)
{
    if ((int32_t)amount <= 0) return;
    const uint32_t t = unit ? d2rd32(unit) : 6;
    if (t == 0) { e->call(0xDC780, game, 0, 0, 0, 0, amount); return; }
    if (t == 1) {
        if (e->call(0xA68E, 0, 0, 0, 0, 0, unit, 0x36)) return;
        const uint32_t k = e->call(0x1220, d2rd32(unit + 4), 0, 0, 0, 0, unit);
        if (e->call(0xA580, 0, 0, 0, 0, 0, k) || e->call(0x22A80, unit, 0, 0, 0, 0, 8) || e->call(0xA7F6, 0, 0, 0, 0, 0, unit)) {
            e->call(0xDC780, game, 0, 0, 0, 0, amount);
            return;
        }
    }
    const uint32_t pile = e->call(0xA5D4, 0, 0, 0, 0, 0, unit, 1);
    if (!game) return;
    const uint32_t gd = d2rd32(game + 0x80);
    if ((int32_t)e->call(0xDAE20, unit, 0, 0, 0, 0, gd) >= 0) return;
    if (unit && d2rd32(unit) == 1) {
        const uint32_t rec = e->call(0xA7BA, 0, 0, 0, 0, 0, d2rd8(gd + 0x6D));
        const int32_t div = (int32_t)d2rd32(rec + 0x14);
        if (!div) return;
        amount = (uint32_t)((int32_t)amount / div);
    }
    uint32_t ebx = d2rd32(gd + 0xA8) + amount;
    if (!pile) {
        e->call(0xA3CA, 0, 0, 0, 0, 0, unit, 1, 1);
        const uint32_t np = e->call(0xA6A0, 0, 0, 0, 0, 0, d2rd32(gd + 0x1C), 2, ebx, d2rd32(game), d2rd32(game + 0xC));
        e->call(0xA5B6, 0, 0, 0, 0, 0, np, 1);
        e->call(0xA75A, 0, 0, 0, 0, 0, np, 0xB2000000u | 0xDBD90u);
        e->call(0xA60A, 0, 0, 0, 0, 0, unit, np, 1);
    } else {
        const uint32_t cur = e->call(0xA790, 0, 0, 0, 0, 0, pile);
        if ((int32_t)cur < (int32_t)ebx) e->call(0xA778, 0, 0, 0, 0, 0, pile, ebx);
        else ebx = cur;
    }
    e->call(0x8EA80, 0, gd, unit, 0, 0, 0xC, ebx, 0, 0, 0);
    if (!unit || d2rd32(unit) != 1) return;
    for (uint32_t n = d2rd32(unit + 0xDC); n;) {
        const uint32_t next = d2rd32(n + 0x24);
        if (d2rd8(n) == 2) { e->call(0x8E6F0, n); e->edi(0x8E6F0, gd); }
        n = next;
    }
    e->call(0x8EA80, 0, gd, unit, 0, 0, 2, d2rd32(gd + 0xA8) + amount + 1, 0, 0, 0);
}
