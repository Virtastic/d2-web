// D2Game+0x4D710 -- SkillSummonPetWithStats: summon a skill's pet next to a player unit and scale its stats.
// ECX = game, EDX = unit, stack (skill id, arg1), ret 8. Skill id outside 0..dt +0xBA0, row address 0, no unit or unit type (dword +0) not 0: 0.
// cls = 0x9EE50(EAX = 0, ECX = 0, EDX = &out; unit, 0, skill, arg1) below 0: 0. b = row byte +0xBE, 0 when it is at or above dt +0xBF0. unit +0xC4 |= 0x40.
// Request {0, unit, cls, 0, out, 0, 0, b, 0xAC9A(unit, row +0xC0, skill, arg1)}: pet = 0xA3610(EAX = request; game): none: 0.
// 0x4E2A0(EAX = unit, ECX = 0; game, pet, arg1), c = 0xAC9A(unit, row +0x13C, skill, arg1), 0x4F970(game, unit, pet, skill, arg1, c).
// Then every node of the list at pet +0xDC (next = node +0x24) of type byte 2 goes to 0x8E6F0(EAX = node); 0x8EA80(ECX = game, EDX = pet; 2, game word +0xA8 + 0x14, 0, 0, 0),
// 0xA3CA(pet, 0x5D, 1), 0xAC76(pet, unit), 0xEFE40(EAX = the AC76 result, ECX = game, EBX = unit +0xD0, EDI = pet; 0). Returns 1.
#include "d2game_seam4e.h"
// 1.13c D2Game+0x4D710
extern "C" uint32_t d2game_4d710(uint32_t game, uint32_t unit, int32_t skill, uint32_t arg1, uint32_t dt, const D2Seam* e)
{
    if (skill < 0 || skill >= (int32_t)d2rd32(dt + 0xBA0)) return 0;
    const uint32_t row = (uint32_t)skill * 0x23C + d2rd32(dt + 0xB98);
    if (!row || !unit || d2rd32(unit)) return 0;
    uint32_t out = arg1;
    const int32_t cls = (int32_t)e->call(0x9EE50, 0, 0, (uint32_t)(uintptr_t)&out, 0, 0, unit, 0, (uint32_t)skill, arg1);
    if (cls < 0) return 0;
    uint32_t b = d2rd8(row + 0xBE);
    if ((int32_t)b >= (int32_t)d2rd32(dt + 0xBF0)) b = 0;
    d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) | 0x40);
    uint32_t q[9] = {};
    q[1] = unit; q[2] = (uint32_t)cls; q[4] = out; q[7] = b;
    q[8] = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(row + 0xC0), (uint32_t)skill, arg1);
    const uint32_t pet = e->call(0xA3610, (uint32_t)(uintptr_t)q, 0, 0, 0, 0, game);
    e->blk(0xA3610, q, 9);
    if (!pet) return 0;
    e->call(0x4E2A0, unit, 0, 0, 0, 0, game, pet, arg1);
    const uint32_t c = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(row + 0x13C), (uint32_t)skill, arg1);
    e->call(0x4F970, 0, 0, 0, 0, 0, game, unit, pet, (uint32_t)skill, arg1);
    (void)c;
    for (uint32_t n = d2rd32(pet + 0xDC); n;) {
        const uint32_t nx = d2rd32(n + 0x24);
        if (d2rd8(n) == 2) e->call(0x8E6F0, n);
        n = nx;
    }
    e->call(0x8EA80, 0, game, pet, 0, 0, 2, d2rd32(game + 0xA8) + 0x14, 0, 0, 0);
    e->call(0xA3CA, 0, 0, 0, 0, 0, pet, 0x5D, 1);
    const uint32_t r76 = e->call(0xAC76, 0, 0, 0, 0, 0, pet, unit);
    e->call(0xEFE40, r76, game, 0, d2rd32(unit + 0xD0), 0, 0);
    return 1;
}
