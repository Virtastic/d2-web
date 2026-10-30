// Retail's in-game pointer is a Units-palette DC6 cel, not the browser cursor. The
// shell decides WHAT is under the pointer through its existing target resolvers;
// this module only maps that authoritative result to an asset and paints it last.
import { decodeDatPalette, dc6FrameToRgba } from './d2-formats.mjs';
import { drawPath, fnv1a, recordDc6 } from './drawlist.mjs';

export const GAME_CURSOR = Object.freeze({
  hand: 'data\\global\\ui\\CURSOR\\ohand.dc6',
  attack: 'data\\global\\ui\\CURSOR\\protate.dc6',
  talk: 'data\\global\\ui\\CURSOR\\orotate.dc6',
  // The icon cursors (identify, repair, buy, sell) are all buysell.dc6; ICON_CURSOR_FRAME picks the frame.
  repair: 'data\\global\\ui\\CURSOR\\buysell.dc6',
  palette: 'data\\global\\palette\\Units\\pal.dat',
});

function canvasFor(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, frame.width); canvas.height = Math.max(1, frame.height);
  if (frame.width && frame.height) canvas.getContext('2d').putImageData(
    new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0);
  return canvas;
}

// Retail's icon cursor is one table entry (buysell, not animated) whose frame is a number set when the
// mode is entered, drawn as is. The vendor buttons store theirs: Buy 3, Sell 4, Repair 1. An item spell
// takes it from the 0x3F packet, which the server fills from the item's spell icon: Books.txt SpellIcon,
// 0 for the Identify scroll and tome (Town Portal's is -1, so it never shows an icon cursor).
export const ICON_CURSOR_FRAME = Object.freeze({ identify: 0, repair: 1, buy: 3, sell: 4 });
export const REPAIR_CURSOR_FRAME = ICON_CURSOR_FRAME.repair;
export const IDENTIFY_CURSOR_FRAME = ICON_CURSOR_FRAME.identify;
// The icon cel is drawn 33 rows below the pointer anchor and never clamped; every other cursor cel is
// drawn at the anchor clamped into the screen. Neither moves the pointer itself.
export const ICON_CURSOR_DROP = 33;
const ICON_KINDS = new Set(Object.keys(ICON_CURSOR_FRAME));

/** Where a cursor frame lands for a pointer at (x, y) on a w x h screen: the anchor handed to the cel
 *  draw and the frame's top-left. The pointer's own coordinates are inputs only. */
export function cursorPlacement(kind, frame, x, y, w, h) {
  const px = Math.round(x), py = Math.round(y) - 1;
  const icon = ICON_KINDS.has(kind);
  const anchorX = icon ? px : Math.min(Math.max(px, 0), w - 1);
  const anchorY = icon ? py + ICON_CURSOR_DROP : Math.min(Math.max(py, 0), h - 1);
  return { anchorX, anchorY, left: anchorX + frame.offsetX, top: anchorY + frame.offsetY - frame.height + 1 };
}
export const NATIVE_CURSOR_IDLE_MS = 5000;

