import test from 'node:test';
import assert from 'node:assert/strict';
import { pickVariant } from '../play/assets/combat-sound-trigger.mjs';

test('pickVariant never repeats the last two picks and stays in the group', () => {
  const hist = new Map(), rng = { s: 1n }, out = [];
  for (let i = 0; i < 200; i++) out.push(pickVariant(100, 3, hist, rng));
  for (const r of out) assert.ok(r >= 100 && r < 103);
  for (let i = 2; i < out.length; i++) assert.notEqual(out[i], out[i - 1]), assert.notEqual(out[i], out[i - 2]);
});
test('size 2 avoids only the last pick; size 1 is the base', () => {
  const hist = new Map(), rng = { s: 7n }, o = [];
  for (let i = 0; i < 50; i++) o.push(pickVariant(5, 2, hist, rng));
  for (let i = 1; i < o.length; i++) assert.notEqual(o[i], o[i - 1]);
  assert.equal(pickVariant(9, 1, hist, rng), 9);
});
