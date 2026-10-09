import { test } from 'node:test';
import assert from 'node:assert/strict';
import { creditsExitFrameIndex } from '../play/assets/credits-screen.mjs';

test('Credits Exit selects its native held cel only while pressed', () => {
  const frames = [{ name: 'idle' }, { name: 'held' }];
  assert.equal(creditsExitFrameIndex(frames, false), 0);
  assert.equal(creditsExitFrameIndex(frames, true), 1);
  assert.equal(creditsExitFrameIndex(frames, false), 0);
  assert.equal(creditsExitFrameIndex([frames[0]], true), 0);
});
