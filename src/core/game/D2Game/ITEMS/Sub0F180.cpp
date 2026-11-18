#include <test_seam.h>
#include "oc20g3.h"

#include <D2DataTbls.h>
#include <D2Seed.h>
#include <Drlg/D2DrlgDrlg.h>

#include "ITEMS/Items.h"

static_assert(sizeof(D2ItemsTxt) == 0x1A8, "items.txt record size");

// 1.13c D2Game+0xF180 (eax = level, ebx = item class, edi = items.txt record; stack = seed, flag; ret 8)
// Whether one items.txt record may drop at `nLevel`: spawnable, not a quest item, level requirement met. Unless `nFlag` is set the rarity
// (reduced by the act number) is rolled against; a class other than -1 must equal the record's first item type.
int32_t __fastcall sub_F180(int32_t nLevel, int32_t nClass, const D2ItemsTxt* pItemsTxt, D2SeedStrc* pSeed, int32_t nFlag)
{
    if (nLevel < 1)
    {
        nLevel = 1;
    }

    if (!pItemsTxt->nSpawnable || pItemsTxt->nQuest || pItemsTxt->nLevel > nLevel)
    {
        return 0;
    }

    if (nFlag == 0)
    {
        const int32_t nLimit = (int32_t)pItemsTxt->nRarity - (int32_t)(D2_CALL(DRLG_GetActNoFromLevelId, nLevel) & 0xFF);
        if (nLimit > 0 && ITEMS_RollLimitedRandomNumber(pSeed, nLimit) != 0)
        {
            return 0;
        }
    }

    if (nClass != -1 && pItemsTxt->wType[0] != nClass)
    {
        return 0;
    }
    return 1;
}
