import test from 'node:test';
import assert from 'node:assert/strict';
import { automapMarkerInViewport, automapScrollOffset, automapViewport } from '../play/assets/automap-overlay.mjs';
import { AUTOMAP_DEFAULTS } from '../play/assets/game-menu.mjs';

test('F234 native startup default is AutoMapLeft enabled, without adding a menu row', () => {
  assert.equal(AUTOMAP_DEFAULTS.left, 'yes');
  assert.deepEqual(Object.keys(AUTOMAP_DEFAULTS), ['mode', 'fade', 'center', 'party', 'names', 'left']);
});

test('F234 full map keeps the whole-screen viewport while AutoMapLeft selects its native anchor', () => {
  assert.deepEqual(automapViewport(800, 600, { mode: 'full', left: true }), {
    anchorX: 96, anchorY: 0,
    viewport: { left: -16, top: -16, right: 800, bottom: 600 },
    cellClip: null,
  });
  assert.deepEqual(automapViewport(800, 600, { mode: 'full', left: false }), {
    anchorX: 533, anchorY: 78,
    viewport: { left: -16, top: -16, right: 800, bottom: 600 },
    cellClip: null,
  });
});

test('F234 mini map uses one-third viewport and the matching left/right cell clip', () => {
  assert.deepEqual(automapViewport(800, 600, { mode: 'mini', left: true }), {
    anchorX: 96, anchorY: 0,
    viewport: { left: 88, top: 0, right: 362, bottom: 200 },
    cellClip: { left: 0, top: 75, right: 279, bottom: 300 },
  });
  assert.deepEqual(automapViewport(800, 600, { mode: 'mini', left: false }), {
    anchorX: 533, anchorY: 78,
    viewport: { left: 525, top: 78, right: 799, bottom: 278 },
    cellClip: { left: 519, top: 57, right: 798, bottom: 282 },
  });
});

test('F234 keeps the unresolved selector 0x6fbcc284 explicit in its alternative mini viewport', () => {
  assert.deepEqual(automapViewport(800, 600, { mode: 'mini', left: true, selector284: 2 }), {
    anchorX: 0, anchorY: 0,
    viewport: { left: -8, top: 0, right: 266, bottom: 200 },
    cellClip: { left: 0, top: -21, right: 279, bottom: 204 },
  });
});

test('F234 native mini viewport predicate is inclusive and separate from the cell clip', () => {
  const { viewport, cellClip } = automapViewport(800, 600, { mode: 'mini', left: true });
  assert.equal(automapMarkerInViewport(88, 0, viewport), true);
  assert.equal(automapMarkerInViewport(87, 0, viewport), false);
  assert.equal(automapMarkerInViewport(362, 100, viewport), true);
  assert.equal(automapMarkerInViewport(363, 100, viewport), false);
  assert.equal(automapMarkerInViewport(200, 200, viewport), true);
  assert.equal(automapMarkerInViewport(200, 201, viewport), false);
  // The native unit viewport extends beyond the cell clip; using the latter would cull this marker.
  assert.equal(350 >= cellClip.left && 350 <= cellClip.right, false);
  assert.equal(automapMarkerInViewport(350, 100, viewport), true);
});

test('F234 native full-map viewport predicate retains the inclusive near-screen bounds', () => {
  const { viewport } = automapViewport(800, 600, { mode: 'full', left: true });
  assert.equal(automapMarkerInViewport(-16, -16, viewport), true);
  assert.equal(automapMarkerInViewport(800, 600, viewport), true);
  assert.equal(automapMarkerInViewport(799, 599, viewport), true);
  assert.equal(automapMarkerInViewport(801, 300, viewport), false);
  assert.equal(automapMarkerInViewport(300, 601, viewport), false);
  assert.equal(automapMarkerInViewport(-17, 0, viewport), false);
});

test('F234 mini Center applies the native scroll offset only to the mini map', () => {
  assert.deepEqual(automapScrollOffset(800, 600, { mode: 'mini', center: 'yes', anchorX: 96, anchorY: 0 }),
    { x: 154, y: 184 });
  assert.deepEqual(automapScrollOffset(800, 600, { mode: 'mini', center: 'yes', anchorX: 533, anchorY: 78 }),
    { x: -283, y: 106 });
  assert.deepEqual(automapScrollOffset(800, 600, { mode: 'mini', center: 'no', anchorX: 96, anchorY: 0 }),
    { x: 0, y: 0 });
  assert.deepEqual(automapScrollOffset(800, 600, { mode: 'full', center: 'yes', anchorX: 96, anchorY: 0 }),
    { x: 0, y: 0 });
});
