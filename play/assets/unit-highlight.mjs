// Retail's DrawMode 7 (HIGHLIGHT): the hovered unit's cel, brighter. V5.
//
// MEASURED on ONE unit's own cel (s556; the retail captures): retail's 8-bit frame
// buffers hold Warriv's WATRlitNUhth frame 15 in mode 5 (== the cel's palette indices, unchanged) and mode 7. The
// mode-7 pixel is a single-valued function of the cel index -- the 26-of-89 "several results" were the 28-frame
// animation gap and the cursor/name plate inside the compared box -- and it is NOT any 256-byte block of the act's
// PL2 (searched at every byte offset) nor a blend-table lookup. It is the index's colour scaled by ~1.69 (clamped)
// and snapped to the nearest palette entry: exactly retail's index on most pixels and within one palette step
// (<= 8/255 a channel, the shared level-3 tolerance) on the rest. The exact retail table is not recovered, so the
// scale is the fitted constant and the tolerance is stated in the gate, not hidden.
export const HIGHLIGHT_SCALE = 1.69;

/** 256-entry index remap for a BGR pal.dat (768 bytes): entry s = nearest palette index to colour(s) x HIGHLIGHT_SCALE. */
export function highlightMap(datBgr, scale = HIGHLIGHT_SCALE) {
  const rgb = (j) => [datBgr[j * 3 + 2], datBgr[j * 3 + 1], datBgr[j * 3]];
  const out = new Uint8Array(256);
  for (let s = 0; s < 256; s++) {
    const t = rgb(s).map((v) => Math.min(255, v * scale));
    let best = s, bd = Infinity;
    for (let j = 0; j < 256; j++) { const c = rgb(j), d = (c[0] - t[0]) ** 2 + (c[1] - t[1]) ** 2 + (c[2] - t[2]) ** 2; if (d < bd) { bd = d; best = j; } }
    out[s] = best;
  }
  return out;
}
