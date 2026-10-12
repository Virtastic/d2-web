// Retail-gated DRLG check (s03-drlg): generate a level with the vendored D2MOO DRLG over our
// tables + tile libraries, and produce the same subtile collision grid summary the retail capture tool
// records from the running 1.13c D2Common.
// The call sequence mirrors the capture exactly: LoadAct -> GetLevel -> InitLevel -> for each
// RoomEx without an active room: AddRoomData / read collision grid / RemoveRoomData.
#include <emscripten.h>
#include <algorithm>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>
#include <D2Common.h>
#include <D2Dungeon.h>
#include <D2Collision.h>
#include <D2CMP.h>
#include <Drlg/D2DrlgDrlg.h>
#include <Drlg/D2DrlgPreset.h>
#include <Drlg/D2DrlgOutdoors.h>
#include <Drlg/D2DrlgDrlgVer.h>
#include <Drlg/D2DrlgOutRoom.h>
#include "../d2moo/compat/datatbls_bridge.h"
#include "../tables/shared.h"
#include "drlg_room_trace.h"
#include "drlg_room_snapshot.h"
#include "../../../tools/vectorfmt/vectorfmt.h"

static D2DrlgActStrc* g_act = nullptr;
static uint8_t g_actNo = 0;
static std::vector<uint16_t> g_grid;
static std::string g_err;
// Bounded diagnostic view of the generated Room2 list. One record per room, in list order.
// The seed is sampled at enumeration: after conditional AddRoomData for inactive rooms, or without
// that call for rooms already active. It is not the seed at D2Game's later AutoPos chooser.
// Candidate trials and acceptance are not observed by this path.
static constexpr size_t kDrlgRoomTraceLimit = 512;
static std::vector<D2DrlgRoomTrace> g_roomTrace;
static int g_roomTraceTruncated = 0;
// Offline DRLG snapshot of the generated RoomEx list. Seed fields are deliberately split by
// lifecycle: generationSeed is D2DrlgRoom.dwInitSeed; roomSeed is the RoomEx pSeed after
// DRLG_InitLevel; activeSeed is the D2ActiveRoom pSeed after SetClientIsInSight has generated
// the room's tiles and before D2Game population. The latter is not Radament's AutoPos input,
// because room activation and earlier preset spawns consume it afterwards.
static constexpr size_t kDrlgRoomSnapshotLimit = 512;
static std::vector<D2DrlgRoomSnapshot> g_roomSnapshot;
static int g_roomSnapshotTruncated = 0;
// The linked RoomEx preset-unit order is the order D2Game sees before it reaches Radament's
// encoded preset index. Keep those identities explicit; aggregate counts cannot establish which
// earlier preset helpers may advance the active-room seed.
static constexpr size_t kDrlgPresetSnapshotLimit = 16384;
static std::vector<D2DrlgPresetSnapshot> g_presetSnapshot;
static int g_presetSnapshotTruncated = 0;
// Tile identity under the CAPTURE's activation protocol (s173). the retail capture tool reads a room's tiles
// immediately after AddRoomData and before RemoveRoomData, one room at a time, so when it reads a
// room fewer of its neighbours are alive than in a session that activates every room and keeps
// them. Whether a shared edge cell's neighbour exists at tile time decides merge-vs-add and hence
// whether the room takes a roll, so the corpus can only be matched by the same protocol.
// Record: room ordinal, layer (0 floor, 1 wall, 2 roof), key x, key y, orientation, main, sub, frame.
struct D2DrlgTileRec { int32_t room, layer, x, y, orientation, main, sub, frame; };
static bool g_collectTiles = false;
static std::vector<D2DrlgTileRec> g_tiles;
// Realm client terrain is the same act allocation and active-room state as the retail client. Keep
// its tile records in the 84-byte session renderer format, but source them from the client-mode act
// that LoadAct/MapReveal packets build above (never from a server-side session).
struct D2DrlgClientTile {
    uint32_t room, layer, ordinal;
    int32_t orientation, mainIndex, subIndex, frameIndex;
    uint32_t material;
    int32_t tileX, tileY, worldX, worldY, screenX, screenY, width, height;
    uint32_t flags, librarySlot, libraryIndex;
    int32_t entryHint;
    uint32_t pop;
};
static_assert(sizeof(D2DrlgClientTile) == 84);
struct D2DrlgClientRoomInfo {
    uint32_t version, size, level, tileX, tileY, tileWidth, tileHeight, sightRefs, active;
};
static_assert(offsetof(D2DrlgClientRoomInfo, sightRefs) == 28);
static_assert(sizeof(D2DrlgClientRoomInfo) == 36);
struct D2DrlgClientTileRef { D2TileLibraryEntryStrc* tile; D2DrlgClientTileRef* next; };
struct D2DrlgClientTileNode {
    int32_t style, sequence, type;
    D2DrlgClientTileRef* refs;
    D2DrlgClientTileNode* next;
};
struct D2DrlgClientTileHash { D2DrlgClientTileNode* nodes[128]; };
static std::vector<D2DrlgClientTile> g_clientTiles;
static std::vector<uint32_t> g_clientTileEntries;
static std::vector<int32_t> g_clientStatusCells;
static std::vector<std::string> g_clientTileLibraries;
static int g_clientRenderLevel;
static int g_clientTownLevelArgument;
static int g_clientMapRoomHintUsed;
static std::string g_clientTileFiles;
extern "C" const char* D2WEB_TileLibraryNameFromHash(const D2TileLibraryHashStrc*);
extern "C" uint32_t D2WEB_TileEntryFileIndex(const void*);
extern "C" int d2_cross_level_sight_on(void);

