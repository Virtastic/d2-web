import { test } from 'node:test';
import assert from 'node:assert/strict';
import { B, install, down, up, poll, drain } from './pad-fixture.mjs';

let now = 1000;
performance.now = () => now;
const wait = (ms) => { now += ms; poll(); };
const shell = { screen: 'game', cursorItem: 0, snapshot: { items: [], player: { x: 0, y: 0, hpMax: 1 } }, controlCodeFor: () => null, project: () => [0, 0],
  uiTargets: () => ({ character: { 0: [100, 100], 1: [100, 140] } }) };
await install(shell);
const downs = () => drain().filter((e) => e.startsWith('pointerdown'));
poll(); drain();

test('stat panel: tap A adds one point; holding A adds one after 400 ms and then one per 150 ms; release stops', () => {
  down(B.A);
  assert.deepEqual(downs(), ['pointerdown:100.5,100.5']);
  wait(399); assert.deepEqual(downs(), []);
  wait(1);   assert.deepEqual(downs(), ['pointerdown:100.5,100.5']);
  wait(149); assert.deepEqual(downs(), []);
  wait(1);   assert.deepEqual(downs(), ['pointerdown:100.5,100.5']);
  wait(150); assert.deepEqual(downs(), ['pointerdown:100.5,100.5']);
  up(B.A); wait(1000); assert.deepEqual(downs(), []);
});

test('stat panel: RB (shift) held with A carries shift on the + click, for the shell to spend 5 if it implements that', () => {
  down(B.RB); drain();
  down(B.A);
  assert.deepEqual(drain().filter((e) => e.startsWith('pointerdown')), ['pointerdown:100.5,100.5:shift']);
  up(B.A, B.RB);
});
