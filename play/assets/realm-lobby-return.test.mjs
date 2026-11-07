import test from 'node:test';
import assert from 'node:assert/strict';
import { mcpReturnToLobby, mcpCreateAndJoin } from './realm-client.mjs';
import { returnToLobby } from './realm-lobby-return.mjs';

// A fake realm that records the request ops of one MCP connection and answers like the real one.
const protocol = {
  mcpSelector: () => [1], mcpFrame: (op, body) => [(body.length + 3) & 255, (body.length + 3) >> 8, op, ...body],
  mcpStartup: () => [], mcpCharlist: () => [8, 0, 0, 0], mcpCharlogon: (n) => [...Buffer.from(`${n}\0`)],
  mcpCreateGame: ({ requestId }) => [requestId & 255, requestId >> 8], mcpJoinGame: ({ requestId }) => [requestId & 255, requestId >> 8],
  free() { this.freed = true; },
};
const frame = (op, body) => Buffer.from([(body.length + 3) & 255, 0, op, ...body]);
const u32 = (n) => [n & 255, (n >> 8) & 255, 0, 0];
function realm({ logonResult = 0, names = ['Ama'] } = {}) {
  let dataCb = () => {}, open = true, acc = Buffer.alloc(0); const ops = [], logons = [];
  const emit = (b) => queueMicrotask(() => dataCb(new Uint8Array(b)));
  const stat = Buffer.alloc(30, 1); stat[13] = 2; stat[25] = 5;
  const list = Buffer.concat(names.map((n) => Buffer.concat([Buffer.alloc(4), Buffer.from(`${n}\0`), stat, Buffer.from([0])])));
  const reply = (op, body) => {
    ops.push(op);
    if (op === 0x01) emit(frame(1, u32(0)));
    if (op === 0x19) emit(frame(0x19, [0, 0, names.length, 0, 0, 0, names.length, 0, ...list]));
    if (op === 0x07) { logons.push(Buffer.from(body).toString().replace(/\0$/, '')); emit(frame(7, u32(logonResult))); }
    if (op === 0x03) emit(frame(3, [body[0], body[1], 0x9e, 0, 0, 0, ...u32(0)]));
    if (op === 0x04) emit(frame(4, [body[0], body[1], 0x9e, 0, 0, 0, 10, 0, 0, 1, 1, 0, 0, 0, ...u32(0)]));
  };
  const transport = {
    get isOpen() { return open; },
    send(bytes) {
      acc = Buffer.concat([acc, Buffer.from(bytes)]);
      if (acc[0] === 1 && ops.length === 0 && acc.length === 1) { acc = acc.subarray(1); return true; }
      while (acc.length >= 3 && acc.length >= (acc[0] | (acc[1] << 8))) { const n = acc[0] | (acc[1] << 8); reply(acc[2], acc.subarray(3, n)); acc = acc.subarray(n); }
      return true;
    },
    onData: (cb) => { dataCb = cb; }, onClose: () => {}, close() { open = false; },
  };
  return { transport, ops, logons };
}

test('mcpReturnToLobby: STARTUP, CHARLIST2, CHARLOGON for the same character, then create works', async () => {
  const { transport, ops, logons } = realm();
  const roster = await mcpReturnToLobby(transport, protocol, { realmReply: [], character: 'ama', keepAliveMs: 1e9 });
  assert.deepEqual(ops, [0x01, 0x19, 0x07]);
  assert.deepEqual(logons, ['Ama']);
  assert.equal(roster.session.characterLoggedOn, 'Ama');
  const joined = await mcpCreateAndJoin(transport, protocol, { realmReply: [], character: 'Ama', gameName: 'g', session: roster.session });
  assert.deepEqual(ops, [0x01, 0x19, 0x07, 0x03, 0x04], 'no second CHARLOGON');
  assert.equal(joined.ticket.token, 0x9e);
  clearInterval(roster.session.keepAlive);
});

test('control: a rejected CHARLOGON or a missing character is an error, not a lobby', async () => {
  await assert.rejects(mcpReturnToLobby(realm({ logonResult: 0x7a }).transport, protocol, { realmReply: [], character: 'Ama', keepAliveMs: 1e9 }), /CHARLOGON was rejected/);
  await assert.rejects(mcpReturnToLobby(realm({ names: ['Bob'] }).transport, protocol, { realmReply: [], character: 'Ama', keepAliveMs: 1e9 }), /not in the MCP character list/);
});

test('returnToLobby keeps the login (realm handoff, chat) and swaps in a fresh MCP session', async () => {
  const r = realm(), chat = { transport: {}, reader: {} }, chatSession = { open: true };
  const flow = { server: { ws: 'x' }, realmReply: [9], realms: ['R'], mpq: 'm', chat, chatSession, mcp: null, protocol: null };
  const out = await returnToLobby({ flow, character: 'Ama', dialMcp: async () => r.transport, bindProtocol: () => protocol,
    mcpReturnToLobby: (t, p, o) => mcpReturnToLobby(t, p, { ...o, keepAliveMs: 1e9 }) });
  assert.equal(out.flow.chatSession, chatSession); assert.equal(out.flow.chat, chat);
  assert.deepEqual(out.flow.realmReply, [9]); assert.equal(out.flow.mcp, r.transport); assert.equal(out.flow.protocol, protocol);
  assert.deepEqual(out.characters.map((c) => c.name), ['Ama']);
  clearInterval(out.flow.mcpSession.keepAlive);
});

test('control: returnToLobby with no handoff, or a failing logon, is a lost session and frees what it opened', async () => {
  await assert.rejects(returnToLobby({ flow: { server: {}, realmReply: null }, character: 'Ama' }), (e) => e.sessionLost === true);
  const r = realm({ logonResult: 0x7a }); protocol.freed = false;
  await assert.rejects(returnToLobby({ flow: { server: {}, realmReply: [1] }, character: 'Ama', dialMcp: async () => r.transport,
    bindProtocol: () => protocol, mcpReturnToLobby: (t, p, o) => mcpReturnToLobby(t, p, { ...o, keepAliveMs: 1e9 }) }), (e) => e.sessionLost === true);
  assert.equal(r.transport.isOpen, false); assert.equal(protocol.freed, true);
});
