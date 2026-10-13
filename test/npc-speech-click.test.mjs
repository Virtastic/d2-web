import test from 'node:test';
import assert from 'node:assert/strict';
import { createNpcSpeechClickGate } from '../play/assets/npc-speech-click.mjs';

test('NPC speech consumes both mouse-down edges and dismisses at 100 ms', () => {
  const gate = createNpcSpeechClickGate();
  assert.equal(gate.pointerDown(0, 0), 'inactive');

  gate.begin(1000);
  assert.equal(gate.pointerDown(0, 1099), 'consumed');
  assert.equal(gate.pointerDown(2, 1099), 'consumed');
  assert.equal(gate.pointerDown(0, 1100), 'dismiss');
  assert.equal(gate.pointerDown(2, 1100), 'dismiss');
});

test('a new speech object restarts the gate and GetTickCount wrap is preserved', () => {
  const gate = createNpcSpeechClickGate();
  gate.begin(0xfffffff0);
  assert.equal(gate.pointerDown(0, 0x53), 'consumed'); // 99 ms across DWORD wrap
  assert.equal(gate.pointerDown(0, 0x54), 'dismiss'); // 100 ms across DWORD wrap

  gate.begin(5000);
  assert.equal(gate.pointerDown(2, 5099), 'consumed');
  gate.cancel();
  assert.equal(gate.pointerDown(2, 6000), 'inactive');
});
