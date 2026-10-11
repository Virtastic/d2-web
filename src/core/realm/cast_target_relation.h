#pragma once

#include <cstdint>

namespace d2::realm {

// The 1.13c client has separate unit-target (0x16) and coordinate-target
// (0x15) skill actions. Both write an aim coordinate for facing, but only the
// coordinate action clears the path's target-unit relation.
template <typename Unit, typename SetPosition, typename SetTarget>
bool applyCastTargetRelation(uint8_t opcode, Unit* target, int x, int y,
        SetPosition&& setPosition, SetTarget&& setTarget) {
    if (opcode == 0x4c) {
        if (!target) return false;
        setPosition(x, y);
        setTarget(target);
        return true;
    }
    if (opcode == 0x4d) {
        setPosition(x, y);
        setTarget(static_cast<Unit*>(nullptr));
        return true;
    }
    return false;
}

// Client 6FB630F0 normalizes only moving missiles. Its 6FB50B30 fallback
// replaces the descriptor coordinates with a point two subtiles along the
// owner's eight-way facing, rather than incrementing the original aim.
template <typename Unit, typename GetX, typename GetY, typename GetDirection>
void normalizeMovingMissileTarget(Unit*& target, Unit* owner, int velocity,
        int sourceX, int sourceY, int& fallbackX, int& fallbackY,
        GetX&& getX, GetY&& getY, GetDirection&& getDirection) {
    if (!velocity || !owner) return;
    const bool coincidentUnit = target && target != owner &&
        getX(target) == getX(owner) && getY(target) == getY(owner);
    if (!coincidentUnit && (target || fallbackX != sourceX || fallbackY != sourceY)) return;
    constexpr int dx[8] = {0, -1, -2, -1, 0, 1, 2, 1};
    constexpr int dy[8] = {2, 1, 0, -1, -2, -1, 0, 1};
    const unsigned direction = static_cast<uint8_t>(getDirection(owner)) >> 3;
    fallbackX = getX(owner) + dx[direction];
    fallbackY = getY(owner) + dy[direction];
    if (coincidentUnit) target = nullptr;
}

} // namespace d2::realm
