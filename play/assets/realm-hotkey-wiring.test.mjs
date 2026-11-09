import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmInput } from './realm-input.mjs';
import { realmFrameReads } from './realm-client-view.mjs';

// The server's hotkey table (0x7B) is applied to the realm view and an F-key sends the skill with the owner the server gave.
test('a realm skill selection carries the owner the server gave the hotkey', () => {
  const out = [];
  const input = createRealmInput({ transmit: (b) => { out.push(Array.from(b)); return true; }, now: () => 0 });
  assert.equal(input.selectSkill(36, true, 0x12345678), true);
  assert.deepEqual(out.at(-1), [0x3c, 36, 0, 0, 0x80, 0x78, 0x56, 0x34, 0x12]);
});

test('the realm hotkey view shows the server table, eight slots, unbound when absent', () => {
  const bound = [{ skill: 36, left: true }, { skill: -1, left: false }];
  const reads = realmFrameReads(null, null, null, 0, null, null, bound);
  assert.deepEqual(reads.hotkeys.slice(0, 2), bound);
  assert.equal(reads.hotkeys.length, 8);
  assert.deepEqual(realmFrameReads(null, null, null, 0, null, null).hotkeys[0], { skill: -1, left: false });
});
