// 1.13c D2Common+0xBF70 sub_BF70: room blob in r[0] (retail 32-bit image).
// Our structs are wider, so build them from the image and write back the two observed fields:
// room flags (+0x28) and map bInited (+0x3c). pFile is always set; river/navi callees are not reached.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "Drlg/D2DrlgDrlg.h"
#include "Drlg/D2DrlgPreset.h"
static uint32_t rd(const uint8_t* b, int o) { uint32_t v; memcpy(&v, b + o, 4); return v; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_bf70(uint32_t* r) {
    uint8_t* room = (uint8_t*)(uintptr_t)r[0];
    uint8_t* maze = (uint8_t*)(uintptr_t)rd(room, 0x20);
    uint8_t* map = (uint8_t*)(uintptr_t)rd(maze, 8);
    uint8_t* lvl = (uint8_t*)(uintptr_t)rd(room, 0x58);
    D2DrlgMapStrc m; memset(&m, 0, sizeof m);
    m.nLevelPrest = (int)rd(map, 0); m.nPickedFile = (int)rd(map, 4);
    m.pFile = (D2DrlgFileStrc*)(uintptr_t)rd(map, 0xc); m.bInited = (BOOL)rd(map, 0x3c);
    D2DrlgPresetRoomStrc pm; memset(&pm, 0, sizeof pm); pm.pMap = &m;
    D2DrlgStrc dr; memset(&dr, 0, sizeof dr);
    D2DrlgLevelStrc lv; memset(&lv, 0, sizeof lv); lv.pDrlg = &dr; lv.nLevelId = (int)rd(lvl, 0x1d0);
    D2DrlgRoomStrc rm; memset(&rm, 0, sizeof rm);
    rm.pMaze = &pm; rm.pLevel = &lv; rm.dwFlags = rd(room, 0x28);
    DRLGPRESET_SpawnHardcodedPresetUnits(&rm);
    uint32_t f = rm.dwFlags, i = (uint32_t)m.bInited;
    memcpy(room + 0x28, &f, 4); memcpy(map + 0x3c, &i, 4);
    r[4] = 0;
}
