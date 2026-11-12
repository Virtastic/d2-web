import { test } from 'node:test';
import assert from 'node:assert/strict';
import { makeTargetHold } from '../play/assets/pad-hold.mjs';
import { pad, B, install, down, up, poll, drain } from './pad-fixture.mjs';

let now = 1000;
performance.now = () => now;
const wait = (ms) => { now += ms; poll(); };
const project = (x, y) => [400 + x * 10, 300 + y * 10];
const mon = (id, x, y) => ({ id, type: 1, mode: 1, hp: 50, hostile: true, x, y });
const snapshot = { player: { x: 0, y: 0, hpMax: 100 }, playerId: 1, level: 5, entities: [mon(2, 3, 0)], items: [], objects: [], warps: [] };
// A paladin's sheet, seeded from the hands and hotkeys: RT Attack, A Sacrifice (left), B Might (aura),
// X the Town Portal tome, RB Holy Bolt (right). Skill selects are logged beside the DOM events.
const selected = { left: 0, right: 101 }, selects = [];
const padSkills = { name: 'Holder', inTown: false, hotkeys: [{ skill: 96 }, { skill: 98 }, { skill: 220 }],
  choices: [{ id: 0, leftSkill: true }, { id: 96, leftSkill: true }, { id: 101 }, { id: 98, aura: true }, { id: 220 }] };
await install({ screen: 'game', uiTargets: () => ({}), snapshot, project, controlCodeFor: () => null, selectedSkills: selected, padSkills,
  selectSkill: (id, side) => { selects.push(`${id} ${side}`); selected[side] = id; } });
poll(); drain();

test('state machine: tap is nothing extra, hold goes down, moves with the target, goes up on release', () => {
  const h = makeTargetHold(250);
  let at = [10, 10];
  const locate = () => at && { screen: at };
  assert.deepEqual(h.step({ edge: true, pressed: true, now: 0, key: 'k', locate }), []);
  assert.deepEqual(h.step({ edge: false, pressed: true, now: 249, locate }), []);
  assert.deepEqual(h.step({ edge: false, pressed: true, now: 250, locate }), [{ type: 'down', x: 10, y: 10 }]);
  at = [12, 10];
  assert.deepEqual(h.step({ edge: false, pressed: true, now: 300, locate }), [{ type: 'move', x: 12, y: 10 }]);
  assert.deepEqual(h.step({ edge: false, pressed: false, now: 400, locate }), [{ type: 'up', x: 12, y: 10 }]);
  // a tap
  h.step({ edge: true, pressed: true, now: 0, key: 'k', locate });
  assert.deepEqual(h.step({ edge: false, pressed: false, now: 100, locate }), []);
  // the target dies while held: up at once, inert until the button is let go
  h.step({ edge: true, pressed: true, now: 0, key: 'k', locate });
  h.step({ edge: false, pressed: true, now: 300, locate });
  at = null;
  assert.deepEqual(h.step({ edge: false, pressed: true, now: 350, locate }), [{ type: 'up', x: 12, y: 10 }]);
  assert.deepEqual(h.step({ edge: false, pressed: true, now: 900, locate }), []);
});

test('pad: A holds Sacrifice on a monster, follows it, and lets go with the button', () => {
  down(B.A);
  assert.deepEqual(drain(), ['pointermove:430.5,300.5', 'pointerdown:430.5,300.5']);
  assert.deepEqual(selects.splice(0), ['96 left']);
  wait(50); assert.deepEqual(drain(), []);
  snapshot.entities[0].x = 4; wait(50);
  assert.deepEqual(drain(), ['pointermove:440.5,300.5']);
  up(B.A);
  assert.deepEqual(drain(), ['pointerup:440.5,300.5']);
  down(B.A); up(B.A);
  assert.deepEqual(drain(), ['pointermove:440.5,300.5', 'pointerdown:440.5,300.5', 'pointerup:440.5,300.5']);
  assert.deepEqual(selects.splice(0), [], 'already in the left hand');
});

test('pad: hold A, the monster dies: released at once, and the hold never moves onto loot', () => {
  snapshot.entities[0].x = 3; poll(); drain();
  down(B.A); drain();
  snapshot.entities.length = 0; snapshot.items = [{ id: 5, mode: 3, invPage: 255, x: 2, y: 0 }]; wait(50);
  assert.deepEqual(drain(), ['pointerup:430.5,300.5']);
  wait(50); up(B.A); assert.deepEqual(drain(), []);
  snapshot.items = []; snapshot.entities.push(mon(2, 3, 0));
});

test('pad: RT (Attack) puts Attack back in the left hand and picks up loot with one click; holding adds nothing', () => {
  snapshot.entities.length = 0; snapshot.items = [{ id: 5, mode: 3, invPage: 255, x: 2, y: 0 }]; poll(); drain();
  pad.buttons[B.RT].pressed = true; poll();
  assert.deepEqual(drain(), ['pointermove:420.5,300.5', 'pointerdown:420.5,300.5', 'pointerup:420.5,300.5']);
  wait(50); wait(50); assert.deepEqual(drain(), []);
  pad.buttons[B.RT].pressed = false; poll(); assert.deepEqual(drain(), []);
  snapshot.items = []; snapshot.entities.push(mon(2, 3, 0)); poll(); drain();
  assert.deepEqual(selects.splice(0), ['0 left']);
});

test('pad: B (an aura) selects it into the right hand with no click; again goes back', () => {
  poll(); drain();
  down(B.B); up(B.B);
  down(B.B); up(B.B);
  assert.deepEqual(drain(), []);
  assert.deepEqual(selects.splice(0), ['98 right', '101 right']);
});

test('pad: X (the portal tome) opens a portal at the hero, outside town only', () => {
  padSkills.inTown = true; down(B.X); up(B.X);
  assert.deepEqual(drain(), []);
  padSkills.inTown = false; down(B.X); up(B.X);
  assert.deepEqual(drain(), ['pointermove:400.5,300.5', 'pointerdown:400.5,300.5', 'pointerup:400.5,300.5']);
  assert.deepEqual(selects.splice(0), ['220 right']);
});
