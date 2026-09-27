// Retail's in-game pointer is a Units-palette DC6 cel, not the browser cursor. The
// shell decides WHAT is under the pointer through its existing target resolvers;
// this module only maps that authoritative result to an asset and paints it last.
import { decodeDatPalette, dc6FrameToRgba } from './d2-formats.mjs';
import { drawPath, fnv1a, recordDc6 } from './drawlist.mjs';

export const GAME_CURSOR = Object.freeze({
  hand: 'data\\global\\ui\\CURSOR\\ohand.dc6',
  attack: 'data\\global\\ui\\CURSOR\\protate.dc6',
  talk: 'data\\global\\ui\\CURSOR\\orotate.dc6',
  palette: 'data\\global\\palette\\Units\\pal.dat',
});

function canvasFor(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, frame.width); canvas.height = Math.max(1, frame.height);
  if (frame.width && frame.height) canvas.getContext('2d').putImageData(
    new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0);
  return canvas;
}

// RETAIL'S POINTER CLOCK, measured from retail's own multi-frame draw lists.
// A cursor left on screen advances ONE CEL PER N RETAIL GAME FRAMES -- the hook's frame numbers are
// retail's game frames (tools/drawlist.py), so these are the real rate:
//   `orotate` 8 frames/cel, over 4.7 whole cycles of a held pointer (f51-warriv-series-stand, 300
//     consecutive frames: 1,2,3,4,5,6,7 repeating every 64 frames; f97-catacombs-door-open confirms);
//   `ohand`   4 frames/cel, all eight cels in a row in three independent captures (f7-hover-long2,
//     f7-hover-long3, f7-away-long, each 0,1,2,3,4,5,6,7 at exactly 4 frames each).
// The count starts at cel 0 the moment that cursor becomes the one drawn: three observed changes of
// cursor (protate->ohand, ohand->orotate, and the head of each capture) all begin at cel 0. The front
// end is a different cursor and does NOT run: `ohand` sits on cel 0 for >1,000 consecutive retail
// frames across a dozen captures (create-selected-necromancer alone is 120).
// `protate` advances too, but its per-cel runs in one held-pointer scene are 6, 7, 18, 20, 22, 24,
// 32, 35, 40, 42, 44, 47 game frames -- NOT a fixed rate, so it is keyed to a state the corpus does
// not name. Its rate is therefore not recovered; it keeps the clock the row declared.
const CEL_GAME_FRAMES = Object.freeze({ hand: 4, attack: 12, talk: 8 });

export function createGameCursor({ read, decodeDc6, documentObject = globalThis.document }) {
  const paletteBytes = read(GAME_CURSOR.palette);
  const palette = decodeDatPalette(paletteBytes);
  const source = (path) => {
    const bytes = read(path), dc6 = decodeDc6(bytes);
    return { path, bytesHash: `0x${fnv1a(bytes).toString(16).padStart(8, '0')}`,
      dc6, canvases: dc6.frames.map((frame) => canvasFor(frame, palette, documentObject)) };
  };
  const assets = Object.freeze({ hand: source(GAME_CURSOR.hand), attack: source(GAME_CURSOR.attack), talk: source(GAME_CURSOR.talk) });
  // The clock's own state: which cursor is drawn and the tick it became the drawn one, so the cel
  // count restarts at 0 on a change of cursor exactly as retail's does. `tick` is the engine tick --
  // one retail game frame (the shell's 25 Hz step) -- which is the unit the rates above are in.
  let kind = null, startTick = 0;
  return {
    draw(ctx, canvas, x, y, context, tick = 0) {
      const next = context?.kind === 'attack' || context?.kind === 'talk' ? context.kind : 'hand';
      // A tick below the start is a restarted session, not a cursor change: count from there.
      if (next !== kind || tick < startTick) { kind = next; startTick = tick; }
      const asset = assets[kind];
      const frames = asset.dc6.frames.length;
      const elapsed = Math.max(0, tick - startTick);
      const frameIndex = frames > 1 ? Math.floor(elapsed / CEL_GAME_FRAMES[kind]) % frames : 0;
      const frame = asset.dc6.frames[frameIndex], image = asset.canvases[frameIndex];
      const left = Math.round(x) + frame.offsetX;
      const top = Math.round(y) - 1 + frame.offsetY - frame.height + 1;
      const requested = { path: drawPath(asset.path), frame: frameIndex, palette: drawPath(GAME_CURSOR.palette) };
      // Retail clips a cel at the viewport and paints the part that fits; nothing is suppressed here.
      // (The corner the frame fixtures park on draws a one-column sliver, and retail's own capture of
      // those scenes HAS that sliver -- see the row's `edge` half. V4-rework first "fixed" this by not
      // drawing a partly-inside cel at all, which cost nothing but was not what retail does and hid
      // the real cause: the fixtures were comparing against a capture whose pointer was parked while
      // ours was wherever the last click left it.)
      const clipped = left < 0 || top < 0 || left + frame.width > canvas.width || top + frame.height > canvas.height;
      ctx.drawImage(image, left, top);
      const blit = recordDc6(asset.path, frameIndex, frame, left, top);
      // `clock` is the measured rate the cel was chosen by and `since` the ticks this cursor has been
      // the drawn one -- the row asserts the cel advances 1 per `clock` frames, and that the cel drawn
      // is the cel retail records for that index (blit.hash against the corpus's own hash).
      return { context: kind, target: context?.target ?? null, requested, partial: clipped,
        clock: { framesPerCel: CEL_GAME_FRAMES[kind], since: elapsed, cels: frames },
        drawn: { path: drawPath(asset.path), frame: frameIndex, palette: drawPath(GAME_CURSOR.palette), bytesHash: asset.bytesHash,
          width: frame.width, height: frame.height, x: left, y: top, blit } };
    },
    assets: Object.fromEntries(Object.entries(assets).map(([kind, asset]) => [kind, {
      path: drawPath(asset.path), bytesHash: asset.bytesHash, frames: asset.dc6.frames.length,
      palette: drawPath(GAME_CURSOR.palette), paletteBytesHash: `0x${fnv1a(paletteBytes).toString(16).padStart(8, '0')}`,
    }])),
  };
}
