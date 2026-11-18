#include "GAME/Game113c.h"
#include "GAME/Event.h"
#include <Units/Units.h>
#include <test_seam.h>

// 1.13c D2Game+0x9D8E0
// EAX = the game, whose +0x74 holds the first unit of its unit chain (pRoomNext links). For every monster unit (type 1) the unit's event timer list (pTimer, pNext links) is walked twice: first reporting each timer of event type 2, then each of type 3, to sub_6FC34890.
void __fastcall GAME_ReportUnitEventTimers(D2GameStrc* pGame, D2UnitStrc* pFirstUnit)
{
    for (D2UnitStrc* pUnit = pFirstUnit; pUnit; pUnit = pUnit->pRoomNext)
    {
        if (pUnit->dwUnitType != UNIT_MONSTER)
        {
            continue;
        }
        for (int32_t nPass = 2; nPass <= 3; ++nPass)
        {
            for (D2EventTimerStrc* pTimer = pUnit->pTimer; pTimer; pTimer = pTimer->pNext)
            {
                if ((uint8_t)pTimer->nEventType == nPass)
                {
                    D2_CALL(sub_6FC34890, pGame, pTimer);
                }
            }
        }
    }
}
