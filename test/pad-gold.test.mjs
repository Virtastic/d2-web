import test from 'node:test';
import assert from 'node:assert/strict';
import { goldInReach } from '../play/assets/pad-targeting.mjs';
import { DEFAULTS } from '../play/assets/pad-settings.mjs';

const snap = (items) => ({ player: { x: 10, y: 10 }, items });
const gold = (id, x, y, extra = {}) => ({ id, x, y, code: 'gld', mode: 3, invPage: 255, ...extra });

test('gold pickup is off by default', () => assert.equal(DEFAULTS.autoGold, false));
test('the nearest ground gold within reach is picked', () => {
  assert.equal(goldInReach(snap([gold(1, 12, 12), gold(2, 11, 10)])).id, 2);
});
test('the engine snapshot record (packed code, mode 5 while dropping) is gold on the ground', () => {
  const real = { id: 9, x: 11, y: 10, code: 543452263, mode: 5, invPage: 255, goldAmount: 7 };
  assert.equal(goldInReach(snap([real])).id, 9);
});
test('gold out of reach, other items and gold not on the ground are ignored', () => {
  assert.equal(goldInReach(snap([gold(1, 20, 20), { ...gold(2, 10, 11), code: 'hp1' }, gold(3, 10, 10, { mode: 0 })])), null);
  assert.equal(goldInReach({ items: [gold(1, 0, 0)] }), null);
});
