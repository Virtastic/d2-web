#pragma once

#include <cstddef>
#include <cstdint>

// Snapshot of one active room at its enumeration point. The caller may activate an inactive
// room immediately before sampling, or skip AddRoomData for a room that was already active.
// These fields do not describe the later D2Game AutoPos chooser invocation.
struct D2DrlgRoomTrace {
    int32_t ordinal, type, tileX, tileY, tileWidth, tileHeight;
    uint32_t activeSeedAtEnumerationLow, activeSeedAtEnumerationHigh;
    int32_t collisionX, collisionY, collisionWidth, collisionHeight;
};
static_assert(sizeof(D2DrlgRoomTrace) == 48, "DRLG room trace ABI");
static_assert(offsetof(D2DrlgRoomTrace, activeSeedAtEnumerationLow) == 24);
static_assert(offsetof(D2DrlgRoomTrace, collisionX) == 32);
