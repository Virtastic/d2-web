// D2Game+0x50E60 -- Skill_AddPlayerSkill: spawn a skill's summoned unit at a spot and finish it.
// EAX = skill id, ECX = x, stack (game, unit, y, level), ret 0x10. `dt` = *sgptDataTables (+0xB98 rows of 0x23C, +0xBA0 count, +0xBF0 count).
// id 0: 0. 0x9EE50(unit, 0, id, level; EDX = &out) below 0: 0. Row byte +0xBE >= dt+0xBF0 becomes 0. Room = 0xA478(unit); 0x1280(EAX = room, EBX = x, EDI = y);
// 0xA808(that) nonzero: 0. Request {9, unit, cls, 0, out, x, y, byte, 0xAC9A(unit, row+0xC0, id, level)}; pet = 0xA3610(EAX = request; game): none: 0.
// 0x4F970(game, unit, pet, id, level, 0), 0x232B0(EBX = 0xF, ESI = game; pet, 0), 0xEFDB0(EBX = 9, ESI = game; pet, 0): pet.
#include "d2game_seam4e.h"
// 1.13c D2Game+0x50E60
extern "C" uint32_t d2game_50e60(int32_t id, uint32_t x, uint32_t game, uint32_t unit, uint32_t y, uint32_t level, uint32_t dt, const D2Seam* e)
{
    if (id == 0) return 0;
    if (id < 0 || id >= (int32_t)d2rd32(dt + 0xBA0)) return 0;   // retail dereferences a null row here
    const uint32_t row = d2rd32(dt + 0xB98) + (uint32_t)id * 0x23C;
    uint32_t out = 0;
    const int32_t cls = (int32_t)e->call(0x9EE50, 0, 0, (uint32_t)(uintptr_t)&out, 0, 0, unit, 0, (uint32_t)id, level);
    if (cls < 0) return 0;
    uint32_t bt = d2rd8(row + 0xBE);
    if ((int32_t)bt >= (int32_t)d2rd32(dt + 0xBF0)) bt = 0;
    const uint32_t rm = e->call(0xA478, 0, 0, 0, 0, 0, unit);
    const uint32_t r2 = e->call(0x1280, rm, 0, 0, x);
    if (e->call(0xA808, 0, 0, 0, 0, 0, r2)) return 0;
    uint32_t q[9] = { 9, unit, (uint32_t)cls, 0, out, x, y, bt, 0 };
    q[8] = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(row + 0xC0), (uint32_t)id, level);
    const uint32_t pet = e->call(0xA3610, (uint32_t)(uintptr_t)q, 0, 0, 0, 0, game);
    e->blk(0xA3610, q, 9);
    if (!pet) return 0;
    e->call(0x4F970, 0, 0, 0, 0, 0, game, unit, pet, (uint32_t)id, level, 0);
    e->call(0x232B0, 0, 0, 0, 0xF, game, pet, 0);
    e->call(0xEFDB0, 0, 0, 0, 9, game, pet, 0);
    return pet;
}