static uint32_t clientTileLibrary(const D2TileLibraryHashStrc* hash) {
    const char* name = hash ? D2WEB_TileLibraryNameFromHash(hash) : nullptr;
    if (!name) return UINT32_MAX;
    for (size_t i = 0; i < g_clientTileLibraries.size(); ++i)
        if (g_clientTileLibraries[i] == name) return (uint32_t)i;
    g_clientTileLibraries.emplace_back(name);
    return (uint32_t)(g_clientTileLibraries.size() - 1);
}

static void collectClientRoomTiles(D2DrlgRoomStrc* room, uint32_t ordinal, uint32_t layer,
        const D2DrlgTileDataStrc* data, int count) {
    if (!room || !data || count <= 0) return;
    for (int i = 0; i < count; ++i) {
        const D2DrlgTileDataStrc& source = data[i];
        if (!source.pTile) continue;
        const D2TileLibraryEntryStrc& tile = *source.pTile;
        D2DrlgClientTile out{};
        out.room = ordinal; out.layer = layer; out.ordinal = (uint32_t)i;
        out.orientation = tile.nType; out.mainIndex = tile.nStyle; out.subIndex = tile.nSequence;
        out.frameIndex = tile.nRarity_Frame; out.material = tile.nFlags;
        out.tileX = source.nPosX; out.tileY = source.nPosY;
        out.worldX = room->nTileXPos + source.nPosX; out.worldY = room->nTileYPos + source.nPosY;
        out.screenX = source.nWidth; out.screenY = source.nHeight;
        out.width = tile.nWidth; out.height = tile.nTotalHeight;
        out.flags = source.dwFlags; out.entryHint = source.unk0x10;
        out.pop = (source.unk0x24 & 2) ? (source.nGreen == 255 ? 1u : 2u) : 0u;
        out.librarySlot = UINT32_MAX; out.libraryIndex = UINT32_MAX;
        const uint32_t bucket = (uint32_t)(2 * (uint8_t)tile.nType - (uint8_t)tile.nSequence + (uint8_t)tile.nStyle) & 0x7Fu;
        for (uint32_t slot = 0; slot < 32 && room->pTiles[slot]; ++slot) {
            auto* hash = reinterpret_cast<D2DrlgClientTileHash*>(room->pTiles[slot]);
            for (D2DrlgClientTileNode* node = hash->nodes[bucket]; node; node = node->next) {
                if (node->type != tile.nType || node->style != tile.nStyle || node->sequence != tile.nSequence) continue;
                uint32_t index = 0;
                for (D2DrlgClientTileRef* ref = node->refs; ref; ref = ref->next, ++index) {
                    if (ref->tile != source.pTile) continue;
                    out.librarySlot = clientTileLibrary(room->pTiles[slot]); out.libraryIndex = index;
                    break;
                }
                if (out.librarySlot != UINT32_MAX) break;
            }
            if (out.librarySlot != UINT32_MAX) break;
        }
        g_clientTiles.push_back(out);
        g_clientTileEntries.push_back(D2WEB_TileEntryFileIndex(source.pTile));
    }
}

