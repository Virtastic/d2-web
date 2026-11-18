#include "GAME/Clients.h"
#include <test_seam.h>
#include <cstdlib>

// The nearest-player update list a client keeps (up to 55 entries, sorted by Manhattan distance): the
// sort node array is built by CLIENTS_RefreshUnitsUpdateList (1.13c D2Game+0x4AD80), one
// D2GAME_ClientsUpdateInsert_6FC6A460 per room unit, and flattened by CLIENTS_SetUnitsUpdateList.

//D2Game.0x6FC33570
// 1.13c D2Game+0x4A370 (ESI = client, stack = the sort array; the walk starts at node 0 and stops at the node whose link is 0)
void __fastcall CLIENTS_SetUnitsUpdateList(D2ClientStrc* pClient, D2ClientUnitUpdateSortStrc* pSort)
{
    D2ClientUnitUpdateSortStrc* pCurrent = pSort;

    pClient->nUnitUpdateIndex = 0;

    do
    {
        D2ClientUnitUpdateStrc* pUpdate = &pClient->unitUpdate[pClient->nUnitUpdateIndex];
        pUpdate->nUnitType = pCurrent->pUnit ? pCurrent->pUnit->dwUnitType : 6;
        pUpdate->nUnitGUID = pCurrent->pUnit ? pCurrent->pUnit->dwUnitId : -1;
        pUpdate->nX = D2_CALL(CLIENTS_GetUnitX, pCurrent->pUnit);
        pUpdate->nY = D2_CALL(CLIENTS_GetUnitY, pCurrent->pUnit);

        ++pClient->nUnitUpdateIndex;

        pCurrent = &pSort[pCurrent->nNextIndex];
    }
    while (pCurrent->nNextIndex);
}

// 1.13c D2Game+0x4A460 (EAX = nX, ESI = pSort; 1.10f inlined it into CLIENTS_RefreshUnitsUpdateList)
// Inserts a player unit into the distance-sorted list; pCurrentMax is the largest distance kept, nUpdateSize the list capacity.
void __fastcall D2GAME_ClientsUpdateInsert_6FC6A460(D2UnitStrc* pUnit, int32_t nY, uint32_t* pCurrentMax, D2ClientStrc* pClient, uint32_t nUpdateSize, D2ClientUnitUpdateSortStrc* pSort, int32_t nX)
{
    if (!pUnit || pUnit->dwUnitType != UNIT_PLAYER)
    {
        return;
    }

    const int32_t nDeltaPosX = std::abs(nX - D2_CALL(CLIENTS_GetUnitX, pUnit));
    const int32_t nDeltaPosY = std::abs(nY - D2_CALL(CLIENTS_GetUnitY, pUnit));
    const uint32_t nDistance = (uint32_t)nDeltaPosX + (uint32_t)nDeltaPosY;

    const uint32_t nCount = pClient->nUnitUpdateIndex;
    if (nCount >= nUpdateSize && nDistance >= *pCurrentMax)
    {
        return;
    }

    // first node whose distance is not below ours; nPrevious is the node in front of it
    int32_t nPrevious = 0;
    int32_t nNext = pSort[0].nNextIndex;
    while (nNext != 0)
    {
        if (!((uint32_t)pSort[nNext].nDistance < nDistance))
        {
            break;
        }
        nPrevious = nNext;
        nNext = pSort[nNext].nNextIndex;
    }

    int32_t nInsertIndex;
    if (nCount == nUpdateSize)
    {
        // the list is full: the tail node is dropped and reused when this unit is closer
        int32_t nBeforeTail = 0;
        while (pSort[pSort[nBeforeTail].nNextIndex].nNextIndex != 0)
        {
            nBeforeTail = pSort[nBeforeTail].nNextIndex;
        }
        const int32_t nTail = pSort[nBeforeTail].nNextIndex;
        if (nCount != 0 && (uint32_t)pSort[nTail].nDistance <= nDistance)
        {
            return;
        }

        *pCurrentMax = nCount == 1 ? nDistance : (uint32_t)pSort[nBeforeTail].nDistance;
        pSort[nBeforeTail].nNextIndex = 0;
        nInsertIndex = nTail;
    }
    else
    {
        pClient->nUnitUpdateIndex = nCount + 1;
        nInsertIndex = nCount;
    }

    if (nPrevious == 0 && nDistance < (uint32_t)pSort[0].nDistance)
    {
        // the new unit becomes the head; the old head moves to the free slot
        if (pClient->nUnitUpdateIndex != 1)
        {
            pSort[nInsertIndex].nNextIndex = pSort[0].nNextIndex;
            pSort[nInsertIndex].pUnit = pSort[0].pUnit;
            pSort[nInsertIndex].nDistance = pSort[0].nDistance;
        }
        pSort[0].nNextIndex = nInsertIndex;
        nInsertIndex = 0;
    }
    else
    {
        pSort[nInsertIndex].nNextIndex = pSort[nPrevious].nNextIndex;
        pSort[nPrevious].nNextIndex = nInsertIndex;
    }

    pSort[nInsertIndex].nDistance = (int32_t)nDistance;
    pSort[nInsertIndex].pUnit = pUnit;
    if (*pCurrentMax < nDistance)
    {
        *pCurrentMax = nDistance;
    }
}
