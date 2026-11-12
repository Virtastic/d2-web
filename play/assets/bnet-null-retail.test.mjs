// BNCS SID_NULL keep-alive vs a stock client on the wire. The vector is a recorded session of an unmodified
// client logged on to a local PvPGN through a byte-exact relay: logon, chat, /f commands, a game create, LEAVECHAT,
// idle; PvPGN echoes each client NULL and the relay injects one extra server NULL. The stock client sent FF 00 04 00
// at 180.092 s and 360.162 s after the BNCS connection opened.
// The session is replayed through startBncsNull on a fake clock: every other client frame goes through our transport
// at its recorded time (so chat, 0x15/0x21 ad polls, 0x1C and 0x10 are shown not to move the base) and no server NULL
// draws a frame back. The stock network pump polls in a 100 ms socket select and compares GetTickCount; the base
// becomes the tick of the poll that sent, so each stock NULL lands up to one poll after the 180 s mark measured
// from the previous one. Our NULLs must match retail's in bytes, count and spacing to that poll.
// The vector is private; without it the test skips.
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import { startBncsNull, BNCS_NULL_MS, BNCS_NULL_FRAME } from './realm-client.mjs';

const path = new URL('../../vectors/bnet-null/live1-rec.jsonl', import.meta.url).pathname;
const have = fs.existsSync(path);
const hex = (b) => Buffer.from(b).toString('hex');
const POLL_MS = 100;    // the pump's select timeout (tv_usec 100000)
const SLACK_MS = 50;    // GetTickCount granularity plus the relay's own timestamps
const RETAIL_NULL = 'ff000400';   // the client frame retail put on the wire, both times

// The BNCS connection of the recording (the one whose client stream is 0x01 then SID_AUTH_INFO): client and server
// frames with their time in ms since the connection opened; `end` is when either side closed it.
function bncsSession(text) {
  const rows = text.trim().split('\n').map((l) => JSON.parse(l));
  const id = rows.find((r) => r.ev === 'chunk' && r.dir === 'c2s' && /^(01)?ff50/.test(r.hex)).id, open = rows.find((r) => r.ev === 'open' && r.id === id).t;
  const ms = (t) => Math.round((t - open) * 1000);
  const out = { c2s: [], s2c: [], end: null };
  const bufs = { c2s: Buffer.alloc(0), s2c: Buffer.alloc(0) };
  let skipped = false;
  for (const r of rows) {
    if (r.id !== id) continue;
    if ((r.ev === 'end' || r.ev === 'close') && out.end === null) out.end = ms(r.t);
    if (r.ev !== 'chunk' && r.ev !== 'inject') continue;
    let b = Buffer.concat([bufs[r.dir], Buffer.from(r.hex, 'hex')]);
    if (r.dir === 'c2s' && !skipped) { b = b.subarray(1); skipped = true; }
    while (b.length >= 4 && b[0] === 0xff && b.length >= b.readUInt16LE(2)) {
      const n = b.readUInt16LE(2);
      out[r.dir].push({ at: ms(r.t), hex: b.subarray(0, n).toString('hex') });
      b = b.subarray(n);
    }
    bufs[r.dir] = b;
  }
  return out;
}

test('our SID_NULL frame is the bytes retail sent: FF 00 04 00', () => {
  assert.equal(hex(BNCS_NULL_FRAME), RETAIL_NULL);
  assert.equal(BNCS_NULL_MS, 180000);
});

test('retail recording: two client NULLs, FF 00 04 00, 180 s apart from the connection open', { skip: !have }, () => {
  const s = bncsSession(fs.readFileSync(path, 'utf8'));
  const nulls = s.c2s.filter((f) => f.hex.startsWith('ff00'));
  assert.deepEqual(nulls.map((f) => f.hex), [RETAIL_NULL, RETAIL_NULL]);
  let prev = 0;
  for (const f of nulls) {
    const gap = f.at - prev;
    assert.ok(gap >= BNCS_NULL_MS - SLACK_MS && gap <= BNCS_NULL_MS + POLL_MS + SLACK_MS, `retail NULL at ${f.at} ms, ${gap} ms after the last`);
    prev = f.at;
  }
  assert.ok(s.s2c.filter((f) => f.hex === RETAIL_NULL).length >= 3, 'the recording holds the server NULLs it was taken for');
});

test('retail recording: our keep-alive replayed over retail\'s session sends the same NULLs, spaced as retail', { skip: !have }, () => {
  const s = bncsSession(fs.readFileSync(path, 'utf8'));
  let t = 0, id = 0; const timers = new Map(), ours = [];
  const tr = { isOpen: true, send(b) { ours.push({ at: t, hex: hex(b) }); return true; } };
  startBncsNull(tr, { now: () => t, setTimer: (fn, ms) => { timers.set(++id, { at: t + ms, fn }); return id; }, clearTimer: (k) => timers.delete(k) });
  const runTo = (to) => {
    for (;;) {
      let k = null, due = null;
      for (const [kk, v] of timers) if (v.at <= to && (!due || v.at < due.at)) { k = kk; due = v; }
      if (!due) break;
      timers.delete(k); t = due.at; due.fn();
    }
    t = to;
  };
  // Every recorded event in time order: other client frames are sent through our transport, server frames
  // (PvPGN's NULL echoes and the injected NULL among them) only advance the clock: our client has no path that answers them.
  const events = [...s.c2s.filter((f) => !f.hex.startsWith('ff00')).map((f) => ({ ...f, send: true })), ...s.s2c].sort((a, b) => a.at - b.at);
  for (const e of events) { runTo(e.at); if (e.send) tr.send(Buffer.from(e.hex, 'hex')); }
  runTo(s.end);
  const oursNull = ours.filter((f) => f.hex.startsWith('ff00')), retailNull = s.c2s.filter((f) => f.hex.startsWith('ff00'));
  assert.equal(oursNull.length, retailNull.length, `ours at ${oursNull.map((f) => f.at)} ms, retail at ${retailNull.map((f) => f.at)} ms`);
  let po = 0, pr = 0;
  oursNull.forEach((f, i) => {
    assert.equal(f.hex, retailNull[i].hex);
    const d = (retailNull[i].at - pr) - (f.at - po);
    assert.ok(d >= -SLACK_MS && d <= POLL_MS + SLACK_MS, `NULL ${i}: ours ${f.at - po} ms after the last, retail ${retailNull[i].at - pr} ms`);
    po = f.at; pr = retailNull[i].at;
  });
});
