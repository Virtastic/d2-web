// 1.13c D2Game+0xACDA0: event callback (ecx game, edx event; stack attacker, unit, damage, packed, arg5; ret 0x14).
// attacker with type [attacker] == 0 and damage with [damage+0x4C] > 0; level = #A802(attacker, packed >> 16, packed) (<= 0 out);
// max = #A4EA(attacker), cur = #A42A(attacker, 8, 0), out unless cur < max; add = #14D0(eax level, ecx 100, edx [damage+0x4C]);
// new = cur + add (<= 0 -> 0, >= max -> max); #A472(attacker, 8, new, 0), #A83E(attacker, 0x98, 0). Returns 1 / 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_acda0(uint32_t game, uint32_t event, uint32_t attacker, uint32_t unit, uint32_t damage, uint32_t packed, uint32_t arg5, const D2Seam* e)
{
    (void)game; (void)event; (void)unit; (void)arg5;
    if (!attacker || d2rd32(attacker) != 0 || !damage || (int32_t)d2rd32(damage + 0x4C) < 1) return 0;
    const int32_t level = (int32_t)e->call(0xA802, 0, 0, 0, 0, 0, attacker, packed >> 16, packed);
    if (level <= 0) return 0;
    const int32_t mx = (int32_t)e->call(0xA4EA, 0, 0, 0, 0, 0, attacker);
    const int32_t cur = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, attacker, 8, 0);
    if (!(cur < mx)) return 0;
    int32_t n = (int32_t)(e->call(0x14D0, (uint32_t)level, 100, d2rd32(damage + 0x4C)) + (uint32_t)cur);
    if (n <= 0) n = 0;
    if (!(n < mx)) n = mx;
    e->call(0xA472, 0, 0, 0, 0, 0, attacker, 8, (uint32_t)n, 0);
    e->call(0xA83E, 0, 0, 0, 0, 0, attacker, 0x98, 0);
    return 1;
}
