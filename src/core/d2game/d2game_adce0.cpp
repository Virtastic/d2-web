// 1.13c D2Game+0xADCE0: event callback (ecx game, edx event; stack attacker, unit, damage, packed, arg5; ret 0x14).
// attacker required; chance = #A802(attacker, packed >> 16, packed) (<= 0 out); id = (packed & 0xFFFF) >> (dt+0xC6C & 31), level = dt+0xC70 & packed & 0xFFFF.
// damage given with byte [damage+4] & 4: roll = #11D0(ecx attacker+0x20, esi 100) must be below chance, then #1420(eax id) nonzero.
// unit given: #F17E0(eax unit, ecx attacker; id, level, 0) else #AAD8/#AAC6([attacker+0x2C]) -> #F1730(eax attacker, ebx x; id, level, y, 0). Returns 1 / 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_adce0(uint32_t dt, uint32_t game, uint32_t event, uint32_t attacker, uint32_t unit, uint32_t damage, uint32_t packed, uint32_t arg5, const D2Seam* e)
{
    (void)game; (void)event; (void)arg5;
    if (!attacker) return 0;
    const int32_t chance = (int32_t)e->call(0xA802, 0, 0, 0, 0, 0, attacker, packed >> 16, packed);
    if (chance <= 0) return 0;
    const uint32_t lo = packed & 0xFFFF;
    const uint32_t level = d2rd32(dt + 0xC70) & lo;
    const uint32_t id = (uint32_t)((int32_t)lo >> (d2rd32(dt + 0xC6C) & 0x1F));
    if (!damage || !(d2rd8(damage + 4) & 4)) return 0;
    if ((int32_t)e->call(0x11D0, 0, attacker + 0x20, 0, 0, 100) >= chance) return 0;
    if (!e->call(0x1420, id)) return 0;
    if (unit) { e->call(0xF17E0, unit, attacker, 0, 0, 0, id, level, 0); return 1; }
    const uint32_t x = e->call(0xAAD8, 0, 0, 0, 0, 0, d2rd32(attacker + 0x2C));
    const uint32_t y = e->call(0xAAC6, 0, 0, 0, 0, 0, d2rd32(attacker + 0x2C));
    e->call(0xF1730, attacker, 0, 0, x, 0, id, level, y, 0);
    return 1;
}
