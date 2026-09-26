// The few GAME/Game.cpp pieces the vendored code needs without the server around it (Game.cpp
// itself is the game-list / thread / packet loop and stays out until the sim needs it).
#include <D2Common.h>
#include <cstdio>
#include <D2Dungeon.h>
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
void d2_note_monster_spawn(D2GameStrc* pGame, D2UnitStrc* pMonster, int32_t nClass, int32_t nX, int32_t nY) {
    if (g_d2SpawnCount >= 8192) return;
    int32_t* e = &g_d2SpawnLog[6 * g_d2SpawnCount++];
    e[0] = pGame ? (int32_t)pGame->dwGameFrame : -1; e[1] = (int32_t)pMonster->dwUnitId; e[2] = nClass; e[3] = nX; e[4] = nY; e[5] = g_d2SpawnPath;
}

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
        if (!D2Common_10074(pRoom)) { { const int32_t was = g_d2SpawnPath; g_d2SpawnPath = 3; SUNITINACTIVE_RestoreInactiveUnits(pGame, pRoom); g_d2SpawnPath = was; } D2Common_10075(pRoom, 1); }
    } else {
        g_d2SpawnPath = 1;
        SUNIT_SpawnPresetUnitsInRoom(pGame, pRoom);
#ifdef D2_MONS_TRACE
        if (tl == d2_trace_level) fprintf(stderr, "  after presets %u\n", pRoom->pSeed.nLowSeed);
#endif
        { const int32_t was = g_d2SpawnPath; g_d2SpawnPath = 3; SUNITINACTIVE_RestoreInactiveUnits(pGame, pRoom); g_d2SpawnPath = was; }
        OBJECTS_PopulationHandler(pGame, pRoom);
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
