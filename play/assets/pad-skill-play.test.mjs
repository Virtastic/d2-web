import test from 'node:test';
import assert from 'node:assert/strict';
import { makeSkillPlay } from './pad-skill-play.mjs';
import { readSheet } from './pad-skill-sheet.mjs';

const memory = () => { const m = new Map(); return { getItem: (k) => (m.has(k) ? m.get(k) : null), setItem: (k, v) => m.set(k, String(v)), m }; };
const recorder = () => {
  const log = [];
  return {
    log,
    io: {
      selectSkill: (id, side) => log.push(`select ${id} ${side}`),
      pointer: (type, x, y, o = {}) => log.push(`${type} ${x},${y} b${o.button}${o.shiftKey ? ' shift' : ''}`),
      fnKey: (fn) => log.push(`fn ${fn}`),
      keyHold: (fn, down) => log.push(`hold ${fn} ${down}`),
    },
  };
};
const act = (o = {}) => ({
  modifierHeld: false, main: false, mainDown: false, slot: -1, slotDown: Array(6).fill(false), interact: false,
  swapTap: false, openWheel: false, openStrip: false, wheelStep: 0, wheelTab: 0, aim: null, stripStep: 0,
  confirm: false, cancel: false, ...o,
});
const press = (i, o = {}) => { const d = Array(6).fill(false); d[i] = true; return act({ slot: i, slotDown: d, ...o }); };
const hold = (i, o = {}) => { const d = Array(6).fill(false); d[i] = true; return act({ slotDown: d, ...o }); };

// A paladin: Attack, Sacrifice (left), Holy Bolt (right), Might and Holy Fire (auras), a passive, the tome.
const choices = [
  { id: 0, leftSkill: true }, { id: 96, leftSkill: true }, { id: 101 }, { id: 98, aura: true }, { id: 102, aura: true },
  { id: 33, passive: true }, { id: 220 }, { id: 218 },
];
const host = (o = {}) => ({
  name: 'Hero', choices, selected: { left: 0, right: 101 }, hotkeys: [{ skill: 96 }, { skill: 98 }, { skill: 102 }, { skill: 218 }, { skill: 220 }],
  inTown: false, ranked: [], hero: [400, 300], ahead: [460, 300], ...o,
});
const foe = (id, x, y) => ({ kind: 'unit', id, screen: [x, y] });

test('the sheet is seeded once from the hands and hotkeys and saved per character', () => {
  const store = memory(), { io } = recorder(), play = makeSkillPlay({ io, storage: store });
  const sheet = play.sheet(host());
  assert.equal(sheet.main, 0);
  assert.deepEqual(sheet.slots, [96, 98, 102, 220, -1, 101, -1, -1, -1, -1, -1, -1]);   // identify tome left out
  assert.ok(store.getItem('d2.padskills.Hero').startsWith('initialized 1\n'));
  const again = makeSkillPlay({ io, storage: store }).sheet(host({ selected: { left: 96, right: 98 } }));
  assert.deepEqual(again, readSheet(store.getItem('d2.padskills.Hero')));
});

test('an aura button selects the aura and the second press goes back, never clicking', () => {
  const r = recorder(), play = makeSkillPlay({ io: r.io, storage: memory() });
  play.step(host(), press(1), { field: true });
  play.step(host({ selected: { left: 0, right: 98 } }), act(), { field: true });
  play.step(host({ selected: { left: 0, right: 98 } }), press(1), { field: true });
  play.step(host({ selected: { left: 0, right: 101 } }), press(2), { field: true });
  play.step(host({ selected: { left: 0, right: 102 } }), press(1), { field: true });
  assert.deepEqual(r.log, ['select 98 right', 'select 101 right', 'select 102 right', 'select 98 right']);
});

