// Retail's DrawMode 7 (HIGHLIGHT): the hovered unit's cel, brighter. V5.
//
// MEASURED on ONE unit's own cel (s556; the retail captures): retail's 8-bit frame
// buffers hold Warriv's WATRlitNUhth frame 15 in mode 5 (== the cel's palette indices, unchanged) and mode 7. The
// mode-7 pixel is a single-valued function of the cel index -- the 26-of-89 "several results" were the 28-frame
// animation gap and the cursor/name plate inside the compared box -- and it is NOT any 256-byte block of the act's
// PL2 (searched at every byte offset) nor a blend-table lookup. It is the index's colour scaled by ~1.69 (clamped)
// and snapped to the nearest palette entry: exactly retail's index on most pixels and within one palette step
// (<= 8/255 a channel, the shared level-3 tolerance) on the rest. The full retail table is not recovered;
// the issue 6 stash's ACT1 framebuffer (s2205) supplies six exact entries, hash-gated below, while all other
// palette/entry pairs still use the stated fitted scale.
export const HIGHLIGHT_SCALE = 1.69;

// Retail 1.13c's ACT1 mode-7 framebuffer for issue 6/TR/NU frame 0 supplies exact remap entries for
// six palette indices where the fitted scale misses: the retail captures
// The hash gate keeps these measured ACT1 entries from leaking into another act's distinct palette.
const ACT1_RETAIL_PALETTE_FNV1A32 = 0x32c903fa;
const ACT1_RETAIL_HIGHLIGHT = new Map([[19,22], [33,33], [36,42], [73,99], [82,12], [182,193]]);

function fnv1a32(bytes) {
  let hash = 0x811c9dc5;
  for (const value of bytes) { hash ^= value; hash = Math.imul(hash, 0x01000193); }
  return hash >>> 0;
}

/** 256-entry index remap for a BGR pal.dat (768 bytes): fitted colour-scale map plus the measured ACT1 entries. */
export function highlightMap(datBgr, scale = HIGHLIGHT_SCALE) {
  const rgb = (j) => [datBgr[j * 3 + 2], datBgr[j * 3 + 1], datBgr[j * 3]];
  const out = new Uint8Array(256);
  for (let s = 0; s < 256; s++) {
    const t = rgb(s).map((v) => Math.min(255, v * scale));
    let best = s, bd = Infinity;
    for (let j = 0; j < 256; j++) { const c = rgb(j), d = (c[0] - t[0]) ** 2 + (c[1] - t[1]) ** 2 + (c[2] - t[2]) ** 2; if (d < bd) { bd = d; best = j; } }
    out[s] = best;
  }
  if (scale === HIGHLIGHT_SCALE && datBgr.length === 768 && fnv1a32(datBgr) === ACT1_RETAIL_PALETTE_FNV1A32)
    for (const [source, retail] of ACT1_RETAIL_HIGHLIGHT) out[source] = retail;
  return out;
}
