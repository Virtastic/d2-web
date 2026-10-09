#pragma once

#include <cstdint>

// Offline diagnostics for a generated level. Seed fields are sampled at distinct lifecycle
// boundaries: RoomEx after DRLG_InitLevel and D2ActiveRoom after its tiles are built, before
// D2Game room population. Neither is the later Radament AutoPos chooser input by itself.
struct D2DrlgRoomSnapshot {
    int32_t ordinal, roomType, presetId;
    int32_t tileX, tileY, tileWidth, tileHeight;
    uint32_t generationSeed, roomSeedLow, roomSeedHigh;
    uint32_t activeSeedLow, activeSeedHigh;
    int32_t activeX, activeY, activeWidth, activeHeight;
    int32_t candidateMinX, candidateMinY, candidateMaxX, candidateMaxY;
    int32_t radamentPresetCount, radamentAnchorX, radamentAnchorY;
};
static_assert(sizeof(D2DrlgRoomSnapshot) == 92, "DRLG room snapshot ABI");

struct D2DrlgPresetSnapshot {
    int32_t roomOrdinal, presetOrdinal, unitType, index, mode, x, y, spawned;
};
static_assert(sizeof(D2DrlgPresetSnapshot) == 32, "DRLG preset snapshot ABI");

extern "C" {
const D2DrlgRoomSnapshot* d2_drlg_room_snapshot();
int d2_drlg_room_snapshot_count();
int d2_drlg_room_snapshot_truncated();
const D2DrlgPresetSnapshot* d2_drlg_preset_snapshot();
int d2_drlg_preset_snapshot_count();
int d2_drlg_preset_snapshot_truncated();
}
