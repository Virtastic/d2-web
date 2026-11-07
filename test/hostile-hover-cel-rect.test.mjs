import test from 'node:test';
import assert from 'node:assert/strict';
import { hostileUnitHoverHit, inUnitHoverBox, selectPointerCandidate } from '../play/gameplay-model.mjs';

// Measured in a Tal Rasha tomb (level 70): a Mummy Sarcophagus (class 228, alive) drawn at anchor
// (400, 308) with its cel at x -6..48, y -92..1 anchor-relative, and a pair of Light Gauntlets on the
// ground at anchor (384, 324). The pointer on the gauntlets' lower sprite sits 16 px left of and
// 16 px below the sarcophagus anchor.
const SARCOPHAGUS_CEL = { xOffset: -6, yOffset: -92, width: 54, height: 93 };
const ON_ITEM = { dx: -16, dy: 16 };

test('a hostile does not take the pointer outside its padded cel rect', () => {
  assert.equal(inUnitHoverBox(ON_ITEM.dx, ON_ITEM.dy, true), true, 'the measured box alone reaches the item');
  assert.equal(hostileUnitHoverHit(ON_ITEM.dx, ON_ITEM.dy, SARCOPHAGUS_CEL), false,
    'retail 0xA6730: the rect is the cel padded 12 px a side, which ends 12 px below the anchor');
  // The item then wins: no unit candidate is left above it.
  const item = { id: 118, mode: 3, invPage: 255, x: 22767, y: 11115 };
  const units = hostileUnitHoverHit(ON_ITEM.dx, ON_ITEM.dy, SARCOPHAGUS_CEL) ? [{ id: 35, type: 1, hostile: true, x: 22766, y: 11113 }] : [];
  assert.equal(selectPointerCandidate({ units, itemsOnCell: [item], objectsUnderPointer: [] }, { reference: { x: 22767, y: 11113 } })?.target, item);
});

test('inside the cel rect, or with no drawn cel, the hostile keeps its box', () => {
  assert.equal(hostileUnitHoverHit(20, -40, SARCOPHAGUS_CEL), true, 'on the sarcophagus itself');
  assert.equal(hostileUnitHoverHit(-18, 12, SARCOPHAGUS_CEL), true, 'the padded corner is inside');
  assert.equal(hostileUnitHoverHit(-19, 12, SARCOPHAGUS_CEL), false, 'one pixel left of it is not');
  assert.equal(hostileUnitHoverHit(ON_ITEM.dx, ON_ITEM.dy, null), true, 'an undrawn unit falls back to the box');
  assert.equal(hostileUnitHoverHit(-30, 0, null), false, 'outside the box is never a hit');
});
