import test from 'node:test';
import assert from 'node:assert/strict';
import { blockSound } from '../play/assets/combat-sound-trigger.mjs';
test('block mode entry maps to block_weapon sounds', () => {
  assert.equal(blockSound(0, 9), 0); // D10: the player block is silent
  assert.equal(blockSound(1, 6), 398);
  assert.equal(blockSound(0, 6), 0);
  assert.equal(blockSound(1, 9), 0);
});
