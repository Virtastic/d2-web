import test from 'node:test';
import assert from 'node:assert/strict';
import { createGameLife, flushUnloadLeave, EXIT_TICKS, JOIN_TIMEOUT_MS, LEAVE_CANCEL_PACKET, LEAVE_WAIT_CANCEL_PACKET, LEAVE_WAIT_MS, LOADING_TIMEOUT_MS, SAVE_AND_EXIT_PACKET } from './bnet-game-life.mjs';
import { reasonString } from './d2gs-deny.mjs';

const mk = (opts = {}) => { let t = 0; const log = { notice: [], left: [], sent: [] };
  const life = createGameLife({ now: () => t, onNotice: (s, r) => log.notice.push([s, r]), onLeave: (r) => log.left.push(r),
    onSend: (b) => log.sent.push([...b]), ...opts });
  return { life, log, at: (v) => { t = v; } }; };

test('save and exit is the single byte 0x69', () => {
  assert.deepEqual([...SAVE_AND_EXIT_PACKET], [0x69]);
});
test('the explicit leave sends the trade CANCEL bytes 4F 02 00 at once and waits in state 7', () => {
  const { life, log, at } = mk();
  life.inGame();
  assert.deepEqual([...LEAVE_CANCEL_PACKET], [0x4f, 0x02, 0, 0, 0, 0, 0]);
  assert.equal(life.leaveGame(), true);
  assert.equal(life.leaveGame(), true);
  assert.deepEqual(log.sent, [[0x4f, 0x02, 0, 0, 0, 0, 0], [0x4f, 0x02, 0, 0, 0, 0, 0]]);
  assert.equal(life.leaving, true);
  assert.equal(life.exiting, false);
});
test('a second exit during the wait restarts the 30 s from its own stamp', () => {
  const { life, log, at } = mk();
  life.inGame(); at(1000); life.leaveGame();
  at(20000); life.leaveGame();
  at(1000 + LEAVE_WAIT_MS + 1); life.checkTimers();
  assert.equal(log.left.length, 0);
  at(20000 + LEAVE_WAIT_MS + 1); life.checkTimers();
  assert.deepEqual(log.sent[2], [0x69]);
  assert.deepEqual(log.left, [0]);
});
test('0x69 goes out only after 30 s, strictly more than 30000 ms, then the teardown runs once', () => {
  const { life, log, at } = mk();
  life.inGame(); at(1000); life.leaveGame();
  at(1000 + LEAVE_WAIT_MS); life.checkTimers();
  assert.equal(log.sent.length, 1);
  assert.equal(log.left.length, 0);
  at(1000 + LEAVE_WAIT_MS + 1); life.checkTimers();
  assert.deepEqual(log.sent[1], [0x69]);
  assert.deepEqual(log.left, [0]);
  life.checkTimers(); life.tick();
  assert.equal(log.sent.length, 2);
  assert.equal(log.left.length, 1);
});
test('a deny during the leave wait is swallowed; the leave still completes', () => {
  const { life, log, at } = mk();
  life.inGame(); life.leaveGame();
  assert.equal(life.deny(0x10), false);
  assert.equal(log.notice.length, 0);
  at(LEAVE_WAIT_MS + 1); life.checkTimers();
  assert.deepEqual(log.left, [0]);
});
test('a leave after an exit countdown has started is refused, so the menu tears down at once', () => {
  const { life } = mk();
  life.inGame(); life.fail(7);
  assert.equal(life.leaveGame(), false);
});
test('unload while in game sends 4F 02 00 then 0x69 back to back, with no wait', () => {
  const { life } = mk();
  life.inGame();
  assert.deepEqual(life.unload().map((b) => [...b]), [[0x4f, 0x02, 0, 0, 0, 0, 0], [0x69]]);
  assert.deepEqual(life.unload(), []);
});
test('unload after the menu leave sends only 0x69, and never twice', () => {
  const { life } = mk();
  life.inGame(); life.leaveGame();
  assert.deepEqual(life.unload().map((b) => [...b]), [[0x69]]);
  assert.deepEqual(life.unload(), []);
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
test('an out-of-range reason becomes 9; a leave in progress blocks later reasons', () => {
  const a = mk(); a.life.fail(0x40); assert.equal(a.life.reason, 9);
  const b = mk(); b.life.leaveGame(); assert.equal(b.life.fail(7), false);
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

test('the wait cancel is 4F 07 with 0 padding, seven bytes', () => {
  assert.deepEqual([...LEAVE_WAIT_CANCEL_PACKET], [0x4f, 0x07, 0, 0, 0, 0, 0]);
});
test('an item action with no wait armed goes ahead and sends nothing', () => {
  const { life, log } = mk();
  life.inGame();
  assert.equal(life.itemAction(), true);
  assert.deepEqual(log.sent, []);
});
test('an item action during the armed wait is refused, sends 4F 07 and cancels the 30 s teardown', () => {
  const { life, log, at } = mk();
  life.inGame(); at(1000); life.leaveGame();
  at(1500);
  assert.equal(life.itemAction(), false);
  assert.deepEqual(log.sent, [[0x4f, 0x02, 0, 0, 0, 0, 0], [0x4f, 0x07, 0, 0, 0, 0, 0]]);
  assert.equal(life.leaving, false);
  at(1000 + LEAVE_WAIT_MS + 5000); life.checkTimers(); life.tick();
  assert.equal(log.sent.length, 2);
  assert.deepEqual(log.left, []);
});
test('after the cancel the menu exit re-arms the wait with a fresh 4F 02', () => {
  const { life, log, at } = mk();
  life.inGame(); at(0); life.leaveGame(); life.itemAction();
  at(10); assert.equal(life.leaveGame(), true);
  assert.deepEqual(log.sent[2], [0x4f, 0x02, 0, 0, 0, 0, 0]);
  assert.equal(life.leaving, true);
});
test('an item action after the wait has expired tears down at once and sends no 4F 07', () => {
  const { life, log, at } = mk();
  life.inGame(); at(0); life.leaveGame();
  at(LEAVE_WAIT_MS + 1);
  assert.equal(life.itemAction(), false);
  assert.deepEqual(log.sent, [[0x4f, 0x02, 0, 0, 0, 0, 0], [0x69]]);
  assert.deepEqual(log.left, [0]);
});
test('an item action during a reason countdown is not a leave wait, so it goes ahead', () => {
  const { life, log } = mk();
  life.inGame(); life.fail(7);
  assert.equal(life.itemAction(), true);
  assert.deepEqual(log.sent, []);
});

const fakeSocket = (open = true) => { const ev = []; return { ev, send: (b) => { if (!open) return false; ev.push(['send', [...b]]); return true; }, close: () => ev.push(['close']) }; };
test('pagehide in game: 4F 02 00, 0x69, then the socket close, in that order, synchronously', () => {
  const { life } = mk(); life.inGame();
  const ws = fakeSocket();
  assert.equal(flushUnloadLeave(life, ws).length, 2);
  assert.deepEqual(ws.ev, [['send', [0x4f, 0x02, 0, 0, 0, 0, 0]], ['send', [0x69]], ['close']]);
});
test('pagehide after the menu leave sends only 0x69 then closes; a second pagehide sends nothing', () => {
  const { life } = mk(); life.inGame(); life.leaveGame();
  const ws = fakeSocket();
  flushUnloadLeave(life, ws);
  assert.deepEqual(ws.ev, [['send', [0x69]], ['close']]);
  const again = fakeSocket();
  assert.deepEqual(flushUnloadLeave(life, again), []);
  assert.deepEqual(again.ev, []);
});
test('pagehide with no game life, a dead socket or an exit countdown running sends and closes nothing extra', () => {
  assert.deepEqual(flushUnloadLeave(null, fakeSocket()), []);
  const { life } = mk(); life.inGame();
  const dead = fakeSocket(false);
  assert.deepEqual(flushUnloadLeave(life, dead), []);
  assert.deepEqual(dead.ev, []);
  const b = mk(); b.life.inGame(); b.life.fail(7);
  const ws = fakeSocket();
  assert.deepEqual(flushUnloadLeave(b.life, ws), []);
  assert.deepEqual(ws.ev, []);
});
