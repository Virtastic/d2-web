// Prior art used: D2MOO DRLG outdoor placement, siege, wilderness and desert routines.
// Retail 1.13c splits the 1.10f initializers; these entry points retain those boundaries.
#include "Drlg/D2DrlgOutdoors.h"
#include "Drlg/D2DrlgOutPlace.h"
#include "Drlg/D2DrlgOutWild.h"
#include "Drlg/D2DrlgOutDesr.h"
#include "Drlg/D2DrlgDrlgRoom.h"
#include "Drlg/D2DrlgOutRoom.h"
#include "Drlg/D2DrlgTileSub.h"
#include "D2DataTbls.h"
#include "D2Seed.h"
#include <algorithm>
#include <cstdlib>

// 1.13c D2Common+0x41070
void D2Common_41070(D2DrlgLevelStrc* level) {
    D2_ASSERT(level->pOutdoors);
    auto* preset = DATATBLS_GetLvlPrestTxtRecord(0x361);
    D2_ASSERT(preset);
    const int step = preset->nSizeX / 8;
    int x = level->pOutdoors->nGridWidth - step;
    for (int i = 0; i < 15; ++i, x -= step) {
        D2_ASSERT(x >= 0);
        DRLGOUTDOORS_SpawnOutdoorLevelPresetEx(level, x, 0, 0x361 + i, 0, 0);
    }
}

// 1.13c D2Common+0x41230
void D2Common_41230(D2DrlgLevelStrc* level) {
    static constexpr int direction[12][2] = {{-1,0},{0,-1},{1,0},{0,1},{0,-1},{1,0},{0,1},{-1,0},{-1,0},{0,-1},{1,0},{0,1}};
    auto* out = level->pOutdoors;
    const int snow = level->nLevelId == 0x75 ? 0x4c : 0;
    const int lastX = out->nGridWidth - 2, lastY = out->nGridHeight - 2;
    int x = lastX, y = 0;
    while (x != 0 || y != lastY) {
        const int index = DRLGGRID_GetGridEntry(out->pGrid, x, y) - (0x371 + snow);
        DRLGOUTDOORS_SpawnOutdoorLevelPresetEx(level, x, y, index + 0x37d + snow, -1, 0);
        x += direction[index][0] * 2;
        y += direction[index][1] * 2;
    }
    DRLGOUTDOORS_SpawnOutdoorLevelPresetEx(level, lastX, 0, 0x38a + snow, -1, 0);
    DRLGOUTDOORS_SpawnOutdoorLevelPresetEx(level, 0, lastY, 0x389 + snow, -1, 0);
}

// 1.13c D2Common+0x41340
void D2Common_41340(D2DrlgLevelStrc* level) {
    auto* out = level->pOutdoors;
    auto* previous = out->pVertex;
    auto* current = previous->pNext;
    const int lookup = 4 + (level->nLevelId == 0x75);
    do {
        int dx, dy, nx, ny;
        DRLGVER_GetCoordDiff(previous, &dx, &dy);
        DRLGVER_GetCoordDiff(current, &nx, &ny);
        int x = previous->nPosX & ~1, y = previous->nPosY & ~1;
        const int endX = current->nPosX & ~1, endY = current->nPosY & ~1;
        const int preset = sub_6FD80BE0(dx, dy, lookup);
        if (!(previous->dwFlags & 2)) {
            while (x != endX || y != endY) {
                x += dx * 2; y += dy * 2;
                DRLGOUTDOORS_SpawnOutdoorLevelPresetEx(level, x, y, preset, -1, 0);
                DRLGGRID_AlterGridFlag(&out->pGrid[2], x, y, 1, FLAG_OPERATION_OR);
            }
        }
        if (previous->dwFlags & 1) {
            const int capX = (std::max(previous->nPosX, current->nPosX) - 4 * std::abs(dx)) & ~1;
            const int capY = (std::max(previous->nPosY, current->nPosY) - 4 * std::abs(dy)) & ~1;
            DRLGGRID_AlterGridFlag(&out->pGrid[2], capX, capY, 0x400, FLAG_OPERATION_OR);
            DRLGGRID_AlterGridFlag(&out->pGrid[2], capX + 2 * std::abs(dx), capY + 2 * std::abs(dy), 0x400, FLAG_OPERATION_OR);
        }
        if (const int corner = sub_6FD80C10(dx * 2, dy * 2, nx * 2, ny * 2, lookup)) {
            DRLGOUTDOORS_SpawnOutdoorLevelPresetEx(level, endX, endY, corner, -1, 0);
            DRLGGRID_AlterGridFlag(&out->pGrid[2], endX, endY, 1, FLAG_OPERATION_OR);
        }
        previous = current; current = current->pNext;
    } while (previous != out->pVertex);
    if (level->nLevelId == 0x6f) {
        DRLGGRID_AlterGridFlag(&out->pGrid[2], out->nGridWidth - 2, out->nGridHeight - 4, 0x400, FLAG_OPERATION_OR);
        DRLGGRID_AlterGridFlag(&out->pGrid[2], out->nGridWidth - 2, out->nGridHeight - 3, 0x400, FLAG_OPERATION_OR);
    }
}

