import test from 'node:test';
import assert from 'node:assert/strict';
import { hoverLife, plateBoxes, platePaintOps, paintHoverPlate } from '../play/assets/hover-lifebar.mjs';
const m = (o) => ({ type: 1, mode: 1, flags: 0, ...o });
const boxes = (u) => plateBoxes(100, 18, 88, 16, hoverLife(u));
// Units/pal.dat's index 5 is the plate's red, index 0 black (reference/riiablo palettes; ACT1 and
// Sky agree).
const palette = new Uint8Array(1024); palette[5 * 4] = 92;
test('full hp: single colour-5 box', () => assert.deepEqual(boxes(m({ hpPct: 100 })), [[100, 18, 188, 34, 5, 1]]));
test('half hp: red then dark remainder', () => assert.deepEqual(boxes(m({ hpPct: 50 })),
  [[100, 18, 144, 34, 5, 1], [144, 18, 188, 34, 0, 2]]));
test('hp 0 falls back to stat/maxhp, clamps to 2', () => {
  assert.equal(hoverLife(m({ hpPct: 0, hp: 50, maxHp: 100 })), 64);
  assert.equal(hoverLife(m({ hpPct: 0, hp: 0, maxHp: 100 })), 2);
  assert.equal(hoverLife(m({ hpPct: 0 })), 128);
});
test('npc, dead, death-mode, flagged: life 0, single dark box', () => {
  for (const u of [{ type: 2, hpPct: 100 }, m({ mode: 0, hpPct: 100 }), m({ mode: 0x0C, hpPct: 100 }), m({ flags: 0x10000, hpPct: 100 })])
    assert.deepEqual(boxes(u), [[100, 18, 188, 34, 0, 2]]);
});
test('single-player stat units (life<<8): hp 128 of 256 -> 64', () => assert.equal(hoverLife(m({ hpPct: 0, hp: 128, maxHp: 256 })), 64));

// The bar is the plate's own paint, not just its record: a full-life plate is one red rectangle and a
// damaged one is red up to lifeW then black -- the two must not paint the same rectangle.
test('plate paint ops: index 5 mode 1 -> pal(92,0,0) at 50%, index 0 mode 2 -> black at 75%', () => {
  assert.deepEqual(platePaintOps([[100, 18, 144, 34, 5, 1], [144, 18, 188, 34, 0, 2]], palette)
    .map((p) => [p.x, p.y, p.width, p.height, p.colour, p.mode, p.style]),
    [[100, 18, 44, 16, 5, 1, 'rgba(92,0,0,0.5)'], [144, 18, 44, 16, 0, 2, 'rgba(0,0,0,0.75)']]);
});
test('paintHoverPlate fills every plate box and records it once', () => {
  const calls = [], records = [];
  const c = { save: () => calls.push(['save']), restore: () => calls.push(['restore']),
    set fillStyle(value) { calls.push(['fillStyle', value]); }, fillRect: (...a) => calls.push(['fillRect', ...a]) };
  const painted = paintHoverPlate(c, [[100, 18, 144, 34, 5, 1], [144, 18, 188, 34, 0, 2]], palette,
    (...a) => records.push(a));
  assert.deepEqual(calls, [['save'], ['fillStyle', 'rgba(92,0,0,0.5)'], ['fillRect', 100, 18, 44, 16],
    ['fillStyle', 'rgba(0,0,0,0.75)'], ['fillRect', 144, 18, 44, 16], ['restore']]);
  assert.deepEqual(records, [['box', 100, 18, 144, 34, 5, 1], ['box', 144, 18, 188, 34, 0, 2]]);
  assert.equal(painted.length, 2);
});
test('a full-life plate paints one rectangle and a life-0 plate paints a black one', () => {
  const paint = (u) => paintHoverPlate({ save() {}, restore() {}, fillStyle: '', fillRect: () => {} },
    boxes(u), palette, () => {}).map((p) => p.style);
  assert.deepEqual(paint(m({ hpPct: 100 })), ['rgba(92,0,0,0.5)']);
  assert.deepEqual(paint(m({ hpPct: 100, mode: 0 })), ['rgba(0,0,0,0.75)']);   // dead monster: life 0, no red
  // Negative control: the two must differ, so a painter that ignores life cannot pass.
  assert.notDeepEqual(paint(m({ hpPct: 100 })), paint(m({ hpPct: 50 })));
  assert.deepEqual(paint(m({ hpPct: 50 })), ['rgba(92,0,0,0.5)', 'rgba(0,0,0,0.75)']);
});
