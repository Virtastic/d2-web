import { test } from 'node:test';
import assert from 'node:assert/strict';
import { d2gsJoin } from './realm-client.mjs';

import { isEngineAbort, callEngine, canRetryJoin, closeAfterSave } from './realm-client.mjs';

test('a wasm abort is recognised and reported as a dead engine', () => {
  const abort = Object.assign(new Error('Aborted(). Build with -sASSERTIONS for more info.'), { name: 'RuntimeError' });
  assert.equal(isEngineAbort(abort), true);
  assert.equal(isEngineAbort(new Error('socket closed')), false);
  let dead = null;
  assert.equal(callEngine(() => { throw abort; }, { onDead: (e) => { dead = e; } }), false);
  assert.equal(dead, abort);
  assert.equal(callEngine(() => { throw new Error('other'); }, { onDead: () => assert.fail('not dead') }), true);
  assert.equal(callEngine(() => {}), true);
});

test('a join is retried only while the one-time logon has not gone out', () => {
  assert.equal(canRetryJoin(new Error('drop')), true);
  assert.equal(canRetryJoin(Object.assign(new Error('drop'), { logonSent: false })), true);
  assert.equal(canRetryJoin(Object.assign(new Error('drop'), { logonSent: true })), false);
  assert.equal(canRetryJoin(new Error('drop'), { attempt: 2 }), false);
  assert.equal(canRetryJoin(new Error('drop'), { expired: true }), false);
  assert.equal(canRetryJoin(Object.assign(new Error('Aborted()'), { name: 'RuntimeError' })), false);
});

test('d2gsJoin marks a failure after the logon was sent', async () => {
  const sent = [];
  const transport = { send: (b) => { sent.push(b); return true; }, onData() {}, onClose() {}, close() {}, isOpen: true };
  const protocol = { greetingParse: () => (2 << 8) | 1, d2gsGamelogon: () => Uint8Array.of(0x68), d2gsClientPlayerReset() {}, d2gsClientSetDifficulty() {} };
  const wire = []; let cb = null;
  transport.onData = (f) => { cb = f; }; 
  const p = d2gsJoin(transport, protocol, { character: 'x', timeoutMs: 50 });
  await new Promise((r) => setTimeout(r, 5)); cb?.(Uint8Array.of(0xaf, 1));
  await assert.rejects(p, (e) => e.logonSent === true);
  assert.equal(sent.length, 1);
  const closedFirst = { ...transport, send: () => false };
  await assert.rejects(d2gsJoin(closedFirst, { ...protocol }, { character: 'x', timeoutMs: 20 }), (e) => e.logonSent === false);
});

test('Save and Exit waits for the server to close the socket, but not past the limit', async () => {
  let closeCb = null, closed = 0;
  const t = { isOpen: true, onClose: (cb) => { closeCb = cb; }, close: () => { closed++; } };
  const p = closeAfterSave(t, { ms: 5000 });
  assert.equal(closed, 0, 'not closed from our side yet');
  closeCb();
  assert.equal(await p, 'closed');
  let fire = null;
  const stuck = { isOpen: true, onClose() {}, close: () => { closed++; } };
  const q = closeAfterSave(stuck, { ms: 3000, setTimer: (f) => { fire = f; return 1; }, clearTimer() {} });
  fire();
  assert.equal(await q, 'timeout');
  assert.equal(await closeAfterSave({ isOpen: false, onClose() { assert.fail(); }, close() {} }), 'closed');
});
