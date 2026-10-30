// The few GAME/Game.cpp pieces the vendored code needs without the server around it (Game.cpp
// itself is the game-list / thread / packet loop and stays out until the sim needs it).
#include <D2Common.h>
#include <cstdio>
#include <emscripten.h>
#include <D2Dungeon.h>
#include <D2Seed.h>
#include <Drlg/D2DrlgDrlg.h>
#include <GAME/Game.h>
#include <GAME/Clients.h>
#include <UNIT/SUnit.h>
#include <UNIT/SUnitInactive.h>
#include <MONSTER/MonsterRegion.h>
#include <OBJECTS/Objects.h>

char gszEmptyString_6FD447EC[8];
D2ServerCallbackFunctions* gpD2EventCallbackTable_6FD45830;   // the host's callback table: none headless

// Which rooms were populated, in order: {level, subtile x, subtile y} per first activation (not a
// retail structure -- the order a walking player's session populated them, for a capture to replay
// against retail; d2_session_activation_log).
int32_t g_d2ActivationLog[10 * 4096]; int32_t g_d2ActivationCount = 0;
// Every monster allocated: {game frame, unit id, class, x, y, path} (path: 1 room population, 2 the
// wandering tick, 3 inactive units restored, 0 anything else -- a minion, a missile's spawn, a script). d2_session_spawn_log.
// Every active room created: {level, subtile x, subtile y, the seed it was given} (d2_session_room_create_log).
int32_t g_d2CreateLog[4 * 8192]; int32_t g_d2CreateCount = 0;
// The creations and populations interleaved, as they happened: {op (0 create, 1 populate), level, x, y}
// (d2_session_room_sequence) -- what a capture replays to settle a walk's population.
int32_t g_d2SeqLog[4 * 16384]; int32_t g_d2SeqCount = 0;
static void noteSeq(int op, int level, int x, int y) { if (g_d2SeqCount < 16384) { int32_t* e = &g_d2SeqLog[4 * g_d2SeqCount++]; e[0] = op; e[1] = level; e[2] = x; e[3] = y; } }
void d2_note_room_create(int level, int x, int y, int seed) { noteSeq(0, level, x, y); if (g_d2CreateCount < 8192) { int32_t* e = &g_d2CreateLog[4 * g_d2CreateCount++]; e[0] = level; e[1] = x; e[2] = y; e[3] = seed; } }
// Opt-in object-population trace: 18 int32s per row, documented by d2_session_object_pop_trace.
// A room without a class-125 object gets one row with object fields set to -1; multiple matches
// produce one row per object, with the same room-boundary values repeated.
constexpr int32_t D2_OBJECT_POP_TRACE_STRIDE = 18;
constexpr int32_t D2_OBJECT_POP_TRACE_CAPACITY = 16384;
int32_t g_d2ObjectPopTrace[D2_OBJECT_POP_TRACE_STRIDE * D2_OBJECT_POP_TRACE_CAPACITY] = {};
int32_t g_d2ObjectPopTraceCount = 0;
int32_t g_d2ObjectPopTraceDropped = 0;
int32_t g_d2ObjectPopTraceEnabled = 0;
int32_t g_d2ObjectPopTraceRoomOrdinal = 0;
// Test-only, one-shot override for a single room's object-control RNG, immediately before
// OBJECTS_PopulationHandler. It deliberately does not touch DRLG/map or room seeds.
static int32_t g_d2ObjectSeedOverrideEnabled = 0;
static int32_t g_d2ObjectSeedOverrideApplied = 0;
static constexpr int32_t D2_F501_OBJECT_SEED_LEVEL = 107;
static constexpr int32_t D2_F501_OBJECT_SEED_X = 7780;
static constexpr int32_t D2_F501_OBJECT_SEED_Y = 5920;
static uint32_t g_d2ObjectSeedOverrideLow = 0;
static uint32_t g_d2ObjectSeedOverrideHigh = 0;
static int32_t g_d2ObjectSeedOverrideState[8] = {};
extern "C" EMSCRIPTEN_KEEPALIVE void d2_session_object_pop_seed_override(int32_t enabled, uint32_t low, uint32_t high) {
    g_d2ObjectSeedOverrideEnabled = enabled != 0;
    g_d2ObjectSeedOverrideApplied = 0;
    g_d2ObjectSeedOverrideLow = low;
    g_d2ObjectSeedOverrideHigh = high;
    for (int32_t& value : g_d2ObjectSeedOverrideState) value = 0;
}
extern "C" void d2_session_object_pop_seed_override_reset() {
    g_d2ObjectSeedOverrideEnabled = 0;
    g_d2ObjectSeedOverrideApplied = 0;
    g_d2ObjectSeedOverrideLow = 0;
    g_d2ObjectSeedOverrideHigh = 0;
    for (int32_t& value : g_d2ObjectSeedOverrideState) value = 0;
}
extern "C" EMSCRIPTEN_KEEPALIVE const int32_t* d2_session_object_pop_seed_override_state() {
    g_d2ObjectSeedOverrideState[0] = g_d2ObjectSeedOverrideEnabled;
    g_d2ObjectSeedOverrideState[1] = g_d2ObjectSeedOverrideApplied;
    g_d2ObjectSeedOverrideState[2] = D2_F501_OBJECT_SEED_LEVEL;
    g_d2ObjectSeedOverrideState[3] = D2_F501_OBJECT_SEED_X;
    g_d2ObjectSeedOverrideState[4] = D2_F501_OBJECT_SEED_Y;
    g_d2ObjectSeedOverrideState[5] = static_cast<int32_t>(g_d2ObjectSeedOverrideLow);
    g_d2ObjectSeedOverrideState[6] = static_cast<int32_t>(g_d2ObjectSeedOverrideHigh);
    return g_d2ObjectSeedOverrideState;
}
static void noteObjectPopTrace(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom,
                               uint32_t roomSeedLowBefore, uint32_t roomSeedHighBefore,
                               uint32_t objectSeedLowBefore, uint32_t objectSeedHighBefore) {
    if (!g_d2ObjectPopTraceEnabled) return;
    const int32_t ordinal = ++g_d2ObjectPopTraceRoomOrdinal;
    const int32_t level = DUNGEON_GetLevelIdFromRoom(pRoom);
    const uint32_t roomSeedLowAfter = pRoom->pSeed.nLowSeed;
    const uint32_t roomSeedHighAfter = pRoom->pSeed.nHighSeed;
    const uint32_t objectSeedLowAfter = pGame->pObjectControl->pSeed.nLowSeed;
    const uint32_t objectSeedHighAfter = pGame->pObjectControl->pSeed.nHighSeed;
    bool found = false;
    auto append = [&](D2UnitStrc* object) {
        if (g_d2ObjectPopTraceCount >= D2_OBJECT_POP_TRACE_CAPACITY) { ++g_d2ObjectPopTraceDropped; return; }
        int32_t* e = &g_d2ObjectPopTrace[D2_OBJECT_POP_TRACE_STRIDE * g_d2ObjectPopTraceCount++];
        e[0] = ordinal;
        e[1] = static_cast<int32_t>(pGame->dwGameFrame);
        e[2] = level;
        e[3] = pRoom->tCoords.nSubtileX;
        e[4] = pRoom->tCoords.nSubtileY;
        e[5] = static_cast<int32_t>(roomSeedLowBefore);
        e[6] = static_cast<int32_t>(roomSeedHighBefore);
        e[7] = static_cast<int32_t>(roomSeedLowAfter);
        e[8] = static_cast<int32_t>(roomSeedHighAfter);
        e[9] = static_cast<int32_t>(objectSeedLowBefore);
        e[10] = static_cast<int32_t>(objectSeedHighBefore);
        e[11] = static_cast<int32_t>(objectSeedLowAfter);
        e[12] = static_cast<int32_t>(objectSeedHighAfter);
        e[13] = object ? static_cast<int32_t>(object->dwUnitId) : -1;
        e[14] = object ? CLIENTS_GetUnitX(object) : -1;
        e[15] = object ? CLIENTS_GetUnitY(object) : -1;
        e[16] = object ? static_cast<int32_t>(object->dwAnimMode) : -1;
        e[17] = object ? static_cast<int32_t>(object->dwFlags) : -1;
    };
    for (D2UnitStrc* unit = pRoom->pUnitFirst; unit; unit = unit->pRoomNext) {
        if (unit->dwUnitType != UNIT_OBJECT || unit->dwClassId != 125) continue;
        found = true;
        append(unit);
    }
    if (!found) append(nullptr);
}
// F500: whether DUNGEON_SetClientIsInSight may follow a near-room link across level boundaries. Keep
// retail's unconditional propagation enabled during live play; the session's single-call bulk town
// construction is excluded by d2_session_bulk_town_create_active because retail's same-flow
// Fortress-to-River capture builds all 12 town rooms but no level-104 rooms. The probe switch can
// still disable propagation explicitly for its counterfactual arm.
int g_d2CrossLevelSight = 1;
extern "C" void d2_set_cross_level_sight(int on) { g_d2CrossLevelSight = on ? 1 : 0; }
extern "C" int d2_session_bulk_town_create_active(void);
extern "C" int d2_cross_level_sight_on(void) {
	return g_d2CrossLevelSight && !d2_session_bulk_town_create_active();
}

