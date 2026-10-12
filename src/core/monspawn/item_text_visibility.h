#pragma once

#include <cstdint>

namespace d2::item_text {

// Retail's IFLAG_IDENTIFIED bit from D2Items.h. Item affix names and magical
// property details are visible to the client only after this bit is set.
inline constexpr uint32_t kIdentifiedItemFlag = 0x00000010u;

inline bool identified(uint32_t itemFlags) {
    return (itemFlags & kIdentifiedItemFlag) != 0;
}

// Retail omits an unidentified item's stat-list payload from the client
// serialization. Keep server-side affix-adjusted stats out of tooltip values.
inline int32_t visibleStatValue(bool isIdentified, int32_t currentValue, int32_t baseValue) {
    return isIdentified ? currentValue : baseValue;
}

// ITEMS_CheckStatRequirement uses this exact positive-stat and threshold rule.
// For unidentified items, apply it to the requirement the client can actually see.
inline bool meetsStatRequirement(int32_t playerStat, int32_t visibleRequirement) {
    return playerStat > 0 && playerStat >= visibleRequirement;
}

// ITEMS_CheckRequirements treats the native -1 level sentinel as unrestricted.
inline bool meetsLevelRequirement(int32_t playerLevel, int32_t visibleRequirement) {
    return visibleRequirement == -1 || playerLevel >= visibleRequirement;
}

} // namespace d2::item_text
