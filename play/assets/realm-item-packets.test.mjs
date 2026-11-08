import test from 'node:test';
import assert from 'node:assert/strict';
import { itemPackets as p, BUTTON } from './realm-item-packets.mjs';
test('stash, cube and belt packets have the 1.13c client sizes and fields', () => {
  assert.deepEqual([...p.button(BUTTON.TRANSMUTE)], [0x4f, 24, 0, 0, 0, 0, 0]);
  assert.deepEqual([...p.button(BUTTON.CLOSE_STASH)], [0x4f, 18, 0, 0, 0, 0, 0]);
  assert.deepEqual([...p.stashButton(BUTTON.DEPOSIT_GOLD, 0x12345)], [0x4f, 20, 0, 1, 0, 0x45, 0x23]);
  assert.deepEqual([...p.shiftToBelt(0x01020304)], [0x63, 4, 3, 2, 1]);
  assert.equal(p.toBelt(1, 2).length, 9);
  assert.equal(p.fromBelt(1).length, 5);
});
