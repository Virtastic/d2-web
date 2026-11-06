// The colour remap of an item's inventory/cursor cel. Retail's inventory-style item blit
// (D2Client+0x6B5A0: grid, worn slot, sockets, the cursor item) draws the cel through the palette
// ITEMS_GetColor(0, item, &c, 1) returns: D2CMP_MixPalette(ItemsTxt.InvTrans, colour), the colour
// being the affix's / unique's / set's own (magic and rare: the affix TransformColor; unique and set:
// the record's InvTransform). The engine reports that (nTrans, nColor) pair packed the way
// mix-palette.mjs reads it, `1 + nColor + 32*nTrans` (16 bits: InvTrans 8, invgreybrown, is above
// 255), as `item.invColourByte` (d2_session_item_inv_colours); 0 or absent = no remap.
//
// F515's captured unique light boots (cov-a4g-inv: InvTrans 8, colour 9) are this rule's row 9 of
// invgreybrown.dat; test/inventory-item-colour.test.mjs checks the retail pixels against it.
import { mixTable } from './mix-palette.mjs';

/** The 256-entry index remap for the item's inventory cel, or null (no remap). */
export function inventoryItemRemap(read, item) {
  return mixTable(read, item?.invColourByte ?? 0);
}

/** A cache key naming the remap, 'identity' when there is none. */
export function inventoryItemColourKey(read, item) {
  return inventoryItemRemap(read, item) ? `mix${item.invColourByte}` : 'identity';
}
