import test from 'node:test';
import assert from 'node:assert/strict';
import { mcpListCharacters, mcpCreateAndJoin } from './realm-client.mjs';

// A fake MCP realm behind a fake transport. `drop()` models the proxy that closes an idle socket.
const protocol = {
  mcpSelector: () => [1],
  mcpFrame: (op, body) => [(body.length + 3) & 255, (body.length + 3) >> 8, op, ...body],
  mcpStartup: () => [],
  mcpCharlist: () => [8, 0, 0, 0],
  mcpCharlogon: (n) => [...Buffer.from(`${n}\0`)],
  mcpCreateGame: ({ requestId }) => [requestId & 255, requestId >> 8],
  mcpJoinGame: ({ requestId }) => [requestId & 255, requestId >> 8],
};
const frame = (op, body) => Buffer.from([(body.length + 3) & 255, 0, op, ...body]);
const u32 = (n) => [n & 255, (n >> 8) & 255, 0, 0];
function realm({ idleMs = 100 } = {}) {
  let dataCb = () => {}, closeCbs = [], open = true, timer = null; const log = [];
  const emit = (b) => queueMicrotask(() => dataCb(new Uint8Array(b)));
  const arm = () => { clearTimeout(timer); timer = setTimeout(() => { open = false; closeCbs.forEach((c) => c()); }, idleMs); };
  const stat = Buffer.alloc(30, 1); stat[13] = 2; stat[25] = 5;
  let loggedOn = false;
  const reply = (op, body) => {
    log.push(op);
    // d2cs: MOTD is valid only after CHARLOGON; anything else is a state mismatch and the socket is dropped.
    if (op === 0x12 && !loggedOn) { open = false; clearTimeout(timer); closeCbs.forEach((c) => c()); return; }
    if (op === 0x07) loggedOn = true;
    if (op === 0x01) emit(frame(1, u32(0)));
    if (op === 0x19) emit(frame(0x19, [0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, ...Buffer.from('Ama\0'), ...stat, 0]));
    if (op === 0x07) emit(frame(7, u32(0)));
    if (op === 0x12) emit(frame(0x12, [0, 0x68, 0x69, 0]));   // MOTD answer
    if (op === 0x03) emit(frame(3, [body[0], body[1], 0x9e, 0, 0, 0, ...u32(0)]));
    if (op === 0x04) emit(frame(4, [body[0], body[1], 0x9e, 0, 0, 0, 10, 0, 0, 1, 1, 0, 0, 0, ...u32(0)]));
  };
  let acc = Buffer.alloc(0);
  const transport = {
    get isOpen() { return open; },
    send(bytes) {
      if (!open) return false; arm(); acc = Buffer.concat([acc, Buffer.from(bytes)]);
      if (acc[0] === 1 && log.length === 0 && acc.length === 1) { acc = acc.subarray(1); return true; }
      while (acc.length >= 3 && acc.length >= (acc[0] | (acc[1] << 8))) {
        const n = acc[0] | (acc[1] << 8); reply(acc[2], acc.subarray(3, n)); acc = acc.subarray(n);
      }
      return true;
    },
    onData: (cb) => { dataCb = cb; }, onClose: (cb) => closeCbs.push(cb), close() { open = false; clearTimeout(timer); },
  };
  arm();
  return { transport, log };
}

test('an idle lobby socket is kept alive; create and join then succeed', async () => {
  const { transport, log } = realm({ idleMs: 100 });
  const roster = await mcpListCharacters(transport, protocol, { realmReply: [], keepAliveMs: 30 });
  await new Promise((r) => setTimeout(r, 350));            // 3.5 idle windows
  assert.equal(transport.isOpen, true);
  assert.ok(log.filter((o) => o === 0x19).length >= 6, 'pre-logon keep-alive is a character-list request');
  assert.equal(log.includes(0x12), false, 'no MOTD before CHARLOGON');
  const joined = await mcpCreateAndJoin(transport, protocol, { realmReply: [], character: 'Ama', gameName: 'g', session: roster.session });
  assert.equal(joined.ticket.token, 0x9e);
  assert.deepEqual(joined.received.filter((o) => o !== 0x12 && o !== 0x19).slice(-3), [0x07, 0x03, 0x04]);
  transport.close();
});

test('without the keep-alive the same idle wait kills the socket (the reported 0/3)', async () => {
  const { transport } = realm({ idleMs: 100 });
  const roster = await mcpListCharacters(transport, protocol, { realmReply: [], keepAliveMs: 1e9 });
  await new Promise((r) => setTimeout(r, 250));
  await assert.rejects(mcpCreateAndJoin(transport, protocol, { realmReply: [], character: 'Ama', gameName: 'g', session: roster.session }),
    /transport closed after 0\/3 bytes/);
});
