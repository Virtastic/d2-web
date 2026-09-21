import test from 'node:test';
import assert from 'node:assert/strict';
import { decodeWav } from '../play/assets/wav.mjs';

function pcm({ channels = 1, sampleRate = 8000, frames = 2 } = {}) {
  const bytes = new Uint8Array(44 + frames * channels * 2), view = new DataView(bytes.buffer);
  bytes.set([...Buffer.from('RIFF')], 0); view.setUint32(4, bytes.length - 8, true); bytes.set([...Buffer.from('WAVEfmt ')], 8);
  view.setUint32(16, 16, true); view.setUint16(20, 1, true); view.setUint16(22, channels, true); view.setUint32(24, sampleRate, true);
  view.setUint32(28, sampleRate * channels * 2, true); view.setUint16(32, channels * 2, true); view.setUint16(34, 16, true);
  bytes.set([...Buffer.from('data')], 36); view.setUint32(40, bytes.length - 44, true); return bytes;
}

test('WAV decoder retains PCM layout and duration', () => {
  assert.deepEqual(decodeWav(pcm({ channels: 2, sampleRate: 11025, frames: 3 })), {
    encoding: 1, channels: 2, sampleRate: 11025, byteRate: 44100, blockAlign: 4, bitsPerSample: 16, dataOffset: 44, dataLength: 12, frames: 3, duration: 3 / 11025,
  });
});
test('WAV decoder rejects bad RIFF bounds and unsupported formats', () => {
  assert.throws(() => decodeWav(new Uint8Array(12)), /RIFF/);
  const broken = pcm(); new DataView(broken.buffer).setUint32(4, 999, true); assert.throws(() => decodeWav(broken), /exceeds/);
  const unsupported = pcm(); new DataView(unsupported.buffer).setUint16(20, 3, true); assert.throws(() => decodeWav(unsupported), /unsupported/);
});
