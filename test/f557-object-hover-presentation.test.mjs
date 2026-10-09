import test from 'node:test';
import assert from 'node:assert/strict';
import {
  nativeDccHoverContains,
  nativeObjectHoverContains,
  selectNativeHoverCandidate,
  selectNativeHoverWinner,
  selectPointerCandidate,
} from '../play/gameplay-model.mjs';

test('F557 single-cell object hover uses closed, padded bounds', () => {
  const stashCel = { xOffset: -25, yOffset: -30, width: 42, height: 43 };
  assert.equal(nativeDccHoverContains(29, 24, stashCel, 0, 0), true, 'native right/bottom edge is inclusive');
  assert.equal(nativeDccHoverContains(30, 24, stashCel, 0, 0), false, 'one pixel past the right edge misses');
  assert.equal(nativeObjectHoverContains(29, 24, { ...stashCel, layers: 1 }, 0, 0), true);
  assert.equal(nativeObjectHoverContains(30, 24, { ...stashCel, layers: 1 }, 0, 0), false);
});

test('F506 invalid object cel cannot create a native hover hit from the anchor fallback', () => {
  assert.equal(nativeObjectHoverContains(4, 3, null, 0, 0), false,
    'native D2Client returns no hit when it cannot build a current-cell rectangle');
  assert.equal(nativeObjectHoverContains(4, 3, { ...null, width: 0, height: 0, layers: 0 }, 0, 0), false);
});

test('F557 hover ranking selects the greater signed native score across mixed accepted candidates', () => {
  const item = { id: 7 }, object = { id: 240 }, ordinary = { id: 513 };
  assert.deepEqual(selectNativeHoverCandidate({ item, npc: ordinary, object }), { kind: 'npc', target: ordinary, type: 1 },
    'neutral unit score 0 outranks item -4 and object -6');
  assert.deepEqual(selectNativeHoverCandidate({ npc: ordinary, item }), { kind: 'npc', target: ordinary, type: 1 },
    'neutral unit score 0 outranks item -4');
});

test('F557 hover ranking retains the existing fallback when there is no better accepted candidate', () => {
  const fallback = { id: 2, type: 2 };
  assert.equal(selectNativeHoverWinner([], fallback), fallback);
  assert.equal(selectNativeHoverWinner([{ id: 3, type: 4 }], fallback).id, 3,
    'the higher-scored item displaces the lower-scored object fallback');
  const otherObject = { id: 4, type: 2 };
  assert.equal(selectNativeHoverWinner([otherObject], fallback), fallback,
    'equal score stays with the caller fallback; F247 does not identify the tie reference');
  assert.equal(selectNativeHoverWinner([{ id: 5, type: 4 }], null).id, 5,
    'a candidate is selected when no fallback exists');
  assert.equal(selectNativeHoverCandidate({ item: { id: 6 } }, fallback).target.id, 6,
    'the integrated mixed-candidate selector also applies the greater score');
});

test('F247/F557 object candidates use strict player-distance tie-breaking in the hover caller', () => {
  const player = { x: 0, y: 0 };
  const farther = { id: 267, classId: 267, x: 3, y: 4 };
  const nearer = { id: 240, classId: 240, x: 2, y: 2 };
  const tied = { id: 239, classId: 240, x: -3, y: -4 };

  assert.deepEqual(selectPointerCandidate({ units: [], itemsOnCell: [], objectsUnderPointer: [farther, nearer] }, { reference: player }),
    { kind: 'object', target: nearer }, 'a strictly nearer accepted object replaces the first object');
  assert.deepEqual(selectPointerCandidate({ units: [], itemsOnCell: [], objectsUnderPointer: [farther, tied] }, { reference: player }),
    { kind: 'object', target: farther }, 'an exact squared-distance tie keeps scan order, not the lower GUID');
});
