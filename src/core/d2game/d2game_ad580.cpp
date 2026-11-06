// 1.13c D2Game+0xAD580: event callback (ecx game, edx event; stack attacker, unit, damage, packed, arg5; ret 0x14).
// attacker and unit required; level = #A802(attacker, packed >> 16, packed) (<= 0 out); as = #A42A(attacker, 0xC, 0), us = #A42A(unit, 0xC, 0).
// chance = ((max(level - 1, 0) * 4 - us) + as + 10) * 5 (event 6: as -= 6 first and chance /= 3), clamped to 0..100; the attacker's seed steps once and its
// low word % 100 must be below chance. damage = min(25 + 2 * (chance - roll), 250) at word 13 of a zeroed 0x70-byte block: #DE0C0(eax unit; game, attacker, 1, &block).
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ad580(uint32_t game, uint32_t event, uint32_t attacker, uint32_t unit, uint32_t damage, uint32_t packed, uint32_t arg5, const D2Seam* e)
{
    (void)damage; (void)arg5;
    if (!attacker || !unit) return 0;
    const int32_t level = (int32_t)e->call(0xA802, 0, 0, 0, 0, 0, attacker, packed >> 16, packed);
    if (level <= 0) return 0;
    uint32_t as = e->call(0xA42A, 0, 0, 0, 0, 0, attacker, 0xC, 0);
    const uint32_t us = e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0xC, 0);
    int32_t c = level - 1;
    if (c < 0) c = 0;
    if (event == 6) as -= event;
    int32_t chance = (int32_t)((((uint32_t)c << 2) - us + as + 10) * 5u);
    if (event == 6) chance /= 3;
    if (chance <= 0) chance = 0; else if (chance >= 100) chance = 100;
    const uint64_t m = (uint64_t)d2rd32(attacker + 0x20) * 0x6AC690C5u + d2rd32(attacker + 0x24);
    d2wr32(attacker + 0x20, (uint32_t)m);
    d2wr32(attacker + 0x24, (uint32_t)(m >> 32));
    const int32_t roll = (int32_t)((uint32_t)m % 100);
    if (roll >= chance) return 0;
    int32_t dmg = (chance - roll) * 2 + 25;
    if (dmg <= 0x19) dmg = 0x19; else if (dmg >= 0xFA) dmg = 0xFA;
    uint32_t block[28] = {};
    block[13] = (uint32_t)dmg;
    e->call(0xDE0C0, unit, 0, 0, 0, 0, game, attacker, 1, (uint32_t)(uintptr_t)block);
    e->blk(0xDE0C0, block, 28);
    return 1;
}
