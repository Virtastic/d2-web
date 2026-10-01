// Retail-gated DRLG check (s03-drlg): generate a level with the vendored D2MOO DRLG over our
// tables + tile libraries, and produce the same subtile collision grid summary the retail capture tool
// records from the running 1.13c D2Common.
// The call sequence mirrors the capture exactly: LoadAct -> GetLevel -> InitLevel -> for each
// RoomEx without an active room: AddRoomData / read collision grid / RemoveRoomData.
#include <emscripten.h>
#include <algorithm>
#include <string>
#include <vector>
#include <D2Common.h>
#include <D2Dungeon.h>
#include <D2Collision.h>
#include <D2CMP.h>
#include <Drlg/D2DrlgDrlg.h>
#include <Drlg/D2DrlgOutdoors.h>
#include <Drlg/D2DrlgDrlgVer.h>
#include <Drlg/D2DrlgOutRoom.h>
#include "../d2moo/compat/datatbls_bridge.h"
#include "../tables/shared.h"
#include "../../../tools/vectorfmt/vectorfmt.h"

static D2DrlgActStrc* g_act = nullptr;
static uint8_t g_actNo = 0;
static std::vector<uint16_t> g_grid;
static std::string g_err;
// Tile identity under the CAPTURE's activation protocol (s173). the retail capture tool reads a room's tiles
// immediately after AddRoomData and before RemoveRoomData, one room at a time, so when it reads a
// room fewer of its neighbours are alive than in a session that activates every room and keeps
// them. Whether a shared edge cell's neighbour exists at tile time decides merge-vs-add and hence
// whether the room takes a roll, so the corpus can only be matched by the same protocol.
// Record: room ordinal, layer (0 floor, 1 wall, 2 roof), key x, key y, orientation, main, sub, frame.
struct D2DrlgTileRec { int32_t room, layer, x, y, orientation, main, sub, frame; };
static bool g_collectTiles = false;
static std::vector<D2DrlgTileRec> g_tiles;
static void collectTiles(int32_t room, int32_t layer, const D2DrlgTileDataStrc* tiles, int count) {
    if (!tiles) return;
    for (int i = 0; i < count; ++i) {
        const D2DrlgTileDataStrc& t = tiles[i];
        if (!t.pTile) continue;
        g_tiles.push_back({ room, layer, t.nWidth, t.nHeight, t.pTile->nType, t.pTile->nStyle, t.pTile->nSequence, t.pTile->nRarity_Frame });
    }
}
// The tile pick stream under the same protocol (s175, vector kind 26): one record per GetTiles call,
// with the GetTileRarity calls after it split into the weight sum and the ladder, labelled with the
// room whose AddRoomData is in progress -- exactly as the retail capture tool labels retail's.
extern void (*g_d2webOnGetTiles)(int, int, int, int, int);
extern void (*g_d2webOnTileRarity)(int);
static std::vector<D2VecTilePick> g_picks;
static int32_t g_pickRoom = -1; static uint32_t g_pickCall = 0; static int g_pickSumLeft = 0;
static uint32_t g_pickSeed = 0; static uint8_t g_pickDiff = 0; static uint16_t g_pickLevel = 0;
extern const D2DrlgRoomStrc* g_d2webTileCacheRoom;
// Parallel to g_picks: the room's tile rectangle (x, y, w, h) and its seed low word at the lookup.
static std::vector<int32_t> g_pickRooms;
// F511: the LEVEL of the rolled room (g_d2webTileCacheRoom), one int per pick -- the rect in
// g_pickRooms is not unique to a level, and the record's own `level` field is the level whose
// d2_drlg_level call is in progress, which is a different thing again (LinkedTileDataManager rolls
// the NEIGHBOUR's seed, and that neighbour can belong to another level).
static std::vector<int32_t> g_pickRoomLevel;
static std::string g_pickNear;
static void onGetTiles(int type, int style, int seq, int n, int firstFrame) {
    { const D2DrlgRoomStrc* q = g_d2webTileCacheRoom;
      g_pickRooms.insert(g_pickRooms.end(), { q ? q->nTileXPos : -1, q ? q->nTileYPos : -1, q ? q->nTileWidth : 0, q ? q->nTileHeight : 0, q ? (int32_t)q->pSeed.nLowSeed : 0 });
      g_pickRoomLevel.push_back(q && q->pLevel ? (int32_t)q->pLevel->nLevelId : -1); }
    // First sighting of a room: its near-room list in list order (drawhook logs retail's the same way).
    { const D2DrlgRoomStrc* q = g_d2webTileCacheRoom; static std::vector<const D2DrlgRoomStrc*> seen;
      if (g_picks.empty()) seen.clear();
      if (q && std::find(seen.begin(), seen.end(), q) == seen.end()) { seen.push_back(q);
        g_pickNear += "near room " + std::to_string(q->nTileXPos) + "," + std::to_string(q->nTileYPos) + " n=" + std::to_string(q->nRoomsNear) + ":";
        for (int i = 0; i < q->nRoomsNear; i++) g_pickNear += " " + std::to_string(q->ppRoomsNear[i]->nTileXPos) + "," + std::to_string(q->ppRoomsNear[i]->nTileYPos);
        g_pickNear += "\n"; } }
    D2VecTilePick r = {};
    r.seed = g_pickSeed; r.difficulty = g_pickDiff; r.act = g_actNo; r.level = g_pickLevel;
    r.room = g_pickRoom; r.call = g_pickCall++; r.type = type; r.style = style; r.sequence = seq;
    r.entries = n; r.chosenFrame = firstFrame;
    g_picks.push_back(r); g_pickSumLeft = n;
}
static void onTileRarity(int w) {
    if (g_picks.empty()) return;
    D2VecTilePick& r = g_picks.back();
    if (g_pickSumLeft > 0) { r.weightSum += w; --g_pickSumLeft; } else { ++r.ladder; r.chosenFrame = w; }
}

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

