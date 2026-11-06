// 1.13c D2Game+0xAD6C0: event callback (ecx game, edx event; stack attacker, unit, damage, packed, arg5; ret 0x14).
// attacker and unit required, [unit+0xC4] bit 2 set; level = #A802(attacker, packed >> 16, packed) (<= 0 out); us = #A42A(unit, 0xC, 0), as = #A42A(attacker, 0xC, 0).
// damage = as > us ? (as - us) * 10 + 25 : 25. Zeroed 0x70-byte block: +4 = 0x4021, +0x24 = level << 8, +0x30 = damage, +0x60 = 0x3D.
// #DE0C0(eax unit; game, attacker, 1, &block) then #DCF20(eax unit; game, attacker, &block). Returns 1 / 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ad6c0(uint32_t game, uint32_t event, uint32_t attacker, uint32_t unit, uint32_t damage, uint32_t packed, uint32_t arg5, const D2Seam* e)
{
    (void)event; (void)damage; (void)arg5;
    if (!attacker || !unit) return 0;
    if (!((d2rd32(unit + 0xC4) >> 2) & 1)) return 0;
    const uint32_t level = e->call(0xA802, 0, 0, 0, 0, 0, attacker, packed >> 16, packed);
    if ((int32_t)level <= 0) return 0;
    const uint32_t us = e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0xC, 0);
    const uint32_t as = e->call(0xA42A, 0, 0, 0, 0, 0, attacker, 0xC, 0);
    uint32_t dmg = 0x19;
    if ((int32_t)as > (int32_t)us) dmg = (as - us) * 10 + 25;
    uint32_t block[28] = {};
    block[1] |= 0x4021;
    block[9] = level << 8;
    block[12] = dmg;
    block[24] = 0x3D;
    e->call(0xDE0C0, unit, 0, 0, 0, 0, game, attacker, 1, (uint32_t)(uintptr_t)block);
    e->blk(0xDE0C0, block, 28);
    e->call(0xDCF20, unit, 0, 0, 0, 0, game, attacker, (uint32_t)(uintptr_t)block);
    return 1;
}
