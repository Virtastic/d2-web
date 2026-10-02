import test from 'node:test';
import assert from 'node:assert/strict';
import { panelSides, viewCentreX } from '../play/assets/panel-view-shift.mjs';

test('NPC speech does not move the world camera into a side-panel layout', () => {
  assert.deepEqual(panelSides({ npc: true }), [false, false]);
  assert.equal(viewCentreX(800, panelSides({ npc: true })), 400);
});

test('actual side panels retain retail half-screen view shifts', () => {
  assert.deepEqual(panelSides({ character: true }), [true, false]);
  assert.equal(viewCentreX(800, panelSides({ character: true })), 600);
  assert.deepEqual(panelSides({ inventory: true }), [false, true]);
  assert.equal(viewCentreX(800, panelSides({ inventory: true })), 200);
});

test('vendor and inventory together occupy both halves', () => {
  const sides = panelSides({ vendor: true, inventory: true });
  assert.deepEqual(sides, [true, true]);
  assert.equal(viewCentreX(1280, sides), 640);
});
