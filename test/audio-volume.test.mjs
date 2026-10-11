import test from 'node:test';
import assert from 'node:assert/strict';
import { createAudioVolume } from '../play/assets/audio-volume.mjs';

function player() {
  return { volume: NaN, setVolume(value) { this.volume = value; } };
}

test('retail master volume attenuates music and sound, music volume additionally attenuates music', () => {
  const storage = { getItem: () => null, setItem() {} };
  const volumes = createAudioVolume({ storage });
  const music = player(), sound = player();
  volumes.attach('music', music);
  volumes.attach('sound', sound);
  assert.equal(music.volume, 0.5);
  assert.equal(sound.volume, 1);

  volumes.set('sound', 0.4);
  assert.equal(sound.volume, 0.4);
  assert.equal(music.volume, 0.2);
  volumes.set('music', 0.3);
  assert.equal(music.volume, 0.12);
  assert.equal(sound.volume, 0.4);
  assert.deepEqual(volumes.evidence.applied, { sound: [0.4], music: [0.12] });
});
