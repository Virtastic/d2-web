import test from 'node:test';
import assert from 'node:assert/strict';
import { rightGroundWalks } from './world-click-dispatch.mjs';

test('a right click on ground walks for a hand-to-hand skill, not for a cast, not under Stand Still', () => {
  assert.equal(rightGroundWalks(2, false), true);   // Attack, Kick, Bash: the engine plans a walk
  assert.equal(rightGroundWalks(1, false), false);  // used at the spot
  assert.equal(rightGroundWalks(-1, false), false); // no skill or no player
  assert.equal(rightGroundWalks(2, true), false);   // Stand Still held
});
