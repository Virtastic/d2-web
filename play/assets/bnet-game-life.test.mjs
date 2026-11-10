import test from 'node:test';
import assert from 'node:assert/strict';
import { createGameLife, lifePhaseFromFrame, flushUnloadLeave, EXIT_TICKS, JOIN_TIMEOUT_MS, LEAVE_CANCEL_PACKET, LEAVE_WAIT_CANCEL_PACKET, LEAVE_WAIT_MS, LOADING_TIMEOUT_MS, SAVE_AND_EXIT_PACKET } from './bnet-game-life.mjs';
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
test('unload while in game sends 0x69 alone, with no 4F packet and no wait', () => {
  const { life } = mk();
  life.inGame();
  assert.deepEqual(life.unload().map((b) => [...b]), [[0x69]]);
  assert.deepEqual(life.unload(), []);
});
test('Save and Exit sends 0x69 at once, tears down once with reason 0, and sends no 4F packet', () => {
  const { life, log, at } = mk();
  life.inGame(); at(5000);
  assert.equal(life.saveAndExit(), true);
  assert.deepEqual(log.sent, [[0x69]]);
  assert.deepEqual(log.left, [0]);
  assert.equal(life.leaving, false, 'no state 7 wait is armed');
  at(5000 + LEAVE_WAIT_MS * 2); life.checkTimers(); life.tick();
  assert.deepEqual(log.sent, [[0x69]]);
  assert.deepEqual(log.left, [0]);
  assert.equal(life.saveAndExit(), false, 'a second exit does nothing');
  assert.deepEqual(life.unload(), []);
});
test('Save and Exit during an armed trade-cancel wait still sends 0x69 at once', () => {
  const { life, log, at } = mk();
  life.inGame(); at(0); life.leaveGame(); at(100);
  assert.equal(life.saveAndExit(), true);
  assert.deepEqual(log.sent, [[0x4f, 0x02, 0, 0, 0, 0, 0], [0x69]]);
  assert.deepEqual(log.left, [0]);
});
test('after Save and Exit with the wait armed, nothing more is sent: no second 0x69 when the 30 s passes, no 4F 07 for an item action', () => {
  const { life, log, at } = mk();
  life.inGame(); at(0); life.leaveGame(); at(100); life.saveAndExit();
  at(40000); life.checkTimers();
  assert.equal(life.itemAction(), true);
  assert.deepEqual(log.sent, [[0x4f, 0x02, 0, 0, 0, 0, 0], [0x69]]);
  assert.deepEqual(log.left, [0]);
});
test('Save and Exit during a reason countdown is refused, so the menu tears down at once', () => {
  const { life, log } = mk();
  life.inGame(); life.fail(7);
  assert.equal(life.saveAndExit(), false);
  assert.deepEqual(log.sent, []);
});
test('unload after the trade-cancel leave sends only 0x69, and never twice', () => {
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
  let t = 0;
  const deadline = joinDeadline({ timers: fake, now: () => t });
  assert.equal(fake.ms, 30000);
  assert.equal(deadline.expired, false);
  t = 30001; fake.jobs[0]();
  const error = await deadline.promise.catch((e) => e);
  assert.equal(error.noticeString, reasonString(6, false));
  assert.equal(deadline.expired, true);
  deadline.cancel(); assert.ok(fake.cleared);
});

