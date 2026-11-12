import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import { soundFalloffRadii, soundClientOffset, soundRangeSquared, soundInStartRange, soundBeyondCutoff, nativeSoundChannel,
  soundFadeLevel, soundLoops } from './sound-mixer-rules.mjs';
import { soundRow, soundRowByHandle } from './unit-sound-resolver.mjs';

const HEADER = 'Sound\tIndex\tFileName\tVolume\tGroup Size\tLoop\tFade In\tFade Out\tDefer Inst\tStop Inst\tDuration\tCompound\tReverb\tFalloff\tCache\tAsync Only\tPriority\tStream\tStereo\tTracking\tSolo\tMusic Vol\tBlock 1\tBlock 2\tBlock 3';
const line = (name, index, file, cols = {}) => {
  const c = [name, String(index), file, '255', ...Array(21).fill('0')];
  for (const [i, v] of Object.entries(cols)) c[i] = String(v);
  return c.join('\t');
};

test('Falloff selects the two client-pixel radii', () => {
  assert.deepEqual([0, 1, 2, 3, 4, 5, 9].map(soundFalloffRadii), [
    { outer: 60, cutoff: 400 }, { outer: 60, cutoff: 700 }, { outer: 200, cutoff: 1000 }, { outer: 400, cutoff: 1500 },
    { outer: 2000, cutoff: 2000 }, { outer: 60, cutoff: 700 }, { outer: 60, cutoff: 700 }]);
});

test('the source offset is in client pixels, 16 per subtile on both axes', () => {
  assert.deepEqual(soundClientOffset({ x: 10, y: 0 }, { x: 0, y: 0 }), { x: 160, y: 160 });
  assert.deepEqual(soundClientOffset({ x: 5, y: 5 }, { x: 5, y: 5 }), { x: 0, y: 0 });
  assert.equal(soundClientOffset({ x: 1 }, { x: 0, y: 0 }), null);
});

test('start and stop range: strict against the cutoff, each axis clamped to 2000', () => {
  assert.equal(soundInStartRange(null, 0), true, 'a source with no place starts');
  assert.equal(soundInStartRange({ x: 399, y: 0 }, 0), true);
  assert.equal(soundInStartRange({ x: 400, y: 0 }, 0), false);
  assert.equal(soundBeyondCutoff({ x: 400, y: 0 }, 0), false, 'exactly at the cutoff neither starts nor stops');
  assert.equal(soundBeyondCutoff({ x: 401, y: 0 }, 0), true);
  assert.equal(soundRangeSquared({ x: 5000, y: -5000 }), 8000000);
  assert.equal(soundBeyondCutoff({ x: 5000, y: 0 }, 4), false, 'one clamped axis sits exactly on the Falloff 4 cutoff');
  assert.equal(soundBeyondCutoff({ x: 5000, y: 1 }, 4), true);
});

test('the channel byte: Music, Master, Falloff and Volume, each stage truncated', () => {
  assert.equal(nativeSoundChannel({ volume: 255, falloff: 1 }, 100, 100, { x: 0, y: 0 }), 255);
  assert.equal(nativeSoundChannel({ volume: 255, falloff: 1 }, 100, 100, { x: 60, y: 0 }), 255, 'inside outer: whole');
  // 160 px with Falloff 1: (700-160)*255/640 = 215.15 -> 215; Volume 200: 200*215/255 = 168.6 -> 168.
  assert.equal(nativeSoundChannel({ volume: 255, falloff: 1 }, 100, 100, { x: 160, y: 0 }), 215);
  assert.equal(nativeSoundChannel({ volume: 200, falloff: 1 }, 100, 100, { x: 160, y: 0 }), 168);
  assert.equal(nativeSoundChannel({ volume: 255, falloff: 0 }, 100, 100, { x: 500, y: 0 }), 0, 'beyond the cutoff clamps to 0');
  assert.equal(nativeSoundChannel({ volume: 255, falloff: 4 }, 100, 100, { x: 2100, y: 0 }), 0);
  assert.equal(nativeSoundChannel({ volume: 110, music: true }, 80, 55), 48, '255 -> 140 -> 112 -> 48');
  assert.equal(nativeSoundChannel({ volume: 110, music: false }, 80, 55), 88);
  assert.equal(nativeSoundChannel({ volume: 255, falloff: 1 }, 100, 100, null, 128), 128, 'a fade level feeds the same chain');
});

test('fades ramp linearly in mixer ticks; Loop only without a Duration', () => {
  assert.deepEqual([0, 3, 6, 12, 13].map((t) => soundFadeLevel(0, 255, 0, 12, t)), [0, 63, 127, 255, 255]);
  assert.deepEqual([0, 6, 12].map((t) => soundFadeLevel(200, 0, 0, 12, t)), [200, 100, 0]);
  assert.equal(soundLoops({ loop: true, duration: 0 }), true);
  assert.equal(soundLoops({ loop: true, duration: 25 }), false);
  assert.equal(soundLoops({ loop: false, duration: 0 }), false);
});

test('the resolver reads every Sounds.txt column and the loader group base', () => {
  const table = [HEADER,
    line('a', 0, 'a.wav', { 3: 200, 5: 1, 6: 12, 7: 12, 13: 1, 16: 100 }),
    line('step_1', 1, 's1.wav', { 4: 4, 13: 1 }), line('step_2', 2, 's2.wav'), line('step_3', 3, 's3.wav'), line('step_4', 4, 's4.wav'),
    line('after', 5, 'after.wav', { 10: 25, 18: 1, 19: 1, 20: 1, 21: 1, 22: -1 })].join('\n');
  const a = soundRow(table, 0);
  assert.deepEqual([a.volume, a.loop, a.fadeIn, a.fadeOut, a.falloff, a.priority, a.groupSize], [200, true, 12, 12, 1, 100, 1]);
  assert.deepEqual([1, 2, 3, 4, 5].map((r) => soundRow(table, r).groupBase), [1, 1, 1, 1, 5]);
  assert.equal(soundRow(table, 1).groupSize, 4);
  const after = soundRowByHandle(table, 'after');
  assert.deepEqual([after.duration, after.stereo, after.tracking, after.solo, after.music, after.blocks[0]], [25, true, true, true, true, -1]);
});

const RETAIL = new URL('../../retail/tables-1.13c/Sounds.txt', import.meta.url);
test('retail rows: the shrine, a footstep group and the rain ambience', { skip: existsSync(RETAIL) ? false : 'retail tables absent' }, () => {
  const t = readFileSync(RETAIL, 'latin1');
  const shrine = soundRow(t, 2673), step = soundRow(t, 2768), rain = soundRowByHandle(t, 'scene_rain');
  assert.deepEqual([shrine.falloff, shrine.tracking, shrine.stereo], [1, true, false]);
  assert.deepEqual([step.groupSize, step.falloff, soundRow(t, 2771).groupBase], [4, 1, 2768]);
  assert.deepEqual([rain.loop, rain.fadeIn, rain.fadeOut, rain.falloff, rain.volume], [true, 12, 12, 1, 200]);
});
