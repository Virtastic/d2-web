import { test } from 'node:test';
import assert from 'node:assert/strict';
import { createRealmWorld } from '../play/assets/realm-world.mjs';

const entry = { classId: 1, flippyFile: 'f', invFile: 'i', invWidth: 1, invHeight: 1, kind: 'misc' };
const world = () => createRealmWorld({ items: { byCode: () => entry }, objects: { byClass: () => null } });
// A compact item on the ground: flags (compact), version, mode 3, x, y, code.
function itemBits() {
  const bits = []; const put = (v, n) => { for (let i = 0; i < n; i++) bits.push((v >> i) & 1); };
  put(0x200000, 32); put(0, 10); put(3, 3); put(5, 16); put(6, 16); put(0x20202020, 32);
  const out = new Uint8Array(Math.ceil(bits.length / 8) + 2);
  bits.forEach((b, i) => { out[i >> 3] |= b << (i & 7); });
  return out;
}
function packet(op, action, guid) {
  const body = itemBits(), head = op === 0x9d ? 13 : 8;
  const p = new Uint8Array(head + body.length);
  p[0] = op; p[1] = action; p[2] = p.length; new DataView(p.buffer).setUint32(4, guid, true);
  p.set(body, head); return p;
}
const sizeOf = (b) => b.length;

test('item packets with an action the handler does not accept change nothing', () => {
  const w = world(); w.setOwner(0);
  w.feed(packet(0x9c, 6, 1), sizeOf);     // 6 belongs to the owner packet, fatal in 0x9c
  w.feed(packet(0x9d, 0, 2), sizeOf);     // 0 belongs to the plain packet, fatal in 0x9d
  assert.equal(w.ownItemCount, 0);
  w.feed(packet(0x9c, 0, 1), sizeOf);
  assert.equal(w.ownItemCount, 1);
});
