import test from 'node:test';
import assert from 'node:assert/strict';
import { createBikAudioPlayer, makeBikAudioBuffer } from '../play/assets/cinematic-audio.mjs';
import { openMovie } from '../play/assets/cinematics.mjs';
import { createCinematicPlayback } from '../play/assets/cinematic-playback.mjs';

function deferred() {
  let resolve, reject;
  const promise = new Promise((yes, no) => { resolve = yes; reject = no; });
  return { promise, resolve, reject };
}

async function settle() { await new Promise((resolve) => setImmediate(resolve)); }

function movieBytes() {
  const bytes = new Uint8Array(44), view = new DataView(bytes.buffer);
  bytes.set([66, 73, 75, 105]); // BIKi
  view.setUint32(8, 2, true); view.setUint32(20, 2, true); view.setUint32(24, 2, true);
  view.setUint32(28, 24, true); view.setUint32(32, 1, true); view.setUint32(40, 1, true);
  return bytes;
}

function runningAudio() {
  const calls = [];
  class Context {
    constructor() { this.state = 'running'; this.currentTime = 0; this.destination = {}; }
    createBuffer(channels, length, sampleRate) {
      const data = Array.from({ length: channels }, () => new Float32Array(length));
      return { duration: length / sampleRate, getChannelData: (channel) => data[channel] };
    }
    createBufferSource() {
      return { connect() {}, disconnect() {}, start(at) { calls.push(['start', at]); }, stop() { calls.push(['stop']); } };
    }
  }
  return { player: createBikAudioPlayer(Context, null), calls };
}

function makeHarness() {
  const calls = [], listeners = new Map();
  class Context {
    constructor() { this.state = 'suspended'; this.currentTime = 0; this.destination = {}; }
    createBuffer(channels, length, sampleRate) {
      const data = Array.from({ length: channels }, () => new Float32Array(length));
      return { numberOfChannels: channels, length, sampleRate, duration: length / sampleRate,
        getChannelData: (channel) => data[channel], data };
    }
    createBufferSource() { return { connect() {}, disconnect() { calls.push('disconnect'); }, start(at) { calls.push(['start', at]); }, stop() { calls.push('stop'); } }; }
    addEventListener(type, fn) { listeners.set(type, fn); }
    removeEventListener(type) { listeners.delete(type); }
    async resume() { this.state = 'running'; listeners.get('statechange')?.(); }
    async close() { this.state = 'closed'; }
  }
  const targetListeners = new Map();
  const target = { addEventListener(type, fn) { targetListeners.set(type, fn); }, fire(type) { targetListeners.get(type)?.(); } };
  return { Context, target, calls };
}

test('Bink channel blocks are concatenated into the declared PCM frame', () => {
  const context = { createBuffer(channels, length, sampleRate) {
    const data = Array.from({ length: channels }, () => new Float32Array(length));
    return { sampleRate, length, data, getChannelData: (ch) => data[ch] };
  } };
  const track = { header: { numChannels: 2, sampleRate: 44100 }, numSamples: 12,
    blocks: [[Float32Array.of(1, 2, 3, 4), Float32Array.of(5, 6, 7, 8)]] };
  const buffer = makeBikAudioBuffer(context, track);
  assert.deepEqual([...buffer.data[0]], [1, 2, 3]);
  assert.deepEqual([...buffer.data[1]], [5, 6, 7]);
  assert.equal(buffer.sampleRate, 44100);
});

test('startup Bink audio connects directly to the output, outside the D2Sound volume graph', () => {
  const sourceConnections = [], destination = { name: 'destination' };
  class Context {
    constructor() { this.state = 'running'; this.currentTime = 0; this.destination = destination; }
    createBuffer(channels, length, sampleRate) {
      const data = Array.from({ length: channels }, () => new Float32Array(length));
      return { duration: length / sampleRate, getChannelData: (channel) => data[channel] };
    }
    createBufferSource() { return { connect(destination) { sourceConnections.push(destination); }, disconnect() {}, start() {}, stop() {} }; }
  }
  const player = createBikAudioPlayer(Context, null);
  assert.equal('setVolume' in player, false, 'D2Sound slider code cannot apply its channel scale to Bink output');
  assert.equal(player.play([{ header: { numChannels: 1, sampleRate: 8000 }, numSamples: 2,
    blocks: [[Float32Array.of(0.25)]] }]), 1);
  assert.deepEqual(sourceConnections, [destination], 'Bink sources use the configured output provider directly');
});

test('Bink packet byte count becomes per-channel sample frames while preserving declared silence', () => {
  const context = { createBuffer(channels, length, sampleRate) {
    const data = Array.from({ length: channels }, () => new Float32Array(length));
    return { sampleRate, length, data, getChannelData: (ch) => data[ch] };
  } };
  // BIKi packet numSamples is a decompressed byte count: 16 bytes / (2 bytes * 2 channels) = 4 frames.
  const track = { header: { numChannels: 2, sampleRate: 44100 }, numSamples: 16,
    blocks: [[Float32Array.of(1, 2, 3), Float32Array.of(5, 6)]] };
  const buffer = makeBikAudioBuffer(context, track);
  assert.equal(buffer.length, 4);
  assert.deepEqual([...buffer.data[0]], [1, 2, 3, 0], 'missing decoded frames remain silent to preserve packet duration');
  assert.deepEqual([...buffer.data[1]], [5, 6, 0, 0]);
});

