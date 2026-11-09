import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmResync } from './realm-resync.mjs';

const pkt = [0x5f, 0x10, 0x00, 0x20, 0x00];

test('a pending 0x5F is sent once as the five bytes {0x5F, x, y}', () => {
  const sent = [];
  let pending = pkt;
  const r = createRealmResync({ take: () => { const p = pending; pending = null; return p; }, send: (b) => { sent.push(b); return true; }, now: () => 0 });
  assert.deepEqual(r.drain(), pkt);
  assert.equal(r.drain(), null);
  assert.deepEqual(sent, [pkt]);
});

test('an identical packet inside 200 ms is dropped, as the 0x143E0 wrapper does', () => {
  let t = 0;
  const sent = [];
  let pending = pkt;
  const r = createRealmResync({ take: () => { const p = pending; pending = null; return p; }, send: (b) => { sent.push(b); return true; }, now: () => t });
  r.drain();
  pending = pkt; t = 199;
  assert.equal(r.drain(), null);
  pending = pkt; t = 200;
  assert.deepEqual(r.drain(), pkt);
  assert.equal(sent.length, 2);
});

test('a refused send is not counted, so the next drain retries it', () => {
  let accept = false;
  let pending = pkt;
  const r = createRealmResync({ take: () => { const p = pending; pending = null; return p; }, send: () => accept, now: () => 0 });
  assert.equal(r.drain(), null);
  pending = pkt; accept = true;
  assert.deepEqual(r.drain(), pkt);
});

test('anything that is not a 5-byte 0x5F is ignored', () => {
  const r = createRealmResync({ take: () => [0x38, 1, 0, 0, 0], send: () => { throw new Error('must not send'); }, now: () => 0 });
  assert.equal(r.drain(), null);
});
