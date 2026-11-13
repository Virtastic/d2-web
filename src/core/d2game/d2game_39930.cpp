// 1.13c D2Game+0x39930 -- skill callback: when a 0-99 roll (the unit's seed at +0x20) is below the skill's first value, raises life and mana by
// the skill's next two values (x256, capped at the unit's maxima) and clears the 0x63 state of the first stack word's stat list
// fastcall: EDX = unit, stack word 0 = stat list, stack word 1 = skill level (ret 0xC); skill row 0x7C = skills table + 0x7C * 0x23C
// Returns EAX at the ret: the table pointer (dt) when the table is short or the skills base is absent, 0 on a missing list or a failed 10501,
// the roll when it is not below the chance, else the result of the final 0xA3CA call.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f39930(NE& e, uint32_t dt, uint32_t unit, uint32_t list, uint32_t lvl)
{
    if ((int32_t)e.rd32(dt + 0xba0) <= 0x7c) return dt;
    const uint32_t row = e.rd32(dt + 0xb98) + 0x11510u;
    if (!row) return dt;
    if (!list) return 0;
    if (!e.imp(0xAF52, list)) return 0;
    const int32_t chance = (int32_t)e.imp(0xAC9A, unit, e.rd32(row + 0x138), 0x7c, lvl);   // Ordinal 10786
    const int32_t roll = (int32_t)e.h(0x1180, 100, unit + 0x20);
    if (roll >= chance) return (uint32_t)roll;
    int32_t hpGain = (int32_t)e.imp(0xAC9A, unit, e.rd32(row + 0x13c), 0x7c, lvl);
    const int32_t hpMax = (int32_t)e.imp(0xA3A0, unit);
    int32_t hp = (int32_t)e.imp(0xA42A, unit, 6, 0) + (int32_t)((uint32_t)hpGain << 8);
    if (hpMax < hp) hp = hpMax;
    e.imp(0xA472, unit, 6, (uint32_t)hp, 0);
    const int32_t mpGain = (int32_t)e.imp(0xAC9A, unit, e.rd32(row + 0x140), 0x7c, lvl);
    const int32_t mpMax = (int32_t)e.imp(0xA4EA, unit);
    int32_t mp = (int32_t)e.imp(0xA42A, unit, 8, 0) + (int32_t)((uint32_t)mpGain << 8);
    if (mpMax < mp) mp = mpMax;
    e.imp(0xA472, unit, 8, (uint32_t)mp, 0);
    const uint32_t last = e.imp(0xA3CA, list, 0x63, 1);
    e.wr32(list + 0xc4, e.rd32(list + 0xc4) & 0xfffffff9u);
    return last;
}
}
