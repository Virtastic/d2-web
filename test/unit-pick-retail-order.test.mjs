import test from 'node:test';
import assert from 'node:assert/strict';
import { retailPickScore, selectRetailPickTarget } from '../play/gameplay-model.mjs';

const m = (id, x, y) => ({ id, type: 1, hostile: true, hp: 10, mode: 1, x, y });
const snap = (entities) => ({ player: { x: 0, y: 0 }, entities });

test('score is floor((2*max+min)/2)', () => {
  assert.equal(retailPickScore({ x: 3, y: 1 }, { x: 0, y: 0 }), 3);
  assert.equal(retailPickScore({ x: 3, y: 2 }, { x: 0, y: 0 }), 4);
});
test('nearest to the pointer wins, not to the player; ties keep first; 10 excluded', () => {
  assert.equal(selectRetailPickTarget(snap([m(1, 1, 0), m(2, 20, 20)]), { x: 20, y: 19 }), 0 || 2);
  assert.equal(selectRetailPickTarget(snap([m(1, 2, 0), m(2, 0, 2)]), { x: 0, y: 0 }), 1);
  assert.equal(selectRetailPickTarget(snap([m(1, 10, 0)]), { x: 0, y: 0 }), 0);
});