// 1.13c D2Common+0x4A5C0: EAX flags, ECX level, stack rectangle/outdoor flags/DT1 mask.
D2DrlgRoomStrc* D2Common_4A5C0(D2DrlgLevelStrc* level, int x, int y, int width, int height, uint32_t roomFlags, uint32_t outdoorFlags, uint32_t outdoorFlagsEx, uint32_t dt1Mask) {
    auto* room = DRLGROOM_AllocRoomEx(level, DRLGTYPE_MAZE);
    room->nTileXPos = x; room->nTileYPos = y;
    room->nTileWidth = width; room->nTileHeight = height;
    DRLGROOM_AddRoomExToLevel(level, room);
    room->dwFlags |= roomFlags | 0x80000;
    room->dwDT1Mask = dt1Mask;
    room->pOutdoor->dwFlags = outdoorFlags;
    room->pOutdoor->dwFlagsEx = outdoorFlagsEx;
    auto* def = DATATBLS_GetLevelDefRecord(level->nLevelId);
    room->pOutdoor->nSubType = def->dwSubType;
    room->pOutdoor->nSubTheme = def->dwSubTheme;
    room->pOutdoor->nSubThemePicked = DRLGTILESUB_PickSubThemes(room, def->dwSubType, def->dwSubTheme);
    return room;
}

// 1.13c D2Common+0x17180: same complete border walk as the vendored routine.
void D2Common_17180(D2DrlgLevelStrc* level) { DRLGOUTPLACE_PlaceAct1245OutdoorBorders(level); }

// 1.13c D2Common+0x3E000: EAX level; final path phase split from the vertex collection.
void D2Common_3E000(D2DrlgLevelStrc* level) {
    // The vendored initializer includes collection (3D5A0), routing (3DD40),
    // path search (17EB0), grid marking and endpoint trimming (3D920), in order.
    DRLGOUTDOORS_SpawnAct1DirtPaths(level);
}

// 1.13c D2Common+0x7E8C0: EAX level; classify cliff runs only, no grid placement.
void D2Common_7E8C0(D2DrlgLevelStrc* level) {
    auto* out = level->pOutdoors;
    auto* first = out->pVertex;
    auto* vertex = first;
    auto* previous = first;
    for (auto* p = first->pNext; p != first; p = p->pNext) previous = p;
    bool crossed = false;
    do {
        if ((vertex->nPosX < vertex->pNext->nPosX && vertex->nPosY < previous->nPosY && !(vertex->dwFlags & 1) && !(previous->dwFlags & 1)) ||
            (vertex->pNext->nPosY < vertex->nPosY && vertex->nPosX < previous->nPosX && !(vertex->dwFlags & 1) && !(previous->dwFlags & 1))) {
            auto* start = vertex;
            D2DrlgVertexStrc* last = nullptr;
            do {
                if (vertex == first) crossed = true;
                if (sub_6FD85350(vertex)) break;
                if (sub_6FD85300(vertex)) last = vertex;
                vertex = vertex->pNext;
            } while (vertex != start);
            if (last) {
                for (auto* p = start; p != last; p = p->pNext) p->nDirection = 1;
                last->nDirection = 1;
                out->dwFlags |= 0x20;
            }
        }
        previous = vertex; vertex = vertex->pNext;
    } while (!crossed && vertex != first);
}

