#pragma once

namespace d2::monspawn {

// Read the target's current path position and pass it to the native facing updater.
// Kept independent of D2UnitStrc so the accepted-target and coordinate rule can be
// checked offline without linking the retail-compatible engine.
template <typename Unit, typename UpdateDirection>
bool updateAttackFacingToTarget(Unit* actor, Unit* target, UpdateDirection updateDirection) {
    if (!actor || !actor->pDynamicPath || !target || !target->pDynamicPath) return false;
    updateDirection(actor,
        target->pDynamicPath->tGameCoords.wPosX,
        target->pDynamicPath->tGameCoords.wPosY);
    return true;
}

} // namespace d2::monspawn
