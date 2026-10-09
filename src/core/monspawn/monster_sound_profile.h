#pragma once

#include <cstdint>

namespace d2::monster_sound {

// FUN_6FAD2A70 uses a nonzero SuperUniques/UMonSound override when present,
// then falls through to the class MonSound profile. Profile zero is a valid
// class-table index; zero is only "no override" for the two override fields.
constexpr int32_t selectProfileId(bool isSuperUnique, bool hasUniqueOrMinionFlag,
                                  int32_t superUniqueProfileId, int32_t uniqueProfileId,
                                  int32_t classProfileId) {
    if (isSuperUnique && superUniqueProfileId > 0) return superUniqueProfileId;
    if (hasUniqueOrMinionFlag && uniqueProfileId > 0) return uniqueProfileId;
    return classProfileId >= 0 ? classProfileId : -1;
}

} // namespace d2::monster_sound
