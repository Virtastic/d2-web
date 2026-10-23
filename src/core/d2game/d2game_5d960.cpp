// 1.13c D2Game+0x5D960: spawn missile 541 from pQuestArg->pTarget to the object with GUID nObjectId (EBX in retail).
// The same code is inlined in A5Q5 (1.10f); 1.13c has it as a separate routine. Object list = game +0x1520 hash.
#include <D2Dungeon.h>
#include <D2DataTbls.h>
#include <Units/Units.h>
#include "GAME/Clients.h"
#include "GAME/Game.h"
#include "MISSILES/Missiles.h"
#include "QUESTS/Quests.h"
#include "UNIT/SUnit.h"

// 1.13c D2Game+0x5D960
void d2game_5D960(D2QuestDataStrc* pQuestData, D2QuestArgStrc* pQuestArg, int32_t nObjectId)
{
    D2_ASSERT(pQuestData->pGame);
    D2UnitStrc* pObject = SUNIT_GetServerUnit(pQuestData->pGame, UNIT_OBJECT, nObjectId);
    if (!pObject)
        return;
    D2MissileStrc m = {};
    m.dwFlags = 0x420;
    m.nMissile = 541;
    m.pOwner = pQuestArg->pTarget;
    m.pOrigin = pQuestArg->pTarget;
    m.nTargetX = CLIENTS_GetUnitX(pObject);
    m.nTargetY = CLIENTS_GetUnitY(pObject);
    m.nSkillLevel = 1;
    D2UnitStrc* pMissile = MISSILES_CreateMissileFromParams(pQuestData->pGame, &m);
    if (pMissile)
        MISSILE_SetTargetX(pMissile, nObjectId);
}
