import test from 'node:test';
import assert from 'node:assert/strict';
import { CONTROL_LAYOUT, resourceRatio } from '../play/assets/control-panel.mjs';

test('life and mana fill ratios clamp authoritative snapshot values', () => {
  assert.deepEqual([resourceRatio(50, 100), resourceRatio(-2, 100), resourceRatio(200, 100), resourceRatio(4, 0)], [.5, 0, 1, 0]);
});

test('the console is centred between the orbs, which is where retail puts it', () => {
  // Not a pinned magic number: derive the offsets from 800ctrlpnl7's own frame widths. The orbs
  // are 117, the console's pieces 128/128/128/86, so the console is 470 in a 566 gap and sits 48
  // in from each orb. Cycle 110 read those margins as seams and filled them with frame 6, which is
  // 39% opaque and put holes in the bar instead.
  const orb = 117, console_ = 128 + 128 + 128 + 86, screen = 800;
  const between = screen - 2 * orb;
  assert.equal(CONTROL_LAYOUT.centerX, orb + (between - console_) / 2);
  assert.equal(CONTROL_LAYOUT.manaX, screen - orb);
  assert.equal(CONTROL_LAYOUT.healthX, 0);
  assert.equal(CONTROL_LAYOUT.height, 104);
});
