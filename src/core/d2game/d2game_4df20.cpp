// 1.13c D2Game+0x4DF20 (sub_4DF20): eax = amount, ebx = unit. Adds amount to stat 6 of the unit, capped at the unit's maximum (#10913);
// returns the amount actually added (0 when amount < 1).
#include "d2game_seam4e.h"
extern "C" int32_t d2game_4df20(int32_t amount, uint32_t unit, const D2Seam* e)
{
    if (amount < 1) return 0;
    int32_t cur = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, 6, 0);
    int32_t mx = (int32_t)e->call(0xA3A0, 0, 0, 0, 0, 0, unit);
    cur = (int32_t)((uint32_t)cur + (uint32_t)amount);
    if (cur > mx) { amount = (int32_t)((uint32_t)amount + ((uint32_t)mx - (uint32_t)cur)); cur = mx; }
    e->call(0xA472, 0, 0, 0, 0, 0, unit, 6, (uint32_t)cur, 0);
    return amount;
}
