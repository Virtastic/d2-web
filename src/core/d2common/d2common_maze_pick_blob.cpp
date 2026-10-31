// DRLGMAZE_PickRoomPreset over a room kept in the retail 32-bit blob layout (orth list +0, maze +0x20,
// level +0x58). r[0] = room blob, r[1] = reset flag. The wasm64 structs are rebuilt field by field at the
// retail offsets; the maze's preset, picked file and flags (blob +0..+0xC) are written back in place.
#include <cstdint>
#include <cstring>
#include "../d2moo/include/Drlg/D2DrlgMaze.h"
#include "../d2moo/include/Drlg/D2DrlgDrlg.h"
#include "../d2moo/include/Drlg/D2DrlgPreset.h"
#include "D2DataTbls.h"

static inline uint8_t* P(uint32_t v) { return (uint8_t*)(uintptr_t)v; }
static inline uint32_t D(const uint8_t* b, uint32_t o) { uint32_t v; memcpy(&v, b + o, 4); return v; }

extern "C" void d2common_maze_pick_room_preset_blob(uint32_t* r) {
    uint8_t* room = P(r[0]);
    D2DrlgOrthStrc orth[4] = {};
    int n = 0;
    D2DrlgOrthStrc* head = nullptr; D2DrlgOrthStrc* tail = nullptr;
    for (uint32_t o = D(room, 0); o && n < 4; o = D(P(o), 0x14), ++n) {
        orth[n].nDirection = (uint8_t)D(P(o), 4);
        if (tail) tail->pNext = &orth[n]; else head = &orth[n];
        tail = &orth[n];
    }
    uint8_t* lv = P(D(room, 0x58));
    D2DrlgStrc drlg = {}; D2LvlMazeTxt mz = {}; D2DrlgLevelStrc level = {}; D2DrlgPresetRoomStrc maze = {}; D2DrlgRoomStrc rm = {};
    drlg.nDifficulty = P(D(lv, 0x1b4))[0x450];
    uint8_t* lmz = P(D(lv, 0x14));
    memcpy(mz.dwRooms, lmz + 4, 12);
    level.pDrlg = &drlg; level.pMaze = &mz;
    level.nLevelType = (int32_t)D(lv, 0x1c0); level.nLevelId = (int32_t)D(lv, 0x1d0);
    uint8_t* mb = P(D(room, 0x20));
    maze.nLevelPrest = (int32_t)D(mb, 0); maze.nPickedFile = (int32_t)D(mb, 4); maze.dwFlags = D(mb, 0xC);
    rm.pDrlgOrth = head; rm.pLevel = &level; rm.pMaze = &maze;
    DRLGMAZE_PickRoomPreset(&rm, (BOOL)r[1]);
    memcpy(mb, &maze.nLevelPrest, 4); memcpy(mb + 4, &maze.nPickedFile, 4); memcpy(mb + 0xC, &maze.dwFlags, 4);
    r[4] = 0;
}
