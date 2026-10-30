// The colour behind an item's cells in a grid (D2Client 0x958A0 -> 0x8C600 -> D2Gfx DrawRectangle, mode 0).
// Retail fills a five-entry table once per act (0x8C6B0): the palette index nearest, by squared RGB
// distance with the first minimum winning (D2CMP #10049), to red, green, blue, yellow and brown.  That is
// why one act's box colour is 234 and another's 138: it is not a constant.  Index 0 red = an item the
// hero cannot use, 1 green = the hovered item, 2 blue = normal, 3 yellow = a box item, 4 brown = unidentified.
export const BOX_RGB = [[128, 0, 0], [0, 128, 0], [0, 0, 128], [128, 128, 0], [128, 64, 64]];
export const BOX_RED = 0, BOX_HOVER = 1, BOX_NORMAL = 2, BOX_UNIDENTIFIED = 4;

// `palette` is 256 RGBA entries (the act's pal.dat decoded).
export function boxIndices(palette) {
  return BOX_RGB.map(([r, g, b]) => { let best = 0, bd = Infinity;
    for (let k = 0; k < 256; k++) { const dr = palette[k * 4] - r, dg = palette[k * 4 + 1] - g, db = palette[k * 4 + 2] - b, d = dr * dr + dg * dg + db * db;
      if (d < bd) { bd = d; best = k; } }
    return best; });
}

// 0x958A0's per-item choice.  `item.flags` is the item's dwItemFlags (0x4 and 0x10 are the two bits it reads),
// `item.propColours` the tooltip's line colours, where 1 is a requirement the hero does not meet
// (ITEMS_CheckRequirements).  Not modelled: 0x8C370's class-specific skill test (only items with
// ItemsTxt +0x12a set).
export function boxSlot(item, hovered) {
  const flags = (item?.flags ?? 0) >>> 0;
  if (hovered) return BOX_HOVER;   // shop-magic-unmet-r21: the hovered item is green although it is red in shop-magic-hover-r21
  if ((item?.propColours || []).includes(1) || (flags & 0x4)) return BOX_RED;
  if (!(flags & 0x10)) return BOX_UNIDENTIFIED;
  return BOX_NORMAL;
}
