#include "MONSTER/MonsterMode.h"
#include <D2Monsters.h>
#include <Units/Monster.h>

// 1.13c D2Game+0x75260
// EAX = the new anim mode, ECX = the unit. On a monster with monster data the stored AI state (+0x54) of 0x10 or more becomes state - 0x10; below that a knockback state held while the new mode is get-hit stays knockback; else the new mode is stored.
void __fastcall MONSTER_StoreAiStateForMode(D2UnitStrc* pUnit, int32_t nAnimMode)
{
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData)
    {
        const int32_t nAiState = (int32_t)pUnit->pMonsterData->dwAiState;
        if (nAiState < 16)
        {
            if (nAiState == MONMODE_KNOCKBACK && nAnimMode == MONMODE_GETHIT)
            {
                nAnimMode = MONMODE_KNOCKBACK;
            }
        }
        else
        {
            nAnimMode = nAiState - 16;
        }

        pUnit->pMonsterData->dwAiState = (uint32_t)nAnimMode;
    }
}