// 1.13c D2Common+0x7EE50: EAX level, ECX is an unused forwarded register argument.
void D2Common_7EE50(D2DrlgLevelStrc* level) {
    if (level->nLevelId == 0x27) return;
    auto* out = level->pOutdoors;
    if ((out->dwFlags & 0x10) && DRLGOUTWILD_TestSpawnRiver(level, out->nGridWidth / 2 - 1))
        DRLGOUTWILD_SpawnRiver(level, out->nGridWidth / 2 - 1);
    if (out->dwFlags & 0x80) DRLGOUTDOORS_SpawnOutdoorLevelPresetEx(level, 0, 0, 3, 1, 0);
    if (out->dwFlags & 0x100) DRLGOUTDOORS_SpawnOutdoorLevelPresetEx(level, out->nGridWidth - 7, 0, 3, 2, 0);
    if (out->dwFlags & 0x200) DRLGOUTDOORS_SpawnOutdoorLevelPresetEx(level, 0, 1, 2, 1, 0);
    if (out->dwFlags & 0x400) DRLGOUTDOORS_SpawnOutdoorLevelPresetEx(level, 0, out->nGridHeight - 6, 2, 1, 0);
    if (!(out->dwFlags & 0x40)) {
        BOOL added;
        if (level->nLevelId == 2) {
            auto* town = DRLG_GetLevel(level->pDrlg, 1);
            added = DRLGOUTDOORS_SpawnPresetFarAway(level, &town->pLevelCoords, 0x34, -1, 1, 15);
        } else added = DRLGOUTDOORS_SpawnOutdoorLevelPreset(level, 0x33, -1, 1, 15);
        D2_ASSERT(added);
        out->dwFlags |= 0x40;
    }
}

// 1.13c D2Common+0x7EF60: ESI level, ECX forwarded; preserves transposed scan bounds.
void D2Common_7EF60(D2DrlgLevelStrc* level) {
    if (level->nLevelId == 0x27) return;
    auto* out = level->pOutdoors;
    if ((out->dwFlags & 0xc) && DRLGOUTWILD_TestSpawnRiver(level, out->nGridWidth - 2))
        DRLGOUTWILD_SpawnRiver(level, out->nGridWidth - 2);
    if ((out->dwFlags & 0x20) && !(out->dwFlags & 0x40)) {
        const bool transpose = SEED_RollRandomNumber(&level->pSeed) & 1;
        bool added = false;
        for (int row = 0; row < out->nGridHeight && !added; ++row)
            for (int col = 0; col < out->nGridWidth && !added; ++col)
                added = DRLGOUTWILD_SpawnCliffCaves(level, transpose ? row : col, transpose ? col : row);
        D2_ASSERT(added);
    }
    if ((out->dwFlags & 0x1c) && !(out->dwFlags & 0x40)) {
        int x = out->nGridWidth - ((~(out->dwFlags >> 4) & 1) | 4);
        const auto random = SEED_RollRandomNumber(&level->pSeed);
        if (random & 1) x = 3;
        const int y = (random & 2) ? 3 : out->nGridHeight - 4;
        DRLGOUTDOORS_SpawnOutdoorLevelPresetEx(level, x, y, 0x33 + (level->nLevelId == 2), -1, 0);
        out->dwFlags |= 0x40;
    }
}

// 1.13c D2Common+0x7F180: EAX level; cottage IDs recovered from caller EAX loads.
void D2Common_7F180(D2DrlgLevelStrc* level) {
    const auto preset = [level](int id) { DRLGOUTDOORS_SpawnOutdoorLevelPreset(level, id, -1, 0, 15); };
    const auto random = [level](int id) { DRLGOUTDOORS_SpawnRandomOutdoorDS1(level, id, -1); };
    const auto cottage = [level](int id, int extra) { DRLGOUTWILD_SpawnCottage(level, id, extra); };
    switch (level->nLevelId) {
    case 2: random(0x2e); cottage(0x2f, 0); break;
    case 3: cottage(0x30, 1); preset(0x2c); break;
    case 4: random(0xa0); random(0x2d); preset(0xa2); cottage(0x2f, 1); cottage(0x2a, 0); preset(0x1f); return;
    case 5: preset(0xa1); preset(0x29); preset(0x28); cottage(0x30, 1); cottage(0x2b, 0); break;
    case 6: preset(0xa3); preset(0x26); preset(0x27); cottage(0x2f, 1); cottage(0x2a, 0); break;
    case 7: cottage(0x30, 1); cottage(0x2b, 0); preset(0x1f); return;
    case 0x11: DRLGOUTDOORS_SpawnOutdoorLevelPresetEx(level, 1, 1, 0x6c, -1, 0); return;
    case 0x27: preset(0x32); preset(0x2e); preset(0x1f); preset(0x26); preset(0x27); break;
    default: return;
    }
    preset(0x1d); preset(0x1e);
}

// 1.13c D2Common+0x858E0: EDI level; nine fixed tomb entries followed by the central warp.
void D2Common_858E0(D2DrlgLevelStrc* level) { DRLGOUTDESR_PlaceTombEntriesInCanyon(level); }

// 1.13c D2Common+0x85930: EDI level; one seed roll selects one of eight five-preset cliff layouts.
void D2Common_85930(D2DrlgLevelStrc* level) { DRLGOUTDESR_PlaceCliffs(level); }
