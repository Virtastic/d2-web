#pragma once

#include <cstdint>

namespace d2::monspawn {

// D2Client's pattern builder stores every nonzero D2Common collision result as blocked.
// An unresolved room produces no local mask, so preserve the native invalid-cell result.
inline constexpr bool lightOcclusionCellBlocked(bool collisionResolved, uint16_t collisionResult) {
    return !collisionResolved || collisionResult != 0;
}

} // namespace d2::monspawn
