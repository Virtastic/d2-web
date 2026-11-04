// D2Common 0x21DD0 port, written from the 1.13c decompile. stdcall, ret 0x1C: EAX = inventory, EDI = item, stack = x, y, recordId, unused, page.
#include <cstdint>
#include "D2Inventory.h"
#include "Units/Units.h"

// 1.13c D2Common+0x21DD0
extern "C" void D2Common_21DD0_PlaceAtPage(D2InventoryStrc* pInventory, D2UnitStrc* pItem, int32_t nXPos, int32_t nYPos,
                                           int32_t nRecordId, int32_t bUnused, uint8_t nPage)
{
	(void)bUnused;
	if (pInventory == nullptr) return;
	INVENTORY_PlaceItemInGrid(pInventory, pItem, nXPos, nYPos, (int32_t)nPage + 2, nRecordId, 0);
}
