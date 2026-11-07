// Which pad the controller reads: standard first, an unmapped (Android built-in) pad otherwise.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { pickPad } from '../play/assets/pad-pick.mjs';

const pad = (index, mapping, { axes = 4, buttons = 17, timestamp = 0, id = `pad${index}` } = {}) => ({
  index, id, mapping, connected: true, timestamp, axes: new Array(axes).fill(0), buttons: Array.from({ length: buttons }, () => ({ pressed: false, value: 0 })),
});

test('an unmapped pad is read when no standard pad is connected (Android reports built-in pads with mapping "")', () => {
  const retroid = pad(0, '', { id: 'Retroid Pocket Controller' });
  assert.equal(pickPad([retroid, null, null, null]), retroid);
});

test('a standard pad wins over an unmapped one', () => {
  const raw = pad(0, '', { timestamp: 9 }), std = pad(1, 'standard', { timestamp: 1 });
  assert.equal(pickPad([raw, std]), std);
});

test('a device too small to be a controller is never read', () => {
  assert.equal(pickPad([pad(0, '', { axes: 2, buttons: 17 }), pad(1, '', { axes: 4, buttons: 6 })]), null);
  assert.equal(pickPad([]), null);
  assert.equal(pickPad(null), null);
  assert.equal(pickPad([{ ...pad(0, ''), connected: false }]), null);
});

test('a mapping the page does not know (not "" and not "standard") is not read', () => {
  assert.equal(pickPad([pad(0, 'xr-standard')]), null);
});

test('of two unmapped pads the one in use stays; with none in use the newest input wins', () => {
  const a = pad(0, '', { timestamp: 5 }), b = pad(1, '', { timestamp: 7 });
  assert.equal(pickPad([a, b]), b);
  assert.equal(pickPad([a, b], 0), a);
  assert.equal(pickPad([a, b], 3), b);
});
