// The colour behind an item's cells in a grid (D2Client 0x958A0 -> 0x8C600 -> D2Gfx DrawRectangle, mode 0).
// Retail fills a five-entry table once per act (0x8C6B0): the palette index nearest, by squared RGB
// distance with the first minimum winning (D2CMP #10049), to red, green, blue, yellow and brown.  That is
// why one act's box colour is 234 and another's 138: it is not a constant.  Index 0 red = an item the
// hero cannot use, 1 green = the hovered item, 2 blue = normal, 3 yellow = a box item, 4 brown = unidentified.
export const BOX_RGB = [[128, 0, 0], [0, 128, 0], [0, 0, 128], [128, 128, 0], [128, 64, 64]];

// `palette` is 256 RGBA entries (the act's pal.dat decoded).
export function boxIndices(palette) {
  return BOX_RGB.map(([r, g, b]) => { let best = 0, bd = Infinity;
    for (let k = 0; k < 256; k++) { const dr = palette[k * 4] - r, dg = palette[k * 4 + 1] - g, db = palette[k * 4 + 2] - b, d = dr * dr + dg * dg + db * db;
      if (d < bd) { bd = d; best = k; } }
    return best; });
}

// The per-item choice of entry, and the painter, are grid-highlight.mjs.