// The capture hands LoadAct D2Client's automap callbacks. DRLGPRESET_GenerateLevel branches on
// their PRESENCE: for an AutoMap preset it InitLevels the vis-linked levels and builds every room
// up front, which changes which neighbours are alive when the next rooms are built. The callbacks'
// own work (the client automap) is not modelled; s173 measures whether presence alone matches.
static void __stdcall noAutomap(D2ActiveRoomStrc*) {}
static void __stdcall noTownAutomap(int32_t, int32_t, int32_t, int32_t) {}

// act is 1-based like the vectors; town level = first level of the act.
EMSCRIPTEN_KEEPALIVE int d2_drlg_act_begin(uint32_t seed, int difficulty, int act) {
    static const int kActFirstLevel[6] = { 1, 40, 75, 103, 109, 137 };
    if (g_act) { DUNGEON_FreeAct(g_act); g_act = nullptr; }
    if (act < 1 || act > 5) return 0;
    g_actNo = (uint8_t)(act - 1);
    g_pickSeed = seed; g_pickDiff = (uint8_t)difficulty; g_pickLevel = 0; g_pickCall = 0; g_pickRoom = -1;   // LoadAct's own lookups: level 0
    g_act = DUNGEON_AllocAct(g_actNo, seed, TRUE, nullptr, (uint8_t)difficulty, nullptr, kActFirstLevel[act - 1], noAutomap, noTownAutomap);
    return g_act != nullptr;
}

EMSCRIPTEN_KEEPALIVE void d2_drlg_act_end() {
    if (g_act) { DUNGEON_FreeAct(g_act); g_act = nullptr; }
    g_grid.clear(); g_grid.shrink_to_fit();
}

// F511 A/B (see d2_drlg_collect_picks below): 1 keeps every room of the level alive after its tile
// stream is read -- the LIVE SESSION's eager build -- instead of the capture protocol's
// SetClientIsInSight / read / UnsetClientIsInSight, which frees each room and so leaves a room's
// shared-edge cells no live neighbour to merge into.
static int g_drlgKeepAlive = 0;
EMSCRIPTEN_KEEPALIVE void d2_drlg_keep_alive(int on) { g_drlgKeepAlive = on ? 1 : 0; }

