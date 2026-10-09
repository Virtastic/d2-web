// Retail's DrawMode 7 (HIGHLIGHT) selects the mode-7 table from the active D2PaletteTableStrc.
// The 1.13c D2Win palette loader builds that 256-byte table from the PL2 base colours: each channel
// is floor(channel * 170 / 100), capped at 255, then mapped to the closest palette entry. See the
// native addresses and evidence The PL2 base colours are the same as
// the corresponding pal.dat entries; this helper accepts the latter's BGR triples.
export const HIGHLIGHT_SCALE = 170 / 100;

/** 256-entry mode-7 index remap for a BGR pal.dat (768 bytes), using the native 1.13c rule by default. */
export function highlightMap(datBgr, scale = HIGHLIGHT_SCALE) {
  const rgb = (j) => [datBgr[j * 3 + 2], datBgr[j * 3 + 1], datBgr[j * 3]];
  const out = new Uint8Array(256);
  for (let s = 0; s < 256; s++) {
    const t = rgb(s).map((v) => Math.min(255, scale === HIGHLIGHT_SCALE ? Math.floor((170 * v) / 100) : v * scale));
    let best = s, bd = Infinity;
    for (let j = 0; j < 256; j++) { const c = rgb(j), d = (c[0] - t[0]) ** 2 + (c[1] - t[1]) ** 2 + (c[2] - t[2]) ** 2; if (d < bd) { bd = d; best = j; } }
    out[s] = best;
  }
  return out;
}
