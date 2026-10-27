import test from 'node:test';
import assert from 'node:assert/strict';
import { createGameClock, STEP, MAX_CATCHUP_UPDATES } from './game-clock.mjs';

// Drawn subtiles per real second = engine ticks per real second x per-tick rate (walk 0.375, run 0.55).
// Retail: 25 ticks/s. Fixed rAF timestamps, no wall clock.
function ticksIn(frameTimes) {
  const c = createGameClock(); let last = frameTimes[0], ticks = 0, perFrameMax = 0;
  for (const now of frameTimes.slice(1)) {
    let t = 0; for (let k = c.updatesFor(now - last); k > 0; k--) t += c.ticksForUpdate();
    last = now; ticks += t; perFrameMax = Math.max(perFrameMax, t);
  }
  return { ticks, perFrameMax, seconds: (frameTimes.at(-1) - frameTimes[0]) / 1000 };
}
const every = (ms, n) => Array.from({ length: n + 1 }, (_, i) => i * ms);
const rate = (r) => r.ticks / r.seconds;

test('steady 60 / 144 / 30 Hz display: 25 ticks a second', () => {
  for (const ms of [1000 / 60, 1000 / 144, 1000 / 30]) {
    const r = ticksIn(every(ms, Math.round(10000 / ms)));
    assert.ok(Math.abs(rate(r) - 25) < 0.2, `${ms} ms frames: ${rate(r)}`);
  }
});

test('a stall never runs more ticks than the real time it covers', () => {
  const t = [0]; for (let i = 0; i < 100; i++) t.push(t.at(-1) + STEP);
  t.push(t.at(-1) + 3000); for (let i = 0; i < 100; i++) t.push(t.at(-1) + STEP);   // one 3 s stall
  const r = ticksIn(t);
  assert.ok(r.ticks <= Math.ceil(r.seconds * 25), `${r.ticks} ticks in ${r.seconds}s`);
  assert.ok(r.perFrameMax <= Math.ceil(STEP * MAX_CATCHUP_UPDATES / 40) + 1, `catch-up burst ${r.perFrameMax}`);
});

test('a slow-but-steady 100 ms frame loop still gets 25 ticks a second (was 15: the 5-update cap)', () => {
  const r = ticksIn(every(100, 100));
  assert.ok(Math.abs(rate(r) - 25) < 0.3, `${rate(r)}`);
});

test('control: the old 5-update cap fails the 100 ms check', () => {
  let acc = 0, g = 0, ticks = 0;
  for (let i = 0; i < 100; i++) { acc = Math.min(acc + 100, STEP * 5); while (acc >= STEP) { acc -= STEP; g += STEP; while (g >= 40) { g -= 40; ticks++; } } }
  assert.ok(Math.abs(ticks / 10 - 25) > 5, `old rate ${ticks / 10}`);
});