// Generates `level` in the current act; fills d2_drlg_out(). 1 on success.
EMSCRIPTEN_KEEPALIVE int d2_drlg_level(int level) {
    if (!g_act) { g_err = "no act"; return 0; }
    D2DrlgStrc* pDrlg = g_act->pDrlg;
    g_pickLevel = (uint16_t)level; g_pickCall = 0;
    D2DrlgLevelStrc* pLevel = DRLG_GetLevel(pDrlg, level);
    if (!pLevel) { g_err = "GetLevel null"; return 0; }
    if (!pLevel->pFirstRoomEx) DRLG_InitLevel(pLevel);
    if (!pLevel->pFirstRoomEx) { g_err = "InitLevel made no rooms"; return 0; }
    const int32_t ox = pLevel->nPosX * 5, oy = pLevel->nPosY * 5;
    const uint32_t w = (uint32_t)pLevel->nWidth * 5, h = (uint32_t)pLevel->nHeight * 5;
    g_grid.assign((size_t)w * h, 0xFFFF);
    g_tiles.clear();
    int32_t roomOrdinal = -1;
    for (D2DrlgRoomStrc* r2 = pLevel->pFirstRoomEx; r2; r2 = r2->pDrlgRoomNext) {
        ++roomOrdinal;
        const int rx = r2->nTileXPos, ry = r2->nTileYPos;
        bool added = false;
        g_pickRoom = roomOrdinal;
        if (!r2->pRoom) { DUNGEON_SetClientIsInSight(g_act, level, rx, ry, nullptr); added = true; }
        D2ActiveRoomStrc* r1 = r2->pRoom;
        if (g_collectTiles && r1 && r1->pRoomTiles) {
            collectTiles(roomOrdinal, 0, r1->pRoomTiles->pFloorTiles, r1->pRoomTiles->nFloors);
            collectTiles(roomOrdinal, 1, r1->pRoomTiles->pWallTiles, r1->pRoomTiles->nWalls);
            collectTiles(roomOrdinal, 2, r1->pRoomTiles->pRoofTiles, r1->pRoomTiles->nRoofs);
        }
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
        if (added && !g_drlgKeepAlive) DUNGEON_UnsetClientIsInSight(g_act, level, rx, ry, nullptr);
        g_pickRoom = -1;
    }
    uint32_t hsh = D2VEC_FNV_INIT;
    for (uint16_t v : g_grid) { hsh ^= v & 0xFF; hsh *= 16777619u; hsh ^= v >> 8; hsh *= 16777619u; }
    g_out[0] = ox; g_out[1] = oy; g_out[2] = (int32_t)w; g_out[3] = (int32_t)h; g_out[4] = (int32_t)hsh;
    g_out[5] = g_actNo == 1 ? pDrlg->nStaffTombLevel : 0;
    return 1;
}

EMSCRIPTEN_KEEPALIVE const int32_t* d2_drlg_out() { return g_out; }
// The rasterised dirt-path grid of every outdoor room tiled while collecting (s178), in
// the retail capture tool's dirtgrid.txt form after its "seed diff level room" prefix:
// "room <ordinal> at <tx>,<ty> <w>x<h> path <W>x<H> <row> <row> ..." with each row "v,v,...,".
extern void (*g_d2webOnDirtPathGrid)(D2DrlgLevelStrc*, D2DrlgRoomStrc*, D2DrlgGridStrc*);
static std::string g_dirtGrids;
static void onDirtPathGrid(D2DrlgLevelStrc* pLevel, D2DrlgRoomStrc* pRoom, D2DrlgGridStrc* g) {
    char head[160];
    (void)pLevel;   // keyed like the capture: the level being walked, not the room's own
    snprintf(head, sizeof head, "%d %d room %d at %d,%d %dx%d path %dx%d", (int)g_pickLevel, g_pickRoom, g_pickRoom,
             pRoom->nTileXPos, pRoom->nTileYPos, pRoom->nTileWidth, pRoom->nTileHeight, g->nWidth, g->nHeight);
    g_dirtGrids += head;
    auto rows = [](const D2DrlgGridStrc* g) {
        for (int y = 0; y < g->nHeight; y++) {
            g_dirtGrids += ' ';
            for (int x = 0; x < g->nWidth; x++) { char v[16]; snprintf(v, sizeof v, "%x,", g->pCellsFlags[g->pCellsRowOffsets[y] + x]); g_dirtGrids += v; }
        }
    };
    rows(g);
    const D2DrlgGridStrc* floor = &pRoom->pOutdoor->pFloorGrid;
    snprintf(head, sizeof head, " floor %dx%d", floor->nWidth, floor->nHeight); g_dirtGrids += head;
    rows(floor);
    g_dirtGrids += '\n';
}
EMSCRIPTEN_KEEPALIVE void d2_drlg_collect_dirtgrids(int enable) { g_d2webOnDirtPathGrid = enable ? onDirtPathGrid : nullptr; g_dirtGrids.clear(); }
EMSCRIPTEN_KEEPALIVE const char* d2_drlg_dirtgrids() { return g_dirtGrids.c_str(); }