test('Bink decoder padding beyond the packet byte count is trimmed', () => {
  const context = { createBuffer(channels, length, sampleRate) {
    const data = Array.from({ length: channels }, () => new Float32Array(length));
    return { sampleRate, length, data, getChannelData: (ch) => data[ch] };
  } };
  const track = { header: { numChannels: 2, sampleRate: 44100 }, numSamples: 8,
    blocks: [[Float32Array.of(1, 2, 3), Float32Array.of(5, 6, 7)]] };
  const buffer = makeBikAudioBuffer(context, track);
  assert.equal(buffer.length, 2);
  assert.deepEqual([...buffer.data[0]], [1, 2]);
  assert.deepEqual([...buffer.data[1]], [5, 6]);
});

test('skipping a Bink movie discards locked audio before the next gesture', async () => {
  const { Context, target, calls } = makeHarness();
  const player = createBikAudioPlayer(Context, target);
  const track = { header: { numChannels: 1, sampleRate: 8000 }, numSamples: 4,
    blocks: [[Float32Array.of(0.1, 0.2, 0.3, 0.4)]] };
  const secondTrack = { ...track, blocks: [[Float32Array.of(0.5, 0.6, 0.7, 0.8)]] };
  assert.equal(player.play([track, secondTrack]), 2);
  assert.equal(player.evidence.scheduled, 0);
  assert.equal(player.evidence.pending, 2, 'decoded buffers wait for activation while the context is suspended');
  player.stop();
  assert.equal(player.evidence.pending, 0, 'stop discards the skipped phase buffers');
  target.fire('pointerdown');
  await new Promise((resolve) => setImmediate(resolve));
  assert.equal(player.evidence.scheduled, 0, 'a skipped movie must not leak its pending tracks into the title phase');
  assert.equal(calls.some((call) => Array.isArray(call) && call[0] === 'start'), false);
  assert.equal(player.play([track, secondTrack]), 2, 'the next movie can enqueue audio on the shared player');
  assert.equal(player.evidence.scheduled, 2);
  assert.deepEqual(calls[0], ['start', 0]);
  assert.deepEqual(calls[1], ['start', 0], 'the new movie starts from the reset timeline');
  await player.close();
  assert.ok(calls.includes('stop'));
});

test('Bink audio schedules suspended buffers after a successful gesture unlock', async () => {
  const calls = [], contextListeners = new Map(), targetListeners = new Map();
  class Context {
    constructor() { this.state = 'suspended'; this.currentTime = 0; this.destination = {}; this.resumeCalls = 0; }
    createBuffer(channels, length, sampleRate) {
      const data = Array.from({ length: channels }, () => new Float32Array(length));
      return { numberOfChannels: channels, length, sampleRate, duration: length / sampleRate,
        getChannelData: (channel) => data[channel] };
    }
    createBufferSource() { return { connect() {}, disconnect() {}, start(at) { calls.push(at); }, stop() {} }; }
    addEventListener(type, fn) { contextListeners.set(type, fn); }
    removeEventListener(type) { contextListeners.delete(type); }
    resume() {
      this.resumeCalls++;
      if (this.resumeCalls === 2) { this.state = 'running'; contextListeners.get('statechange')?.(); }
      return Promise.resolve();
    }
  }
  const target = { addEventListener(type, fn) { targetListeners.set(type, fn); }, fire(type) { targetListeners.get(type)?.(); } };
  const player = createBikAudioPlayer(Context, target);
  const track = { header: { numChannels: 1, sampleRate: 8000 }, numSamples: 4,
    blocks: [[Float32Array.of(0.1, 0.2, 0.3, 0.4)]] };
  assert.equal(player.play([track]), 1);
  assert.equal(player.play([track]), 1, 'subsequent film packets also wait while suspended');
  assert.equal(player.evidence.scheduled, 0);
  assert.equal(player.evidence.pending, 2);
  target.fire('pointerdown');
  await new Promise((resolve) => setImmediate(resolve));
  assert.equal(player.evidence.state, 'suspended');
  assert.equal(player.evidence.scheduled, 0);
  assert.equal(player.evidence.pending, 2, 'a resume that remains suspended keeps both buffers pending');
  target.fire('pointerdown');
  await new Promise((resolve) => setImmediate(resolve));
  assert.equal(player.evidence.state, 'running');
  assert.equal(player.evidence.scheduled, 2, 'successful activation schedules the pending audio');
  assert.equal(player.evidence.pending, 0);
  assert.equal(player.play([track]), 1, 'a packet received after unlock is scheduled');
  assert.equal(player.evidence.scheduled, 3);
  assert.deepEqual(calls, [0, 0, 0.00025]);
  await player.close();
});

