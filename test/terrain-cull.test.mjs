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
