import test from 'node:test';
import assert from 'node:assert/strict';
import { advanceIzualGhostS1Clock, resetIzualGhostS1Clock } from '../play/assets/izual-ghost-clock.mjs';
import { parseMonsterAnimations } from '../play/assets/monster-renderer.mjs';

test('class-406 S1 starts at local frame zero and advances on the session frame clock', () => {
  const actor = { id: 9, classId: 406, mode: 8, gameFrame: 800, frameCountPrecise: 20 * 256, animSpeed: 17 };
  let clock = advanceIzualGhostS1Clock(actor);
  assert.deepEqual(clock, { mode: 8, gameFrame: 800, framePrecise: 0 });
  clock = advanceIzualGhostS1Clock({ ...actor, gameFrame: 801 }, clock);
  assert.equal(clock.framePrecise, 17);
  clock = advanceIzualGhostS1Clock({ ...actor, gameFrame: 802 }, clock);
  assert.equal(clock.framePrecise, 34);
});

test('class-406 S1 uses the recovered native client speed of 120', () => {
  const bytes = new Uint8Array(96), view = new DataView(bytes.buffer);
  view.setUint32(0, 9, true); view.setUint32(4, 406, true); view.setUint32(12, 8, true);
  view.setUint32(20, 20 * 256, true); view.setUint16(88, 120, true);
  const actor = { ...parseMonsterAnimations(bytes, 1)[0], gameFrame: 1200 };
  let clock = advanceIzualGhostS1Clock(actor);
  assert.equal(clock.framePrecise, 0);
  clock = advanceIzualGhostS1Clock({ ...actor, gameFrame: 1201 }, clock);
  assert.equal(clock.framePrecise, 120);
  clock = advanceIzualGhostS1Clock({ ...actor, gameFrame: 1203 }, clock);
  assert.equal(clock.framePrecise, 360);
});

test('class-406 S1 holds its current precise frame at the terminal step and waits for the mode snapshot', () => {
  const actor = { classId: 406, mode: 8, gameFrame: 42, frameCountPrecise: 20 * 256, animSpeed: 120 };
  // 5040 + 120 reaches 5120. The monster fallback row selects policy 1, which skips this step;
  // policy 2 would clamp to 4864 and falsely describe the separate player-mode table.
  let clock = { mode: 8, gameFrame: 41, framePrecise: 5040 };
  clock = advanceIzualGhostS1Clock(actor, clock);
  assert.equal(clock.framePrecise, 5040);
  assert.equal(clock.framePrecise >>> 8, 19, 'the held precise value still draws the final S1 cel');
  assert.equal(clock.mode, 8);
  clock = advanceIzualGhostS1Clock({ ...actor, gameFrame: 43 }, clock);
  assert.deepEqual(clock, { mode: 8, gameFrame: 43, framePrecise: 5040 });

  // D2Game's end-animation event requests NU; presentation follows once that engine mode arrives.
  assert.equal(advanceIzualGhostS1Clock({ ...actor, mode: 1, gameFrame: 44 }, clock), null);
});

test('class-406 S1 catch-up holds the terminal frame across batched engine frames', () => {
  const actor = { classId: 406, mode: 8, frameCountPrecise: 20 * 256, animSpeed: 120 };
  const start = { mode: 8, gameFrame: 9, framePrecise: 19 * 256 - 120 };
  const terminal = advanceIzualGhostS1Clock({ ...actor, gameFrame: 13 }, start);
  assert.deepEqual(terminal, { mode: 8, gameFrame: 13, framePrecise: 5104 });

  const stepped = advanceIzualGhostS1Clock({ ...actor, gameFrame: 14 }, terminal);
  const caughtUp = advanceIzualGhostS1Clock({ ...actor, gameFrame: 14 }, start);
  assert.deepEqual(caughtUp, stepped);
  assert.deepEqual(caughtUp, { mode: 8, gameFrame: 14, framePrecise: 5104 });
});

