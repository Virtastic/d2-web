// The area ambience loop's Fade In / Fade Out and the day/night cross-fade (0x4F860, 0x88230, 0x88550): the level
// steps once per mixer tick along an integer ramp; each step is a channel byte of the row's Volume.
import test from 'node:test';
import assert from 'node:assert/strict';
import { createAmbientSound, soundByIndex, AMBIENT_SWAP_TICKS } from './ambient-sound.mjs';

const wav = () => { const b = new Uint8Array(48), v = new DataView(b.buffer); b.set([...Buffer.from('RIFF')], 0); v.setUint32(4, 40, true); b.set([...Buffer.from('WAVEfmt ')], 8); v.setUint32(16, 16, true); v.setUint16(20, 1, true); v.setUint16(22, 1, true); v.setUint32(24, 22050, true); v.setUint32(28, 44100, true); v.setUint16(32, 2, true); v.setUint16(34, 16, true); b.set([...Buffer.from('data')], 36); v.setUint32(40, 4, true); return b; };
const files = {
  Levels: 'Id\tSoundEnv\n1\t1\n2\t2',
  SoundEnviron: 'Index\tDay Ambience\tNight Ambience\tDay Event\tNight Event\tEvent Delay\n0\t0\t0\t0\t0\t0\n1\t10\t11\t0\t0\t0\n2\t20\t21\t0\t0\t0',
  Sounds: 'Sound\tIndex\tFileName\tVolume\tGroup Size\tLoop\tFade In\tFade Out\nx\t10\ttown.wav\t100\t0\t1\t12\t12\nx\t11\ttownn.wav\t100\t0\t1\t12\t12\nx\t20\tbirds.wav\t80\t0\t1\t12\t12\nx\t21\tcrick.wav\t90\t0\t1\t6\t300',
};
function fake() {
  const clock = { now: 5 }, nodes = [], sources = [];
  class Ctx {
    state = 'running'; destination = {}; get currentTime() { return clock.now; }
    createGain() {
      const n = { sets: [], gain: { value: 1, setValueAtTime(v, t) { n.sets.push([v, t]); }, cancelScheduledValues(t) { n.sets = n.sets.filter(([, at]) => at < t); } }, connect() {}, disconnect() {} };
      nodes.push(n); return n;
    }
    createBufferSource() { const s = { stops: [], connect() {}, disconnect() {}, start() { s.started = true; }, stop(t) { s.stops.push(t); } }; sources.push(s); return s; }
    async decodeAudioData() { return {}; }
    async resume() {}
  }
  const read = (p) => (p.includes('excel') ? Buffer.from(files[p.match(/(\w+)\.txt/)[1]], 'latin1') : wav());
  return { clock, nodes, sources, Ctx, read };
}
const ramp = (volume, from, to, ticks) => Array.from({ length: ticks + 1 }, (_, k) => Math.trunc(volume * (Math.trunc((to - from) * k / ticks) + from) / 255));
const bytes = (n) => n.sets.map(([v]) => Math.round(v * 255));
const times = (n) => n.sets.map(([, t]) => Number(t.toFixed(6)));

test('soundByIndex reads the Fade In and Fade Out columns', () => {
  assert.deepEqual(soundByIndex(files.Sounds, 21), { id: 21, file: 'crick.wav', volume: 90, fadeIn: 6, fadeOut: 300 });
});

test('a new area loop fades in over its row Fade In, one level step per tick', async () => {
  const f = fake(), a = createAmbientSound(f.read, f.Ctx);
  await a.sync(2, 0);
  assert.deepEqual(bytes(f.nodes[1]), ramp(80, 0, 255, 12));
  assert.deepEqual(times(f.nodes[1]), Array.from({ length: 13 }, (_, k) => Number((5 + k * 0.04).toFixed(6))));
});

test('another area: the old loop fades out over its Fade Out and stops at the end, the new fades in', async () => {
  const f = fake(), a = createAmbientSound(f.read, f.Ctx);
  await a.sync(2, 0);
  f.clock.now = 7;
  await a.sync(1, 0);
  assert.deepEqual(bytes(f.nodes[1]).slice(13), ramp(80, 255, 0, 12));
  assert.deepEqual(f.sources[0].stops.map((t) => Number(t.toFixed(6))), [7.48]);
  assert.deepEqual(bytes(f.nodes[2]), ramp(100, 0, 255, 12));
  assert.equal(a.evidence.loopId, 10);
});

test('day to night in one area cross-fades over the longer of 250 ticks and the row fade', async () => {
  const f = fake(), a = createAmbientSound(f.read, f.Ctx);
  await a.sync(2, 0);
  f.clock.now = 7;
  await a.sync(2, 1);
  assert.equal(AMBIENT_SWAP_TICKS, 250);
  assert.deepEqual(bytes(f.nodes[1]).slice(13), ramp(80, 255, 0, 250), 'day row Fade Out 12: 250 ticks');
  assert.deepEqual(f.sources[0].stops.map((t) => Number(t.toFixed(6))), [17]);
  assert.deepEqual(bytes(f.nodes[2]), ramp(90, 0, 255, 250), 'night row Fade In 6: 250 ticks');
  f.clock.now = 30;
  await a.sync(2, 0);
  assert.equal(f.sources[1].stops.length, 1);
  assert.deepEqual(bytes(f.nodes[2]).slice(251), ramp(90, 255, 0, 300), 'night row Fade Out 300: its own');
});

test('a slider change inside a fade re-steps the rest of it from the current tick', async () => {
  const f = fake(), a = createAmbientSound(f.read, f.Ctx);
  await a.sync(2, 0);
  f.clock.now = 5 + 6 * 0.04;
  a.setVolume(0.5, { sound: 0.5, music: 1 });
  const n = f.nodes[1];
  const rest = n.sets.filter(([, t]) => t >= f.clock.now - 1e-9);
  assert.equal(rest.length, 7);
  assert.deepEqual(rest.map(([v]) => Math.round(v * 255)), [6, 7, 8, 9, 10, 11, 12].map((k) => Math.trunc(80 * Math.trunc(Math.trunc(255 * k / 12) * 50 / 100) / 255)));
});
