import test from 'node:test';
import assert from 'node:assert/strict';
import { advanceIzualGhostS1Clock, resetIzualGhostS1Clock } from '../play/assets/izual-ghost-clock.mjs';

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
  const actor = { id: 9, classId: 406, mode: 8, gameFrame: 1200, frameCountPrecise: 20 * 256, animSpeed: 120 };
  let clock = advanceIzualGhostS1Clock(actor);
  assert.equal(clock.framePrecise, 0);
  clock = advanceIzualGhostS1Clock({ ...actor, gameFrame: 1201 }, clock);
  assert.equal(clock.framePrecise, 120);
  clock = advanceIzualGhostS1Clock({ ...actor, gameFrame: 1203 }, clock);
  assert.equal(clock.framePrecise, 360);
});

test('class-406 S1 mode-8 policy clamps at the terminal boundary and holds the final cel', () => {
  const actor = { classId: 406, mode: 8, gameFrame: 10, frameCountPrecise: 20 * 256, animSpeed: 0x100 };
  let clock = { mode: 8, gameFrame: 9, framePrecise: 19 * 256 };
  clock = advanceIzualGhostS1Clock(actor, clock);
  assert.equal(clock.framePrecise, 19 * 256);
  clock = advanceIzualGhostS1Clock({ ...actor, gameFrame: 11 }, clock);
  assert.equal(clock.framePrecise, 19 * 256);
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

test('the F37 clock does not apply to other classes, modes, or sequence-backed units', () => {
  const actor = { classId: 406, mode: 8, gameFrame: 10, frameCountPrecise: 20 * 256, animSpeed: 17 };
  assert.equal(advanceIzualGhostS1Clock({ ...actor, classId: 405 }), null);
  assert.equal(advanceIzualGhostS1Clock({ ...actor, mode: 1 }), null);
  assert.equal(advanceIzualGhostS1Clock({ ...actor, hasAnimSequence: true }), null);
});
