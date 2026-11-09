import test from 'node:test';
import assert from 'node:assert/strict';
import { bncsLogin } from './realm-client.mjs';

// A refused logon keeps the connection, and the next LOG IN reuses it. The server allows one selector and one
// AUTH_INFO/AUTH_CHECK per connection (a second 0x01 gets the socket closed), so the retry must send only the logon.
const fr = (id, body) => Buffer.concat([Buffer.from([0xff, id, (body.length + 4) & 255, (body.length + 4) >> 8]), Buffer.from(body)]);
const u32 = (n) => [n & 255, (n >> 8) & 255, (n >> 16) & 255, (n >>> 24) & 255];

test('a second bncsLogin on the same transport skips the selector and auth, and the server stays up', async () => {
  const sent = []; let onData = () => {}, open = true, logons = 0;
  const push = (b) => queueMicrotask(() => onData(Uint8Array.from(b)));
  const transport = {
    get isOpen() { return open; }, close() { open = false; }, onClose() {},
    onData: (cb) => { onData = cb; },
    send(b) {
      const buf = Buffer.from(b);
      if (buf.length === 1 && buf[0] === 1) { if (sent.some((s) => s.sel)) { open = false; return false; } sent.push({ sel: true }); return true; }
      sent.push({ id: buf[1] });
      if (buf[1] === 0x50) push(fr(0x50, [...u32(0), ...u32(7), ...Buffer.alloc(8), ...Buffer.from('ver.mpq\0f\0')]));
      if (buf[1] === 0x51) push(fr(0x51, [...u32(0), 0]));
      if (buf[1] === 0x3a) { logons++; if (logons === 2) push(fr(0x59, [])); push(fr(0x3a, u32(logons === 1 ? 1 : 0))); }   // the second logon is preceded by a SETEMAIL prompt   // first: no such account
      if (buf[1] === 0x40) push(fr(0x40, [...u32(0), ...u32(0)]));
      if (buf[1] === 0x3e) push(fr(0x3e, [...u32(0), ...u32(1), ...u32(0), ...u32(0), 10, 0, 0, 1, 0x18, 0xe0, 0, 0, ...Buffer.alloc(48)]));
      return true;
    },
  };
  const protocol = {
    bncsSelector: () => Uint8Array.of(1),
    bncsFrame: (id, body) => Uint8Array.from([0xff, id, (body.length + 4) & 255, 0, ...body]),
    bncsAuthInfo: () => new Uint8Array(4),
    bncsAuthCheck: () => ({ body: [1], clientToken: 5 }),
    bncsLogon: ({ serverToken }) => [serverToken, 0],
    bncsQueryRealms: () => [],
    bncsRealmEx: () => [0],
    tickCount: () => 1, realmToken: () => 2,
  };
  await assert.rejects(bncsLogin(transport, protocol, { account: 'a', password: 'b', timeoutMs: 500 }), (e) => e.logonResult === 1);
  const second = bncsLogin(transport, protocol, { account: 'a', password: 'b', timeoutMs: 500 });
  const res = await second.catch((e) => e);
  assert.equal(transport.isOpen, true, 'the retry did not get the socket closed');
  assert.equal(sent.filter((s) => s.sel).length, 1, 'one selector per connection');
  assert.equal(sent.filter((s) => s.id === 0x50).length, 1, 'one AUTH_INFO per connection');
  assert.equal(sent.filter((s) => s.id === 0x3a).length, 2, 'the retry sent its logon');
  assert.notEqual(res?.logonResult, 1, 'the retry reached a later step than the refused logon');
});
