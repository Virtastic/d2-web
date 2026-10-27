import test from 'node:test';
import assert from 'node:assert/strict';
import { selectHoverUnit } from '../play/gameplay-model.mjs';

test('overlapping hostile and NPC: click winner is the one nearer the player, not monsters-first', () => {
  const player = { x: 0, y: 0 };
  const hostile = { id: 1, type: 1, hostile: true, x: 6, y: 0, mode: 1 };
  const npc = { id: 2, type: 1, npc: true, x: 2, y: 0, mode: 1 };
  assert.equal(selectHoverUnit([hostile, npc], player), npc);
  assert.equal(selectHoverUnit([npc, hostile], player), npc);
  assert.equal(selectHoverUnit([hostile, { ...hostile, id: 3, x: 1 }], player).id, 3);
  assert.equal(selectHoverUnit([{ ...hostile, mode: 12 }, npc], { x: 9, y: 0 }), npc, 'corpse (-8) loses');
});
