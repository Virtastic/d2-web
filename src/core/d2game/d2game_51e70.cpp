// D2Game+0x51E70 -- Skill_AddClassSkills: spawn the skill's pets round the unit's spot and link them together.
// ECX = game, EDX = unit, stack (skill id, level), ret 8. `ty`/`tx` are the 12-entry dword offset tables at D2Game+0xFC17C (y) and +0xFC1AC (x).
// 0x9FA30(EAX = unit, ECX = game, EBX = &x; &y) zero: 0. unit +0xC4 |= 0x40. For each of the 12 offsets: pet = 0x50E60(EAX = skill, ECX = x + tx[i]; game, unit, y + ty[i], level); none: next.
// The first pet (type 1: monster data +0x14, then +0x28 gets flag 2 at +8, owner id +0x2C = unit +0xC (-1 without unit), owner type +0x30 = unit type (6), +0x28 = game);
// a later pet (type 1) gets the first pet's id and type there and +0x28 = game, then 0x20CF0(EAX = first; game). Every pet: 0xAC76(pet, unit); data = pet +0x2C;
// y = data +0x10 (pet type 2, 4, 5) or 0xA3B8(data) (0 without data); x = data +0xC or 0xA454(data); 0xB1BC(x, y, x0, y0) result goes to 0xAFE2(data, result). Returns 1.
#include "d2game_seam4e.h"
// 1.13c D2Game+0x51E70
extern "C" uint32_t d2game_51e70(uint32_t game, uint32_t unit, uint32_t skill, uint32_t level, uint32_t ty, uint32_t tx, const D2Seam* e)
{
    uint32_t x0 = 0, y0 = 0;
    if (!e->call(0x9FA30, unit, game, 0, (uint32_t)(uintptr_t)&x0, 0, (uint32_t)(uintptr_t)&y0)) return 0;
    if (unit) d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) | 0x40);
    uint32_t first = 0;
    for (uint32_t i = 0; i < 12; i++) {
        const uint32_t pet = e->call(0x50E60, skill, x0 + d2rd32(tx + 4 * i), 0, 0, 0, game, unit, y0 + d2rd32(ty + 4 * i), level);
        if (!pet) continue;
        if (!first) {
            first = pet;
            const uint32_t ot = unit ? d2rd32(unit) : 6, oi = unit ? d2rd32(unit + 0xC) : 0xFFFFFFFFu;
            if (d2rd32(pet) == 1) {
                const uint32_t p = d2rd32(d2rd32(pet + 0x14) + 0x28);
                d2wr8(p + 8, d2rd8(p + 8) | 2); d2wr32(p + 0x2C, oi); d2wr32(p + 0x30, ot); d2wr32(p + 0x28, game);
            }
        } else {
            if (d2rd32(pet) == 1) {
                const uint32_t p = d2rd32(d2rd32(pet + 0x14) + 0x28);
                d2wr32(p + 0x2C, d2rd32(first + 0xC)); d2wr32(p + 0x30, d2rd32(first)); d2wr32(p + 0x28, game);
            }
            e->call(0x20CF0, first, 0, 0, 0, 0, game);
        }
        e->call(0xAC76, 0, 0, 0, 0, 0, pet, unit);
        const uint32_t t = d2rd32(pet), data = d2rd32(pet + 0x2C);
        const bool direct = t == 2 || (t > 3 && t <= 5);
        const uint32_t y = direct ? d2rd32(data + 0x10) : (data ? e->call(0xA3B8, 0, 0, 0, 0, 0, data) : 0);
        const uint32_t x = direct ? d2rd32(data + 0xC) : (data ? e->call(0xA454, 0, 0, 0, 0, 0, data) : 0);
        const uint32_t r = e->call(0xB1BC, 0, 0, 0, 0, 0, x, y, x0, y0);
        e->call(0xAFE2, 0, 0, 0, 0, 0, d2rd32(pet + 0x2C), r);
    }
    return 1;
}
