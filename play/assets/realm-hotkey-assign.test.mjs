import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmInput } from './realm-input.mjs';
import { routeRealmPackets } from './realm-packet-routes.mjs';

const rig = () => {
  const out = [];
  const input = createRealmInput({ transmit: (b) => { out.push(Array.from(b)); return true; }, now: () => 0 });
  return { out, input };
};
const OWNER = 0xffffffff;

test('a fresh F-key assign sends one bind packet', () => {
  const { out, input } = rig();
  assert.equal(input.assignHotkey(2, 36, true, OWNER, []), true);
  assert.deepEqual(out, [[0x51, 36, 0x80, 2, 0, 0xff, 0xff, 0xff, 0xff]]);
});

test('re-assigning a bound slot unbinds it first, and the same skill elsewhere is cleared after the bind', () => {
  const { out, input } = rig();
  const table = Array.from({ length: 16 }, () => ({ skill: -1, left: false, owner: OWNER }));
  table[1] = { skill: 40, left: false, owner: OWNER };
  table[5] = { skill: 36, left: true, owner: OWNER };
  input.assignHotkey(1, 36, false, OWNER, table);
  assert.deepEqual(out, [
    [0x51, 0xff, 0xff, 1, 0, 0xff, 0xff, 0xff, 0xff],   // unbind slot 1
    [0x51, 36, 0, 1, 0, 0xff, 0xff, 0xff, 0xff],        // bind slot 1 to 36, right hand
    [0x51, 0xff, 0xff, 5, 0, 0xff, 0xff, 0xff, 0xff],   // clear the duplicate in slot 5
  ]);
});

test('a server 0x7B frame reaches the hotkey applier with its bytes intact', () => {
  const got = [];
  const frame = Uint8Array.of(0x7b, 3, 36, 0x80, 0x78, 0x56, 0x34, 0x12);
  routeRealmPackets(frame, () => 8, { applyHotkey: (p) => got.push(Array.from(p)) });
  assert.deepEqual(got, [Array.from(frame)]);
});

test('bad slot or skill sends nothing', () => {
  const { out, input } = rig();
  assert.equal(input.assignHotkey(16, 36, true, OWNER, []), false);
  assert.equal(input.assignHotkey(0, -1, true, OWNER, []), false);
  assert.deepEqual(out, []);
});
