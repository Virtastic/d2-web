#pragma once

namespace d2::realm {

// Drop a dependent unit's DynamicPath target before the target CUnit is released.
template <typename Target, typename Dependent, typename GetPath, typename GetTarget, typename ClearTarget>
bool clearTargetRelationTo(Target* target, Dependent* dependent, GetPath getPath,
        GetTarget getTarget, ClearTarget clearTarget) {
    if (!target || !dependent) return false;
    auto* path = getPath(dependent);
    if (!path || getTarget(path) != target) return false;
    clearTarget(dependent);
    return true;
}

}  // namespace d2::realm