static void rebuildClientRoomTiles(int level) {
    g_clientTiles.clear(); g_clientTileEntries.clear(); g_clientTileLibraries.clear();
    if (!g_act || !g_act->pDrlg) return;
    D2DrlgLevelStrc* pLevel = DRLG_GetLevel(g_act->pDrlg, level);
    if (!pLevel) return;
    uint32_t ordinal = 0;
    for (D2DrlgRoomStrc* room = pLevel->pFirstRoomEx; room; room = room->pDrlgRoomNext, ++ordinal) {
        D2ActiveRoomStrc* active = room->pRoom;
        if (!active || !active->pRoomTiles) continue;
        collectClientRoomTiles(room, ordinal, 0, active->pRoomTiles->pFloorTiles, active->pRoomTiles->nFloors);
        collectClientRoomTiles(room, ordinal, 1, active->pRoomTiles->pWallTiles, active->pRoomTiles->nWalls);
        collectClientRoomTiles(room, ordinal, 2, active->pRoomTiles->pRoofTiles, active->pRoomTiles->nRoofs);
    }
}
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
static int beginClientAct(uint32_t seed, int difficulty, int act, int townLevel) {
    if (g_act) { DUNGEON_FreeAct(g_act); g_act = nullptr; }
    g_clientTiles.clear(); g_clientTileEntries.clear(); g_clientTileLibraries.clear();
    g_clientRenderLevel = 0;
    if (act < 1 || act > 5) return 0;
    g_actNo = (uint8_t)(act - 1);
    g_pickSeed = seed; g_pickDiff = (uint8_t)difficulty; g_pickLevel = 0; g_pickCall = 0; g_pickRoom = -1;   // LoadAct's own lookups: level 0
    g_act = DUNGEON_AllocAct(g_actNo, seed, TRUE, nullptr, (uint8_t)difficulty, nullptr, townLevel, noAutomap, noTownAutomap);
    g_clientTownLevelArgument = townLevel;
    return g_act != nullptr;
}

EMSCRIPTEN_KEEPALIVE int d2_drlg_client_town_level_argument() { return g_clientTownLevelArgument; }
EMSCRIPTEN_KEEPALIVE int d2_drlg_client_map_room_hint_used() { return g_clientMapRoomHintUsed; }

EMSCRIPTEN_KEEPALIVE int d2_drlg_act_begin(uint32_t seed, int difficulty, int act) {
    static const int kActFirstLevel[6] = { 1, 40, 75, 103, 109, 137 };
    if (act < 1 || act > 5) return 0;
    return beginClientAct(seed, difficulty, act, kActFirstLevel[act - 1]);
}

// D2Client 0x03 carries nTownLevelId in bytes 6-7. Client DRLG allocation currently begins with
// LEVEL_NONE, but retain and pass the native argument rather than substituting the vector default.
EMSCRIPTEN_KEEPALIVE int d2_drlg_client_act_begin(uint32_t seed, int difficulty, int act, int townLevel) {
    return beginClientAct(seed, difficulty, act, townLevel);
}

// Session-mode counterpart for probes that compare the vendored DRLG with an in-game server act.
// LEVEL_LoadAct calls DUNGEON_AllocAct with bClient=FALSE, which initializes the town immediately
// and omits DRLGFLAG_ONCLIENT; the vector capture path above intentionally remains client-mode.
EMSCRIPTEN_KEEPALIVE int d2_drlg_act_begin_server(uint32_t seed, int difficulty, int act) {
    static const int kActFirstLevel[6] = { 1, 40, 75, 103, 109, 137 };
    if (g_act) { DUNGEON_FreeAct(g_act); g_act = nullptr; }
    if (act < 1 || act > 5) return 0;
    g_actNo = (uint8_t)(act - 1);
    g_pickSeed = seed; g_pickDiff = (uint8_t)difficulty; g_pickLevel = 0; g_pickCall = 0; g_pickRoom = -1;
    g_act = DUNGEON_AllocAct(g_actNo, seed, FALSE, nullptr, (uint8_t)difficulty, nullptr, kActFirstLevel[act - 1], nullptr, nullptr);
    return g_act != nullptr;
}

