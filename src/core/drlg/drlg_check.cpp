// Retail-gated DRLG check (s03-drlg): generate a level with the vendored D2MOO DRLG over our
// tables + tile libraries, and produce the same subtile collision grid summary the retail capture tool
// records from the running 1.13c D2Common.
// The call sequence mirrors the capture exactly: LoadAct -> GetLevel -> InitLevel -> for each
// RoomEx without an active room: AddRoomData / read collision grid / RemoveRoomData.
#include <emscripten.h>
#include <string>
#include <vector>
#include <D2Common.h>
#include <D2Dungeon.h>
#include <D2Collision.h>
#include <Drlg/D2DrlgDrlg.h>
#include "../d2moo/compat/datatbls_bridge.h"
#include "../tables/shared.h"
#include "../../../tools/vectorfmt/vectorfmt.h"

static D2DrlgActStrc* g_act = nullptr;
static uint8_t g_actNo = 0;
static std::vector<uint16_t> g_grid;
static std::string g_err;
// Result of the last d2_drlg_level: originX, originY, width, height, hash, tombArea
static int32_t g_out[6];

extern "C" {

// Loads the DRLG tables from the (already mounted) VFS. 1 on success.
EMSCRIPTEN_KEEPALIVE int d2_drlg_init() {
    static int done = 0;
    if (done) return done > 0;
    done = d2::drlgLoadTables(d2::sharedVfs(), d2::sharedLang()) ? 1 : -1;
    return done > 0;
}

// act is 1-based like the vectors; town level = first level of the act.
EMSCRIPTEN_KEEPALIVE int d2_drlg_act_begin(uint32_t seed, int difficulty, int act) {
    static const int kActFirstLevel[6] = { 1, 40, 75, 103, 109, 137 };
    if (g_act) { DUNGEON_FreeAct(g_act); g_act = nullptr; }
    if (act < 1 || act > 5) return 0;
    g_actNo = (uint8_t)(act - 1);
    g_act = DUNGEON_AllocAct(g_actNo, seed, TRUE, nullptr, (uint8_t)difficulty, nullptr, kActFirstLevel[act - 1], nullptr, nullptr);
    return g_act != nullptr;
}

EMSCRIPTEN_KEEPALIVE void d2_drlg_act_end() {
    if (g_act) { DUNGEON_FreeAct(g_act); g_act = nullptr; }
    g_grid.clear(); g_grid.shrink_to_fit();
}

// Generates `level` in the current act; fills d2_drlg_out(). 1 on success.
EMSCRIPTEN_KEEPALIVE int d2_drlg_level(int level) {
    if (!g_act) { g_err = "no act"; return 0; }
    D2DrlgStrc* pDrlg = g_act->pDrlg;
    D2DrlgLevelStrc* pLevel = DRLG_GetLevel(pDrlg, level);
    if (!pLevel) { g_err = "GetLevel null"; return 0; }
    if (!pLevel->pFirstRoomEx) DRLG_InitLevel(pLevel);
    if (!pLevel->pFirstRoomEx) { g_err = "InitLevel made no rooms"; return 0; }
    const int32_t ox = pLevel->nPosX * 5, oy = pLevel->nPosY * 5;
    const uint32_t w = (uint32_t)pLevel->nWidth * 5, h = (uint32_t)pLevel->nHeight * 5;
    g_grid.assign((size_t)w * h, 0xFFFF);
    for (D2DrlgRoomStrc* r2 = pLevel->pFirstRoomEx; r2; r2 = r2->pDrlgRoomNext) {
        const int rx = r2->nTileXPos, ry = r2->nTileYPos;
        bool added = false;
        if (!r2->pRoom) { DUNGEON_SetClientIsInSight(g_act, level, rx, ry, nullptr); added = true; }
        D2ActiveRoomStrc* r1 = r2->pRoom;
        D2RoomCollisionGridStrc* coll = r1 ? r1->pCollisionGrid : nullptr;
        if (coll) {
            const int64_t x = (int64_t)coll->pRoomCoords.nSubtileX - ox, y = (int64_t)coll->pRoomCoords.nSubtileY - oy;
            const uint32_t cx = (uint32_t)coll->pRoomCoords.nSubtileWidth, cy = (uint32_t)coll->pRoomCoords.nSubtileHeight;
            const uint16_t* p = coll->pCollisionMask;
            for (uint32_t j = 0; j < cy; j++) for (uint32_t i = 0; i < cx; i++, p++) {
                const int64_t gx = x + i, gy = y + j;
                if (gx >= 0 && gy >= 0 && gx < (int64_t)w && gy < (int64_t)h) g_grid[(size_t)gy * w + (size_t)gx] = *p;
            }
        }
        if (added) DUNGEON_UnsetClientIsInSight(g_act, level, rx, ry, nullptr);
    }
    uint32_t hsh = D2VEC_FNV_INIT;
    for (uint16_t v : g_grid) { hsh ^= v & 0xFF; hsh *= 16777619u; hsh ^= v >> 8; hsh *= 16777619u; }
    g_out[0] = ox; g_out[1] = oy; g_out[2] = (int32_t)w; g_out[3] = (int32_t)h; g_out[4] = (int32_t)hsh;
    g_out[5] = g_actNo == 1 ? pDrlg->nStaffTombLevel : 0;
    return 1;
}

EMSCRIPTEN_KEEPALIVE const int32_t* d2_drlg_out() { return g_out; }
EMSCRIPTEN_KEEPALIVE const uint16_t* d2_drlg_grid() { return g_grid.data(); }
EMSCRIPTEN_KEEPALIVE const char* d2_drlg_err() { return g_err.c_str(); }

} // extern "C"
