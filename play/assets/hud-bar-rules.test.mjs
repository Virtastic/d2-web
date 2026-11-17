// The control-panel bar rules (hud_bar_rules.h; D2Client+0x26970 / 0x27BD0 / 0x27A90 / 0x278B0). The expected numbers were
// produced by compiling the header natively; the stub binds the same arithmetic for tests without a wasm build.
import test from 'node:test';
import assert from 'node:assert/strict';
import { bindStubRules } from './client-rules-stub.mjs';
import { expBarPixels, orbFill, staminaBar } from './client-rules.mjs';

test('experience bar pixels', () => {
  bindStubRules();
  assert.equal(expBarPixels(5, 10), 59);
  assert.equal(expBarPixels(11, 10), 0);          // past the next level draws nothing, not a full bar
  assert.equal(expBarPixels(40000000, 80000000), 59);   // >= 0x226B902 scales both by 7 bits
  assert.equal(expBarPixels(0, 0), 0);
});

test('orb fill rows', () => {
  bindStubRules();
  assert.equal(orbFill(1000, 2560, 0), 31);
  assert.equal(orbFill(1, 2560, 0), 0);
  assert.equal(orbFill(50, 2560, 0, true), 2);   // a living player's non-empty life orb shows two rows at least
  assert.equal(orbFill(3000, 2560, 1), 80);       // mana clamps to its maximum
  assert.equal(orbFill(0, 0, 0), 0);
});

test('stamina bar', () => {
  bindStubRules();
  assert.deepEqual(staminaBar(256, 2560), { width: 10, slot: 0 });
  assert.deepEqual(staminaBar(2000, 2560), { width: 79, slot: 1 });
  assert.deepEqual(staminaBar(3000, 2560), { width: 102, slot: 2 });
  assert.deepEqual(staminaBar(5, 0), { width: 0, slot: 0 });
});
