import test from 'node:test';
import assert from 'node:assert/strict';
import { createGameCursor, cursorPlacement, ICON_CURSOR_FRAME, ICON_CURSOR_DROP } from '../play/assets/game-cursor.mjs';

// Frame geometry of the retail cursor cels (width x height, offsetX, offsetY), frame 0 of each file.
const GEOMETRY = {
  ohand: { width: 32, height: 26, offsetX: -1, offsetY: 24, frames: 8 },
  protate: { width: 30, height: 27, offsetX: -1, offsetY: 24, frames: 8 },
  orotate: { width: 31, height: 30, offsetX: -2, offsetY: 24, frames: 8 },
  buysell: { width: 32, height: 40, offsetX: 0, offsetY: 0, frames: 10 },
};

function fakeCursor() {
  globalThis.ImageData ??= class { constructor(data, width, height) { Object.assign(this, { data, width, height }); } };
  const documentObject = { createElement: () => ({ width: 0, height: 0, getContext: () => ({ putImageData() {} }) }) };
  const read = (path) => (path.endsWith('pal.dat') ? new Uint8Array(768) : new TextEncoder().encode(path));
  const decodeDc6 = (bytes) => {
    const name = new TextDecoder().decode(bytes).replace(/^.*\\/, '').replace(/\.dc6$/, '');
    const g = GEOMETRY[name];
    return { frames: Array.from({ length: g.frames }, () => ({ ...g, flipped: 0,
      indices: new Uint8Array(g.width * g.height), opaque: new Uint8Array(g.width * g.height) })) };
  };
  let time = 0;
  return createGameCursor({ read, decodeDc6, documentObject, now: () => time });
}

function screen() {
  const draws = [];
  return { draws, canvas: { width: 800, height: 600, style: {} },
    ctx: { drawImage: (image, x, y) => draws.push({ x, y }) } };
}

test('icon cursors use the frame the mode sets: identify 0, repair 1, buy 3, sell 4', () => {
  assert.deepEqual({ ...ICON_CURSOR_FRAME }, { identify: 0, repair: 1, buy: 3, sell: 4 });
  const cursor = fakeCursor();
  for (const [kind, frame] of Object.entries(ICON_CURSOR_FRAME)) {
    const { ctx, canvas } = screen();
    const drawn = cursor.draw(ctx, canvas, 400, 300, { kind });
    assert.match(drawn.drawn.path, /buysell$/i, kind);
    assert.equal(drawn.drawn.frame, frame, kind);
    assert.equal(drawn.context, kind);
  }
});

test('each mode draws at a fixed position for a pointer at (400, 300)', () => {
  const cursor = fakeCursor();
  const cases = [
    // kind, file, frame, left, top
    ['hand', /protate/, 0, 399, 297],      // anchor (400, 299); top = 299 + 24 - 27 + 1
    ['attack', /protate/, 0, 399, 297],
    ['talk', /orotate/, 0, 398, 294],      // 299 + 24 - 30 + 1
    ['identify', /buysell/, 0, 400, 293],  // anchor (400, 299 + 33); top = 332 + 0 - 40 + 1
    ['repair', /buysell/, 1, 400, 293],
    ['buy', /buysell/, 3, 400, 293],
    ['sell', /buysell/, 4, 400, 293],
  ];
  for (const [kind, file, frame, left, top] of cases) {
    const { ctx, canvas, draws } = screen();
    const drawn = cursor.draw(ctx, canvas, 400, 300, { kind });
    assert.match(drawn.drawn.path, file, kind);
    assert.equal(drawn.drawn.frame, frame, kind);
    assert.deepEqual({ x: drawn.drawn.x, y: drawn.drawn.y }, { x: left, y: top }, kind);
    assert.deepEqual(draws, [{ x: left, y: top }], kind);
    // The draw record carries the anchor handed to the cel draw.
    const anchorY = kind in ICON_CURSOR_FRAME ? 299 + ICON_CURSOR_DROP : 299;
    assert.deepEqual({ x: drawn.drawn.blit.x, y: drawn.drawn.blit.y }, { x: 400, y: anchorY }, kind);
  }
});

test('entering and leaving a mode never moves the pointer and keeps the icon at the same offset', () => {
  const cursor = fakeCursor();
  const pointer = Object.freeze({ x: 123.4, y: 456.6 });
  const seq = ['hand', 'identify', 'hand', 'repair', 'hand', 'buy', 'sell', 'hand'];
  const tops = seq.map((kind) => {
    const { ctx, canvas } = screen();
    const drawn = cursor.draw(ctx, canvas, pointer.x, pointer.y, { kind });
    return { kind, x: drawn.drawn.x, y: drawn.drawn.y };
  });
  assert.deepEqual(pointer, { x: 123.4, y: 456.6 });
  const icon = tops.filter((t) => t.kind in ICON_CURSOR_FRAME);
  assert.ok(icon.every((t) => t.x === 123 && t.y === 457 - 1 + ICON_CURSOR_DROP - 40 + 1));
  const hand = tops.filter((t) => t.kind === 'hand');
  assert.ok(hand.every((t) => t.x === hand[0].x && t.y === hand[0].y));
});

test('cel cursors clamp their anchor into the screen; the icon cursor is never clamped', () => {
  const f = GEOMETRY.ohand;
  assert.deepEqual(cursorPlacement('hand', f, -5, 0, 800, 600), { anchorX: 0, anchorY: 0, left: -1, top: -1 });
  assert.deepEqual(cursorPlacement('hand', f, 900, 700, 800, 600), { anchorX: 799, anchorY: 599, left: 798, top: 598 });
  const b = GEOMETRY.buysell;
  assert.deepEqual(cursorPlacement('identify', b, 799, 599, 800, 600), { anchorX: 799, anchorY: 631, left: 799, top: 592 });
  assert.deepEqual(cursorPlacement('identify', b, -5, 0, 800, 600), { anchorX: -5, anchorY: 32, left: -5, top: -7 });
});
