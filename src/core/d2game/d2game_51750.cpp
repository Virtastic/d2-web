// D2Game+0x51750 -- Player_LearnSkillInstance: instantiate a skill's unit for a player and attach it.
// ECX = game, EDX = unit, stack (skill id, arg1), ret 8. `dt` = *sgptDataTables (+0xB98 rows of 0x23C, +0xBA0 count, +0xA80 and +0xBF0 counts).
// Callees: thunk 0xAC9A (unit, row+0xC0, id, arg1), 0xA3610 (EAX = 9-dword request, stack game), 0x50070 (EAX = created unit, ESI = unit, EBX = arg1, stack game, id),
// thunk 0xA4C0 (unit), 0x8E2A0 (EAX = player-data +0x9C), 0xEFE40 (ECX = game, EBX = unit+0xD0, stack 0).
#include "d2game_seam4e.h"
// 1.13c D2Game+0x51750
extern "C" uint32_t d2game_51750(uint32_t game, uint32_t unit, int32_t id, uint32_t arg1, uint32_t dt, const D2Seam* e)
{
    if (unit) d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) | 0x40);
    if (id == 0) return 0;
    if (id < 0 || id >= (int32_t)d2rd32(dt + 0xBA0)) return 0;   // retail dereferences a null row here
    const uint32_t row = d2rd32(dt + 0xB98) + (uint32_t)id * 0x23C;
    const int32_t w = (int16_t)d2rd16(row + 0xBC);
    if (w < 0 || w >= (int32_t)d2rd32(dt + 0xA80)) return 0;
    uint32_t dl = d2rd8(row + 0xBE);
    if ((int32_t)dl >= (int32_t)d2rd32(dt + 0xBF0)) dl = 0;
    uint32_t cl = d2rd8(row + 0xBF);
    if (cl >= 0x10) cl = 1;
    uint32_t q[9] = { 0, unit, (uint32_t)w, 0, cl, 0, 0, dl, 0 };
    q[8] = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(row + 0xC0), (uint32_t)id, arg1);
    const uint32_t pet = e->call(0xA3610, (uint32_t)(uintptr_t)q, 0, 0, 0, 0, game);
    e->blk(0xA3610, q, 9);
    if (!pet) return 0;
    e->call(0x50070, pet, 0, 0, arg1, unit, game, (uint32_t)id);
    uint32_t eax = 0;
    if (unit && d2rd32(unit) == 0) eax = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9C);
    e->call(0x8E2A0, eax);
    e->call(0xEFE40, 0, game, 0, d2rd32(unit + 0xD0), 0, 0);
    return 1;
}
