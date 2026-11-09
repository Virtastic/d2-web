// The screen retail shows between the Battle.net lobby and the game: ui\Loading\loadingscreen.dc6 on
// black, from the CREATE GAME / JOIN GAME click until the world is ready. The cel drawn is retail's
// loading frame: the frame index, clamped to the last frame of the DC6, and the index goes up by one per
// step. A step is one call of that frame with 1, made by the room pump on its gated passes (all but one
// pass in 32). step() is that call. Until the engine reports its pump passes, the shell steps once per
// update tick while the join runs (GUESS: retail's walk calls the frame once per list entry but the first of
// every 32; step(rooms) takes that count from the engine, step() with no count is the one-per-tick placeholder).
// The bar under the cel (game modes 6-9 only; a realm join is mode 3, so the shell draws none) is retail's: outline, black well, and a fill whose width the
// character-file upload moves in 250 ms sweeps. The rule is in the engine (d2_client_loadbar_*); this file
// only holds the clock. A realm join runs no upload (the character stays server-side).
// JOIN_LOADING_TIMEOUT_MS is a browser guard counted from the join click. Retail's own load timer (0x435D0)
// gives up a load after the same 180000 ms from a start tick it keeps itself; the two start points are not yet
// shown to be the same tick, and retail's exit reason for that path is not recovered.
export const JOIN_LOADING_TIMEOUT_MS = 180000;
import { PL2_ALPHA } from './frontend.mjs';
const ART = 'data\\global\\ui\\Loading\\loadingscreen.dc6', PALETTE = 'data\\global\\palette\\Loading\\pal.dat', PL2 = 'data\\global\\palette\\Loading\\Pal.PL2';

const SWEEP_MS = 250;   // the Sleep(250) after each sweep step
// wasm: { width(a, total, k), chunks(size), finalWidth(chunks), walkCalls(rooms), rects(mode, w, h, suppress, width) } over the d2_client_loadbar_* exports
// mode: the game mode retail's multiplayer module stores for a Battle.net realm join (3, the sequence join through the realm client); the launcher stores nothing for the realm path. The bar and the character upload run only in modes 6-9 (TCP/IP host or address join), so a realm join shows the cel alone.
export function createJoinLoading({ now = () => performance.now(), wasm = null, mode = 3 } = {}) {
  let state = 'idle', t0 = 0, steps = 0, timeline = [], finalFill = 0;
  return {
    get state() { return state; },
    get shown() { return state === 'loading'; },
    // the loading frame's index before the clamp: the cel loader resets it to 0
    get steps() { return steps; },
    // saveBytes: the size of the character file an upload would send (retail sends it in 255-byte packets and sweeps the fill per packet)
    start({ saveBytes = 0 } = {}) {
      state = 'loading'; t0 = now(); steps = 0; timeline = []; finalFill = 0;
      if (wasm && saveBytes > 0) {
        const n = wasm.chunks(saveBytes), at = (a, total) => { for (let k = 0; k < 8; k++) timeline.push(wasm.width(a, total, k)); };
        at(0, 0);   // the opening sweep: eight steps at width 0
        for (let i = 0; i < n; i++) at(i, n);
        finalFill = wasm.finalWidth(n);
      }
    },
    // the fill word at this moment
    get fill() { if (state !== 'loading' || !timeline.length) return 0; const i = Math.floor((now() - t0) / SWEEP_MS); return i < timeline.length ? timeline[i] : finalFill; },
    // the bar's rectangles in the 800x600 art space: [x0, y0, x1, y1, colour (0 border, 1 black, 2 fill), draw mode] per rectangle
    bar(w = 800, h = 600) {
      if (!wasm) return [];
      const r = wasm.rects(mode, w, h, 0, this.fill), out = [];
      for (let i = 0; i < r.length; i += 6) out.push(r.slice(i, i + 6));
      return out;
    },
    // The world is ready (LoadAct, LoadComplete, MapReveal): the game scene takes over.
    ready() { if (state !== 'loading') return false; state = 'idle'; return true; },
    // The join failed or the wait ran out: the lobby shows again (its own failure notice).
    fail() { if (state !== 'loading') return false; state = 'idle'; return true; },
    expired() { return state === 'loading' && now() - t0 >= JOIN_LOADING_TIMEOUT_MS; },
    // one call of retail's loading frame with 1 (the frame index goes up); a no-op outside the loading screen
    step(rooms) { if (state === 'loading') steps += rooms === undefined || !wasm ? 1 : wasm.walkCalls(rooms); },
  };
}

