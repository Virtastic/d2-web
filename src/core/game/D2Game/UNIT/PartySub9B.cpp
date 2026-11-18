#include <D2Game/oc20g11.h>

#include <Fog.h>
#include <D2StatList.h>
#include <Units/Units.h>
#include "../../../compat/test_seam.h"

#include "UNIT/Party.h"

// 1.13c D2Game+0x9BD00 (sub_9BD00): the party with the given id, or null. A missing party control is fatal.
D2PartyStrc* __fastcall sub_9BD00(D2GameStrc* pGame, int16_t nPartyId)
{
    D2_ASSERT(pGame->pPartyControl);

    for (D2PartyStrc* pParty = pGame->pPartyControl->pParties; pParty; pParty = pParty->pNext)
    {
        if (pParty->nPartyId == nPartyId)
        {
            return pParty;
        }
    }
    return nullptr;
}

// 1.13c D2Game+0x9BD40 (sub_9BD40): the next free party id; the control's counter becomes that id plus one.
void __fastcall sub_9BD40(D2GameStrc* pGame)
{
    D2PartyControlStrc* pPartyControl = pGame->pPartyControl;

    int16_t nPartyId = pPartyControl->field_0 < 3 ? 3 : pPartyControl->field_0;
    while (D2PartyStrc* pParty = pPartyControl->pParties)
    {
        while (pParty->nPartyId != nPartyId)
        {
            pParty = pParty->pNext;
            if (!pParty)
            {
                goto done;
            }
        }
        ++nPartyId;
        if (nPartyId < 3)
        {
            nPartyId = 3;
        }
    }
done:
    pPartyControl->field_0 = nPartyId + 1;
}

// 1.13c D2Game+0x9BF40
// The party's node count, or -1 when no party has the id.
int32_t __fastcall PARTY_GetNodeCount(D2GameStrc* pGame, int16_t nPartyId)
{
    D2PartyStrc* pParty = sub_9BD00(pGame, nPartyId);
    if (!pParty)
    {
        return -1;
    }

    int32_t nCount = 0;
    for (D2PartyNodeStrc* pNode = pParty->pPartyNodes; pNode; pNode = pNode->pNext)
    {
        ++nCount;
    }
    return nCount;
}

// 1.13c D2Game+0x9C7D0
// Unlinks the first party with the id from the control's list and destroys it (0x9C6F0). A missing control is fatal.
void __fastcall PARTY_Delete(D2GameStrc* pGame, int16_t nPartyId)
{
    D2_ASSERT(pGame->pPartyControl);

    D2PartyControlStrc* pPartyControl = pGame->pPartyControl;
    D2PartyStrc* pPrevious = nullptr;
    for (D2PartyStrc* pParty = pPartyControl->pParties; pParty; pPrevious = pParty, pParty = pParty->pNext)
    {
        if (pParty->nPartyId != nPartyId)
        {
            continue;
        }
        if (pPrevious)
        {
            pPrevious->pNext = pParty->pNext;
        }
        else
        {
            pPartyControl->pParties = pParty->pNext;
        }
        D2_CALL(sub_6FCBC6F0, pGame, pParty);
        return;
    }
}

// 1.13c D2Game+0x9BC70 (eax = amount, ebx = unit, edi = out1): splits an amount into what the unit's gold
// limit still accepts (out1) and the overflow (out2).
void __fastcall sub_9BC70(D2UnitStrc* pUnit, int32_t* pOverflow, int32_t nAmount, int32_t* pAccepted)
{
    const int32_t nLimit = (int32_t)D2_CALL(UNITS_GetInventoryGoldLimit, pUnit);
    const int32_t nCurrent = D2_CALL(STATLIST_UnitGetStatValue, pUnit, 0xE, 0);
    *pOverflow = 0;
    *pAccepted = nAmount;
    if (nLimit < (int32_t)((uint32_t)nCurrent + (uint32_t)nAmount))
    {
        *pAccepted = (int32_t)((uint32_t)nLimit - (uint32_t)nCurrent);
        *pOverflow = (int32_t)((uint32_t)nAmount - (uint32_t)*pAccepted);
    }
}
