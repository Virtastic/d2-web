#include "ITEMS/Items.h"
#include <test_seam.h>
#include <D2Inventory.h>

// 1.13c D2Game+0xE1C0
// Purges one item from one inventory. The decompile hides both arguments: EBX = pItem, EDI = pInventory. The inventory's item list is
// scanned for the node whose unit is pItem; a hit removes it from the inventory and stops. Without a hit, an item sitting on the cursor
// is cleared from the cursor. This is the body of the 1.10f ITEMS_PlayerIterateCallback_RemoveItem once the player's inventory is picked
// (1.13c D2Game+0xE7E0 passes it in). Delta from 1.10f: none.
void __fastcall ITEMS_RemoveItemFromInventory(D2UnitStrc* pItem, D2InventoryStrc* pInventory)
{
	for (D2UnitStrc* i = INVENTORY_GetFirstItem(pInventory); i; i = INVENTORY_GetNextItem(i))
	{
		if (INVENTORY_UnitIsItem(i) == pItem)
		{
			INVENTORY_RemoveItemFromInventory(pInventory, pItem);
			return;
		}
	}

	D2UnitStrc* pCursorItem = INVENTORY_GetCursorItem(pInventory);
	if (pCursorItem && pCursorItem == pItem)
	{
		INVENTORY_SetCursorItem(pInventory, nullptr);
	}
}
