import test from 'node:test';
import assert from 'node:assert/strict';
import { nativeHoverSpecialGate, selectNativeHoverWinner, selectPointerCandidate, selectUnitTypePickCandidate, unitTypePickPriority } from '../play/gameplay-model.mjs';

test('F14: greater signed type priority wins, independent of candidate distance', () => {
  const unit = { id: 1, type: 1, x: 0, y: 0 };
  const item = { id: 2, type: 4, x: 1, y: 0 };
  const object = { id: 3, type: 2, x: 100, y: 0 };

  assert.equal(unitTypePickPriority(2), -6);
  assert.equal(unitTypePickPriority(4), -4);
  assert.equal(unitTypePickPriority(1), 0);
  // Greater signed score must replace a lower-scored incumbent whichever one arrives first.
  for (const [lower, higher] of [[object, item], [item, unit], [object, unit]]) {
    assert.equal(selectUnitTypePickCandidate([lower, higher]), higher);
    assert.equal(selectUnitTypePickCandidate([higher, lower]), higher);
  }
  assert.equal(selectUnitTypePickCandidate([object, item, unit]), unit);
  assert.equal(selectUnitTypePickCandidate([object, item], {
    accepted: (candidate) => candidate.id !== object.id,
  }), item, 'only candidates accepted by the caller enter priority arbitration');
});

test('F14: diagnostic selection without a player reference preserves encounter order', () => {
  const first = { id: 40, type: 1, x: 3, y: 4 };
  const second = { id: 7, type: 1, x: 0, y: 0, preciseX: 0.25, preciseY: 0.25 };

  assert.equal(selectUnitTypePickCandidate([first, second]), first,
    'without the native player reference, the selector keeps its stable fallback');
  assert.equal(selectUnitTypePickCandidate([second, first]), second,
    'reversing encounter order reverses the stable winner');
});

test('F14: equal signed priorities use squared path distance to the player and keep exact ties stable', () => {
  const reference = { x: 0, y: 0 };
  const farther = { id: 50, type: 1, x: 3, y: 4 };
  const closer = { id: 51, type: 1, x: 0, y: 4 };

  assert.equal(selectUnitTypePickCandidate([farther, closer], { reference }), closer,
    'a lower squared path distance replaces an equal-priority incumbent');
  assert.equal(selectUnitTypePickCandidate([farther, { ...farther, id: 52 }], { reference }), farther,
    'exact distance ties keep the first accepted candidate');

  const selected = selectPointerCandidate({ units: [farther, closer] }, { reference });
  assert.equal(selected.target.id, closer.id,
    'the runtime hover selector forwards the explicit player-path reference');
  const clickedObject = { id: 54, type: 2 };
  assert.equal(selectPointerCandidate({ object: clickedObject }, { click: true, reference }).target, clickedObject,
    'a lone click candidate keeps the pre-existing click policy');
  // Retail resolves a click through the same hover resolver (D2Client 0x51E80 type-priority walk):
  // with overlapping lists the unit (priority 0) outranks the object (-6), tie broken by path distance.
  assert.equal(selectPointerCandidate({ object: clickedObject, units: [farther, closer] }, { click: true, reference }).target,
    closer, "overlapping click candidates follow the hover resolver's priority walk");
  assert.equal(selectNativeHoverWinner([farther, closer], null, reference), closer,
    'the standalone picker model follows the same path-distance branch');
});

test('F14: only explicit #10648 and #10494(0x8f) results affect type-1 candidates', () => {
  const ordinary = { id: 1, type: 1, x: 0, y: 0 };
  const object = { id: 2, type: 2, x: 50, y: 0 };
  const forced = { ...ordinary, id: 3, ordinal10648: true };
  const skipped = { ...ordinary, id: 4, ordinal10494_8f: true };

  assert.deepEqual(nativeHoverSpecialGate(ordinary), [
    { status: 'unresolved', ordinal: 10648 },
    { status: 'unresolved', ordinal: 10494, argument: 0x8f },
  ]);
  assert.deepEqual(nativeHoverSpecialGate(forced), [
    { status: 'resolved', ordinal: 10648, priority: -8 },
    { status: 'unresolved', ordinal: 10494, argument: 0x8f },
  ]);
  assert.equal(selectUnitTypePickCandidate([object, ordinary]), ordinary,
    'an unknown #10648 result does not get inferred from mode or state');
  assert.equal(selectUnitTypePickCandidate([object, forced]), object,
    'a confirmed #10648 result supplies -8, which ranks below object -6');
  assert.deepEqual(nativeHoverSpecialGate(skipped), [
    { status: 'unresolved', ordinal: 10648 },
    { status: 'skip', ordinal: 10494, argument: 0x8f },
  ]);
  assert.equal(selectUnitTypePickCandidate([skipped, object]), object,
    'a confirmed state-query result skips that scan candidate');
});

