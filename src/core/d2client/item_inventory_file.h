// The inventory graphic retail's panels draw for an item. ItemsTxt.invfile is only the first candidate:
// a unique (or set) item whose quality file is non-empty uses that, and an item whose ItemTypes row
// carries variable inventory graphics (Ring, Amulet, the charms, Jewel) uses the variant its own
// nInvGfxIdx picks -- the 3-bit picture index of the item's record (D2Common #10726/#10789/#10790).
// The local game's panels and the online client's grids read this one rule.
#pragma once
#include <D2Items.h>
#include <DataTbls/ItemsTbls.h>

static inline const char* d2client_item_inventory_file(D2UnitStrc* item, const D2ItemsTxt* txt) {
    const int quality = ITEMS_GetItemQuality(item);
    if (quality == ITEMQUAL_UNIQUE && txt->szUniqueInvFile[0]) return txt->szUniqueInvFile;
    if (quality == ITEMQUAL_SET && txt->szSetInvFile[0]) return txt->szSetInvFile;
    if (ITEMS_GetVarInvGfxCount(item)) return ITEMS_GetVarInvGfxString(item, ITEMS_GetVarGfxIndex(item));
    return txt->szInvFile;
}