test('a session reset starts a reused actor ID at frame zero', () => {
  const clocks = new Map();
  const oldSession = { id: 9, classId: 406, mode: 8, gameFrame: 800, frameCountPrecise: 20 * 256, animSpeed: 17 };
  clocks.set(oldSession.id, advanceIzualGhostS1Clock(oldSession));
  clocks.set(oldSession.id, advanceIzualGhostS1Clock({ ...oldSession, gameFrame: 810 }, clocks.get(oldSession.id)));
  assert.equal(clocks.get(oldSession.id).framePrecise, 170);

  resetIzualGhostS1Clock(clocks);
  const newSession = { ...oldSession, gameFrame: 0 };
  const clock = advanceIzualGhostS1Clock(newSession, clocks.get(newSession.id));
  assert.deepEqual(clock, { mode: 8, gameFrame: 0, framePrecise: 0 });
});

test('leaving S1 and reentering it starts a fresh local clock', () => {
  const actor = { id: 9, classId: 406, mode: 8, gameFrame: 800, frameCountPrecise: 20 * 256, animSpeed: 17 };
  const clocks = new Map();
  let clock = advanceIzualGhostS1Clock(actor, clocks.get(actor.id));
  if (clock) clocks.set(actor.id, clock); else clocks.delete(actor.id);
  clock = advanceIzualGhostS1Clock({ ...actor, gameFrame: 810 }, clocks.get(actor.id));
  if (clock) clocks.set(actor.id, clock); else clocks.delete(actor.id);
  assert.equal(clock.framePrecise, 170);

  clock = advanceIzualGhostS1Clock({ ...actor, mode: 1, gameFrame: 811 }, clocks.get(actor.id));
  if (clock) clocks.set(actor.id, clock); else clocks.delete(actor.id);
  assert.equal(clocks.has(actor.id), false);
  clock = advanceIzualGhostS1Clock({ ...actor, gameFrame: 812 }, clocks.get(actor.id));
  if (clock) clocks.set(actor.id, clock); else clocks.delete(actor.id);
  assert.deepEqual(clock, { mode: 8, gameFrame: 812, framePrecise: 0 });
});

test('a native frame reset restarts S1 even when the intermediate mode was never drawn', () => {
  const actor = { id: 9, classId: 406, mode: 8, gameFrame: 800, framePrecise: 0,
    frameCountPrecise: 20 * 256, animSpeed: 120 };
  let clock = advanceIzualGhostS1Clock(actor);
  clock = advanceIzualGhostS1Clock({ ...actor, gameFrame: 802, framePrecise: 240 }, clock);
  assert.equal(clock.framePrecise, 240);

  // The native #11090 mode setter resets the CUnit frame on a real mode change. If the
  // away-and-back assignment occurs between renders, this rollback is the surviving signal.
  clock = advanceIzualGhostS1Clock({ ...actor, gameFrame: 803, framePrecise: 0 }, clock);
  assert.equal(clock.framePrecise, 0);
  clock = advanceIzualGhostS1Clock({ ...actor, gameFrame: 804, framePrecise: 120 }, clock);
  assert.equal(clock.framePrecise, 120);
});

test('a same-mode native refresh preserves the S1 clock when its frame does not reset', () => {
  const actor = { id: 9, classId: 406, mode: 8, gameFrame: 800, framePrecise: 0,
    frameCountPrecise: 20 * 256, animSpeed: 120 };
  let clock = advanceIzualGhostS1Clock(actor);
  clock = advanceIzualGhostS1Clock({ ...actor, gameFrame: 802, framePrecise: 240 }, clock);
  clock = advanceIzualGhostS1Clock({ ...actor, gameFrame: 803, framePrecise: 360 }, clock);
  assert.equal(clock.framePrecise, 360);
});

test('the F37 clock does not apply to other classes, modes, or sequence-backed units', () => {
  const actor = { classId: 406, mode: 8, gameFrame: 10, frameCountPrecise: 20 * 256, animSpeed: 17 };
  assert.equal(advanceIzualGhostS1Clock({ ...actor, classId: 405 }), null);
  assert.equal(advanceIzualGhostS1Clock({ ...actor, mode: 1 }), null);
  assert.equal(advanceIzualGhostS1Clock({ ...actor, hasAnimSequence: true }), null);
});
