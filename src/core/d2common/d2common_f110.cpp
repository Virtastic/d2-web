// Prior art used: D2MOO DRLGMAZE_RollBasicPresets loop body (1.10f), split by 1.13c into
// per-room helpers; checked against the cached 1.13c decompiles.
#include <cstdint>
#include "D2DataTbls.h"
#include "D2Seed.h"
#include "Drlg/D2DrlgDrlg.h"
#include "Drlg/D2DrlgDrlgRoom.h"
#include "Drlg/D2DrlgMaze.h"
#include "Drlg/D2DrlgPreset.h"

// 1.13c D2Common+0x0C690 (BuildArea; level in EAX, map/flags/single-room on the stack)
extern "C" D2DrlgRoomStrc* d2_fn_d2common_c690(D2DrlgLevelStrc* level, D2DrlgMapStrc* map, int32_t flags, int32_t single)
{
    return DRLGPRESET_BuildArea(level, map, flags, single);
}

// 1.13c D2Common+0x0ECB0 (pick file; EAX=picked file, EBX=level, stack=map)
extern "C" void d2_fn_d2common_ecb0(D2DrlgMapStrc* map, int32_t picked, D2DrlgLevelStrc* level)
{
    if (picked != -1) { map->nPickedFile = picked; return; }
    int32_t def = map->pLvlPrestTxtRecord->dwDef, files = map->pLvlPrestTxtRecord->dwFiles, base;
    switch (level->nLevelType) {
    case 3: base = 0x34; break;   case 4: base = 0x6c; break;   case 7: base = 0xa7; break;
    case 8: base = 0xcd; break;   case 10: base = 0x101; break; case 0xd: base = 0x12d; break;
    case 0x11: base = 0x19d; break; case 0x12: base = 0x1e1; break; case 0x16: base = 0x2f1; break;
    case 0x18: base = 0x298; break; case 0x19: base = 0x2c0; break; case 0x1c: base = 0x344; break;
    case 0x21: base = 0x3ea; break; case 0x22: base = 0x422; break; case 0x23: base = 0x41c; break;
    default: return;
    }
    if (!(base < def && def < base + 16)) return;
    D2DrlgBuildStrc* b = level->pBuild;
    while (b && b->nPreset != def) b = b->pNext;
    if (!b) {
        b = D2_ALLOC_STRC_POOL(level->pDrlg->pMempool, D2DrlgBuildStrc);
        b->nPreset = def; b->nDivisor = files;
        b->nRand = SEED_RollLimitedRandomNumber(&level->pSeed, files);
        b->pNext = level->pBuild; level->pBuild = b;
    }
    b->nRand = (b->nRand + 1) % b->nDivisor;
    map->nPickedFile = b->nRand;
}

// 1.13c D2Common+0x0F110 (EAX=level, arg=room): one iteration of the maze preset roll
extern "C" void d2_fn_d2common_f110(D2DrlgLevelStrc* level, D2DrlgRoomStrc* room)
{
    D2DrlgMapStrc* map = DRLGPRESET_AllocDrlgMap(level, room->pMaze->nLevelPrest, &room->pDrlgCoord, &level->pSeed);
    d2_fn_d2common_ecb0(map, room->pMaze->nPickedFile, level);
    D2DrlgRoomStrc* built = d2_fn_d2common_c690(level, map, 0, room->nTileWidth <= 12 && room->nTileHeight <= 12);
    for (D2DrlgOrthStrc* o = room->pDrlgOrth; o; o = o->pNext)
        if (o->bInit == 1) DRLGROOM_AllocDrlgOrthsForRooms(built, o->pDrlgRoom, o->nDirection);
    DRLGROOM_FreeRoomEx(room);
}
