import test from 'node:test';
import assert from 'node:assert/strict';
import { experienceBarRatio } from '../play/assets/control-panel.mjs';
test('xp bar spans the current level, empties on level-up', () => {
  assert.equal(experienceBarRatio(499, 500, 0), 499 / 500);
  assert.equal(experienceBarRatio(532, 1500, 500), 32 / 1000);
  assert.equal(experienceBarRatio(1500, 3750, 1500), 0);
  assert.equal(experienceBarRatio(5, 0, 0), 0);
});
