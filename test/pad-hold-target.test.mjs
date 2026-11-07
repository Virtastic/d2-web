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
await install({ screen: 'game', uiTargets: () => ({}), snapshot, project, controlCodeFor: () => null });
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

test('pad: tap A is the plain click; hold A on a monster is a held shift-click that re-aims and releases on A up', () => {
  down(B.A);
  assert.deepEqual(drain(), ['pointermove:430.5,300.5', 'pointerdown:430.5,300.5', 'pointerup:430.5,300.5']);
  wait(200); assert.deepEqual(drain(), []);
  wait(60);  assert.deepEqual(drain(), ['pointerdown:430.5,300.5:shift']);
  snapshot.entities[0].x = 4; wait(50);
  assert.deepEqual(drain(), ['pointermove:440.5,300.5:shift']);
  wait(50); assert.deepEqual(drain(), []);
  up(B.A);
  assert.deepEqual(drain(), ['pointerup:440.5,300.5:shift']);
  down(B.A); up(B.A); drain();
  assert.deepEqual(drain(), []);
});

test('pad: hold A, the monster dies: released without waiting for A up', () => {
  snapshot.entities[0].x = 3; poll(); drain();
  down(B.A); drain(); wait(300); drain();
  snapshot.entities.length = 0; wait(50);
  assert.deepEqual(drain(), ['pointerup:430.5,300.5:shift']);
  up(B.A); assert.deepEqual(drain(), []);
  snapshot.entities.push(mon(2, 3, 0));
});

test('pad: hold B casts the right skill held (Hydra style), at a target and at the ground ahead', () => {
  snapshot.entities.length = 0; snapshot.entities.push(mon(2, 3, 0)); poll(); drain();
  down(B.B); drain(); wait(300);
  assert.deepEqual(drain(), ['pointerdown:430.5,300.5:shift']);
  up(B.B); assert.deepEqual(drain(), ['pointerup:430.5,300.5:shift']);
  snapshot.entities.length = 0; poll(); drain();
  down(B.B); drain(); wait(300);
  assert.deepEqual(drain(), ['pointerdown:400.5,460.5:shift']);
  up(B.B); assert.deepEqual(drain(), ['pointerup:400.5,460.5:shift']);
  snapshot.entities.push(mon(2, 3, 0));
});

test('pad: A on something that is not a monster is a click only, never a hold', () => {
  snapshot.entities.length = 0; snapshot.items = [{ id: 5, mode: 3, invPage: 255, x: 2, y: 0 }]; poll(); drain();
  down(B.A); drain(); wait(600);
  assert.deepEqual(drain(), []);
  up(B.A); assert.deepEqual(drain(), []);
});
