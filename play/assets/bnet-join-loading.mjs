// The screen retail shows between the Battle.net lobby and the game: ui\Loading\loadingscreen.dc6 on
// black, from the CREATE GAME / JOIN GAME click until the world is ready. Its frame is the progress:
// it climbs toward the last-but-one frame while the join runs and reaches the last when the world is
// up. The state is deterministic (clock passed in); the shell draws it and hands over to the game.
export const JOIN_LOADING_TIMEOUT_MS = 180000;
const ART = 'data\\global\\ui\\Loading\\loadingscreen.dc6', PALETTE = 'data\\global\\palette\\Loading\\pal.dat';

export function createJoinLoading({ now = () => performance.now() } = {}) {
  let state = 'idle', t0 = 0;
  return {
    get state() { return state; },
    get shown() { return state === 'loading'; },
    start() { state = 'loading'; t0 = now(); },
    // The world is ready (LoadAct, LoadComplete, MapReveal): the game scene takes over.
    ready() { if (state !== 'loading') return false; state = 'idle'; return true; },
    // The join failed or the wait ran out: the lobby shows again (its own failure notice).
    fail() { if (state !== 'loading') return false; state = 'idle'; return true; },
    expired() { return state === 'loading' && now() - t0 >= JOIN_LOADING_TIMEOUT_MS; },
    progress() { return state === 'loading' ? 1 - Math.exp(-(now() - t0) / 6000) : 0; },
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
    draw(ctx, w, h, progress) {
      ctx.fillStyle = '#000'; ctx.fillRect(0, 0, w, h);
      const i = Math.min(frames.length - 1, Math.floor(progress * frames.length));
      const s = Math.min(w / 800, h / 600);
      if (frames[i]) ctx.drawImage(frames[i], Math.round((w - 800 * s) / 2 + 272 * s), Math.round((h - 600 * s) / 2 + 172 * s), 256 * s, 256 * s);
    },
  };
}
