// 1.13c D2Game+0xACBA0: ACBA0 mana-leech event callback (event 0x17), ecx = game, edx = event, stack attacker, unit, damage, packed, arg5 (ret 0x14).
// Rule: as ACB10 with the attacker unit type (dword 0) required to be 0, max mana #A4EA, stat 8, notify 0x98.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_acba0(uint32_t attacker, uint32_t unit, uint32_t packed, const D2Seam* e)
{
    if (!attacker || !unit) return 0;
    if (d2rd32(attacker) != 0) return 0;
    const int32_t amt = (int32_t)e->call(0xA802, 0, 0, 0, 0, 0, attacker, packed >> 16, packed);
    if (!amt) return 0;
    const int32_t mx = (int32_t)e->call(0xA4EA, 0, 0, 0, 0, 0, attacker);
    const int32_t cur = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, attacker, 8, 0);
    if (cur >= mx) return 0;
    int32_t v = (int32_t)((uint32_t)cur + ((uint32_t)amt << 8));
    if (v <= 0) v = 0;
    if (v >= mx) v = mx;
    e->call(0xA472, 0, 0, 0, 0, 0, attacker, 8, (uint32_t)v, 0);
    e->call(0xA83E, 0, 0, 0, 0, 0, attacker, 0x98, 0);
    return 1;
}
