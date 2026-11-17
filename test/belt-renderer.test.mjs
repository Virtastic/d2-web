import test from 'node:test';
import assert from 'node:assert/strict';
import { parseBeltOps } from '../play/assets/belt-draw-engine.mjs';
import { createBeltDraw } from '../play/assets/belt-draw-engine.mjs';

// What the belt draws and where is the engine's (D2Client belt draw port, proved on its own); the shell only unpacks its ops.

test('the engine op list unpacks into frames, item cels and labels', () => {
  const text = [...'12'].map((c) => c.charCodeAt(0));
  const ops = parseBeltOps(Int32Array.from([1, 444, 559, 2, 0, 423, 562, 3, 425, 590, 4, 2, ...text]));
  assert.deepEqual(ops, [{ op: 'bg', x: 444, y: 559 }, { op: 'item', item: 0, x: 423, y: 562 }, { op: 'text', text: '12', x: 425, y: 590, color: 4 }]);
  assert.throws(() => parseBeltOps(Int32Array.from([9])));
});

test('no engine export, no belt draw', () => {
  assert.equal(createBeltDraw({}, () => null), null);
});

test('belt hotkey labels follow the configured Use Belt bindings', async () => {
  const { beltKeyNames, CONFIGURE_CONTROLS_SLOTS } = await import('../play/assets/configure-controls.mjs');
  const blank = CONFIGURE_CONTROLS_SLOTS.map((s) => (s ? [s[1], s[2]] : null));
  assert.deepEqual(beltKeyNames(blank), ['1', '2', '3', '4']);
  blank[CONFIGURE_CONTROLS_SLOTS.findIndex((s) => s && s[0] === 'Use Belt 2')] = ['F9', null];
  blank[CONFIGURE_CONTROLS_SLOTS.findIndex((s) => s && s[0] === 'Use Belt 4')] = [null, null];
  assert.deepEqual(beltKeyNames(blank), ['1', 'F9', '3', null]);
});