// Static 1.13c RE recovers the event timeout and table cadence. The `protate` LCG predicate is
// recovered, but the browser lacks the native player-seed stream after conditional world-render
// consumers. This model holds its first cel and exposes that limitation rather than inventing a seed
// history. Calls to next() correspond one-for-one with cursor draws.
export function createGameCursorClock({ now = () => globalThis.performance?.now?.() ?? Date.now() } = {}) {
  let lastInputMs = now(), phase = 'spin';
  let handDraws = 0, rotateDraws = 0;
  let contextKind = null, contextDraws = 0;
  return {
    noteInput() { lastInputMs = now(); phase = 'spin'; handDraws = 0; rotateDraws = 0; },
    next(context) {
      const kind = context?.kind === 'attack' || context?.kind === 'talk' || ICON_KINDS.has(context?.kind) ? context.kind : 'hand';
      if (kind !== contextKind) { contextKind = kind; contextDraws = 0; }
      const elapsedMs = Math.max(0, now() - lastInputMs);
      if (kind === 'talk') {
        const draw = { file: 'orotate', frame: Math.floor(contextDraws / 8) % 8,
          framesPerCel: 8, since: contextDraws, elapsedMs, state: 'talk' };
        contextDraws++;
        return draw;
      }
      // The icon cursor is not animated: its frame stays what entering the mode set.
      if (ICON_KINDS.has(kind)) return { file: 'buysell', frame: ICON_CURSOR_FRAME[kind], framesPerCel: null, since: contextDraws, elapsedMs, state: kind };
      if (kind === 'attack') {
        const draw = { file: 'protate', frame: 0, framesPerCel: null,
          since: contextDraws, elapsedMs, state: 'attack-rate-unresolved' };
        contextDraws++;
        return draw;
      }
      if (phase === 'spin') {
        const draw = { file: 'protate', frame: 0, framesPerCel: null,
          since: 0, elapsedMs, state: 'protate-sequence-unresolved' };
        // D2Client updates the state after drawing the current cel; the new ohand cel appears on
        // the following render call, not the one that first observes the expired timeout.
        if (elapsedMs > NATIVE_CURSOR_IDLE_MS) { phase = 'hand'; handDraws = 0; }
        return draw;
      }
      if (phase === 'hand' && handDraws >= 32) { phase = 'rotate'; rotateDraws = 0; }
      if (phase === 'hand') {
        const draw = { file: 'ohand', frame: Math.floor(handDraws / 4),
          framesPerCel: 4, since: handDraws, elapsedMs, state: 'hand-sequence' };
        handDraws++;
        return draw;
      }
      const draw = { file: 'orotate', frame: Math.floor(rotateDraws / 8) % 8,
        framesPerCel: 8, since: rotateDraws, elapsedMs, state: 'idle-rotate' };
      rotateDraws++;
      return draw;
    },
  };
}

export function createGameCursor({ read, decodeDc6, documentObject = globalThis.document, now }) {
  const paletteBytes = read(GAME_CURSOR.palette);
  const palette = decodeDatPalette(paletteBytes);
  const source = (path) => {
    const bytes = read(path), dc6 = decodeDc6(bytes);
    return { path, bytesHash: `0x${fnv1a(bytes).toString(16).padStart(8, '0')}`,
      dc6, canvases: dc6.frames.map((frame) => canvasFor(frame, palette, documentObject)) };
  };
  const assets = Object.freeze({ hand: source(GAME_CURSOR.hand), attack: source(GAME_CURSOR.attack), talk: source(GAME_CURSOR.talk), repair: source(GAME_CURSOR.repair) });
  const fileOf = { ohand: 'hand', protate: 'attack', orotate: 'talk', buysell: 'repair' };
  const clock = createGameCursorClock({ now });
  return {
    noteInput() { clock.noteInput(); },
    draw(ctx, canvas, x, y, context) {
      const timing = clock.next(context);
      const { file, frame: frameIndex, framesPerCel: perCel } = timing;
      const kind = context?.kind === 'attack' || context?.kind === 'talk' || ICON_KINDS.has(context?.kind) ? context.kind : 'hand';
      const asset = assets[fileOf[file]];
      const frame = asset.dc6.frames[frameIndex], image = asset.canvases[frameIndex];
      const { left, top } = cursorPlacement(kind, frame, x, y, canvas.width, canvas.height);
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
      // `clock` exposes the recovered timing model and labels unresolved protate timing plainly.
      return { context: kind, target: context?.target ?? null, requested, partial: clipped,
        clock: { framesPerCel: perCel, since: timing.since, elapsedMs: timing.elapsedMs,
          state: timing.state, cels: asset.dc6.frames.length, file },
        drawn: { path: drawPath(asset.path), frame: frameIndex, palette: drawPath(GAME_CURSOR.palette), bytesHash: asset.bytesHash,
          width: frame.width, height: frame.height, x: left, y: top, blit } };
    },
    assets: Object.fromEntries(Object.entries(assets).map(([kind, asset]) => [kind, {
      path: drawPath(asset.path), bytesHash: asset.bytesHash, frames: asset.dc6.frames.length,
      palette: drawPath(GAME_CURSOR.palette), paletteBytesHash: `0x${fnv1a(paletteBytes).toString(16).padStart(8, '0')}`,
    }])),
  };
}
