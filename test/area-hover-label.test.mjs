import test from 'node:test';
import assert from 'node:assert/strict';
import { areaEntranceLabel, warpAtPoint } from '../play/assets/area-hover-label.mjs';

// A level-entrance warp: origin (100, 100) projects to (400, 300); LvlWarp's screen select box is
// the inclusive rectangle x 400-24 .. 400+24, y 300-40 .. 300+16.
const warp = { slot: 2, destinationLevel: 2, warpId: 7, enabled: true, located: true, x: 101, y: 99,
  originX: 100, originY: 100, select: { x: -24, y: -40, dx: 48, dy: 56 } };
const project = (wx, wy) => [wx * 4, wy * 3];

test('a level-entrance warp hit resolves to the destination level name label', () => {
  const hit = warpAtPoint([warp], 400, 300, project, 1);
  assert.equal(hit, warp);
  assert.deepEqual(areaEntranceLabel(hit, 'Blood Moor'), {
    kind: 'warp', id: 2, warpId: 7, destinationLevel: 2, label: 'Blood Moor', x: 101, y: 99 });
});

test('inclusive select-box edges hit; one pixel outside misses on every edge', () => {
  for (const [x, y] of [[376, 300], [424, 300], [400, 260], [400, 316]])
    assert.equal(warpAtPoint([warp], x, y, project, 1), warp, `${x},${y} edge`);
  for (const [x, y] of [[375, 300], [425, 300], [400, 259], [400, 317]])
    assert.equal(warpAtPoint([warp], x, y, project, 1), null, `${x},${y} outside`);
});

test('disabled, unlocated and same-level warps are not entrances; no name means no label', () => {
  for (const bad of [{ ...warp, enabled: false }, { ...warp, located: false }])
    assert.equal(warpAtPoint([bad], 400, 300, project, 1), null);
  assert.equal(warpAtPoint([warp], 400, 300, project, 2), null);
  assert.equal(areaEntranceLabel(warp, ''), null);
  assert.equal(areaEntranceLabel(null, 'Blood Moor'), null);
});

test('the smallest select box wins when two warps overlap', () => {
  const big = { ...warp, slot: 1, select: { x: -100, y: -100, dx: 200, dy: 200 } };
  assert.equal(warpAtPoint([big, warp], 400, 300, project, 1), warp);
});