export function createJoinLoadingArt({ read, decodeDc6, decodeDatPalette, documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(PALETTE));
  let alpha = null;   // AlphaBlend[0][src] for src = 0..255: a mode-1 rectangle over the black screen
  try { const pl2 = read(PL2); if (pl2 && pl2.length >= PL2_ALPHA + 65536) alpha = pl2.subarray(PL2_ALPHA, PL2_ALPHA + 256); } catch { /* no PL2: the outline falls back to the plain colour */ }
  const frames = decodeDc6(read(ART)).frames.map((f) => {
    const c = documentObject.createElement('canvas');
    c.width = Math.max(1, f.width); c.height = Math.max(1, f.height);
    const rgba = new Uint8ClampedArray(f.width * f.height * 4);
    for (let i = 0; i < f.indices.length; i++) if (f.opaque[i]) rgba.set(palette.subarray(f.indices[i] * 4, f.indices[i] * 4 + 4), i * 4);
    c.getContext('2d').putImageData(new ImageData(rgba, c.width, c.height), 0, 0);
    return c;
  });
  return {
    // steps: the loading frame's index before the clamp (joinLoading.steps); the cel is the clamped frame
    draw(ctx, w, h, steps, bar = []) {
      ctx.fillStyle = '#000'; ctx.fillRect(0, 0, w, h);
      const sc = Math.min(w / 800, h / 600), ox = (w - 800 * sc) / 2, oy = (h - 600 * sc) / 2;
      // bar colours: palette indices of (0xFF,0xFF,0), black, (0x60,0,0) by nearest RGB (retail's lookup, recovered: squared RGB distance over the 256 entries, strict less-than so the first minimum wins; the argument order is the palette's byte order, which this array's RGB order is assumed to match)
      const near = (r, g, b) => { let best = 0, bd = Infinity; for (let i = 0; i < 256; i++) { const d = (palette[i * 4] - r) ** 2 + (palette[i * 4 + 1] - g) ** 2 + (palette[i * 4 + 2] - b) ** 2; if (d < bd) { bd = d; best = i; } } return best; };
      const cols = [near(255, 255, 0), 0, near(0x60, 0, 0)];
      // Draw mode of each rectangle (the rule is the active driver's rectangle routine, the DirectDraw one): 5 stores the colour index; 1 stores the PL2 AlphaBlend table entry [under][colour] (its second table, the 50% one), under being the index already on the screen (black, 0, here).
      for (const [x0, y0, x1, y1, c, mode] of bar) { const q = (mode === 1 && alpha ? alpha[cols[c]] : cols[c]) * 4; ctx.fillStyle = `rgb(${palette[q]},${palette[q + 1]},${palette[q + 2]})`; ctx.fillRect(Math.round(ox + x0 * sc), Math.round(oy + y0 * sc), Math.round((x1 - x0) * sc), Math.round((y1 - y0) * sc)); }
      const i = Math.min(frames.length - 1, Math.max(0, Math.floor(steps)));
      const s = Math.min(w / 800, h / 600);
      if (frames[i]) ctx.drawImage(frames[i], Math.round((w - 800 * s) / 2 + 272 * s), Math.round((h - 600 * s) / 2 + 172 * s), 256 * s, 256 * s);
    },
  };
}
