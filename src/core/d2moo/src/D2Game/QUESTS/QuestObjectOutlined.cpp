// 1.13c function that 1.10f keeps inline in the A4Q2 scroll-message and NPC-deactivate callbacks (ACT4Q2_Callback11 / Callback02): spawn the
// act 5 portal (object 566) beside the NPC.
#include <test_seam.h>
#include "QUESTS/ACT4/A4Q2.h"

#include <D2Dungeon.h>
#include <D2DataTbls.h>
#include <Drlg/D2DrlgDrlg.h>
#include <DataTbls/ObjectsIds.h>
#include <DataTbls/ObjectsTbls.h>
#include <Fog.h>
#include "GAME/Game.h"
#include "OBJECTS/Objects.h"
#include "QUESTS/Quests.h"
#include "UNIT/SUnit.h"

// 1.13c D2Game+0xBAFF0 (eax = pQuestArg, stack = the A4Q2 record): find a free cell five columns right of the target, build the 566 object unit
// there the way SUNIT_AllocUnitData's object branch does, then mark the record (+0x44 = 0, +0x45 portal spawned). Returns the unit, nullptr when
// there is no free cell or no unit.
D2UnitStrc* __fastcall QUESTS_SpawnPortalToAct5_BAFF0(D2QuestArgStrc* pQuestArg, D2Act4Quest2Strc* pQuestDataEx)
{
    D2CoordStrc pCoord = {};
    D2_CALL(UNITS_GetCoords, pQuestArg->pTarget, &pCoord);
    pCoord.nX += 5;

    D2ActiveRoomStrc* pRoom = D2_CALL(UNITS_GetRoom, pQuestArg->pTarget);
    D2_CALL(QUESTS_GetFreePosition, D2_CALL(UNITS_GetRoom, pQuestArg->pTarget), &pCoord, 2, 0x400, &pRoom, 100);
    if (!pRoom)
    {
        return nullptr;
    }

    D2GameStrc* pGame = pQuestArg->pGame;
    D2UnitStrc* pUnit = D2_CALL(UNITS_AllocUnit, pGame->pMemoryPool, UNIT_OBJECT);
    if (!pUnit)
    {
        return nullptr;
    }

    pUnit->dwUnitType = UNIT_OBJECT;
    pUnit->dwClassId = 566;
    pUnit->pMemoryPool = pGame->pMemoryPool;
    pUnit->pGame = pGame;
    if (pGame->bExpansion)
    {
        pUnit->dwFlagEx |= UNITFLAGEX_ISEXPANSION;
    }
    else
    {
        pUnit->dwFlagEx &= ~UNITFLAGEX_ISEXPANSION;
    }
    pUnit->dwFlagEx |= UNITFLAGEX_SERVERUNIT;

    pUnit->nAct = (uint8_t)D2_CALL(DRLG_GetActNoFromLevelId, D2_CALL(DUNGEON_GetLevelIdFromRoom, pRoom));
    pUnit->pDrlgAct = pGame->pAct[pUnit->nAct];
    pUnit->pQuestEventList = nullptr;

    D2_CALL(SUNIT_InitSeed, pUnit, &pGame->pGameSeed);
    pUnit->dwFlags |= UNITFLAG_INITSEEDSET;
    pUnit->dwNodeIndex = 11;

    uint32_t nUnitGUID = pGame->dwLastUsedUnitGUID[UNIT_OBJECT] + 1;
    if (nUnitGUID == 0xFFFFFFFFu)
    {
        nUnitGUID = 1;
    }
    pGame->dwLastUsedUnitGUID[UNIT_OBJECT] = nUnitGUID;

    pUnit->pObjectData = (D2ObjectDataStrc*)D2_CALL(FOG_AllocPool, pGame->pMemoryPool, sizeof(D2ObjectDataStrc), "..\\Source\\D2Game\\UNIT\\SUnit.cpp", 613, 0);
    memset(pUnit->pObjectData, 0, sizeof(D2ObjectDataStrc));
    pUnit->pObjectData->pObjectTxt = D2_CALL(DATATBLS_GetObjectsTxtRecord, 566);
    pUnit->dwAnimMode = OBJMODE_OPERATING;
    D2_CALL(UNITS_AllocStaticPath, pUnit);
    D2_CALL(OBJECTS_InitHandler, pGame, pUnit, nUnitGUID, pRoom, pCoord.nX, pCoord.nY);
    D2_CALL(SUNIT_Add, pUnit, pCoord.nX, pCoord.nY, pGame, pRoom, 1);

    pQuestDataEx->unk0x44 = 0;
    pQuestDataEx->bPortalToAct5Spawned = 1;
    pUnit->dwFlags |= UNITFLAG_ISRESURRECT | UNITFLAG_ISINIT;
    return pUnit;
}
