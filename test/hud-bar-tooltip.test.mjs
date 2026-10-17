import test from 'node:test';
import assert from 'node:assert/strict';
import { hudBarTooltip } from '../play/assets/hud-bar-tooltip.mjs';
const s = { experience: 1234, nextExperience: 2000, stamina: 74, staminaMax: 90, fmtExp: 'Experience: %u / %u', fmtStamina: 'Stamina: %d / %d' };
test('experience bar tooltip (retail D2Client+0x26970)', () => {
  assert.deepEqual(hudBarTooltip(800, 600, 300, 560, s), { bar: 'experience', text: 'Experience: 1234 / 2000', x: 254, y: 549, color: 0 });
  assert.equal(hudBarTooltip(800, 600, 253, 560, s), null);
  assert.equal(hudBarTooltip(800, 600, 377, 566, s)?.bar, 'experience');
  assert.equal(hudBarTooltip(800, 600, 300, 567, s), null);
});
test('stamina bar tooltip (retail D2Client+0x278B0)', () => {
  assert.deepEqual(hudBarTooltip(800, 600, 320, 580, s), { bar: 'stamina', text: 'Stamina: 74 / 90', x: 324, y: 548, color: 0 });
  assert.equal(hudBarTooltip(800, 600, 272, 580, s), null);
  assert.equal(hudBarTooltip(800, 600, 375, 591, s)?.bar, 'stamina');
  assert.deepEqual(hudBarTooltip(800, 600, 320, 580, { ...s, stamina: 99 }), { bar: 'stamina', text: 'Stamina: 90 / 90', x: 324, y: 548, color: 3 });
});