EMSCRIPTEN_KEEPALIVE void d2_drlg_act_end() {
    if (g_act) { DUNGEON_FreeAct(g_act); g_act = nullptr; }
    g_grid.clear(); g_grid.shrink_to_fit();
    g_roomTrace.clear(); g_roomTraceTruncated = 0;
    g_roomSnapshot.clear(); g_roomSnapshotTruncated = 0;
    g_presetSnapshot.clear(); g_presetSnapshotTruncated = 0;
    g_clientTiles.clear(); g_clientTileEntries.clear(); g_clientTileLibraries.clear();
    g_clientStatusCells.clear();
    g_clientTownLevelArgument = 0; g_clientMapRoomHintUsed = 0;
}

// D2Client 1.13c opcode 0x07 calls D2Common ordinal 10401 with (act,eLevel,x,y,currentRoom).
// That ordinal is DUNGEON_SetClientIsInSight in our 1.13c map (community APIs call it AddRoomData).
// Apply every packet in arrival order; the active rooms are the client's terrain source.
static D2ActiveRoomStrc* clientRoomAt(int level, int roomTileX, int roomTileY) {
    if (!g_act || level <= 0) return nullptr;
    for (D2ActiveRoomStrc* room = DUNGEON_GetRoomFromAct(g_act); room; room = room->pRoomNext) {
        if (room->pDrlgRoom && room->pDrlgRoom->pLevel && room->pDrlgRoom->pLevel->nLevelId == level &&
            room->pDrlgRoom->nTileXPos == roomTileX && room->pDrlgRoom->nTileYPos == roomTileY) return room;
    }
    return nullptr;
}

static D2ActiveRoomStrc* clientRoomHint(int level, int playerLevel, int hasPlayer, int roomTileX, int roomTileY) {
    if (!hasPlayer || playerLevel != level) return nullptr;
    return clientRoomAt(level, roomTileX, roomTileY);
}

// The realm state adapter uses this only to track the level of an already-active client room.
// A player packet cannot invent a current level when its coordinates are not in an active room.
EMSCRIPTEN_KEEPALIVE int d2_drlg_client_level_at(int playerX, int playerY) {
    if (!g_act) return 0;
    for (D2ActiveRoomStrc* room = DUNGEON_GetRoomFromAct(g_act); room; room = room->pRoomNext) {
        if (room->pDrlgRoom && room->pDrlgRoom->pLevel && DungeonTestRoomGame(room, playerX, playerY))
            return room->pDrlgRoom->pLevel->nLevelId;
    }
    return 0;
}

EMSCRIPTEN_KEEPALIVE int d2_drlg_client_map_reveal(int level, int x, int y, int hasPlayer, int playerLevel, int playerX, int playerY) {
    if (!g_act || level <= 0 || x < 0 || y < 0) { g_err = "invalid client MapReveal"; return 0; }
    D2DrlgLevelStrc* pLevel = DRLG_GetLevel(g_act->pDrlg, level);
    if (!pLevel) { g_err = "MapReveal level unavailable"; return 0; }
    D2ActiveRoomStrc* hint = clientRoomHint(level, playerLevel, hasPlayer, playerX, playerY);
    g_clientMapRoomHintUsed = hint != nullptr;
    DUNGEON_SetClientIsInSight(g_act, level, x, y, hint);
    if (hasPlayer && playerLevel > 0) g_clientRenderLevel = playerLevel;
    else if (g_clientRenderLevel <= 0) g_clientRenderLevel = level;
    rebuildClientRoomTiles(g_clientRenderLevel);
    return 1;
}

EMSCRIPTEN_KEEPALIVE int d2_drlg_client_map_hide(int level, int x, int y, int hasPlayer, int playerLevel, int playerX, int playerY) {
    if (!g_act || level <= 0 || x < 0 || y < 0) { g_err = "invalid client MapHide"; return 0; }
    D2DrlgLevelStrc* pLevel = DRLG_GetLevel(g_act->pDrlg, level);
    if (!pLevel) { g_err = "MapHide level unavailable"; return 0; }
    D2ActiveRoomStrc* hint = clientRoomHint(level, playerLevel, hasPlayer, playerX, playerY);
    g_clientMapRoomHintUsed = hint != nullptr;
    DUNGEON_UnsetClientIsInSight(g_act, level, x, y, hint);
    if (hasPlayer && playerLevel > 0) g_clientRenderLevel = playerLevel;
    else if (g_clientRenderLevel <= 0) g_clientRenderLevel = level;
    rebuildClientRoomTiles(g_clientRenderLevel);
    return 1;
}

