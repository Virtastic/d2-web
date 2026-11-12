import test from 'node:test';
import assert from 'node:assert/strict';
import { createUnitSoundPlayer, MIXER_TICK_SECONDS } from './unit-sound-player.mjs';

const HEADER = 'Sound\tIndex\tFileName\tVolume\tGroup Size\tLoop\tFade In\tFade Out\tDefer Inst\tStop Inst\tDuration\tCompound\tReverb\tFalloff\tCache\tAsync Only\tPriority\tStream\tStereo\tTracking\tSolo\tMusic Vol';
const line = (name, index, cols = {}) => {
  const c = [name, String(index), `${name}.wav`, '255', ...Array(18).fill('0')];
  for (const [i, v] of Object.entries(cols)) c[i] = String(v);
  return c.join('\t');
};
const TABLE = [HEADER,
  line('hit', 0, { 13: 1 }),                       // an ordinary mono row, Falloff 1, not Tracking
  line('step_1', 1, { 4: 4, 13: 1 }), line('step_2', 2), line('step_3', 3), line('step_4', 4),
  line('fire_loop', 5, { 5: 1, 6: 12, 7: 12, 13: 1, 19: 1 }),
  line('burst', 6, { 10: 25 }),
  line('mute', 7, { 3: 0 }),
  line('near', 8, { 13: 0, 19: 1 }),
  line('wide', 9, { 18: 1, 13: 1 }),
  line('defer', 10, { 8: 1 }),
  line('cut', 11, { 9: 1 }),
  line('rain', 12, { 3: 200, 5: 1, 6: 12, 7: 12, 13: 1 }),
].join('\n');

function wav() {
  const b = new Uint8Array(46), v = new DataView(b.buffer);
  for (const [o, s] of [[0, 'RIFF'], [8, 'WAVE'], [12, 'fmt '], [36, 'data']]) for (let i = 0; i < 4; i++) b[o + i] = s.charCodeAt(i);
  v.setUint32(4, 38, true); v.setUint32(16, 16, true); v.setUint16(20, 1, true); v.setUint16(22, 1, true); v.setUint32(24, 22050, true);
  v.setUint32(28, 44100, true); v.setUint16(32, 2, true); v.setUint16(34, 16, true); v.setUint32(40, 2, true);
  return b;
}

function rig(script = []) {
  const gains = [], sources = [], mergers = [], reads = [], clock = { now: 10 };
  class Ctx {
    state = 'running'; destination = {}; get currentTime() { return clock.now; }
    createGain() {
      const ramps = [];
      const node = { ramps, connections: [], connect(to) { node.connections.push(to); }, disconnect() {},
        gain: { value: 1, setValueAtTime(v, t) { ramps.push(['set', v, t]); }, linearRampToValueAtTime(v, t) { ramps.push(['ramp', v, t]); } } };
      gains.push(node); return node;
    }
    createChannelMerger() { const m = { connect() {}, disconnect() {} }; mergers.push(m); return m; }
    resume() { return Promise.resolve(); }
    decodeAudioData() { return Promise.resolve({ duration: 1 }); }
    createBufferSource() {
      const s = { stops: [], connect() {}, disconnect() {}, start(...a) { s.started = true; s.startArgs = a; }, stop(t) { s.stops.push(t); } };
      sources.push(s); return s;
    }
  }
  const bytes = wav(), asked = [];
  const read = (p) => { reads.push(p); return p === 'data\\global\\excel\\Sounds.txt' ? new TextEncoder().encode(TABLE) : bytes; };
  const ended = [];
  const player = createUnitSoundPlayer(read, Ctx, null, null, { variant: (row) => { asked.push(row); return script.shift() ?? row; }, ended: (h) => ended.push(h) });
  player.ended = ended;
  player.setVolume(1, { sound: 1, music: 1 });
  return { player, gains, sources, mergers, reads, asked, clock };
}
const settle = () => new Promise((resolve) => setImmediate(resolve));
const unitAt = (x, y = 0, id = 5) => ({ entities: [{ type: 1, id, preciseX: x, preciseY: y }] });
const LISTENER = { x: 0, y: 0 };

test('every mono row takes Falloff, the radial gain and the pan, not only the shrine', async () => {
  const r = rig();
  r.player.updateSpatialGain(unitAt(10), LISTENER);
  const rec = await r.player.playRow(0, { unitType: 1, unitId: 5 });
  assert.equal(rec.started, true);
  assert.deepEqual(rec.clientOffset, { x: 160, y: 160 });
  assert.equal(rec.appliedSpatialGainByte, 240);
  assert.equal(rec.nativeSpatialPanCode, 178, 'playRow pans: spatial is no longer forced off');
  assert.equal(r.mergers.length, 1);
  // Falloff 1 at 226 px: 188; with gain 240: 176.
  const node = r.gains.find((g) => Math.abs(g.gain.value - 176 / 255) < 1e-12);
  assert.ok(node, 'the source node carries channel 176/255');
});

