import test from 'node:test';
import assert from 'node:assert/strict';
import { bncsLogin } from './realm-client.mjs';

const hex = (b) => Buffer.from(b).toString('hex');

// A Battle.net server sends SID_PING (0x25) right after the selector, before the AUTH_INFO reply
// (captured: ff250800afd35900). The login must echo it and keep waiting for the reply, not abort.
test('bncsLogin echoes a leading SID_PING and then reads the AUTH_INFO reply', async () => {
  const sent = []; let onData = () => {};
  const transport = {
    isOpen: true, close() {}, onClose() {},
    onData: (cb) => { onData = cb; },
    send: (b) => {
      sent.push(hex(b));
      if (sent.length === 2) {   // selector + AUTH_INFO sent: the server answers ping first, then the reply
        onData(Uint8Array.from(Buffer.from('ff250800afd35900', 'hex')));
        const body = Buffer.concat([Buffer.alloc(20), Buffer.from('ver-IX86-1.mpq\0A=1 B=2 C=3 4 A=A+S B=B-C C=C^A A=A^B\0')]);
        const head = Buffer.from([0xff, 0x50, (body.length + 4) & 255, (body.length + 4) >> 8]);
        onData(Uint8Array.from(Buffer.concat([head, body])));
      }
      return true;
    },
  };
  let asked = null;
  const protocol = {
    bncsSelector: () => Uint8Array.of(1),
    bncsFrame: (id, body) => Uint8Array.from([0xff, id, (body.length + 4) & 255, 0, ...body]),
    bncsAuthInfo: () => new Uint8Array(54),
    bncsAuthCheck: (a) => { asked = a; throw new Error('stop after AUTH_INFO'); },
  };
  await assert.rejects(bncsLogin(transport, protocol, { account: 'a', password: 'b', timeoutMs: 500 }), /stop after AUTH_INFO/);
  assert.ok(sent.includes('ff250800afd35900'), 'ping echoed');
  assert.equal(asked.mpq, 'ver-IX86-1.mpq');
});