test('a held skill follows a monster and lets go when none is left; Attack on loot clicks once', () => {
  const r = recorder(), play = makeSkillPlay({ io: r.io, storage: memory() });
  const seen = [foe(7, 500, 320)];
  play.step(host({ ranked: seen }), press(0), { field: true });            // Sacrifice, left hand
  play.step(host({ ranked: [foe(7, 510, 325)] }), hold(0), { field: true });
  play.step(host({ ranked: [{ kind: 'item', id: 3, screen: [520, 330] }] }), hold(0), { field: true });
  play.step(host({ ranked: [] }), act(), { field: true });
  assert.deepEqual(r.log, [
    'select 96 left', 'pointermove 500,320 b0', 'pointerdown 500,320 b0',
    'pointermove 510,325 b0', 'pointerup 510,325 b0',
  ]);
  r.log.length = 0;
  const loot = [{ kind: 'item', id: 3, screen: [520, 330] }];
  play.step(host({ ranked: loot }), act({ main: true, mainDown: true }), { field: true });   // RT = Attack
  play.step(host({ ranked: loot }), act({ mainDown: true }), { field: true });
  assert.deepEqual(r.log, ['pointermove 520,330 b0', 'pointerdown 520,330 b0', 'pointerup 520,330 b0']);
});

test('a left skill with nothing to hit casts in place; a right skill clicks ahead; Attack does nothing', () => {
  const r = recorder(), play = makeSkillPlay({ io: r.io, storage: memory() });
  play.step(host(), press(0), { field: true });
  play.step(host({ selected: { left: 96, right: 101 } }), act(), { field: true });
  play.step(host(), press(5), { field: true });
  play.step(host(), act(), { field: true });
  play.step(host(), act({ main: true, mainDown: true }), { field: true });
  assert.deepEqual(r.log, [
    'select 96 left', 'pointermove 460,300 b0 shift', 'pointerdown 460,300 b0 shift', 'pointerup 460,300 b0 shift',
    'pointermove 460,300 b2', 'pointerdown 460,300 b2', 'pointerup 460,300 b2',
  ]);
});

test('the Town Portal tome casts at the hero outside town only', () => {
  const r = recorder(), play = makeSkillPlay({ io: r.io, storage: memory() });
  play.step(host({ inTown: true }), press(3), { field: true });
  play.step(host(), act(), { field: true });
  play.step(host(), press(3), { field: true });
  assert.deepEqual(r.log, ['select 220 right', 'pointermove 400,300 b2', 'pointerdown 400,300 b2', 'pointerup 400,300 b2']);
});

test('the wheel assigns the stick-picked skill to the pressed button and saves it', () => {
  const store = memory(), r = recorder(), play = makeSkillPlay({ io: r.io, storage: store });
  play.step(host(), act({ openWheel: true }), { field: true });
  assert.equal(play.uiMode, 'wheel');
  // the wheel's skills: 0 96 101 98 102 220 (passive and identify left out); just left of straight down is wedge 3
  play.step(host(), act({ aim: [-0.2, 0.98] }), { field: false });
  play.step(host(), press(4, { modifierHeld: true }), { field: false });
  assert.equal(play.sheet(host()).slots[10], 98);
  assert.match(store.getItem('d2.padskills.Hero'), /\na4 98\n/);
  play.step(host(), act({ cancel: true }), { field: false });
  assert.equal(play.uiMode, 'none');
  assert.deepEqual(r.log, []);
});

test('the panel list steps and opens a panel through its retail function', () => {
  const r = recorder(), play = makeSkillPlay({ io: r.io, storage: memory() });
  play.step(host(), act({ openStrip: true }), { field: true });
  play.step(host(), act({ stripStep: 1 }), { field: false });
  play.step(host(), act({ confirm: true }), { field: false });
  play.step(host(), act({ openStrip: true }), { field: true });
  play.step(host(), act({ stripStep: -1 }), { field: false });
  play.step(host(), act({ confirm: true }), { field: false });
  assert.deepEqual(r.log, ['fn inventory', 'fn escape']);
});

test('the hint names the button holding Attack and what it would do', () => {
  const play = makeSkillPlay({ io: recorder().io, storage: memory() });
  play.step(host({ ranked: [{ kind: 'npc', id: 1, screen: [1, 1] }] }), act(), { field: true });
  assert.equal(play.ui.hint, 'RT . Talk');
  play.step(host({ ranked: [{ kind: 'object', id: 1, screen: [1, 1] }] }), act({ modifierHeld: true }), { field: true });
  assert.equal(play.ui.hint, 'LT+RT . Use');
});
