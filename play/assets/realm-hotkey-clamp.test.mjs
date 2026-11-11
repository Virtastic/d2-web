import test from 'node:test';
import assert from 'node:assert/strict';
import { bindEngineProtocol } from './realm-client.mjs';

// A fake engine whose skills table holds `count` rows. Retail's 0x7B handler stores -1 for a skill above that count.
function engine(count) {
  const Module = { HEAPU8: new Uint8Array(1 << 16), _malloc: () => 16, _free: () => {},
    ccall: (name) => { if (name === 'd2_realm_skill_count') return count; throw new Error(name); } };
  if (count !== undefined) Module._d2_realm_skill_count = () => count;
  return bindEngineProtocol(Module);
}
const packet = (skill, left = false) => Uint8Array.from([0x7b, 3, skill & 0xff, (skill >> 8) | (left ? 0x80 : 0), 9, 0, 0, 0]);

test('a hotkey skill above the table row count stores -1; the count itself is kept (retail compares >)', () => {
  const p = engine(357);
  p.d2gsApplyHotkey(packet(356)); assert.equal(p.d2gsHotkeys()[3].skill, 356);
  p.d2gsApplyHotkey(packet(357)); assert.equal(p.d2gsHotkeys()[3].skill, 357);
  p.d2gsApplyHotkey(packet(358, true)); assert.deepEqual(p.d2gsHotkeys()[3], { skill: -1, left: true, owner: 9 });
});

test('without the export nothing clamps', () => {
  const p = engine(undefined);
  p.d2gsApplyHotkey(packet(0xfff)); assert.equal(p.d2gsHotkeys()[3].skill, 0xfff);
});
