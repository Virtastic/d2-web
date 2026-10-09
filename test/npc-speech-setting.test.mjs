import test from 'node:test';
import assert from 'node:assert/strict';
import { createNpcSpeechSetting, npcSpeechHasAudio, npcSpeechHasText } from '../play/assets/npc-speech-setting.mjs';

test('native NPC Speech modes gate only the Text Only selection', () => {
  assert.equal(npcSpeechHasAudio(2), true, 'Audio and Text');
  assert.equal(npcSpeechHasAudio(0), true, 'Audio Only');
  assert.equal(npcSpeechHasAudio(1), false, 'Text Only');
  assert.equal(npcSpeechHasAudio(-1), false, 'invalid values do not enable voice');
  assert.equal(npcSpeechHasText(0), false, 'Audio Only');
  assert.equal(npcSpeechHasText(1), true, 'Text Only');
  assert.equal(npcSpeechHasText(2), true, 'Audio and Text');
});

test('NPC Speech preference defaults to retail Audio and Text and persists valid native values', () => {
  const values = new Map();
  const storage = { getItem: (key) => values.get(key) ?? null, setItem: (key, value) => values.set(key, value) };
  assert.equal(createNpcSpeechSetting({ storage }).mode, 2);
  const setting = createNpcSpeechSetting({ storage });
  assert.equal(setting.set(1), 1);
  assert.equal(createNpcSpeechSetting({ storage }).mode, 1);
  values.set('d2web.audio.npcSpeech', '99');
  assert.equal(createNpcSpeechSetting({ storage }).mode, 2, 'out-of-range browser values fall back to retail default');
  assert.throws(() => setting.set(3), /0, 1 or 2/);
});