// The rooms flagged POPULATION_ZERO when a level's populated-room count (MonsterRegion unk0x0C) was taken:
// {level, subtile x, subtile y, has room tiles, rooms near} per flagged room (d2_session_popzero_log).
int32_t g_d2PopZeroLog[5 * 1024]; int32_t g_d2PopZeroCount = 0;
void d2_note_popzero(D2DrlgActStrc* pAct, int32_t nLevelId) {
    D2DrlgLevelStrc* lv = pAct ? DRLG_GetLevel(pAct->pDrlg, nLevelId) : nullptr;
    for (D2DrlgRoomStrc* r = lv ? lv->pFirstRoomEx : nullptr; r; r = r->pDrlgRoomNext) if ((r->dwFlags & DRLGROOMFLAG_POPULATION_ZERO) && g_d2PopZeroCount < 1024) {
        int32_t* e = &g_d2PopZeroLog[5 * g_d2PopZeroCount++]; e[0] = nLevelId; e[1] = r->nTileXPos * 5; e[2] = r->nTileYPos * 5; e[3] = r->pTileGrid ? 1 : 0; e[4] = r->nRoomsNear;
    }
}
int32_t g_d2SpawnPath = 0; int32_t g_d2SpawnLog[6 * 8192]; int32_t g_d2SpawnCount = 0;
// Engine missiles made since the shell last drained them: {unit id, class}. A missile that lives less than a tick (a Thunder Storm bolt) is never in a snapshot; the client still sounds its creation.
int32_t g_d2MissileBirths[2 * 1024]; int32_t g_d2MissileBirthCount = 0;
void d2_note_missile_birth(D2UnitStrc* pMissile) { if (g_d2MissileBirthCount < 1024) { g_d2MissileBirths[2 * g_d2MissileBirthCount] = (int32_t)pMissile->dwUnitId; g_d2MissileBirths[2 * g_d2MissileBirthCount + 1] = (int32_t)pMissile->dwClassId; ++g_d2MissileBirthCount; } }
void d2_note_monster_spawn(D2GameStrc* pGame, D2UnitStrc* pMonster, int32_t nClass, int32_t nX, int32_t nY) {
    if (g_d2SpawnCount >= 8192) return;
    int32_t* e = &g_d2SpawnLog[6 * g_d2SpawnCount++];
    e[0] = pGame ? (int32_t)pGame->dwGameFrame : -1; e[1] = (int32_t)pMonster->dwUnitId; e[2] = nClass; e[3] = nX; e[4] = nY; e[5] = g_d2SpawnPath;
}

