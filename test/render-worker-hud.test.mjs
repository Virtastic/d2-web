import { test } from 'node:test';
import assert from 'node:assert/strict';
import { hudState, hudDraw, resetHud, HUD_OPS } from '../play/render-worker-hud.mjs';

const input = { w: 1024, h: 600, snapshot: { player: { hp: 10, hpMax: 20, mana: 5, manaMax: 9 }, items: [{ mode: 2, gridX: 1, invFile: 'invpot', x: 1 }, { mode: 0, gridX: 0 }] },
  advancement: { statPoints: 5, experience: 3, nextExperience: 9, prevExperience: 0 }, screen: [0, 0, 0, 0, 50, 100], left: true,
  beltBoxes: [{ left: 1, right: 2, top: 3, bottom: 4 }], beltKeys: ['1', '2', '3', '4'] };

test('hudState is structured-clone safe and plain', () => {
  const s = hudState(input);
  assert.deepEqual(structuredClone(s), s);
  assert.equal(s.items.length, 1); assert.equal(s.offsetX < 0 || s.offsetX > 0, true);
  assert.equal(hudState({ ...input, clearScreen: true }), null);
  assert.ok(HUD_OPS.includes('celex'));
});

test('hudDraw records missed paths and does not throw', () => {
  resetHud();
  const missed = new Set(), env = { missed, documentObject: {}, read: (p) => { missed.add(p); throw new Error('missing ' + p); } };
  assert.doesNotThrow(() => hudDraw({}, { meta: { hud: hudState(input) } }, {}, env));
  assert.ok(missed.size > 0);
});

test('hudDraw without meta.hud draws nothing', () => {
  let touched = 0; const env = { read: () => { touched++; throw new Error('x'); }, missed: new Set() };
  hudDraw({}, { meta: {} }, {}, env); hudDraw({}, {}, {}, env);
  assert.equal(touched, 0);
});
