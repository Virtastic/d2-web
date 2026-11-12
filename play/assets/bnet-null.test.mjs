// BNCS SID_NULL keep-alive: BNClient's network pump sends FF 00 04 00 once 180 s (cmp ecx,0x2bf20) have passed
// since the last one, and never answers a server's 0x00. Fake clock: every timing is exact, no real wait.
import test from 'node:test';
import assert from 'node:assert/strict';
import { startBncsNull, BNCS_NULL_MS, BNCS_NULL_FRAME } from './realm-client.mjs';
import { bncsPacket, createBnetChat } from './bnet-chat.mjs';

const hex = (b) => Buffer.from(b).toString('hex');
const NULL = 'ff000400';

// A fake clock: `advance(ms)` runs every timer due within the window, in order, at its own due time.
function clock() {
  let t = 0, id = 0; const timers = new Map();
  return {
    now: () => t,
    setTimer: (fn, ms) => { const k = ++id; timers.set(k, { at: t + ms, fn }); return k; },
    clearTimer: (k) => { timers.delete(k); },
    advance(ms) {
      const end = t + ms;
      for (;;) {
        let due = null, key = null;
        for (const [k, v] of timers) if (v.at <= end && (!due || v.at < due.at)) { due = v; key = k; }
        if (!due) break;
        t = due.at; timers.delete(key); due.fn();
      }
      t = end;
    },
    get pending() { return timers.size; },
  };
}

// A transport that records what it is sent and can be closed or refuse sends.
function transport() {
  const sent = [];
  const tr = { isOpen: true, sent, send(b) { if (!tr.isOpen) return false; sent.push(hex(b)); return true; }, close() { tr.isOpen = false; } };
  return tr;
}

test('SID_NULL is FF 00 04 00 and first goes out at exactly 180000 ms, not a millisecond earlier', () => {
  assert.equal(BNCS_NULL_MS, 180000);
  assert.equal(hex(BNCS_NULL_FRAME), NULL);
  const c = clock(), tr = transport();
  startBncsNull(tr, c);
  c.advance(179999); assert.deepEqual(tr.sent, []);
  c.advance(1); assert.deepEqual(tr.sent, [NULL]);
});

test('after a NULL the next one follows 180000 ms later, and only then', () => {
  const c = clock(), tr = transport();
  startBncsNull(tr, c);
  c.advance(540000); assert.deepEqual(tr.sent, [NULL, NULL, NULL]);
  c.advance(179999); assert.equal(tr.sent.length, 3);
  c.advance(1); assert.equal(tr.sent.length, 4);
});

test('other traffic does not move the base: a send made outside the keep-alive leaves the 180 s mark where it was', () => {
  const c = clock(), tr = transport();
  startBncsNull(tr, c);
  c.advance(100000); tr.send(bncsPacket(0x0e, [0x68, 0x69, 0])); // a chat line at 100 s
  c.advance(79999); assert.deepEqual(tr.sent.slice(1), []);
  c.advance(1); assert.equal(tr.sent[tr.sent.length - 1], NULL);
  assert.equal(tr.sent.filter((x) => x === NULL).length, 1);
});

test('one keep-alive per connection: a second start is a no-op', () => {
  const c = clock(), tr = transport();
  startBncsNull(tr, c); startBncsNull(tr, c);
  assert.equal(c.pending, 1);
  c.advance(180000); assert.deepEqual(tr.sent, [NULL]);
});

test('a closed transport ends the keep-alive at its next due time without a send', () => {
  const c = clock(), tr = transport();
  startBncsNull(tr, c);
  tr.isOpen = false;
  c.advance(180000); assert.deepEqual(tr.sent, []);
  assert.equal(c.pending, 0);
});

test('a refused send ends the keep-alive', () => {
  const c = clock(), tr = transport();
  tr.send = () => false;
  startBncsNull(tr, c);
  c.advance(180000); assert.equal(c.pending, 0);
  c.advance(900000); assert.equal(c.pending, 0);
});

test('stop() cancels the pending timer', () => {
  const c = clock(), tr = transport();
  const stop = startBncsNull(tr, c);
  stop(); assert.equal(c.pending, 0);
  c.advance(360000); assert.deepEqual(tr.sent, []);
});

test('a server SID_NULL is flagged, never answered: the chat sends nothing back', async () => {
  const sent = []; let buf = new Uint8Array(0); const waiters = [];
  const pump = () => { for (let i = waiters.length - 1; i >= 0; i--) if (buf.length >= waiters[i].n) { const w = waiters.splice(i, 1)[0]; w.resolve(buf.subarray(0, w.n)); } };
  const reader = { want: (n) => new Promise((resolve) => { waiters.push({ n, resolve }); pump(); }),
    skip: (n) => { buf = buf.subarray(n); }, cancel: () => {} };
  const tr = { isOpen: true, send(b) { sent.push(hex(b)); return true; }, close() { tr.isOpen = false; } };
  const chat = createBnetChat({ transport: tr, reader, character: 'Ama' });
  const frame = bncsPacket(0x00); buf = Uint8Array.from(frame); pump();
  await new Promise((r) => setImmediate(r));
  assert.deepEqual(sent, []);
  chat.close();
});