// Act I wilderness dirt paths of `level` in the current act (s178), in the retail capture tool's text form
// after its "seed diff level" prefix: "paths <n> | x,y x,y ... | ...". Read after d2_drlg_level,
// the point at which the capture reads retail's.
static std::string g_dirt;
EMSCRIPTEN_KEEPALIVE const char* d2_drlg_dirt_paths(int level) {
    g_dirt.clear();
    D2DrlgLevelStrc* pLevel = g_act ? DRLG_GetLevel(g_act->pDrlg, level) : nullptr;
    if (!pLevel || !pLevel->pOutdoors) { g_dirt = "no-outdoors"; return g_dirt.c_str(); }
    D2DrlgOutdoorInfoStrc* o = pLevel->pOutdoors;
    g_dirt = "paths " + std::to_string(o->nVertices);
    for (int i = 0; i < o->nVertices && i < 6; i++) {
        g_dirt += " |";
        int guard = 0;
        for (D2DrlgVertexStrc* v = o->pPathStarts[i]; v && guard < 64; v = v->pNext, guard++)
            g_dirt += " " + std::to_string(v->nPosX) + "," + std::to_string(v->nPosY);
    }
    return g_dirt.c_str();
}
// Collect the tile pick stream (vectorfmt.h D2VecTilePick) from the next d2_drlg_act_begin on.
EMSCRIPTEN_KEEPALIVE void d2_drlg_collect_picks(int enable) {
    g_picks.clear(); g_pickRooms.clear(); g_pickRoomLevel.clear(); g_pickNear.clear();
    g_d2webOnGetTiles = enable ? onGetTiles : nullptr;
    g_d2webOnTileRarity = enable ? onTileRarity : nullptr;
}
EMSCRIPTEN_KEEPALIVE const D2VecTilePick* d2_drlg_picks() { return g_picks.data(); }
EMSCRIPTEN_KEEPALIVE int d2_drlg_pick_count() { return (int)g_picks.size(); }
EMSCRIPTEN_KEEPALIVE const int32_t* d2_drlg_pick_rooms() { return g_pickRooms.data(); }
EMSCRIPTEN_KEEPALIVE const int32_t* d2_drlg_pick_levels() { return g_pickRoomLevel.data(); }
EMSCRIPTEN_KEEPALIVE const char* d2_drlg_pick_near() { return g_pickNear.c_str(); }   // near lists, first sighting per room   // 5 per pick: room x, y, w, h (tiles), seed low
EMSCRIPTEN_KEEPALIVE void d2_drlg_collect_tiles(int enable) { g_collectTiles = enable != 0; g_tiles.clear(); }
EMSCRIPTEN_KEEPALIVE const D2DrlgTileRec* d2_drlg_tiles() { return g_tiles.data(); }
EMSCRIPTEN_KEEPALIVE int d2_drlg_tile_count() { return (int)g_tiles.size(); }
EMSCRIPTEN_KEEPALIVE const uint16_t* d2_drlg_grid() { return g_grid.data(); }
int D2WEB_ComponentToken(int nIndex, uint32_t* pCode, uint32_t* pWeaponClass);
// Character select's portrait: appearance index -> "tok wcl" (4-char token, 3-char weapon class; the
// codes' trailing spaces trimmed), "" when the index names none. Needs the data tables (d2_drlg_init).
static std::string g_componentToken;
EMSCRIPTEN_KEEPALIVE const char* d2_component_token(int index) {
    uint32_t code = 0, wclass = 0; g_componentToken.clear();
    if (!D2WEB_ComponentToken(index, &code, &wclass)) return g_componentToken.c_str();
    for (int i = 0; i < 4; i++) { char ch = (char)((code >> (8 * i)) & 0xFF); if (ch && ch != ' ') g_componentToken += ch; }
    g_componentToken += ' ';
    for (int i = 0; i < 4; i++) { char ch = (char)((wclass >> (8 * i)) & 0xFF); if (ch && ch != ' ') g_componentToken += ch; }
    return g_componentToken.c_str();
}
EMSCRIPTEN_KEEPALIVE const char* d2_drlg_err() { return g_err.c_str(); }

} // extern "C"
