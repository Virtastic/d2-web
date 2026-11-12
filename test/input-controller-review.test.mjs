// Pad disconnect hides every typed-field grid; a rebound hotkey presses its own key name or nothing; an empty panel is no panel.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { pad, B, install, down, up, poll, drain, events as raw } from './pad-fixture.mjs';

const snapshot = { player: { x: 0, y: 0, hpMax: 100 }, playerId: 1, level: 1, entities: [], items: [], objects: [], warps: [] };
let rebound = null, targets = {};
const shell = { screen: 'game', uiTargets: () => targets, snapshot, project: (x, y) => [400 + x * 10, 300 + y * 10], controlCodeFor: () => rebound };
const { keyNameForCode } = await install(shell);
const events = () => raw.splice(0).filter((e) => e.type.startsWith('key')).map((e) => `${e.type}:${e.code}:${e.key}`);
const grids = () => document.body.children.filter((c) => /keys$/.test(c.id || ''));

test('disconnect hides the name, gold and chat grids', () => {
  assert.equal(grids().length, 3);
  poll();
  for (const g of grids()) g.style.display = 'flex';
  const real = navigator.getGamepads;
  navigator.getGamepads = () => [];
  poll();
  navigator.getGamepads = real;
  assert.deepEqual(grids().map((g) => g.style.display), ['none', 'none', 'none']);
});

test('keyNameForCode names a rebound code or returns empty', () => {
  assert.deepEqual(['KeyJ', 'Digit7', 'F9', 'Space', 'Numpad5'].map(keyNameForCode), ['j', '7', 'F9', ' ', '']);
});

test('a rebound key (Swap Weapons, an LT tap) presses its own key name; an unknown code presses nothing', () => {
  poll(); drain();
  rebound = 'KeyJ';
  down(B.LT); up(B.LT);
  const ev = events();
  assert.deepEqual(ev, ['keydown:KeyJ:j', 'keyup:KeyJ:j']);
  rebound = 'Numpad5';
  down(B.LT); up(B.LT);
  assert.deepEqual(events(), []);
  rebound = null;
});

test('a panel with no points does not throw and leaves world mode on', () => {
  targets = { waypoint: { open: 'not a point' } };
  assert.doesNotThrow(() => poll());
  assert.equal(globalThis.__d2Controller.evidence.mode, 'world');
});
