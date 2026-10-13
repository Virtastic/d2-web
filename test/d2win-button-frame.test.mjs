import test from 'node:test';
import assert from 'node:assert/strict';
import { d2winHeldFrame } from '../play/assets/d2win-button-frame.mjs';

test('enabled D2Win control uses the pressed frame only during its held interval', () => {
  const frames = [{}, {}];
  assert.equal(d2winHeldFrame(frames), 0);
  assert.equal(d2winHeldFrame(frames, 0, true), 1);
  assert.equal(d2winHeldFrame(frames, 0, true, false), 0);
  assert.equal(d2winHeldFrame([{}], 0, true), 0);
});
