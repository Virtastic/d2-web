import test from 'node:test';
import assert from 'node:assert/strict';
import { BELT_FALLBACK, beltBoxes } from '../play/assets/belt-renderer.mjs';

// This test used to pin { x: 342, y: 561, cells: 4, cell: 31 } -- numbers typed into the renderer
// with no source, which put the potions 81px left of the slots the panel art draws for them.
// Pinning them made the defect permanent and called it coverage. The real layout is Belts.txt,
// read through retail's DATATBLS_GetInvRectFromBeltsTxt and handed over by the engine, so what is
// worth asserting here is that the renderer USES what it is given and that the boot-time fallback
// matches retail's own 800x600 numbers.

test('the fallback belt matches retail Belts.txt at 800x600', () => {
  // From d2_session_belt_boxes(2, 1, ...): 29x29 boxes, first column at x=423, bottom row y=562.
  assert.deepEqual(BELT_FALLBACK, { x: 423, y: 562, cells: 4, cell: 29, pitch: 31 });
});

test('the engine boxes are used verbatim when supplied', () => {
  const given = [
    { left: 10, right: 39, top: 100, bottom: 129 },
    { left: 41, right: 70, top: 100, bottom: 129 },
  ];
  assert.equal(beltBoxes(given), given);
});

test('the fallback is a real four-column row when the engine has nothing', () => {
  const boxes = beltBoxes(null);
  assert.equal(boxes.length, 4);
  assert.deepEqual(boxes.map((b) => b.right - b.left), [29, 29, 29, 29]);
  assert.deepEqual(boxes.map((b) => b.left), [423, 454, 485, 516]);
  assert.ok(boxes.every((b) => b.bottom - b.top === 29));
});
