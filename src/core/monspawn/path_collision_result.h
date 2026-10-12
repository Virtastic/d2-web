#pragma once

namespace d2::path {

// A path collision query with no resolvable room/grid is blocked in retail.
// The diagnostic's -1 sentinel is reserved for the caller having no path context.
constexpr int collisionQueryResult(bool hasQueryRoom, bool collides) {
    return hasQueryRoom ? static_cast<int>(collides) : 1;
}

} // namespace d2::path
