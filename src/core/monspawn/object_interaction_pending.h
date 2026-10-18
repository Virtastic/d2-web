#pragma once

namespace d2::monspawn {

// 1.13c D2Game+0xD01D0 (sub_6FC828D0, the interaction dispatcher the click reaches)
// Retail's walk-mode per-frame handler sub_6FC7F780 completes an interaction started by an
// out-of-range click: once its own path run ends (`v44 == 2`) it re-enters sub_6FC828D0 with the
// interact target stored by UNITS_SetInteractData, and that call operates the unit when it is now
// in range. The session reproduces the same completion for object clicks (sub_6FC828D0's
// UNIT_OBJECT branch), which otherwise only walks.
//
// The path still has to name the clicked object: a later move or a click on something else
// repoints or clears pTargetUnit, and the old click is superseded (the F715 stale-command rule).
// A spent path (`dwPathPoints == 0`) is the arrival condition retail's handler uses.
template <typename DynamicPath, typename Unit>
inline bool objectInteractionIsPendingOn(const DynamicPath* path, const Unit* pendingObject) noexcept {
    return path && pendingObject && path->pTargetUnit == pendingObject;
}

template <typename DynamicPath, typename Unit>
inline bool objectInteractionShouldComplete(const DynamicPath* path, const Unit* pendingObject, bool dying) noexcept {
    return !dying && objectInteractionIsPendingOn(path, pendingObject) && path->dwPathPoints == 0;
}

} // namespace d2::monspawn
