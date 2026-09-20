import test from 'node:test';
import assert from 'node:assert/strict';
import { CONTROL_LAYOUT, resourceRatio } from '../play/assets/control-panel.mjs';

test('life and mana fill ratios clamp authoritative snapshot values', () => {
  assert.deepEqual([resourceRatio(50, 100), resourceRatio(-2, 100), resourceRatio(200, 100), resourceRatio(4, 0)], [.5, 0, 1, 0]);
});

test('800-pixel retail control layout preserves orb anchors and panel extent', () => {
  assert.deepEqual(CONTROL_LAYOUT, { healthX: 0, centerX: 165, manaX: 683, orbY: 0, height: 104 });
});
