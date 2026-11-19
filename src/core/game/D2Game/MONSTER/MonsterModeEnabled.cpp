#include "MONSTER/MonsterMode.h"
#include "MONSTER/MonsterRegion.h"
#include <D2BitManip.h>
#include <D2DataTbls.h>
#include <D2Monsters.h>
#include <DataTbls/MonsterTbls.h>
#include <test_seam.h>

extern const D2MonModeInfoTableStrc stru_6FD28738[16];

// 1.13c D2Game+0x74F20
// ECX = the unit, EAX = the mode (signed, must be below 16). A mode whose table row has the first flag set is always allowed; one with the second flag clear and the third set is allowed when the unit's class has the mode's bit in its MonStats2 moving-mode flags.
uint32_t __fastcall MONSTERMODE_IsModeEnabledForUnit(D2UnitStrc* pUnit, int32_t nMode)
{
	if (D2Seam<&MONSTERMODE_IsModeEnabledForUnit>::hook) return D2Seam<&MONSTERMODE_IsModeEnabledForUnit>::hook(pUnit, nMode);
    if (pUnit && nMode < 16)
    {
        const D2MonModeInfoTableStrc* pRecord = &stru_6FD28738[nMode];
        if (pRecord->unk0x00)
        {
            return 1;
        }

        if (!pRecord->unk0x04 && pRecord->unk0x08)
        {
            D2MonStats2Txt* pMonStats2TxtRecord = D2_CALL(MONSTERREGION_GetMonStats2TxtRecord, pUnit->dwClassId);
            if (pMonStats2TxtRecord)
            {
                return pMonStats2TxtRecord->nModeEnabledWhenMovingFlags[nMode >> 3] & gdwBitMasks[nMode & 7];
            }
        }
    }

    return 0;
}
