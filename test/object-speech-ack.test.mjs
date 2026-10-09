import test from 'node:test';
import assert from 'node:assert/strict';
import { nativeObjectSpeechAcknowledgement } from '../play/assets/object-speech-ack.mjs';

test('native object scroll responses use the null target and retain the offered string', () => {
  for (const stringId of [127, 396, 20002, 20131, 20169]) {
    assert.deepEqual(nativeObjectSpeechAcknowledgement({ guid: 1234, stringId }), {
      guid: 0xffffffff,
      stringId,
    });
  }
});

test('unmapped object speech does not invent a 0x31 response', () => {
  assert.equal(nativeObjectSpeechAcknowledgement({ guid: 1234, stringId: 999 }), null);
  assert.equal(nativeObjectSpeechAcknowledgement(null), null);
});