test('the join deadline is strict: exactly 30000 ms does not fire, one more does', async () => {
  let t = 5000;
  const fake = { jobs: [], setTimeout(f, ms) { this.jobs.push([f, ms]); return this.jobs.length; }, clearTimeout() {} };
  const deadline = joinDeadline({ timers: fake, now: () => t });
  t += 30000; fake.jobs[0][0]();
  assert.equal(deadline.expired, false);
  assert.equal(fake.jobs[1][1], 1);
  t += 1; fake.jobs[1][0]();
  assert.equal(deadline.expired, true);
  await deadline.promise.catch(() => {});
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
test('pagehide in game: 0x69 then the socket close, in that order, synchronously', () => {
  const { life } = mk(); life.inGame();
  const ws = fakeSocket();
  assert.equal(flushUnloadLeave(life, ws).length, 1);
  assert.deepEqual(ws.ev, [['send', [0x69]], ['close']]);
});
test('pagehide after the trade-cancel leave sends only 0x69 then closes; a second pagehide sends nothing', () => {
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

test('a dead connection is reason 7 only after the game was entered; before that the join clock gives reason 6', () => {
  const a = mk(); assert.equal(a.life.connectionLost(), false); assert.equal(a.life.exiting, false);
  a.at(JOIN_TIMEOUT_MS + 1); a.life.checkTimers(); assert.equal(a.life.reason, 6);
  const b = mk(); b.life.inGame(); assert.equal(b.life.connectionLost(), true); assert.equal(b.life.reason, 7);
});
test('loading suspends the 30 s join clock and load complete ends it for good', () => {
  const a = mk(); a.life.loading(); a.at(JOIN_TIMEOUT_MS * 4); a.life.checkTimers(); assert.equal(a.life.exiting, false);
  a.life.inGame(); a.at(JOIN_TIMEOUT_MS * 20); a.life.checkTimers(); assert.equal(a.life.exiting, false);
});
test('0x05 starts loading, 0x04 enters the game, packet by packet', () => {
  const size = (b) => ({ 0x05: 1, 0x04: 1, 0x01: 8 }[b[0]] ?? 0);
  assert.equal(lifePhaseFromFrame(Uint8Array.of(0x05), size), 'loading');
  assert.equal(lifePhaseFromFrame(Uint8Array.of(0x05, 0x04), size), 'ingame');
  assert.equal(lifePhaseFromFrame(Uint8Array.of(0x04, 0x05), size), 'loading');   // the later packet overwrites the flags
  assert.equal(lifePhaseFromFrame(Uint8Array.of(0x01, 4, 4, 4, 4, 4, 4, 4, 0x05), size), 'loading');
  assert.equal(lifePhaseFromFrame(Uint8Array.of(0x01, 4, 4, 4, 4, 4, 4, 4), size), null);
});

test('a stock join stream (01 00 02 59 0B 03, then 04 in a later frame) enters the game only at 04', () => {
  const sz = { 0x00: 1, 0x01: 8, 0x02: 1, 0x03: 12, 0x04: 1, 0x0b: 6, 0x59: 26 };
  const size = (b) => sz[b[0]];
  const pkt = (op) => Uint8Array.from({ length: sz[op] }, (_, i) => (i === 0 ? op : 0));
  const cat = (...ops) => Uint8Array.from(ops.flatMap((op) => [...pkt(op)]));
  assert.equal(lifePhaseFromFrame(cat(0x01, 0x00, 0x02, 0x59, 0x0b, 0x03), size), null);
  assert.equal(lifePhaseFromFrame(cat(0x04), size), 'ingame');
});
test('the real server burst after 0x6B carries 0x59, 0x0B and 0x04 in one frame and enters the game', () => {
  const size = (b) => ({ 0x59: 26, 0x0b: 6, 0x1d: 3, 0x04: 1, 0x48: 11 }[b[0]] ?? 0);
  const pkt = (id, n) => { const a = new Uint8Array(n); a[0] = id; return a; };
  const frame = Uint8Array.from([...pkt(0x59, 26), ...pkt(0x0b, 6), ...pkt(0x1d, 3), ...pkt(0x48, 11), ...pkt(0x04, 1), ...pkt(0x48, 11)]);
  assert.equal(lifePhaseFromFrame(frame, size), 'ingame');
  assert.equal(lifePhaseFromFrame(frame.subarray(0, 26 + 6 + 3 + 11), size), null);   // before 0x04 nothing is decided
});