EMSCRIPTEN_KEEPALIVE int d2_drlg_client_render_level() { return g_clientRenderLevel; }

EMSCRIPTEN_KEEPALIVE int d2_drlg_client_select_render_level(int level) {
    if (!g_act || level <= 0 || !DRLG_GetLevel(g_act->pDrlg, level)) return 0;
    g_clientRenderLevel = level;
    rebuildClientRoomTiles(level);
    return 1;
}

// Engine-only readback for a local fixture to name a real room in the seeded client DRLG. This
// does not alter room visibility; production terrain still changes only through MapReveal packets.
EMSCRIPTEN_KEEPALIVE int d2_drlg_client_first_room_xy(int level, int32_t* out) {
    if (!g_act || !out) return 0;
    D2DrlgLevelStrc* pLevel = DRLG_GetLevel(g_act->pDrlg, level);
    if (!pLevel) return 0;
    if (!pLevel->pFirstRoomEx) DRLG_InitLevel(pLevel);
    if (!pLevel->pFirstRoomEx) return 0;
    out[0] = pLevel->pFirstRoomEx->nTileXPos;
    out[1] = pLevel->pFirstRoomEx->nTileYPos;
    return 1;
}

// Read the client Act's current DRLG status union in tile coordinates. This is the same
// `fRoomStatus < ROOMSTATUS_COUNT` membership rule used by the retail room hook's op-3/op-4
// census, but against the separately allocated client-mode Act rather than the game/server Act.
EMSCRIPTEN_KEEPALIVE const int32_t* d2_drlg_client_room_status_cells(int levelId) {
    g_clientStatusCells.clear();
    if (!g_act || !g_act->pDrlg) return g_clientStatusCells.data();
    D2DrlgLevelStrc* level = DRLG_GetLevel(g_act->pDrlg, levelId);
    g_clientStatusCells.push_back(0);
    if (!level || !level->pFirstRoomEx) return g_clientStatusCells.data();
    for (D2DrlgRoomStrc* room = level->pFirstRoomEx; room; room = room->pDrlgRoomNext) {
        if (room->fRoomStatus >= ROOMSTATUS_COUNT) continue;
        g_clientStatusCells.push_back(room->nTileXPos);
        g_clientStatusCells.push_back(room->nTileYPos);
        g_clientStatusCells.push_back((int32_t)room->fRoomStatus);
    }
    g_clientStatusCells[0] = (int32_t)((g_clientStatusCells.size() - 1) / 3);
    return g_clientStatusCells.data();
}

EMSCRIPTEN_KEEPALIVE const D2DrlgClientTile* d2_drlg_client_tiles() { return g_clientTiles.data(); }
EMSCRIPTEN_KEEPALIVE int d2_drlg_client_tile_count() { return (int)g_clientTiles.size(); }
EMSCRIPTEN_KEEPALIVE const uint32_t* d2_drlg_client_tile_entries() { return g_clientTileEntries.data(); }
EMSCRIPTEN_KEEPALIVE int d2_drlg_client_tile_files(char* out, int cap) {
    if (!out || cap <= 0) return 0;
    int written = 0, used = 0;
    for (const std::string& name : g_clientTileLibraries) {
        const int n = (int)name.size() + 1;
        if (used + n > cap) break;
        memcpy(out + used, name.c_str(), (size_t)n); used += n; written++;
    }
    return written;
}

// Resolve a client-side CUnit's existing active room without streaming a new room. Native client
// constructors use this coordinate lookup before initializing the unit's seed; callers choose
// whether this constructor consumes one active-room seed roll for dwInitSeed. This is distinct
// from the CUnit's mutable pSeed. A later reassignment uses the same lookup without re-rolling.
EMSCRIPTEN_KEEPALIVE int d2_drlg_client_unit_context(int x, int y, int rollRoomSeed,
        uint32_t* levelId, uint32_t* roomTileX, uint32_t* roomTileY,
        uint32_t* unitInitSeed) {
    if (!levelId || !roomTileX || !roomTileY || !unitInitSeed) return 0;
    *levelId = *roomTileX = *roomTileY = *unitInitSeed = 0;
    if (!g_act) return 0;
    // The native constructor skips room resolution only for an unpositioned (0,0) assignment.
    if (x == 0 && y == 0) return 1;
    D2ActiveRoomStrc* room = DUNGEON_FindRoomBySubtileCoordinates(g_act, x, y);
    if (!room || !room->pDrlgRoom || !room->pDrlgRoom->pLevel) return 0;
    D2DrlgRoomStrc* drlgRoom = room->pDrlgRoom;
    *levelId = static_cast<uint32_t>(DUNGEON_GetLevelIdFromRoom(room));
    *roomTileX = static_cast<uint32_t>(drlgRoom->nTileXPos);
    *roomTileY = static_cast<uint32_t>(drlgRoom->nTileYPos);
    if (rollRoomSeed) {
        // D2Client's 0x59 constructor rolls the seed on the active client-room record returned by
        // its Act-coordinate lookup. D2MOO names the logical record D2ActiveRoomStrc (its seed is
        // not the D2DrlgRoom generation seed above).
        *unitInitSeed = static_cast<uint32_t>(SEED_RollRandomNumber(&room->pSeed));
    }
    return 1;
}

