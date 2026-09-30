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

// RETAIL'S POINTER CLOCK, measured from retail's own per-frame draw lists.
// The file is NOT chosen by the target. An unlabelled pointer runs one timed idle sequence:
//   protate  76 game frames (cels 0,1,2.. irregular: 5/38/33 on empty ground, 25/51 parked-away --
//            the per-cel split varies capture to capture, only the 76-frame total and the order hold)
//   ohand    8 cels x 4 game frames = 32
//   orotate  8 cels x 8 game frames, cycling forever
// A pointer held on a LABELLED unit (the mode-7 highlight + name plate) pins `orotate`, 8 frames/cel,
// cycling, in every frame of the capture. Hostile targets are not in that capture, so `attack` keeps
// `protate` on the rate the row declared (12), stated as unmeasured. The sequence restarts at cel 0
// when the context changes; the front end is a different cursor and does not run.
export const IDLE_SEQUENCE = Object.freeze({ protate: 76, ohand: 32 });
// protate's cel over its 76 frames: the empty-ground capture's split (frames 0-4, 5-42, 43-75).
const protateCel = (e) => (e < 5 ? 0 : e < 43 ? 1 : 2);

export function createGameCursor({ read, decodeDc6, documentObject = globalThis.document }) {
  const paletteBytes = read(GAME_CURSOR.palette);
  const palette = decodeDatPalette(paletteBytes);
  const source = (path) => {
    const bytes = read(path), dc6 = decodeDc6(bytes);
    return { path, bytesHash: `0x${fnv1a(bytes).toString(16).padStart(8, '0')}`,
      dc6, canvases: dc6.frames.map((frame) => canvasFor(frame, palette, documentObject)) };
  };
  const assets = Object.freeze({ hand: source(GAME_CURSOR.hand), attack: source(GAME_CURSOR.attack), talk: source(GAME_CURSOR.talk) });
  const fileOf = { ohand: 'hand', protate: 'attack', orotate: 'talk' };
  // The clock's own state: which cursor is drawn and the tick it became the drawn one, so the cel
  // count restarts at 0 on a change of cursor exactly as retail's does. `tick` is the engine tick --
  // one retail game frame (the shell's 25 Hz step) -- which is the unit the rates above are in.
  let kind = null, startTick = 0;
  return {
    draw(ctx, canvas, x, y, context, tick = 0) {
      const next = context?.kind === 'attack' || context?.kind === 'talk' ? context.kind : 'hand';
      // A tick below the start is a restarted session, not a cursor change: count from there.
      if (next !== kind || tick < startTick) { kind = next; startTick = tick; }
      const elapsed = Math.max(0, tick - startTick);
      let file, frameIndex, perCel;
      if (kind === 'talk') { file = 'orotate'; perCel = 8; frameIndex = Math.floor(elapsed / 8) % 8; }
      else if (kind === 'attack') { file = 'protate'; perCel = 12; frameIndex = Math.floor(elapsed / 12) % 8; }
      else if (elapsed < IDLE_SEQUENCE.protate) { file = 'protate'; perCel = null; frameIndex = protateCel(elapsed); }
      else if (elapsed < IDLE_SEQUENCE.protate + IDLE_SEQUENCE.ohand) { file = 'ohand'; perCel = 4; frameIndex = Math.floor((elapsed - IDLE_SEQUENCE.protate) / 4); }
      else { file = 'orotate'; perCel = 8; frameIndex = Math.floor((elapsed - IDLE_SEQUENCE.protate - IDLE_SEQUENCE.ohand) / 8) % 8; }
      const asset = assets[fileOf[file]];
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
        clock: { framesPerCel: perCel, since: elapsed, cels: asset.dc6.frames.length, file },
        drawn: { path: drawPath(asset.path), frame: frameIndex, palette: drawPath(GAME_CURSOR.palette), bytesHash: asset.bytesHash,
          width: frame.width, height: frame.height, x: left, y: top, blit } };
    },
    assets: Object.fromEntries(Object.entries(assets).map(([kind, asset]) => [kind, {
      path: drawPath(asset.path), bytesHash: asset.bytesHash, frames: asset.dc6.frames.length,
      palette: drawPath(GAME_CURSOR.palette), paletteBytesHash: `0x${fnv1a(paletteBytes).toString(16).padStart(8, '0')}`,
    }])),
  };
}
