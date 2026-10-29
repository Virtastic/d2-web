import test from 'node:test';
import assert from 'node:assert/strict';
import { pickVariant } from '../play/assets/combat-sound-trigger.mjs';

// skill-pal2 (919e1b37cf): retail submits the group base row, so no variant is drawn. These assert the base row
// (the old "stays in group / never repeats the last two picks" and "size 2 avoids the last pick" checks were removed).
test('pickVariant returns the group base row for every group size and history', () => {
  const hist = new Map(), rng = { s: 1n };
  for (let i = 0; i < 200; i++) assert.equal(pickVariant(100, 3, hist, rng), 100);
});
test('pickVariant returns the base for size 2 and size 1 alike', () => {
  const hist = new Map(), rng = { s: 7n };
  for (let i = 0; i < 50; i++) assert.equal(pickVariant(5, 2, hist, rng), 5);
  assert.equal(pickVariant(9, 1, hist, rng), 9);
});
