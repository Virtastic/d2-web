// 1.13c D2Game+0xADBF0: event callback (ecx game, edx event; stack attacker, unit, damage, packed, arg5; ret 0x14).
// chance = #A802(attacker, packed >> 16, packed) (<= 0 out); the attacker's seed (attacker+0x20/+0x24, 64-bit LCG x 0x6AC690C5 + hi) steps once
// and its low word % 100 must be below chance. id = (packed & 0xFFFF) >> (dt+0xC6C & 31) in [0, dt+0xBA0) with a nonzero row; level = dt+0xC70 & packed & 0xFFFF.
// unit given: #F17E0(eax unit, ecx attacker; id, level, 0) else #AAD8/#AAC6([attacker+0x2C]) -> #F1730(eax attacker, ebx x; id, level, y, 0). Returns 1 / 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_adbf0(uint32_t dt, uint32_t game, uint32_t event, uint32_t attacker, uint32_t unit, uint32_t damage, uint32_t packed, uint32_t arg5, const D2Seam* e)
{
    (void)game; (void)event; (void)damage; (void)arg5;
    if (!attacker) return 0;
    const int32_t chance = (int32_t)e->call(0xA802, 0, 0, 0, 0, 0, attacker, packed >> 16, packed);
    if (chance <= 0) return 0;
    const uint32_t lo = packed & 0xFFFF;
    const uint32_t level = d2rd32(dt + 0xC70) & lo;
    const int32_t id = (int32_t)lo >> (d2rd32(dt + 0xC6C) & 0x1F);
    const uint64_t m = (uint64_t)d2rd32(attacker + 0x20) * 0x6AC690C5u + d2rd32(attacker + 0x24);
    d2wr32(attacker + 0x20, (uint32_t)m);
    d2wr32(attacker + 0x24, (uint32_t)(m >> 32));
    if ((int32_t)((uint32_t)m % 100) >= chance) return 0;
    if (id < 0 || id >= (int32_t)d2rd32(dt + 0xBA0)) return 0;
    if (!(d2rd32(dt + 0xB98) + (uint32_t)id * 0x23C)) return 0;
    if (unit) { e->call(0xF17E0, unit, attacker, 0, 0, 0, (uint32_t)id, level, 0); return 1; }
    const uint32_t x = e->call(0xAAD8, 0, 0, 0, 0, 0, d2rd32(attacker + 0x2C));
    const uint32_t y = e->call(0xAAC6, 0, 0, 0, 0, 0, d2rd32(attacker + 0x2C));
    e->call(0xF1730, attacker, 0, 0, x, 0, (uint32_t)id, level, y, 0);
    return 1;
}
