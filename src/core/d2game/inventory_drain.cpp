#include "inventory_drain.h"
#include <D2Inventory.h>
#include <UNIT/SUnit.h>
#include <cstdlib>

// 1.13c D2Game+0x8F470: EAX inventory, stack game. Native engine pointers
// remain native width; no retail byte-layout casts are used on engine objects.
void D2GAME_DrainInventory_113c(D2InventoryStrc* inventory, D2GameStrc* game)
{
    if (!inventory) return;
    for (D2UnitStrc* node = INVENTORY_GetFirstItem(inventory); node;) {
        D2UnitStrc* item = INVENTORY_UnitIsItem(node);
        D2UnitStrc* next = INVENTORY_GetNextItem(node);
        // Retail reports Fog assertion 0x1c6 and exits on an invalid item.
        if (!item) std::exit(-1);
        (void)INVENTORY_GetItemNodePage(node); // #10698 is a getter, not removal.
        SUNIT_RemoveUnit(game, item);
        node = next; // deletion invalidates node and its item data
    }
}
