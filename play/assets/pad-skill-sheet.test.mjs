import { test } from 'node:test';
import assert from 'node:assert/strict';
import {
  EMPTY, UNSET, INTERACT, initialSheet, slotOf, triggerOf, blankSheet, setSlot, stableWedge, wedgeCenter, nextPage,
  makeHoldSet, makeTriggerHold, auraChoice, writeSheet, readSheet, makeSheetStore, assignableName, portalName, bindsAction, panelFn,
} from './pad-skill-sheet.mjs';

test('the first sheet puts the left hand on RT, the right hand on RB and the hotkeys on A B X Y LB, then the second set', () => {
  const s = initialSheet(0, 36, [36, 39, 40, 39, 42, 43, 44, 45, 46, 47, 48, 49, 51, 52]);
  assert.equal(s.main, 0);
  assert.equal(s.altMain, UNSET);
  assert.deepEqual(s.slots, [39, 40, 42, 43, 44, 36, 45, 46, 47, 48, 49, 51]);
});

test('a disabled button is skipped by the seed and stays empty', () => {
  const enabled = [true, false, true, true, true, false, false];
  const s = initialSheet(0, 36, [39, 40, 41], enabled);
  assert.equal(s.main, EMPTY);
  assert.deepEqual(s.slots.slice(0, 6), [39, EMPTY, 40, 41, EMPTY, EMPTY]);
});

test('an unsaved LT+RT is RT; a saved clear is a clear', () => {
  const s = blankSheet();
  s.main = 7;
  assert.equal(triggerOf(s, true), 7);
  s.altMain = EMPTY;
  assert.equal(triggerOf(s, true), EMPTY);
  assert.equal(triggerOf(s, false), 7);
});

test('the save text round-trips, and a missing amain still means RT', () => {
  const s = initialSheet(0, 36, [39, 40]);
  setSlot(s, true, 3, INTERACT);
  const text = writeSheet(s);
  assert.equal(text.split('\n')[0], 'initialized 1');
  assert.ok(!text.includes('amain'));
  assert.deepEqual(readSheet(text), s);
  assert.deepEqual(readSheet('main 5\nbogus\ns9 3\ns2 x\na1 -4\n'), { main: 5, altMain: UNSET, slots: [-1, -1, -1, -1, -1, -1, -1, -4, -1, -1, -1, -1] });
});

test('a stick at a wedge centre picks that wedge; a resting thumb keeps the highlight', () => {
  for (let i = 0; i < 8; i++) {
    const a = wedgeCenter(i, 8);
    assert.equal(stableWedge(Math.cos(a), Math.sin(a), 8, -1), i);
  }
  assert.equal(stableWedge(0.1, 0.1, 8, 5), 5);
  // 0.05 rad past the edge of wedge 2 stays on 2; 0.2 rad past moves on.
  const edge = wedgeCenter(2, 8) + Math.PI / 8;
  assert.equal(stableWedge(Math.cos(edge + 0.05), Math.sin(edge + 0.05), 8, 2), 2);
  assert.equal(stableWedge(Math.cos(edge + 0.2), Math.sin(edge + 0.2), 8, 2), 3);
});

test('the d-pad pages keep the place on the page, wrap, and clamp on a short last page', () => {
  assert.equal(nextPage(3, 20, 1), 11);
  assert.equal(nextPage(11, 20, 1), 19);
  assert.equal(nextPage(3, 20, -1), 19);
  assert.equal(nextPage(19, 20, 1), 3);
  assert.equal(nextPage(5, 5, 1), 4);
  assert.equal(nextPage(0, 0, 1), 0);
});

test('a held button keeps the set it was pressed in', () => {
  const hold = makeHoldSet();
  assert.equal(hold(2, true, true, false), 0);
  assert.equal(hold(2, false, true, true), 0);
  assert.equal(hold(2, false, false, true), -1);
  assert.equal(hold(2, true, true, true), 1);
  const rt = makeTriggerHold();
  assert.deepEqual([rt(true, true, true), rt(false, true, false), rt(false, false, false)], [1, 1, -1]);
});

test('an aura press switches, and the same aura again returns to the remembered skill', () => {
  assert.deepEqual(auraChoice(0, 98, 36, false, 0), { next: 98, remembered: 36 });
  assert.deepEqual(auraChoice(98, 99, 98, true, 36), { next: 99, remembered: 36 });
  assert.deepEqual(auraChoice(99, 99, 99, true, 36), { next: 36, remembered: 36 });
});

test('names: Kick and the identify entries are not offered; the three portal names are the tome', () => {
  assert.equal(assignableName('KICK'), false);
  assert.equal(assignableName('Book of Identify'), false);
  assert.equal(assignableName('Dragon Talon'), true);
  assert.ok(portalName('Book of Townportal') && portalName('Town Portal') && !portalName('Teleport'));
  assert.ok(bindsAction(INTERACT) && bindsAction(-8) && !bindsAction(EMPTY) && !bindsAction(5));
  assert.equal(panelFn(-5), 'inventory');
});

test('the store seeds a character once and keeps each sheet under its own key', () => {
  const backing = new Map();
  const storage = { getItem: (k) => (backing.has(k) ? backing.get(k) : null), setItem: (k, v) => backing.set(k, v) };
  let seeds = 0;
  const store = makeSheetStore(storage, 'd2.padskills');
  const a = store.ensure('Ama', () => { seeds++; return initialSheet(0, 6, [7]); });
  store.ensure('Ama', () => { seeds++; return blankSheet(); });
  assert.equal(seeds, 1);
  assert.equal(slotOf(a, false, 0), 7);
  assert.deepEqual([...backing.keys()], ['d2.padskills.Ama']);
  const again = makeSheetStore(storage, 'd2.padskills');
  assert.deepEqual(again.get('Ama'), a);
  assert.equal(again.get('Sor'), null);
});
