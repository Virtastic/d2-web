// 1.13c keeps the end-of-animation tail shared by the two inferno-style skill launchers (B6840/B6A60) as its own function;
// 1.10f inlines it in SkillMonst.cpp. Split out here so the one body serves both.
#include <test_seam.h>
#include "SKILLS/SkillMonst.h"

#include <D2Monsters.h>
#include <D2DataTbls.h>
#include "GAME/Event.h"
#include "GAME/Game.h"
#include "MONSTER/MonsterRegion.h"

// 1.13c D2Game+0xB6720 (register inputs: eax = pGame, ebx = pUnit)
int32_t __fastcall SKILLS_EndAnimTail_B6720(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
    int32_t nAnimLength = 11;
    if (pUnit)
    {
        if (pUnit->dwUnitType == UNIT_MONSTER)
        {
            D2MonStats2Txt* pMonStats2TxtRecord = D2_CALL(MONSTERREGION_GetMonStats2TxtRecord, pUnit->dwClassId);
            if (pMonStats2TxtRecord)
            {
                nAnimLength = pMonStats2TxtRecord->nInfernoLen;
            }
        }

        D2EventTimerStrc* pTimer = pUnit->pTimer;
        while (pTimer)
        {
            D2EventTimerStrc* pNext = pTimer->pNext;
            if (pTimer->nEventType == EVENTTYPE_MODECHANGE)
            {
                D2_CALL(sub_6FC34890, pGame, pTimer);
            }
            pTimer = pNext;
        }
    }

    D2_CALL(D2GAME_InitTimer_6FC351D0, pGame, pUnit, EVENTTYPE_ENDANIM, pGame->dwGameFrame + nAnimLength, nullptr, 0, 0);
    return 1;
}
