// 1.13c D2Game+0xACA70: ACA70 life-leech event callback (event 0x18), ecx = game, edx = event, stack attacker, unit, damage, packed, arg5 (ret 0x14).
// Rule: attacker and unit non-null, #A814(unit) nonzero, amt = #A802(attacker, packed>>16, packed) nonzero, max = #A3A0(attacker), cur = stat 6; cur >= max returns 0; v = cur + amt*256 clamped to [0, max]; set stat 6 (#A472), notify 0x97 (#A83E), return 1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_aca70(uint32_t attacker, uint32_t unit, uint32_t packed, const D2Seam* e)
{
    if (!attacker || !unit) return 0;
    if (!e->call(0xA814, 0, 0, 0, 0, 0, unit)) return 0;
    const int32_t amt = (int32_t)e->call(0xA802, 0, 0, 0, 0, 0, attacker, packed >> 16, packed);
    if (!amt) return 0;
    const int32_t mx = (int32_t)e->call(0xA3A0, 0, 0, 0, 0, 0, attacker);
    const int32_t cur = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, attacker, 6, 0);
    if (cur >= mx) return 0;
    int32_t v = (int32_t)((uint32_t)cur + ((uint32_t)amt << 8));
    if (v <= 0) v = 0;
    if (v >= mx) v = mx;
    e->call(0xA472, 0, 0, 0, 0, 0, attacker, 6, (uint32_t)v, 0);
    e->call(0xA83E, 0, 0, 0, 0, 0, attacker, 0x97, 0);
    return 1;
}
