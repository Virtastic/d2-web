#pragma once

#include <cstdint>

namespace d2::item_text {

// 1.13c D2Client+0x546E0: the number a stat's description line shows. ItemStatCost op 2..5 first
// scale the value by the op-base stat of the player (that stat's own ValShift removed) and shift
// it right by OpParam; then every value drops the stat's ValShift. maxhp/maxmana/maxstamina and
// the per-level life/mana stats carry ValShift 8, so a +25 life item holds 6400 and shows 25.
inline int32_t descValue(int32_t value, int op, int opParam, int32_t opBaseValue, int opBaseShift, int valShift) {
    if (op > 1 && op < 6) value = (opBaseValue >> (opBaseShift & 0x1F)) * value >> (opParam & 0x1F);
    return value >> (valShift & 0x1F);
}

}  // namespace d2::item_text