// F106 diagnosis (act II, non-retail counters): WHICH keeper populated a room, told apart by counting
// rather than inferred from an object census. `sub_6FC385A0` takes exactly one of two branches --
// `dwFlags & 1` set means it has already spawned and is only restoring inactive units -- so the two
// counts plus the room's own flag say whether the spawn branch ran AT ALL for a room that has no
// units. `g_d2PendingPassRuns` counts the passes that actually entered the loop (the act's
// bHasPendingRoomsUpdates was raised), which is the other half: a room can be built and never offered
// to this function. Read out by d2_session_pending_probe (monspawn_check.cpp).
int32_t g_d2PopulateSpawnBranch = 0, g_d2PopulateRestoreBranch = 0, g_d2PendingPassRuns = 0, g_d2PendingPassRooms = 0;
// D2Game.0x6FC385A0 (1.13c 0x6FC4CB80): populate a room the first time it becomes active.
// Order is the one retail runs and the retail capture tool recorded.
void __fastcall sub_6FC385A0(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom) {
#ifdef D2_MONS_TRACE
    const int tl = DUNGEON_GetLevelIdFromRoom(pRoom); if (tl == d2_trace_level) fprintf(stderr, "act room %d,%d: seed0 %u\n", pRoom->tCoords.nSubtileX, pRoom->tCoords.nSubtileY, pRoom->pSeed.nLowSeed);
#endif
    g_d2SpawnPath = 2; sub_6FC679F0(pGame, pRoom); g_d2SpawnPath = 0;
#ifdef D2_MONS_TRACE
    if (tl == d2_trace_level) fprintf(stderr, "  after tick %u\n", pRoom->pSeed.nLowSeed);
#endif
    if (pRoom->dwFlags & 1) {
        ++g_d2PopulateRestoreBranch;
        if (!D2Common_10074(pRoom)) { { const int32_t was = g_d2SpawnPath; g_d2SpawnPath = 3; SUNITINACTIVE_RestoreInactiveUnits(pGame, pRoom); g_d2SpawnPath = was; } D2Common_10075(pRoom, 1); }
    } else {
        ++g_d2PopulateSpawnBranch;
        g_d2SpawnPath = 1;
        SUNIT_SpawnPresetUnitsInRoom(pGame, pRoom);
#ifdef D2_MONS_TRACE
        if (tl == d2_trace_level) fprintf(stderr, "  after presets %u\n", pRoom->pSeed.nLowSeed);
#endif
        { const int32_t was = g_d2SpawnPath; g_d2SpawnPath = 3; SUNITINACTIVE_RestoreInactiveUnits(pGame, pRoom); g_d2SpawnPath = was; }
        const bool traceObjectPop = g_d2ObjectPopTraceEnabled != 0;
        const uint32_t roomSeedLowBeforeObjects = traceObjectPop ? pRoom->pSeed.nLowSeed : 0;
        const uint32_t roomSeedHighBeforeObjects = traceObjectPop ? pRoom->pSeed.nHighSeed : 0;
        if (g_d2ObjectSeedOverrideEnabled && !g_d2ObjectSeedOverrideApplied
            && DUNGEON_GetLevelIdFromRoom(pRoom) == D2_F501_OBJECT_SEED_LEVEL
            && pRoom->tCoords.nSubtileX == D2_F501_OBJECT_SEED_X
            && pRoom->tCoords.nSubtileY == D2_F501_OBJECT_SEED_Y) {
            g_d2ObjectSeedOverrideState[7] = static_cast<int32_t>(roomSeedLowBeforeObjects);
            SEED_SetSeeds(&pGame->pObjectControl->pSeed, g_d2ObjectSeedOverrideLow, g_d2ObjectSeedOverrideHigh);
            g_d2ObjectSeedOverrideApplied = 1;
            // It is a one-shot diagnostic input. Consume it at the target callback so it cannot
            // affect a second room even if the test runner leaves this session alive.
            g_d2ObjectSeedOverrideEnabled = 0;
        }
        const uint32_t objectSeedLowBeforeObjects = traceObjectPop ? pGame->pObjectControl->pSeed.nLowSeed : 0;
        const uint32_t objectSeedHighBeforeObjects = traceObjectPop ? pGame->pObjectControl->pSeed.nHighSeed : 0;
        OBJECTS_PopulationHandler(pGame, pRoom);
        if (traceObjectPop) noteObjectPopTrace(pGame, pRoom, roomSeedLowBeforeObjects, roomSeedHighBeforeObjects,
            objectSeedLowBeforeObjects, objectSeedHighBeforeObjects);
#ifdef D2_MONS_TRACE
        if (tl == d2_trace_level) fprintf(stderr, "  after objects %u\n", pRoom->pSeed.nLowSeed);
#endif
        noteSeq(1, DUNGEON_GetLevelIdFromRoom(pRoom), pRoom->tCoords.nSubtileX, pRoom->tCoords.nSubtileY);
        int32_t* e = g_d2ActivationCount < 4096 ? &g_d2ActivationLog[10 * g_d2ActivationCount++] : nullptr;
        D2MonsterRegionStrc* reg = e ? MONSTERREGION_GetMonsterRegionFromLevelId(pGame->pMonReg, DUNGEON_GetLevelIdFromRoom(pRoom)) : nullptr;
        if (e) { e[0] = DUNGEON_GetLevelIdFromRoom(pRoom); e[1] = pRoom->tCoords.nSubtileX; e[2] = pRoom->tCoords.nSubtileY; e[3] = (int32_t)pRoom->pSeed.nLowSeed;
                 e[4] = reg ? (int32_t)reg->dwUniqueCount : -1; e[5] = reg ? reg->unk0x04 : -1; e[6] = reg ? reg->unk0x0C : -1; }
        D2GAME_PopulateRoom_6FC67190(pGame, pRoom);
        if (e) { e[7] = (int32_t)pRoom->pSeed.nLowSeed; e[8] = reg ? (int32_t)reg->dwUniqueCount : -1; e[9] = (int32_t)pGame->dwGameFrame; }
        g_d2SpawnPath = 0;
        pRoom->dwFlags |= 1u;
        D2Common_10075(pRoom, 1);
    }
}

// D2Game.0x6FC3B2B0: tell every client the unit is gone, then remove it. No clients here.
void __fastcall sub_6FC3B2B0(D2UnitStrc* pUnit, D2GameStrc* pGame) { SUNIT_RemoveUnit(pGame, pUnit); }
