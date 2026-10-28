import test from 'node:test';
import assert from 'node:assert/strict';
import { automapOrigin, automapCellTopY } from '../play/assets/automap-overlay.mjs';
import { viewCentreX } from '../play/assets/panel-view-shift.mjs';

test('automap origin is the hero screen point, with and without a side panel', () => {
  const none = { x: 0, y: 0 };
  assert.deepEqual(automapOrigin(800, 600, none), { x: 400, y: 292 });
  const hx = viewCentreX(800, [true, false]);
  assert.notEqual(hx, 400);
  assert.deepEqual(automapOrigin(800, 600, none, hx), { x: Math.round(hx), y: 292 });
});

// No panel: the hero marker origin does not depend on level type; only the cell top row does (f5b outdoor AY=-29, Den s2120/s2121 cave AY=1).
test('no panel: marker origin and cell rows per level type match the retail-measured offsets', () => {
  const o = automapOrigin(800, 600, { x: 0, y: 0 });
  assert.deepEqual([o.x + 8, o.y - 7], [408, 285]);               // retail hero glyph centre, every level type
  for (const [name, cave, ay] of [['town', false, -29], ['outdoor', false, -29], ['cave', true, 1]])
    assert.equal(automapCellTopY(o.y, 8942, 8942, 0.1, cave), o.y + ay, name);   // a cell on the hero's own world row
});
