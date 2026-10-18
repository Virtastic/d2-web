import test from 'node:test';
import assert from 'node:assert/strict';
import { unitLightAlpha } from '../play/assets/session-terrain.mjs';

// D2Client+0x9F120 lights a unit from the light map cell under it. The pure mapping is the cell's
// own 0..255 intensity as a brightness fraction, clamped: 0 outside every light (the palette's
// level-0 row, black), 1 at full daylight. Without this the unit pass paints every monster, object,
// item and the player at full brightness and a dark area shows its whole population.
test('unit light alpha: the cell is the brightness, clamped to 0..1', () => {
  assert.equal(unitLightAlpha(0), 0);            // outside the player's radius: black
  assert.equal(unitLightAlpha(255), 1);          // full daylight
  assert.equal(unitLightAlpha(128), 128 / 255);  // half lit
  assert.equal(unitLightAlpha(-7), 0);           // clamped, never negative
  assert.equal(unitLightAlpha(999), 1);          // clamped, never > 1
});
