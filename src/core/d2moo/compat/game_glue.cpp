// The few GAME/Game.cpp pieces the vendored code needs without the server around it (Game.cpp
// itself is the game-list / thread / packet loop and stays out until the sim needs it).
#include <D2Common.h>
#include <cstdio>
#include <D2Dungeon.h>
#include <GAME/Game.h>
#include <GAME/Clients.h>
#include <UNIT/SUnit.h>
#include <UNIT/SUnitInactive.h>
#include <MONSTER/MonsterRegion.h>
#include <OBJECTS/Objects.h>

char gszEmptyString_6FD447EC[8];
D2ServerCallbackFunctions* gpD2EventCallbackTable_6FD45830;   // the host's callback table: none headless

// D2Game.0x6FC385A0 (1.13c 0x6FC4CB80): populate a room the first time it becomes active.
// Order is the one retail runs and the retail capture tool recorded.
void __fastcall sub_6FC385A0(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom) {
#ifdef D2_MONS_TRACE
    const int tl = DUNGEON_GetLevelIdFromRoom(pRoom); if (tl == d2_trace_level) fprintf(stderr, "act room %d,%d: seed0 %u\n", pRoom->tCoords.nSubtileX, pRoom->tCoords.nSubtileY, pRoom->pSeed.nLowSeed);
#endif
    sub_6FC679F0(pGame, pRoom);
#ifdef D2_MONS_TRACE
    if (tl == d2_trace_level) fprintf(stderr, "  after tick %u\n", pRoom->pSeed.nLowSeed);
#endif
    if (pRoom->dwFlags & 1) {
        if (!D2Common_10074(pRoom)) { SUNITINACTIVE_RestoreInactiveUnits(pGame, pRoom); D2Common_10075(pRoom, 1); }
    } else {
        SUNIT_SpawnPresetUnitsInRoom(pGame, pRoom);
#ifdef D2_MONS_TRACE
        if (tl == d2_trace_level) fprintf(stderr, "  after presets %u\n", pRoom->pSeed.nLowSeed);
#endif
        SUNITINACTIVE_RestoreInactiveUnits(pGame, pRoom);
        OBJECTS_PopulationHandler(pGame, pRoom);
#ifdef D2_MONS_TRACE
        if (tl == d2_trace_level) fprintf(stderr, "  after objects %u\n", pRoom->pSeed.nLowSeed);
#endif
        D2GAME_PopulateRoom_6FC67190(pGame, pRoom);
        pRoom->dwFlags |= 1u;
        D2Common_10075(pRoom, 1);
    }
}

// D2Game.0x6FC3B2B0: tell every client the unit is gone, then remove it. No clients here.
void __fastcall sub_6FC3B2B0(D2UnitStrc* pUnit, D2GameStrc* pGame) { SUNIT_RemoveUnit(pGame, pUnit); }
