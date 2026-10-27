import test from 'node:test';
import assert from 'node:assert/strict';
import { createSkillSelectRenderer, SKILL_SELECT_LAYOUT as L } from '../play/assets/skill-select.mjs';

test('skill button hover sets and clears the hand (no stale plate); an empty skill list clears too', () => {
  const r = createSkillSelectRenderer(() => new Uint8Array(768), {});
  const o = { x: L.left.x + 2, y: L.left.y + 2 };
  // 800x600 canvas: the left hand's HUD origin is the canvas origin (scale 1).
  r.hover(o.x, o.y, 800, 600, []);
  assert.equal(r.hoverSide, 'left');
  r.hover(1, 1, 800, 600, []);
  assert.equal(r.hoverSide, null);
});
