import test from 'node:test';
import assert from 'node:assert/strict';
import { bncsCreateAccount, bncsChangePassword } from './realm-client.mjs';

const frame = (id, body) => Uint8Array.from([0xff, id, (body.length + 4) & 255, (body.length + 4) >> 8, ...body]);
const authInfoBody = Buffer.concat([Buffer.alloc(20), Buffer.from('ver-IX86-1.mpq\0A=1 B=2 C=3 4 A=A+S B=B-C C=C^A A=A^B\0')]);

// The server refuses an account request until AUTH_CHECK has passed, so the packet ids sent must run 0x50, 0x51, then the request.
function rig(replyTo3d = [0, 0, 0, 0]) {
  const ids = []; let onData = () => {};
  const transport = {
    isOpen: true, close() {}, onClose() {}, onData: (cb) => { onData = cb; },
    send: (b) => {
      if (b[0] === 0xff) {
        ids.push(b[1]);
        if (b[1] === 0x50) onData(frame(0x50, authInfoBody));
        else if (b[1] === 0x51) onData(frame(0x51, [0, 0, 0, 0]));
        else onData(frame(b[1], replyTo3d));
      }
      return true;
    },
  };
  const protocol = {
    bncsSelector: () => Uint8Array.of(1),
    bncsFrame,
    bncsAuthInfo: () => new Uint8Array(54),
    bncsAuthCheck: () => ({ body: new Uint8Array(8), clientToken: 1 }),
  };
  return { ids, transport, protocol };
}
function bncsFrame(id, body) { return frame(id, body); }

test('bncsCreateAccount passes AUTH_CHECK before it sends the create packet', async () => {
  const { ids, transport, protocol } = rig();
  await bncsCreateAccount(transport, protocol, { account: 'ama', password: 'pw', timeoutMs: 500 });
  assert.deepEqual(ids.slice(0, 3), [0x50, 0x51, 0x3d]);
});

test('bncsChangePassword passes AUTH_CHECK before its own packet', async () => {
  const { ids, transport, protocol } = rig();
  await bncsChangePassword(transport, protocol, { account: 'ama', password: 'pw', newPassword: 'pw2', timeoutMs: 500 }).catch(() => {});
  assert.deepEqual(ids.slice(0, 2), [0x50, 0x51]);
});

// The account screens and the login share the connection the connect opened (retail has one Bnclient connection): the
// version check runs once on it, and a request after it, or the LOG IN after a request, goes straight to its own packet.
test('an account request then another on the same connection checks the version once', async () => {
  const { ids, transport, protocol } = rig([1, 0, 0, 0]);
  protocol.bncsLogon = () => new Uint8Array(48); protocol.tickCount = () => 1;
  await bncsChangePassword(transport, protocol, { account: 'ama', password: 'pw', newPassword: 'pw2', timeoutMs: 500 });
  await bncsChangePassword(transport, protocol, { account: 'ama', password: 'pw2', newPassword: 'pw3', timeoutMs: 500 });
  assert.deepEqual(ids, [0x50, 0x51, 0x31, 0x31]);
  assert.equal(transport.bncsAuth.clientToken, 1);
  assert.equal(transport.bncsAuth.serverToken, 0);
});

test('a version or key reply that never comes raises the timeout event, a closed link the generic one', async () => {
  const { loginErrorMessage } = await import('./bnet-login-screen.mjs');
  const string = (i) => `s${i}`;
  const silent = (reply) => { const r = rig(); const send = r.transport.send; r.transport.send = (b) => (b[1] === reply ? true : send(b)); return r; };
  for (const stalled of [0x50, 0x51]) {
    const { transport, protocol } = silent(stalled);
    const error = await bncsCreateAccount(transport, protocol, { account: 'ama', password: 'pw', timeoutMs: 20 }).then(() => null, (e) => e);
    assert.equal(error?.failureEvent, 0x11, `stalled ${stalled.toString(16)}`);
    assert.deepEqual(loginErrorMessage(error, 'ama', string), { title: 's5248', body: 's5354' });
  }
  assert.deepEqual(loginErrorMessage(new Error('transport closed after 0/4 bytes'), 'ama', string), { title: 's5248', body: 's5347' });
});

test('a link lost right after the AUTH_INFO reply raises event 0xB, not the generic one', async () => {
  const { transport, protocol } = rig();
  let close = () => {}; const onData = transport.onData, send = transport.send;
  transport.onClose = (cb) => { close = cb; };
  transport.send = (b) => { const ok = send(b); if (b[1] === 0x50) close(); return ok; };
  void onData;
  const error = await bncsCreateAccount(transport, protocol, { account: 'ama', password: 'pw', timeoutMs: 500 }).then(() => null, (e) => e);
  assert.equal(error?.failureEvent, 0xb);
});

test('the 45 s reply wait is one deadline: a slow header and body together do not get it twice', async () => {
  const { transport, protocol } = rig();
  const send = transport.send; let feed = () => {};
  transport.onData = (cb) => { feed = cb; };
  transport.send = (b) => { if (b[1] === 0x50) { const f = frame(0x50, authInfoBody); feed(f.subarray(0, 4)); return true; } return send(b); };
  const t = Date.now();
  const error = await bncsCreateAccount(transport, protocol, { account: 'ama', password: 'pw', timeoutMs: 100 }).then(() => null, (e) => e);
  assert.equal(error?.failureEvent, 0x11);
  assert.ok(Date.now() - t < 180, `took ${Date.now() - t} ms`);
});
