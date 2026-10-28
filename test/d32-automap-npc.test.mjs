import test from 'node:test';
import assert from 'node:assert/strict';
import { automapUnitScreen, automapNpcsInViewport, automapViewport } from '../play/assets/automap-overlay.mjs';

// D32: retail 0x6fb119e0 places a unit in automap space relative to the hero, never at its world-screen spot.
test('NPC marker sits on the automap lattice, hero at the origin', () => {
  const hero = { x: 100.5, y: 200.5 };
  assert.deepEqual(automapUnitScreen(400, 300, hero, 100.5, 200.5, 0.1), { sx: 400, sy: 300 });
  const p = automapUnitScreen(400, 300, hero, 110.5, 200.5, 0.1);   // 10 tiles east: +16 x, +8 y at scale .1 per tile
  assert.equal(p.sx, 416); assert.equal(p.sy, 308);
  const { viewport } = automapViewport(800, 600, { mode: 'full', left: true });
  assert.equal(automapNpcsInViewport([p], viewport).length, 1);
});
