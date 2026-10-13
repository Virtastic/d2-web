import test from 'node:test';
import assert from 'node:assert/strict';
import { createNpcSpeechClickGate } from '../play/assets/npc-speech-click.mjs';
import { dispatchObjectSpeechPointer } from '../play/assets/object-speech-input.mjs';

test('object speech consumes left/right down and acknowledges only after native 100 ms gate', () => {
  const speech = { guid: 154, stringId: 20002, text: 'The Ancients await.' };
  const clickGate = createNpcSpeechClickGate();
  clickGate.begin(1000);
  const acknowledged = [];
  const dispatch = (button, now) => dispatchObjectSpeechPointer({ screenIsGame: true, button, speech,
    dismiss: (message) => acknowledged.push(message), clickGate, now });
  assert.equal(dispatch(0, 1099), true, 'the initial press is consumed');
  assert.deepEqual(acknowledged, []);
  assert.equal(dispatch(2, 1100), true, 'either native speech mouse-down can dismiss after the gate');
  assert.deepEqual(acknowledged, [speech]);
});

test('object message acknowledgement does not consume other clicks or screens', () => {
  const speech = { guid: 154, stringId: 20002 };
  const clickGate = createNpcSpeechClickGate();
  clickGate.begin(500);
  let called = false;
  const dismiss = () => { called = true; };
  const dispatch = (screenIsGame, button, target) => dispatchObjectSpeechPointer({ screenIsGame, button,
    speech: target, dismiss, clickGate, now: 700 });
  assert.equal(dispatch(false, 0, speech), false);
  assert.equal(dispatch(true, 1, speech), false, 'middle-click is not in the recovered speech table');
  assert.equal(dispatch(true, 0, null), false);
  assert.equal(called, false);
});