// Compatibility name used by the realm 0x59/0x15 player assignment adapter. Keep this wrapper
// so existing callers retain their ABI while missile and other client CUnit constructors share
// the same native active-room lookup primitive.
EMSCRIPTEN_KEEPALIVE int d2_drlg_client_player_context(int x, int y, int rollRoomSeed,
        uint32_t* levelId, uint32_t* roomTileX, uint32_t* roomTileY,
        uint32_t* unitInitSeed) {
    return d2_drlg_client_unit_context(x, y, rollRoomSeed,
        levelId, roomTileX, roomTileY, unitInitSeed);
}

// Readback for the production client's active-room seed. The protocol scenario uses this before
// 0x59 to verify that assignment consumes exactly one native room-seed roll.
EMSCRIPTEN_KEEPALIVE int d2_drlg_client_room_seed_at(int x, int y, uint32_t* low, uint32_t* high) {
    if (!low || !high || !g_act) return 0;
    D2ActiveRoomStrc* room = DUNGEON_FindRoomBySubtileCoordinates(g_act, x, y);
    if (!room) return 0;
    *low = room->pSeed.nLowSeed;
    *high = room->pSeed.nHighSeed;
    return 1;
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
    g_roomTrace.clear(); g_roomTraceTruncated = 0;
    g_roomSnapshot.clear(); g_roomSnapshotTruncated = 0;
    g_presetSnapshot.clear(); g_presetSnapshotTruncated = 0;
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
        if (g_roomTrace.size() < kDrlgRoomTraceLimit) {
            D2DrlgRoomTrace trace{};
            trace.ordinal = roomOrdinal; trace.type = r2->nType;
            trace.tileX = r2->nTileXPos; trace.tileY = r2->nTileYPos;
            trace.tileWidth = r2->nTileWidth; trace.tileHeight = r2->nTileHeight;
            trace.activeSeedAtEnumerationLow = r1 ? r1->pSeed.nLowSeed : r2->pSeed.nLowSeed;
            trace.activeSeedAtEnumerationHigh = r1 ? r1->pSeed.nHighSeed : r2->pSeed.nHighSeed;
            if (r1 && r1->pCollisionGrid) {
                const D2DrlgCoordsStrc& c = r1->pCollisionGrid->pRoomCoords;
                trace.collisionX = c.nSubtileX; trace.collisionY = c.nSubtileY;
                trace.collisionWidth = c.nSubtileWidth; trace.collisionHeight = c.nSubtileHeight;
            } else trace.collisionX = trace.collisionY = -1;
            g_roomTrace.push_back(trace);
        } else g_roomTraceTruncated = 1;
        if (g_roomSnapshot.size() < kDrlgRoomSnapshotLimit) {
            D2DrlgRoomSnapshot snapshot{};
            snapshot.ordinal = roomOrdinal; snapshot.roomType = r2->nType;
            snapshot.tileX = rx; snapshot.tileY = ry;
            snapshot.tileWidth = r2->nTileWidth; snapshot.tileHeight = r2->nTileHeight;
            snapshot.presetId = r2->nType == DRLGTYPE_PRESET && r2->pMaze ? r2->pMaze->nLevelPrest : -1;
            snapshot.generationSeed = r2->dwInitSeed;
            snapshot.roomSeedLow = r2->pSeed.nLowSeed; snapshot.roomSeedHigh = r2->pSeed.nHighSeed;
            snapshot.activeX = snapshot.activeY = -1;
            snapshot.candidateMinX = snapshot.candidateMinY = 1;
            snapshot.candidateMaxX = snapshot.candidateMaxY = 0;
            snapshot.radamentAnchorX = snapshot.radamentAnchorY = -1;
            if (r1) {
                snapshot.activeSeedLow = r1->pSeed.nLowSeed; snapshot.activeSeedHigh = r1->pSeed.nHighSeed;
                const D2DrlgCoordsStrc& c = r1->tCoords;
                snapshot.activeX = c.nSubtileX; snapshot.activeY = c.nSubtileY;
                snapshot.activeWidth = c.nSubtileWidth; snapshot.activeHeight = c.nSubtileHeight;
                snapshot.candidateMinX = c.nSubtileX + 1; snapshot.candidateMinY = c.nSubtileY + 1;
                snapshot.candidateMaxX = c.nSubtileX + c.nSubtileWidth - 2;
                snapshot.candidateMaxY = c.nSubtileY + c.nSubtileHeight - 2;
            }
            // The 1.13c DS1 linker encodes Radament as monster preset index 734+10=744.
            // Keep the map-local/room-relative preset anchor separate from AutoPos's later
            // random candidate, which D2Game chooses after this DRLG snapshot is taken.
            int32_t presetOrdinal = 0;
            for (D2PresetUnitStrc* unit = r2->pPresetUnits; unit; unit = unit->pNext, ++presetOrdinal) {
                if (g_presetSnapshot.size() < kDrlgPresetSnapshotLimit) {
                    g_presetSnapshot.push_back({ roomOrdinal, presetOrdinal, unit->nUnitType, unit->nIndex,
                        unit->nMode, unit->nXpos, unit->nYpos, unit->bSpawned });
                } else g_presetSnapshotTruncated = 1;
                if (unit->nUnitType == 1 && unit->nIndex == 744) {
                    ++snapshot.radamentPresetCount;
                    if (snapshot.radamentPresetCount == 1) {
                        snapshot.radamentAnchorX = unit->nXpos;
                        snapshot.radamentAnchorY = unit->nYpos;
                    }
                }
            }
            g_roomSnapshot.push_back(snapshot);
        } else g_roomSnapshotTruncated = 1;
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
// Diagnostic records for the last generated level (bounded to 512 rooms). The room rectangle and
// active seed are snapshots at enumeration; collision words come from d2_drlg_grid.
EMSCRIPTEN_KEEPALIVE const D2DrlgRoomTrace* d2_drlg_rooms() { return g_roomTrace.data(); }
EMSCRIPTEN_KEEPALIVE int d2_drlg_room_count() { return (int)g_roomTrace.size(); }
EMSCRIPTEN_KEEPALIVE int d2_drlg_room_trace_truncated() { return g_roomTraceTruncated; }
// Ordered snapshot of the last generated level, bounded to 512 RoomEx records. Seed fields stop
// before D2Game population; this diagnoses the generation inputs without claiming an AutoPos result.
EMSCRIPTEN_KEEPALIVE const D2DrlgRoomSnapshot* d2_drlg_room_snapshot() { return g_roomSnapshot.data(); }
EMSCRIPTEN_KEEPALIVE int d2_drlg_room_snapshot_count() { return (int)g_roomSnapshot.size(); }
EMSCRIPTEN_KEEPALIVE int d2_drlg_room_snapshot_truncated() { return g_roomSnapshotTruncated; }
EMSCRIPTEN_KEEPALIVE const D2DrlgPresetSnapshot* d2_drlg_preset_snapshot() { return g_presetSnapshot.data(); }
EMSCRIPTEN_KEEPALIVE int d2_drlg_preset_snapshot_count() { return (int)g_presetSnapshot.size(); }
EMSCRIPTEN_KEEPALIVE int d2_drlg_preset_snapshot_truncated() { return g_presetSnapshotTruncated; }
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

// Movement projections may attach a short-lived client CUnit to an existing visible room. These
// accessors never stream rooms; the D2GS MapReveal/MapHide feed owns that lifetime.
D2DrlgActStrc* d2_drlg_client_act() { return g_act; }
D2ActiveRoomStrc* d2_drlg_client_active_room(int x, int y) {
    return g_act ? DUNGEON_FindRoomBySubtileCoordinates(g_act, x, y) : nullptr;
}
int d2_drlg_client_room_freed_by_hide(int level, int x, int y, int hasPlayer, int playerLevel,
        int playerX, int playerY, void* activeRoom);
// Read-only native RoomEx identity for focused client MapReveal/MapHide lifecycle rows.
// nearIndex -1 returns the room containing (x,y); nonnegative indices select its ppRoomsNear.
EMSCRIPTEN_KEEPALIVE int d2_drlg_client_room_info(int x, int y, int nearIndex, D2DrlgClientRoomInfo* out) {
    if (!out) return 0;
    *out = {};
    if (!g_act) return 0;
    D2ActiveRoomStrc* active = DUNGEON_FindRoomBySubtileCoordinates(g_act, x, y);
    D2DrlgRoomStrc* room = active ? active->pDrlgRoom : nullptr;
    if (!room) return 0;
    if (nearIndex >= 0) {
        if (nearIndex >= room->nRoomsNear || !room->ppRoomsNear) return 0;
        room = room->ppRoomsNear[nearIndex];
    }
    if (!room) return 0;
    out->version = 1;
    out->size = sizeof(*out);
    out->level = room->pLevel ? static_cast<uint32_t>(room->pLevel->nLevelId) : 0;
    out->tileX = static_cast<uint32_t>(room->nTileXPos);
    out->tileY = static_cast<uint32_t>(room->nTileYPos);
    out->tileWidth = static_cast<uint32_t>(room->nTileWidth);
    out->tileHeight = static_cast<uint32_t>(room->nTileHeight);
    out->sightRefs = room->wRoomsInList[ROOMSTATUS_CLIENT_IN_SIGHT];
    out->active = room->pRoom != nullptr;
    return 1;
}
EMSCRIPTEN_KEEPALIVE int d2_drlg_client_room_freed_by_hide_at(int level, int hideX, int hideY,
        int carrierX, int carrierY, int hasPlayer, int playerLevel, int playerX, int playerY) {
    D2ActiveRoomStrc* carrier = g_act ? DUNGEON_FindRoomBySubtileCoordinates(g_act, carrierX, carrierY) : nullptr;
    return d2_drlg_client_room_freed_by_hide(level, hideX, hideY, hasPlayer, playerLevel,
        playerX, playerY, carrier);
}
int d2_drlg_client_room_freed_by_hide(int level, int x, int y, int hasPlayer, int playerLevel,
        int playerX, int playerY, void* activeRoom) {
    if (!g_act || !g_act->pDrlg || !activeRoom) return 0;
    D2DrlgLevelStrc* pLevel = DRLG_GetLevel(g_act->pDrlg, level);
    D2ActiveRoomStrc* hint = clientRoomHint(level, playerLevel, hasPlayer, playerX, playerY);
    D2DrlgRoomStrc* root = pLevel ? DRLG_GetRoomExFromCoordinates(x, y, g_act->pDrlg,
        hint ? hint->pDrlgRoom : nullptr, pLevel) : nullptr;
    if (!root || root->wRoomsInList[ROOMSTATUS_CLIENT_IN_SIGHT] == 0) return 0;

    struct HideDelta { int32_t values[ROOMSTATUS_COUNT]{}; };
    std::unordered_map<D2DrlgRoomStrc*, HideDelta> deltas;
    auto propagate = [&](auto&& self, D2DrlgRoomStrc* room, int status) -> void {
        if (!room || status >= ROOMSTATUS_COUNT) return;
        for (int i = 0; i < room->nRoomsNear; ++i) {
            D2DrlgRoomStrc* nearRoom = room->ppRoomsNear[i];
            if (!nearRoom || (nearRoom->pLevel != room->pLevel && !d2_cross_level_sight_on())) continue;
            deltas[nearRoom].values[status]--;
            self(self, nearRoom, status + 1);
        }
    };
    deltas[root].values[ROOMSTATUS_CLIENT_IN_SIGHT]--;
    propagate(propagate, root, ROOMSTATUS_CLIENT_IN_SIGHT + 1);
    auto* target = static_cast<D2ActiveRoomStrc*>(activeRoom)->pDrlgRoom;
    if (!target) return 0;
    auto it = deltas.find(target);
    if (it == deltas.end()) return 0;
    for (int status = 0; status < ROOMSTATUS_COUNT; ++status) {
        const uint16_t after = static_cast<uint16_t>(target->wRoomsInList[status] + it->second.values[status]);
        if (after != 0) return 0;
    }
    return 1;
}

} // extern "C"
