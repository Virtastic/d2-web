import test from 'node:test';
import assert from 'node:assert/strict';
import { hudOrigin, panelShift } from '../play/assets/hud-origin.mjs';
import { barParts, halfParts } from '../play/assets/wide-hud.mjs';

test('800x600 is untouched: every origin (0,0), no strips', () => {
  for (const a of ['centre', 'bottom', 'bottom-left', 'bottom-right']) assert.deepEqual(hudOrigin(800, 600, a), { scale: 1, x: 0, y: 0 });
  assert.deepEqual(barParts(800, 600), []);
});
test('D2GL rules at 1920x1080: panels from the centre, belt from the bottom, hero to W/4', () => {
  assert.deepEqual([hudOrigin(1920, 1080).x + 400, hudOrigin(1920, 1080).y + 60], [960, 300]);
  assert.equal(hudOrigin(1920, 1080, 'bottom').y, 480);
  assert.equal(hudOrigin(1920, 1080, 'bottom-right').x, 1120);
  assert.equal(panelShift(1920, false, true), -480);
  assert.equal(halfParts(1920, 1080, 'right').find((p) => p.name === 'NeoD2MRFancyBorderInterfaceRight').x, 1280);
});
