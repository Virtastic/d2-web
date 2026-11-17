import test from 'node:test';
import assert from 'node:assert/strict';
import { bindStubRules } from './client-rules-stub.mjs';
import { orbDrawList } from './client-rules.mjs';
import { replayOrbs, CELL_FLUID, CELL_OVERLAP, DRAW_CEL, DRAW_CELEX } from './draw-list.mjs';

test('a fixed orb state gives the ported draw order: life fluid, life overlap, mana fluid, mana overlap', () => {
  bindStubRules();
  const list = orbDrawList(1000 * 256 / 256, 2560, 1280, 2560, true, CELL_FLUID, CELL_OVERLAP);
  assert.deepEqual(list.map((r) => [r.rva, r.cell, r.frame, r.x, r.y, r.rows]), [
    [DRAW_CELEX, CELL_FLUID, 0, 29, 587, 31],
    [DRAW_CEL, CELL_OVERLAP, 0, 28, 595, 5],
    [DRAW_CELEX, CELL_FLUID, 1, 689, 587, 40],
    [DRAW_CEL, CELL_OVERLAP, 1, 690, 591, 5],
  ]);
});

test('an empty life orb draws only its overlap, and no maximum draws nothing', () => {
  bindStubRules();
  assert.deepEqual(orbDrawList(0, 2560, 0, 0, true, 1, 2).map((r) => r.rva), [DRAW_CEL]);
  assert.equal(orbDrawList(5, 0, 5, 0, true, 1, 2).length, 0);
});

test('replay puts the bottom rows at the cel bottom and moves each orb out by the edge', () => {
  bindStubRules();
  const calls = [], rec = [];
  const ctx = { drawImage: (...a) => calls.push(a) };
  const img = (h) => ({ height: h, width: 10 });
  const cels = { [CELL_FLUID]: [{ image: img(80), path: 'f', frame: 0 }, { image: img(80), path: 'f', frame: 1 }],
    [CELL_OVERLAP]: [{ image: img(88), path: 'o', frame: 0 }, { image: img(88), path: 'o', frame: 1 }] };
  replayOrbs(ctx, orbDrawList(1000, 2560, 1280, 2560, true, CELL_FLUID, CELL_OVERLAP), cels, 100, (...a) => rec.push(a));
  assert.deepEqual(rec.map((r) => [r[0], r[1], r[3], r[4], r[5]?.extra]), [
    ['f', 0, -71, 508, 31], ['o', 0, -72, 508, undefined], ['f', 1, 789, 508, 40], ['o', 1, 790, 504, undefined]]);
  assert.deepEqual(calls[0], [cels[CELL_FLUID][0].image, 0, 49, 10, 31, -71, 12 + 49, 10, 31]);
});
