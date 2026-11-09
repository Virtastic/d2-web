// Online-client counterparts of the single-player session's DRLG readbacks (level rect, picked
// preset file, seam edges). They read the client DRLG act that the D2GS MapReveal/0x03 feed built
// (d2_drlg_client_act), so no session or engine object is needed. Read-only; every value is the
// DRLG field the single-player readback already reads.
//
// Confirmed: the rect fields (nPosX/nPosY/nWidth/nHeight, written by DRLG_SetLevelPositionAndSize)
// and the picked file (pPreset->nDirection, written by DRLG_InitLevel / GenerateLevel).
// GUESS: the seam edge geometry is the single-player derivation (monspawn_check.cpp
// d2_session_seam_edges); the retail D2Client decompile of its automap seam path is not yet
// cross-checked against it.

#include <algorithm>
#include <cstdint>
#include <cstring>

#include <emscripten.h>

#include <D2Dungeon.h>
#include <Drlg/D2DrlgDrlg.h>

extern "C" {
D2DrlgActStrc* d2_drlg_client_act();
}

namespace {

// Rect of a level in subtiles, the single-player unit (x5 = tile to subtile scaling).
int32_t g_realmLevelRect[4];
int32_t g_realmSeamEdges[32 * 4];

D2DrlgLevelStrc* realmLevel(int32_t levelId) {
    D2DrlgActStrc* act = d2_drlg_client_act();
    if (!act || !act->pDrlg || levelId <= 0) return nullptr;
    return DRLG_GetLevel(act->pDrlg, levelId);
}

}  // namespace

extern "C" {

// {x, y, width, height} in subtiles. Zeros if the client act has no such level.
EMSCRIPTEN_KEEPALIVE const int32_t* d2_realm_client_level_rect(int32_t levelId) {
    std::memset(g_realmLevelRect, 0, sizeof g_realmLevelRect);
    D2DrlgLevelStrc* level = realmLevel(levelId);
    if (!level) return g_realmLevelRect;
    g_realmLevelRect[0] = level->nPosX * 5;
    g_realmLevelRect[1] = level->nPosY * 5;
    g_realmLevelRect[2] = level->nWidth * 5;
    g_realmLevelRect[3] = level->nHeight * 5;
    return g_realmLevelRect;
}

// The preset file the DRLG picked for the level, or -1 while the level is not built.
EMSCRIPTEN_KEEPALIVE int32_t d2_realm_client_level_picked_file(int32_t levelId) {
    D2DrlgLevelStrc* level = realmLevel(levelId);
    return level && level->pPreset ? level->pPreset->nDirection : -1;
}

// Seam segments between the realm player's level (hereLevel, from the client room the D2GS feed
// reports) and destinationLevel. Up to 32 segments {x0, y0, x1, y1} in subtiles; returns the count.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_seam_edges(uint32_t destinationLevel, int32_t hereLevel) {
    int count = 0;
    D2DrlgLevelStrc* level = realmLevel(hereLevel);
    if (!level) return 0;
    if (!level->pFirstRoomEx) DRLG_InitLevel(level);   // client-mode levels build lazily, as in d2_drlg_client_first_room_xy
    for (D2DrlgRoomStrc* roomEx = level->pFirstRoomEx; roomEx && count < 32; roomEx = roomEx->pDrlgRoomNext) {
        for (int32_t n = 0; n < roomEx->nRoomsNear && count < 32; ++n) {
            D2DrlgRoomStrc* near = roomEx->ppRoomsNear[n];
            if (!near || !near->pLevel || near->pLevel->nLevelId != (int32_t)destinationLevel) continue;
            int32_t dx = 0, dy = 0;
            if (near->nTileXPos == roomEx->nTileXPos + roomEx->nTileWidth) dx = 1;
            else if (near->nTileXPos + near->nTileWidth == roomEx->nTileXPos) dx = -1;
            else if (near->nTileYPos == roomEx->nTileYPos + roomEx->nTileHeight) dy = 1;
            else if (near->nTileYPos + near->nTileHeight == roomEx->nTileYPos) dy = -1;
            else continue;
            const int32_t lo = dx ? std::max(roomEx->nTileYPos, near->nTileYPos) : std::max(roomEx->nTileXPos, near->nTileXPos);
            const int32_t hi = dx ? std::min(roomEx->nTileYPos + roomEx->nTileHeight, near->nTileYPos + near->nTileHeight)
                                  : std::min(roomEx->nTileXPos + roomEx->nTileWidth, near->nTileXPos + near->nTileWidth);
            if (lo >= hi) continue;
            const int32_t line = dx > 0 ? 5 * (roomEx->nTileXPos + roomEx->nTileWidth) : dx < 0 ? 5 * roomEx->nTileXPos :
                                 dy > 0 ? 5 * (roomEx->nTileYPos + roomEx->nTileHeight) : 5 * roomEx->nTileYPos;
            int32_t* e = &g_realmSeamEdges[4 * count++];
            e[0] = dx ? line : 5 * lo; e[1] = dx ? 5 * lo : line; e[2] = dx ? line : 5 * hi; e[3] = dx ? 5 * hi : line;
        }
    }
    return count;
}

EMSCRIPTEN_KEEPALIVE const int32_t* d2_realm_client_seam_edge_data() { return g_realmSeamEdges; }

}  // extern "C"
