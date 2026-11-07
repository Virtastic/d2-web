// The device-report reader on its own: synthetic axis samples in, the buttons the controller will read out.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { createReportReader } from '../play/assets/pad-report.mjs';

const mk = (axes, buttons = {}, extra = {}) => ({
  index: 0, id: 'Retroid Pocket Controller (Vendor: 2020 Product: 3001)', mapping: '', connected: true, timestamp: 1,
  axes, buttons: Array.from({ length: 17 }, (_, i) => ({ pressed: Boolean(buttons[i]), value: buttons[i] ? 1 : 0 })), ...extra,
});
const pressed = (pad) => pad.buttons.flatMap((b, i) => (b.pressed ? [i] : []));

test('a standard pad is returned as the very same object, untouched', () => {
  const r = createReportReader();
  const std = mk([0, 0, 0, 0, 1, 1, 0, 0], {}, { mapping: 'standard' });
  assert.equal(r.read(std), std);
});

test('a hat pair on axes 6 and 7 reads as the d-pad buttons 12-15 once both axes have moved', () => {
  const r = createReportReader();
  r.read(mk([0, 0, 0, 0, 0, 0, 0, 0]));
  assert.deepEqual(pressed(r.read(mk([0, 0, 0, 0, 0, 0, 1, 0]))), [], 'the first axis alone is not yet a pair');
  assert.deepEqual(pressed(r.read(mk([0, 0, 0, 0, 0, 0, 1, -1]))), [12, 15], 'right and up');
  assert.deepEqual(pressed(r.read(mk([0, 0, 0, 0, 0, 0, 0, 0]))), []);
  assert.deepEqual(pressed(r.read(mk([0, 0, 0, 0, 0, 0, -1, 1]))), [13, 14], 'left and down');
});

test('a stick on axes 0-3 never becomes a hat or a trigger', () => {
  const r = createReportReader();
  for (const v of [0.3, -1, 0.8, -0.5, 0]) r.read(mk([v, v, v, v, 0, 0, 0, 0]));
  assert.deepEqual(pressed(r.read(mk([0, 0, 0, 0, 0, 0, 0, 0]))), []);
});

test('an analog trigger on axis 4 presses button 6 (LT) past half travel; the second trigger is button 7', () => {
  const r = createReportReader();
  assert.deepEqual(pressed(r.read(mk([0, 0, 0, 0, 0.3, 0.3, 0, 0]))), [], 'a fraction is analog; under half travel is not a press');
  assert.deepEqual(pressed(r.read(mk([0, 0, 0, 0, 0.9, 0, 0, 0]))), [6]);
  const both = r.read(mk([0, 0, 0, 0, 0.9, 0.6, 0, 0]));
  assert.deepEqual(pressed(both), [6, 7]);
  assert.equal(both.buttons[6].value, 0.9, 'the analog value is carried');
});

test('a button the pad itself reports stays pressed; the reader only adds', () => {
  const r = createReportReader();
  r.read(mk([0, 0, 0, 0, 0, 0, 0, 0]));
  r.read(mk([0, 0, 0, 0, 0, 0, 1, 0]));
  const out = r.read(mk([0, 0, 0, 0, 0, 0, 0, 0], { 12: true }));
  assert.deepEqual(pressed(out), [12]);
});

test('a disconnect or a new pad index starts the classification over', () => {
  const r = createReportReader();
  r.read(mk([0, 0, 0, 0, 0.5, 0, 0, 0]));
  r.read(mk([0, 0, 0, 0, 0.7, 0, 0, 0]));          // axis 4 is a trigger on index 0
  assert.deepEqual(pressed(r.read(mk([0, 0, 0, 0, 0.9, 0, 0, 0]))), [6]);
  assert.equal(r.read(null), null);                // disconnect
  assert.deepEqual(pressed(r.read(mk([0, 0, 0, 0, 1, 0, 0, 0]))), [], 'after a reconnect, a full pull has not yet shown it analog');
  assert.deepEqual(pressed(r.read(mk([0, 0, 0, 0, 0.7, 0, 0, 0]))), [6], 'the analog reading re-classifies it');
  assert.deepEqual(pressed(r.read(mk([0, 0, 0, 0, 1, 0, 0, 0], {}, { index: 1 }))), [], 'a new index is a new device');
});

test('an unused constant axis is neither a hat nor a trigger', () => {
  const r = createReportReader();
  for (let i = 0; i < 5; i++) r.read(mk([0, 0, 0, 0, 0, 0, 0, 0]));
  assert.deepEqual(pressed(r.read(mk([0, 0, 0, 0, 0, 0, 0, 0]))), []);
});

test('a browser Gamepad (fields are prototype getters) keeps its axes, id, index and mapping through the reader', () => {
  class FakeGamepad {
    get id() { return 'Retroid Pocket Controller (Vendor: 2022 Product: 3001)'; }
    get index() { return 0; }
    get mapping() { return ''; }
    get connected() { return true; }
    get timestamp() { return 5; }
    get axes() { return [0.5, -0.5, 0, 1]; }
    get buttons() { return Array.from({ length: 17 }, () => ({ pressed: false, value: 0 })); }
  }
  const out = createReportReader().read(new FakeGamepad());
  assert.deepEqual(out.axes, [0.5, -0.5, 0, 1]);
  assert.equal(out.index, 0);
  assert.equal(out.mapping, '');
  assert.match(out.id, /Retroid/);
});