test('a phase change during decoder creation abandons the stale boot movie', async () => {
  const creation = deferred(), { player, calls } = runningAudio();
  let generation = 0, decoded = false;
  const openingGeneration = generation;
  const opening = openMovie({ read: movieBytes, path: 'old.bik', audioPlayer: player,
    isCurrent: () => openingGeneration === generation,
    createDecoder: () => creation.promise });
  generation++;
  creation.resolve({ isSupported: true, async getNextFrame() { decoded = true; } });
  assert.equal(await opening, null);
  assert.equal(decoded, false, 'a stale opener must stop before decoding its first frame');
  assert.equal(player.evidence.scheduled, 0);
  assert.deepEqual(calls, []);
  await player.close();
});

test('an explicit film acquires its owned audio context before asynchronous decoder startup', async () => {
  const creation = deferred();
  let contexts = 0, closes = 0;
  class Context {
    constructor() { contexts++; this.state = 'running'; this.currentTime = 0; this.destination = {}; }
    async close() { closes++; }
  }
  const documentObject = { createElement: () => ({ getContext: () => ({ putImageData() {} }) }) };
  const opening = openMovie({ read: movieBytes, path: 'd2intro.bik', documentObject,
    AudioContextClass: Context, gestureTarget: { addEventListener() {} },
    createDecoder: () => creation.promise, isCurrent: () => false });

  assert.equal(contexts, 1, 'openMovie starts synchronously in the trusted title-menu action');
  creation.resolve({ isSupported: true, async getNextFrame() { throw new Error('stale movie decoded'); } });
  assert.equal(await opening, null);
  assert.equal(closes, 1, 'a decoder that becomes stale closes only its newly owned context');
});

test('a phase change during initial decode blocks old audio and stale discard preserves the new queue', async () => {
  const frame = deferred(), { player, calls } = runningAudio();
  let generation = 0;
  const openingGeneration = generation;
  const documentObject = { createElement: () => ({
    getContext: () => ({ putImageData() {} }),
  }) };
  const opening = openMovie({ read: movieBytes, path: 'old.bik', documentObject, audioPlayer: player,
    isCurrent: () => openingGeneration === generation,
    createDecoder: async () => ({ isSupported: true, getNextFrame: () => frame.promise }) });
  await new Promise((resolve) => setImmediate(resolve));
  generation++;
  const track = { header: { numChannels: 1, sampleRate: 8000 }, numSamples: 4,
    blocks: [[Float32Array.of(0.1, 0.2)]] };
  frame.resolve({ videoFrame: { yuv: Uint8Array.of(16, 16, 128, 128, 128, 128), lineSize: [2, 1, 1] }, audioTracks: [track] });
  const staleMovie = await opening;
  assert.ok(staleMovie, 'the opener can resolve after its phase has become stale');
  assert.equal(player.evidence.scheduled, 0, 'the initial old-phase frame must not enqueue audio');
  staleMovie.discard();
  assert.equal(player.play([track]), 1, 'discarding the stale movie must leave the next phase audio queue usable');
  assert.equal(player.evidence.scheduled, 1);
  assert.deepEqual(calls, [['start', 0]]);
  await player.close();
});

test('skipping while a title movie is opening closes a stale opener and permits a new movie', async () => {
  const first = deferred(), opened = [];
  const playback = createCinematicPlayback(({ path, isCurrent }) => {
    opened.push({ path, isCurrent });
    return path === 'old.bik' ? first.promise : Promise.resolve({ close() {}, discard() {} });
  }, () => 123);
  assert.equal(playback.play(0, 'old.bik'), true);
  assert.equal(opened[0].isCurrent(), true);
  playback.stop();
  assert.equal(opened[0].isCurrent(), false, 'skip invalidates the opener predicate');
  assert.equal(playback.play(0, 'new.bik'), true, 'skip releases the opening slot immediately');
  first.resolve({ discard() { opened.push({ discarded: true }); } });
  await settle();
  assert.equal(opened.some((entry) => entry.discarded), true, 'the late old movie discards only its own resources');
  assert.equal(playback.playing, 0);
  assert.equal(playback.movie !== null, true, 'the newer movie remains active after the stale opener settles');
});

test('skipping after title movie playback starts closes its owned movie audio', async () => {
  let closes = 0;
  const playback = createCinematicPlayback(() => Promise.resolve({ close() { closes++; } }), () => 456);
  playback.play(0, 'd2intro.bik');
  await settle();
  assert.equal(playback.movie !== null, true);
  assert.equal(playback.startedAt, 456);
  playback.stop();
  assert.equal(closes, 1, 'skip closes the active movie, including its owned AudioContext and sources');
  assert.equal(playback.movie, null);
  assert.equal(playback.playing, null);
});