test('F14: object #11020 stays explicitly unresolved while accepted object candidates use the table', () => {
  const object = { id: 21, type: 2, x: 10, y: 0 };
  const item = { id: 22, type: 4, x: 0, y: 0 };
  assert.deepEqual(nativeHoverSpecialGate(object), [{
    status: 'unresolved', ordinal: 11020, additionalClientCheck: true,
  }]);
  assert.equal(selectUnitTypePickCandidate([item, object]), item,
    'the higher item score wins without inferring the native object gate result');
});

test('F14: hover arbitration spans accepted hit candidates and leaves the nearby-item fallback separate', () => {
  const candidates = {
    units: [
      { id: 31, npc: true, type: 1, x: 0, y: 0 },
      { id: 32, hostile: true, type: 1, x: 1, y: 0 },
    ],
    itemsOnCell: [{ id: 33, type: 4, x: 0, y: 0 }],
    objectsUnderPointer: [{ id: 34, type: 2, x: 10, y: 0 }],
    item: { id: 35, type: 4, x: 2, y: 0 },
  };
  const picked = selectPointerCandidate(candidates);
  assert.equal(picked.kind, 'npc');
  assert.equal(picked.target.id, 31, 'neutral type 1 (0) outranks item (-4) and object (-6)');

  const onlyFallback = selectPointerCandidate({ units: [], itemsOnCell: [], objectsUnderPointer: [], item: candidates.item });
  assert.deepEqual(onlyFallback, { kind: 'item', target: candidates.item },
    'the pickup-radius item remains a fallback and does not enter the hit-candidate ranking');
});

test('hover pick skips missiles (D2Client FUN_6fb01e80 type 3)', async () => {
  const { selectUnitTypePickCandidate } = await import('../play/gameplay-model.mjs');
  const m = { unitType: 3, id: 1, x: 0, y: 0 }, mon = { unitType: 1, id: 2, x: 5, y: 5 };
  assert.equal(selectUnitTypePickCandidate([m, mon]), mon);
  assert.equal(selectUnitTypePickCandidate([m]), null);
});

test('F14: a click in a crowd ties by squared distance to the player like hover (FUN_6FB01E80/6FB010B0)', () => {
  const reference = { x: 0, y: 0 };
  const far = { id: 60, x: 9, y: 9, npc: true };
  const near = { id: 61, x: 1, y: 1, npc: true };
  assert.equal(selectPointerCandidate({ units: [far, near], itemsOnCell: [], objectsUnderPointer: [], itemOnCell: null }, { click: true, reference }).target.id, 61);
  assert.equal(selectPointerCandidate({ units: [near, far], itemsOnCell: [], objectsUnderPointer: [], itemOnCell: null }, { click: true, reference }).target.id, 61);
});

test('engine: click unit pick among box hits is nearest the player, not the cursor', async () => {
  const { selectPrimaryTarget } = await import('../play/gameplay-model.mjs');
  const mk = (id, x, y) => ({ id, type: 1, x, y, hp: 10, hpMax: 10, hostile: true, mode: 1 });
  const snap = { player: { x: 0, y: 0 }, entities: [mk(9, 10, 0), mk(8, 4, 0)] };
  const box = () => true;
  assert.equal(selectPrimaryTarget(snap, { x: 10, y: 0 }, Infinity, box), 9);
  assert.equal(selectPrimaryTarget(snap, snap.player, Infinity, box), 8);
});
