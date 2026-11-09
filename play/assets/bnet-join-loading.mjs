// The screen retail shows between the Battle.net lobby and the game: ui\Loading\loadingscreen.dc6 on
// black, from the CREATE GAME / JOIN GAME click until the world is ready. The cel drawn is retail's
// loading frame: the frame index, clamped to the last frame of the DC6, and the index goes up by one per
// step. A step is one call of that frame with 1, made by the room pump on its gated passes (all but one
// pass in 32). step() is that call. Until the engine reports its pump passes, the shell steps once per
// update tick while the join runs.
// JOIN_LOADING_TIMEOUT_MS is a browser guard counted from the join click. Retail's own load timer (0x435D0)
// gives up a load after the same 180000 ms from a start tick it keeps itself; the two start points are not yet
// shown to be the same tick, and retail's exit reason for that path is not recovered.
export const JOIN_LOADING_TIMEOUT_MS = 180000;
const ART = 'data\\global\\ui\\Loading\\loadingscreen.dc6', PALETTE = 'data\\global\\palette\\Loading\\pal.dat';

export function createJoinLoading({ now = () => performance.now() } = {}) {
  let state = 'idle', t0 = 0, steps = 0;
  return {
    get state() { return state; },
    get shown() { return state === 'loading'; },
    // the loading frame's index before the clamp: the cel loader resets it to 0
    get steps() { return steps; },
    start() { state = 'loading'; t0 = now(); steps = 0; },
    // The world is ready (LoadAct, LoadComplete, MapReveal): the game scene takes over.
    ready() { if (state !== 'loading') return false; state = 'idle'; return true; },
    // The join failed or the wait ran out: the lobby shows again (its own failure notice).
    fail() { if (state !== 'loading') return false; state = 'idle'; return true; },
    expired() { return state === 'loading' && now() - t0 >= JOIN_LOADING_TIMEOUT_MS; },
    // one call of retail's loading frame with 1 (the frame index goes up); a no-op outside the loading screen
    step() { if (state === 'loading') steps++; },
  };
}

export function createJoinLoadingArt({ read, decodeDc6, decodeDatPalette, documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(PALETTE));
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
    draw(ctx, w, h, steps) {
      ctx.fillStyle = '#000'; ctx.fillRect(0, 0, w, h);
      const i = Math.min(frames.length - 1, Math.max(0, Math.floor(steps)));
      const s = Math.min(w / 800, h / 600);
      if (frames[i]) ctx.drawImage(frames[i], Math.round((w - 800 * s) / 2 + 272 * s), Math.round((h - 600 * s) / 2 + 172 * s), 256 * s, 256 * s);
    },
  };
}
