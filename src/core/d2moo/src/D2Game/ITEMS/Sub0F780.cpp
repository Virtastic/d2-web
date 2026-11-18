#include <test_seam.h>
#include "oc20g3.h"

#include <D2DataTbls.h>
#include <D2Seed.h>

#include "GAME/Game.h"
#include "ITEMS/Items.h"

// 1.13c D2Game+0xF780 (stack = game, seed, level, class, flag, skip-type-0x28; ret 0x18)
// Picks a random misc.txt record (the third range of the item table) that sub_F180 accepts and the game version allows; returns its
// index in items.txt, or -1. Type 0x28 records are left out unless `bAllowType28`. Retail's candidate buffer is an uninitialised stack array:
// with no candidate its slot 0 is returned, which still holds the first candidate of the previous call -- kept in `nStale0`.
int32_t __fastcall sub_F780(D2GameStrc* pGame, D2SeedStrc* pSeed, int32_t nLevel, int32_t nClass, int32_t nFlag, int32_t bAllowType28)
{
    static int32_t nStale0 = 0;

    D2ItemDataTbl* pTbl = D2_CALL(DATATBLS_GetItemDataTables);
    const D2ItemsTxt* pRec = pTbl->pMisc;
    if (!pRec)
    {
        return -1;
    }

    const int32_t nStart = (int32_t)(pRec - pTbl->pItemsTxt);
    const int32_t nCount = (int32_t)((pTbl->pItemsTxt + pTbl->nItemsTxtRecordCount) - pRec);
    int32_t anCandidates[0x400];
    int32_t nCandidates = 0;
    for (int32_t i = 0; i < nCount; ++i, ++pRec)
    {
        if ((bAllowType28 != 0 || pRec->wType[0] != 0x28) && sub_F180(nLevel, nClass, pRec, pSeed, nFlag)
            && (pGame->bExpansion || pRec->wVersion < 100) && nCandidates < 0x3FF)
        {
            anCandidates[nCandidates++] = i + nStart;
        }
    }

    if (nCandidates <= 0)
    {
        return nStale0;
    }
    nStale0 = anCandidates[0];
    return anCandidates[ITEMS_RollLimitedRandomNumber(pSeed, nCandidates)];
}
