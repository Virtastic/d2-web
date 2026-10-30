import test from 'node:test';
import assert from 'node:assert/strict';
import { createGameClock, STEP, MAX_CATCHUP_UPDATES, rafStarved, RAF_STARVED_MS } from './game-clock.mjs';

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

// rAF at 1 Hz on a visible page (the player's log: gap 1000, updates 15, ticks 6, hidden false), with the shell's
// 40 ms starvation timer sharing the clock and `last`, as gameplay-shell.mjs runs them. No wall clock.
function starvedRun(withTimer) {
  const c = createGameClock(); let last = 0, rafAt = 0, ticks = 0;
  const pay = (now) => { for (let k = c.updatesFor(Math.max(0, now - last)); k > 0; k--) ticks += c.ticksForUpdate(); last = now; };
  for (let now = 40; now <= 10000; now += 40) {
    if (now % 1000 === 0) { pay(now); rafAt = now; }
    else if (withTimer && rafStarved(now, rafAt, true)) pay(now);
  }
  return ticks;
}

test('rAF starved to 1 Hz on a visible page: the timer keeps 25 ticks a second', () => {
  const ticks = starvedRun(true);
  assert.ok(Math.abs(ticks / 10 - 25) < 0.5, `${ticks / 10} ticks/s`);
});

test('control: without the timer a 1 Hz rAF runs the game at a quarter speed', () => {
  assert.ok(starvedRun(false) / 10 < 7, `${starvedRun(false) / 10} ticks/s`);
});

test('a hidden page or a recent frame is not starved', () => {
  assert.equal(rafStarved(1000, 0, false), false);
  assert.equal(rafStarved(1000, 1000 - RAF_STARVED_MS + 1, true), false);
  assert.equal(rafStarved(1000, 1000 - RAF_STARVED_MS, true), true);
});
