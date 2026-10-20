// 1.13c D2Game+0x39B40 -- register-argument variant of MISSMODE_RollDamageValue (0x56290):
// ECX = mastery stat (-1 = none), EAX = max-damage stat, stack = unit, min-damage stat.
// Same body (max read first, min second, swap if inverted, mastery % on both, seed roll in
// [min, max]); only the calling convention differs. Callers: 0x3A280 (unmapped), 0x3A4E0.
#include "MISSILES/MissMode.h"

int32_t d2game_roll_damage_value_39b40(int32_t nMasteryStat, int32_t nMaxDamStat, D2UnitStrc* pUnit, int32_t nMinDamStat)
{
    return MISSMODE_RollDamageValue(pUnit, nMinDamStat, nMaxDamStat, nMasteryStat);
}
