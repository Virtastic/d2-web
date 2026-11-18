// 1.13c functions that 1.10f inlined into the cube code (PlrTrade.cpp); split out here as retail has them.
#include <test_seam.h>
#include "PLAYER/PlrTrade.h"

#include <D2Inventory.h>
#include <D2Items.h>
#include "GAME/SCmd.h"
#include "ITEMS/ItemMode.h"

// 1.13c D2Game+0x711D0 (ecx = pClient, eax = pPlayer, stack = pGame): tell the client about, and remove, every item the player holds in the
// cube page; returns how many were removed.
int32_t __fastcall PLRTRADE_RemoveCubeItems_711D0(D2ClientStrc* pClient, D2UnitStrc* pPlayer, D2GameStrc* pGame)
{
    if (!pPlayer->pInventory)
    {
        return 0;
    }

    int32_t nCount = 0;
    int32_t nFlags = 0;
    D2UnitStrc* pNextItem = nullptr;
    for (D2UnitStrc* pCurrentItem = D2_CALL(INVENTORY_GetFirstItem, pPlayer->pInventory); pCurrentItem != nullptr; pCurrentItem = pNextItem)
    {
        D2UnitStrc* pItem = D2_CALL(INVENTORY_UnitIsItem, pCurrentItem);
        pNextItem = D2_CALL(INVENTORY_GetNextItem, pCurrentItem);
        if (pItem && D2_CALL(ITEMS_GetInvPage, pItem) == INVPAGE_CUBE)
        {
            nFlags = 0;
            D2_CALL(ITEMS_SetOrRemoveFlag, &nFlags, 0x20, 1);
            D2_CALL(ITEMS_SetItemCell, pItem, INVPAGE_CUBE);
            D2_CALL(D2GAME_UpdateClientItem_6FC3E9D0, pClient, pPlayer, pItem, nFlags);
            D2_CALL(D2GAME_RemoveItem_6FC471F0, pGame, pPlayer, pItem, 0);
            ++nCount;
        }
    }
    return nCount;
}
