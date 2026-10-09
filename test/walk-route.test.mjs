import test from 'node:test';
import assert from 'node:assert/strict';
import { nearestStandableNear } from '../play/assets/walk-route.mjs';

test('NPC approach fallback chooses a standable aim inside the native 50-subtile command gate', () => {
  const walkable = new Set(['50,0', '60,0']);
  const aim = nearestStandableNear({ x: 55, y: 0 }, 5,
    (x, y) => walkable.has(`${x},${y}`), { x: 0, y: 0 });
  assert.deepEqual(aim, { x: 50, y: 0 }, 'the farther standable candidate is outside the native command range');
});

test('NPC approach fallback returns no aim when every in-range cell fails its arrival tolerance', () => {
  const walkable = new Set(['60,0']);
  assert.equal(nearestStandableNear({ x: 55, y: 0 }, 5,
    (x, y) => walkable.has(`${x},${y}`), { x: 0, y: 0 }), null,
  'an out-of-range aim is not sent to the engine');
});
