#pragma once

#include "D2Collision.h"
#include "Path/Path.h"

namespace d2::path {

// #10851 receives the dynamic path's starting room. In particular, the big
// patterns resolve the footprint's (x-1,y-1) corner before its center. Resolving
// the center first changes the set of rooms visible to that lookup.
inline int movementCollisionQuery(const D2DynamicPathStrc& path, int x, int y) {
    return COLLISION_CheckAnyCollisionWithPattern(path.pRoom, x, y,
        path.dwCollisionPattern, static_cast<uint16_t>(path.nMoveTestCollisionMask)) != 0;
}

} // namespace d2::path
