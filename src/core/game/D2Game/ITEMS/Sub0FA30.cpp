#include <test_seam.h>
#include "oc20g3.h"

#include <D2DataTbls.h>
#include <D2Seed.h>

#include "GAME/Game.h"
#include "ITEMS/Items.h"

// 1.13c D2Game+0xFA30 (ecx = class; stack = game, seed, level, flag; ret 0x10)
// Picks a random armor.txt record (the first range of the item table) that sub_F180 accepts and the game version allows; returns its
// index in items.txt, or -1. With no candidate retail returns slot 0 of its uninitialised candidate buffer: `nStale0` keeps the previous call's.
int32_t __fastcall sub_FA30(int32_t nClass, D2GameStrc* pGame, D2SeedStrc* pSeed, int32_t nLevel, int32_t nFlag)
{
    static int32_t nStale0 = 0;

    D2ItemDataTbl* pTbl = D2_CALL(DATATBLS_GetItemDataTables);
    const D2ItemsTxt* pRec = pTbl->pArmor;
    if (!pRec)
    {
        return -1;
    }

    const int32_t nStart = (int32_t)(pRec - pTbl->pItemsTxt);
    const int32_t nCount = (int32_t)(pTbl->pMisc - pRec);
    int32_t anCandidates[0x400];
    int32_t nCandidates = 0;
    for (int32_t i = 0; i < nCount; ++i, ++pRec)
    {
        if (sub_F180(nLevel, nClass, pRec, pSeed, nFlag) && (pGame->bExpansion || pRec->wVersion < 100) && nCandidates < 0x3FF)
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
