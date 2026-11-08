import test from 'node:test';
import assert from 'node:assert/strict';
import { createGameLife, EXIT_TICKS, JOIN_TIMEOUT_MS, LOADING_TIMEOUT_MS, SAVE_AND_EXIT_PACKET } from './bnet-game-life.mjs';
import { reasonString } from './d2gs-deny.mjs';

const mk = (opts = {}) => { let t = 0; const log = { notice: [], left: [] };
  const life = createGameLife({ now: () => t, onNotice: (s, r) => log.notice.push([s, r]), onLeave: (r) => log.left.push(r), ...opts });
  return { life, log, at: (v) => { t = v; } }; };

test('save and exit is the single byte 0x69, sent once', () => {
  const { life } = mk();
  assert.deepEqual([...SAVE_AND_EXIT_PACKET], [0x69]);
  assert.equal(life.saveAndExit(), SAVE_AND_EXIT_PACKET);
  assert.equal(life.saveAndExit(), null);
});
test('a deny shows its text and leaves after 600 ticks', () => {
  const { life, log } = mk();
  life.inGame();
  assert.equal(life.deny(0x10), true);
  assert.equal(log.notice.length, 1);
  for (let i = 1; i < EXIT_TICKS; i++) assert.equal(life.tick(), false);
  assert.equal(life.tick(), true);
  assert.deepEqual(log.left.length, 1);
  assert.equal(life.tick(), false);
});
test('the first reason wins', () => {
  const { life, log } = mk();
  life.inGame();
  life.connectionLost();
  assert.equal(life.hostLeft(), false);
  assert.equal(log.notice[0][1], 7);
});
test('30 s join timeout is reason 6, not once in game', () => {
  const a = mk(); a.at(JOIN_TIMEOUT_MS + 1); a.life.checkTimers();
  assert.equal(a.life.reason, 6);
  const b = mk(); b.life.inGame(); b.at(JOIN_TIMEOUT_MS * 5); b.life.checkTimers();
  assert.equal(b.life.exiting, false);
});
test('3 minutes of loading is reason 7', () => {
  const { life, at } = mk();
  life.loading(); at(LOADING_TIMEOUT_MS); life.checkTimers(); assert.equal(life.exiting, false);
  at(LOADING_TIMEOUT_MS + 1); life.checkTimers(); assert.equal(life.reason, 7);
});
test('expansion characters get the expansion wording for reasons 0x14 and 0x15', () => {
  const { life, log } = mk({ expansion: true });
  life.fail(0x14); assert.equal(log.notice[0][0], 21794);
});
test('an out-of-range reason becomes 9; save and exit blocks later reasons', () => {
  const a = mk(); a.life.fail(0x40); assert.equal(a.life.reason, 9);
  const b = mk(); b.life.saveAndExit(); assert.equal(b.life.fail(7), false);
});

import { joinDeadline } from './bnet-game-life.mjs';
test('a join that has not entered the game after 30 seconds fails with the timeout notice', async () => {
  const fake = { jobs: [], setTimeout(f, ms) { this.ms = ms; this.jobs.push(f); return 1; }, clearTimeout() { this.cleared = true; } };
  const deadline = joinDeadline({ timers: fake });
  assert.equal(fake.ms, 30000);
  assert.equal(deadline.expired, false);
  fake.jobs[0]();
  const error = await deadline.promise.catch((e) => e);
  assert.equal(error.noticeString, reasonString(6, false));
  assert.equal(deadline.expired, true);
  deadline.cancel(); assert.ok(fake.cleared);
});
