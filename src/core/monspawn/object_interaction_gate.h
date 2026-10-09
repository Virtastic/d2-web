#pragma once

namespace d2::monspawn {

// Retail's object-interaction packet only calls the object handler when the target is both
// in interaction range and unobstructed by the player-flying collision mask. Keep the
// collision query lazy: the native branch does not evaluate it when range already fails.
template <typename CollisionBlocked>
inline bool canOperateObjectNow(bool inInteractRange, CollisionBlocked&& collisionBlocked) {
    return inInteractRange && !collisionBlocked();
}

} // namespace d2::monspawn
