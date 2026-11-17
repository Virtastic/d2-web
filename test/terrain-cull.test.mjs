import { test } from 'node:test';
import assert from 'node:assert/strict';
import { floorSubmissionVisible } from '../play/assets/session-terrain.mjs';

test('floor horizontal cull uses the submitted point and native half-open surface bounds', () => {
  assert.equal(floorSubmissionVisible(-80, 292, 800, 600), true);
  assert.equal(floorSubmissionVisible(-112, 292, 800, 600), false);
  assert.equal(floorSubmissionVisible(848, 292, 800, 600), true);
  assert.equal(floorSubmissionVisible(880, 292, 800, 600), false);
});

test('floor vertical cull retains the measured bottom limit', () => {
  assert.equal(floorSubmissionVisible(400, 559, 800, 600), true);
  assert.equal(floorSubmissionVisible(400, 560, 800, 600), false);
  assert.equal(floorSubmissionVisible(400, -80, 800, 600), true);
  assert.equal(floorSubmissionVisible(400, -81, 800, 600), false);
});

test('the per-tile flags made on the engine side equal the pass deciding itself', async () => {
  const { floorSubmitFlags } = await import('../play/assets/session-terrain.mjs');
  const gate = (l, t, r, b, x, y) => (l <= x && x < r && t <= y && y < b ? 1 : 0);
  const tiles = [{ orientation: 0, screenX: 0, screenY: 0 }, { orientation: 0, screenX: 5000, screenY: 0 }, { orientation: 3, screenX: 0, screenY: 0 }, { orientation: 0, screenX: 100, screenY: -9000 }];
  const player = { x: 10, y: 10 };
  const f = floorSubmitFlags(tiles, player, 800, 600, 400, gate), g = floorSubmitFlags(tiles, player, 800, 600, 400, null);
  assert.deepEqual([...f], [...g]);
  assert.equal(f[2], 2);
  assert.equal(f[1], 0);
  assert.equal(f[3], 0);
});

test('the engine gate, when wired, decides the same half-open rectangle', () => {
  const gate = (l, t, r, b, x, y) => (l <= x && x < r && t <= y && y < b ? 1 : 0);
  for (const [x, y] of [[-80, 292], [-81, 292], [879, 0], [880, 0], [400, 559], [400, 560], [400, -80], [400, -81]])
    assert.equal(floorSubmissionVisible(x, y, 800, 600, gate), floorSubmissionVisible(x, y, 800, 600));
});