test('a stereo row is neither attenuated by the radial gain nor panned', async () => {
  const r = rig();
  r.player.updateSpatialGain(unitAt(10), LISTENER);
  const rec = await r.player.playRow(9, { unitType: 1, unitId: 5 });
  assert.equal(rec.appliedSpatialGainByte, 255);
  assert.equal(r.mergers.length, 0);
});

test('a Group Size row plays the variant the engine picks; a queued start brings its own', async () => {
  const r = rig([3]);
  const a = await r.player.playRow(1);
  assert.equal(a.variantRow, 3);
  assert.equal(a.handle, 'step_3');
  assert.ok(r.reads.includes('data\\global\\sfx\\step_3.wav'));
  const b = await r.player.playRow(1, { variantRow: 4, queueHandle: 9 });
  assert.equal(b.variantRow, 4, 'the queue started variant 4');
  assert.equal(b.handle, 'step_4');
  assert.deepEqual(r.asked, [1], 'the engine is asked only for the unqueued play');
  r.sources.at(-1).onended();
  assert.deepEqual(r.player.ended, [9], 'the ended source frees its queue entry');
  const c = await r.player.playRow(7, { queueHandle: 11 });
  assert.equal(c.silent, true);
  assert.deepEqual(r.player.ended, [9, 11], 'a queued start that never sounds frees its entry at once');
});

test('Loop and Fade In: an attached looping row loops and ramps in over 12 ticks', async () => {
  const r = rig();
  r.player.updateSpatialGain(unitAt(2), LISTENER);
  const rec = await r.player.playRow(5, { unitType: 1, unitId: 5 });
  assert.equal(rec.looping, true);
  assert.equal(r.sources[0].loop, true);
  assert.equal(rec.fadeInSeconds, 12 * MIXER_TICK_SECONDS);
  // The level steps once per mixer tick, 0 -> 255 over 12 ticks (integer ramp), each step a channel value.
  const node = r.gains.find((g) => g.ramps.length);
  const g = rec.appliedSpatialGainByte;
  assert.deepEqual(node.ramps.map(([, , t]) => Number(t.toFixed(6))), Array.from({ length: 13 }, (_, k) => Number((10 + k * 0.04).toFixed(6))));
  assert.deepEqual(node.ramps.map(([, v]) => Math.round(v * 255)), Array.from({ length: 13 }, (_, k) => Math.trunc(Math.trunc(255 * k / 12) * g / 255)));
  // Its unit freed: the loop stops through Fade Out.
  r.player.stopUnitSounds(1, 5);
  assert.equal(rec.stopped, 'unit-freed');
  assert.equal(rec.fadeOutSeconds, 0.48);
  assert.deepEqual(r.sources[0].stops.map((t) => Number(t.toFixed(6))), [10.48]);
});

test('a looping row with nothing to stop it plays once', async () => {
  const r = rig();
  const rec = await r.player.playRow(5);
  assert.equal(rec.looping, false);
  assert.equal(rec.loopHeld, true);
  assert.equal(r.sources[0].loop, false);
});

test('a looping source stops when its unit leaves the snapshot', async () => {
  const r = rig();
  r.player.updateSpatialGain(unitAt(2), LISTENER);
  const rec = await r.player.playRow(5, { unitType: 1, unitId: 5 });
  r.player.updateSpatialGain({ entities: [] }, LISTENER);
  assert.equal(rec.stopped, 'unit-freed');
});

test('Duration stops the source after Duration + 1 ticks', async () => {
  const r = rig();
  const rec = await r.player.playRow(6);
  assert.equal(rec.durationSeconds, 26 * MIXER_TICK_SECONDS);
  assert.deepEqual(r.sources[0].stops.map((t) => Number(t.toFixed(6))), [11.04]);
});

test('Volume 0 never plays; a source past its cutoff never starts', async () => {
  const r = rig();
  const mute = await r.player.playRow(7);
  assert.equal(mute.silent, true);
  r.player.updateSpatialGain(unitAt(30), LISTENER);
  const far = await r.player.playRow(8, { unitType: 1, unitId: 5 });
  assert.equal(far.outOfRange, true);
  assert.equal(r.sources.length, 0);
});

