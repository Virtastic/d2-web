import { unlockOnGesture } from './audio-unlock.mjs';
/**
 * Send the PCM blocks emitted by unbikit's Bink audio decoder to Web Audio. Bink audio is
 * interleaved by frame but returned as per-channel float blocks. Bink's packet `numSamples` field
 * is a decompressed byte count, not a per-channel sample-frame count. Convert it to frames using
 * 16-bit samples and the track's channel count, then copy PCM blocks into that declared duration;
 * if the decoder output is short, the remainder stays silent as the packet specifies.
 */
export function makeBikAudioBuffer(context, track) {
  const channels = track.header.numChannels;
  if (!channels) return null;
  const blocks = track.blocks ?? [];
  const decodedBytes = Math.max(0, Math.trunc(Number(track.numSamples) || 0));
  const length = Math.floor(decodedBytes / (2 * channels));
  if (!length) return null;
  const buffer = context.createBuffer(channels, length, track.header.sampleRate);
  for (let channel = 0; channel < channels; channel++) {
    const destination = buffer.getChannelData(channel);
    let at = 0;
    for (const block of blocks) {
      const samples = block[channel];
      if (!samples) continue;
      const count = Math.min(samples.length, length - at);
      if (count <= 0) break;
      destination.set(samples.subarray(0, count), at);
      at += count;
    }
  }
  return buffer;
}

export function createBikAudioPlayer(AudioContextClass = globalThis.AudioContext || globalThis.webkitAudioContext,
  gestureTarget = globalThis.window) {
  if (!AudioContextClass) return null;
  const context = new AudioContextClass(), sources = new Set(), pending = [];
  let nextTime = 0, scheduled = 0;
  const schedule = (buffers) => {
    if (!buffers.length || context.state !== 'running') return;
    const at = Math.max(context.currentTime, nextTime);
    let duration = 0;
    for (const buffer of buffers) {
      const source = context.createBufferSource();
      source.buffer = buffer; source.connect(context.destination);
      source.onended = () => { sources.delete(source); source.disconnect(); };
      source.start(at); sources.add(source); scheduled++;
      duration = Math.max(duration, buffer.duration);
    }
    nextTime = at + duration;
  };
  unlockOnGesture(context, gestureTarget, () => schedule(pending.splice(0)));
  return {
    play(tracks) {
      const buffers = [];
      for (const track of tracks ?? []) {
        const buffer = makeBikAudioBuffer(context, track);
        if (!buffer) continue;
        buffers.push(buffer);
      }
      const count = buffers.length;
      if (count && context.state === 'running') schedule(buffers);
      else if (count && context.state === 'suspended') pending.push(...buffers);
      return count;
    },
    get evidence() { return { state: context.state, scheduled, pending: pending.length, sampleTime: nextTime }; },
    // A title movie can be skipped before the browser grants audio activation. Stop any sources
    // already scheduled and forget decoded buffers waiting for browser activation. The boot player
    // is shared across phases, so a skipped film must not leak its pending audio into the next one.
    stop() { pending.length = 0; for (const source of sources) { try { source.stop(); } catch {} source.disconnect(); } sources.clear(); nextTime = context.currentTime; },
    close() { this.stop(); return context.close?.(); },
  };
}
