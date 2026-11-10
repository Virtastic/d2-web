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