test('a Tracking source follows its unit and stops at once past its cutoff', async () => {
  const r = rig();
  r.player.updateSpatialGain(unitAt(2), LISTENER);
  const rec = await r.player.playRow(8, { unitType: 1, unitId: 5 });
  assert.equal(rec.started, true);
  r.player.updateSpatialGain(unitAt(10), LISTENER);
  assert.deepEqual(rec.clientOffset, { x: 160, y: 160 });
  assert.equal(rec.appliedSpatialGainByte, 240);
  assert.equal(rec.stopped, undefined);
  r.player.updateSpatialGain(unitAt(30), LISTENER);
  assert.equal(rec.stopped, 'out-of-range');
  assert.equal(rec.fadeOutSeconds, 0);
  assert.deepEqual(r.sources[0].stops, [undefined]);
});

test('a looping Tracking source past its cutoff goes back to the queue and resumes from its place over 3 ticks', async () => {
  const r = rig();
  r.player.updateSpatialGain(unitAt(2), LISTENER);
  const rec = await r.player.playRow(5, { unitType: 1, unitId: 5, queueHandle: 9 });
  r.clock.now = 10.3;
  r.player.updateSpatialGain(unitAt(40), LISTENER);
  assert.equal(rec.stopped, 'out-of-range');
  assert.deepEqual(r.sources[0].stops, [undefined]);
  r.sources[0].onended();
  assert.deepEqual(r.player.ended, [], 'the parked entry keeps its queue slot');
  r.player.updateSpatialGain(unitAt(39), LISTENER);
  await settle();
  assert.equal(r.sources.length, 1, 'still beyond the cutoff: it waits');
  r.clock.now = 11;
  r.player.updateSpatialGain(unitAt(2), LISTENER);
  await settle();
  assert.equal(r.sources.length, 2);
  assert.equal(r.sources[1].loop, true);
  assert.deepEqual(r.sources[1].startArgs.map((v) => Number(v.toFixed(6))), [11, 0.3]);
  assert.equal(rec.fadeInSeconds, 3 * MIXER_TICK_SECONDS);
});

test('a looping row submitted past its cutoff waits for its unit, and leaves when the unit is gone', async () => {
  const r = rig();
  r.player.updateSpatialGain(unitAt(40), LISTENER);
  const rec = await r.player.playRow(5, { unitType: 1, unitId: 5, queueHandle: 4 });
  assert.equal(rec.outOfRange, true);
  assert.deepEqual(r.player.ended, []);
  r.player.updateSpatialGain(unitAt(2), LISTENER);
  await settle();
  assert.equal(rec.started, true);
  assert.equal(rec.fadeInSeconds, 12 * MIXER_TICK_SECONDS, 'a first start takes the row Fade In');
  r.player.updateSpatialGain(unitAt(40), LISTENER);
  const late = await r.player.playRow(5, { unitType: 1, unitId: 5, queueHandle: 8 });
  assert.equal(late.outOfRange, true);
  r.player.updateSpatialGain({ entities: [] }, LISTENER);
  assert.ok(r.player.ended.includes(8), 'its unit freed, the waiting entry leaves the queue');
});

test('Defer Inst drops a start while its group plays; Stop Inst cuts the earliest one and starts', async () => {
  const r = rig();
  const a = await r.player.playRow(10);
  const b = await r.player.playRow(10);
  assert.equal(a.started, true);
  assert.equal(b.deferred, true);
  assert.equal(b.started, false);
  const c = await r.player.playRow(11);
  r.clock.now = 10.1;
  const d = await r.player.playRow(11);
  assert.equal(c.stopped, 'stop-inst');
  assert.deepEqual(r.sources[1].stops, [undefined]);
  assert.equal(d.started, true);
  assert.equal(a.stopped, undefined, 'another group is untouched');
});

test('the weather loop fades in to its first level, follows later levels and fades out at 0', async () => {
  const r = rig();
  const rec = r.player.setWeatherLevel(12, 6);
  await settle();
  assert.equal(rec.started, true);
  assert.equal(r.sources[0].loop, true, 'a held loop loops with no unit');
  const node = r.gains.find((g) => g.ramps.length);
  // Volume 200, level 0 to 6 over 12 ticks.
  assert.deepEqual(node.ramps.map(([, v]) => Math.round(v * 255)), Array.from({ length: 13 }, (_, k) => Math.trunc(200 * Math.trunc(6 * k / 12) / 255)));
  r.player.setWeatherLevel(12, 12);
  assert.equal(node.gain.value, 1, 'inside the fade the ramp keeps going');
  r.clock.now = 10.5;
  r.player.setWeatherLevel(12, 12);
  assert.equal(Math.round(node.gain.value * 255), Math.trunc(200 * 12 / 255));
  r.player.setWeatherLevel(12, 0);
  assert.equal(rec.stopped, 'weather-ended');
  assert.equal(rec.fadeOutSeconds, 12 * MIXER_TICK_SECONDS);
  assert.equal(r.player.setWeatherLevel(12, 0), null);
});
