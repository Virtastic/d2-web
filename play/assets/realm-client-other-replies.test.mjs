import test from 'node:test';
import assert from 'node:assert/strict';
import { mcpCreateAndJoin, bncsCreateAccount } from './realm-client.mjs';

const mcpFrame = (op, body) => Uint8Array.from([(body.length + 3) & 255, (body.length + 3) >> 8, op, ...body]);
function realm(beforeCreate) {
  let onData = () => {};
  const transport = {
    onData: (f) => { onData = f; }, onClose: () => {},
    send: (bytes) => {
      const op = bytes[2], id = bytes[3] | (bytes[4] << 8);
      if (op === 0x03) { for (const f of beforeCreate) onData(f); onData(mcpFrame(0x03, [id & 255, id >> 8, 6, 0, 0, 0, 0, 0, 0, 0])); }
      if (op === 0x04) onData(mcpFrame(0x04, [id & 255, id >> 8, 7, 0, 10, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0]));
    },
  };
  const protocol = {
    mcpFrame: (op, body) => Uint8Array.from([0, 0, op, (body[0] ?? 0) & 255, (body[1] ?? 0) & 255]),
    mcpCreateGame: ({ requestId }) => [requestId & 255, requestId >> 8],
    mcpJoinGame: ({ requestId }) => [requestId & 255, requestId >> 8],
  };
  const session = { transport, protocol, joined: false, list: { characters: [{ name: 'Hero' }] }, characterLoggedOn: 'Hero', received: [], nextRequestId: 2 };
  return { transport, protocol, session };
}

test('a busy realm answers CREATE GAME with its queue notice first: the place is reported and the create still completes', async () => {
  const { transport, protocol, session } = realm([mcpFrame(0x14, [3, 0, 0, 0]), mcpFrame(0x14, [1, 0, 0, 0])]);
  const places = [];
  const made = await mcpCreateAndJoin(transport, protocol, { realmReply: [1], character: 'Hero', gameName: 'x', session, onQueue: (n) => places.push(n) });
  assert.deepEqual(places, [3, 1]);
  assert.equal(made.game.createToken, 6);
  assert.equal(made.ticket.token, 7);
});

test('a reply to some other realm request arriving during the create wait is passed over, not a failure', async () => {
  const { transport, protocol, session } = realm([mcpFrame(0x11, [0, 0, 0, 0, 0]), mcpFrame(0x05, [9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])]);
  const made = await mcpCreateAndJoin(transport, protocol, { realmReply: [1], character: 'Hero', gameName: 'x', session });
  assert.equal(made.ticket.token, 7);
});

test('a queue notice restarts the create wait, and a realm that then stays silent still ends it', async () => {
  const { transport, protocol, session } = realm([]);
  const send = transport.send; let onData;
  const hold = transport.onData; transport.onData = (f) => { onData = f; hold(f); };
  transport.send = (bytes) => { if (bytes[2] === 0x03) setTimeout(() => onData(mcpFrame(0x14, [2, 0, 0, 0])), 30); else send(bytes); };
  const t0 = Date.now();
  const error = await mcpCreateAndJoin(transport, protocol, { realmReply: [1], character: 'Hero', gameName: 'x', session, gameReplyMs: 50 }).catch((e) => e);
  assert.equal(error.mcpStage, 'create');
  assert.ok(Date.now() - t0 >= 75, 'the wait ran again from the notice');
});

const bncsFrame = (id, body) => Uint8Array.from([0xff, id, (body.length + 4) & 255, (body.length + 4) >> 8, ...body]);
test('a Battle.net frame with another id ahead of each awaited reply is passed over', async () => {
  const authInfoBody = [...Buffer.alloc(20), ...Buffer.from('ver-IX86-1.mpq\0A=1 B=2 C=3 4 A=A+S B=B-C C=C^A A=A^B\0')];
  const ids = []; let onData = () => {};
  const transport = {
    isOpen: true, close() {}, onClose() {}, onData: (cb) => { onData = cb; },
    send: (b) => {
      if (b[0] !== 0xff) return true;
      ids.push(b[1]);
      // A keep-alive, an optional-work notice and a news packet: none is the reply being waited for.
      onData(bncsFrame(0x00, [])); onData(bncsFrame(0x4a, [0x6d, 0]));
      if (b[1] === 0x50) onData(bncsFrame(0x50, authInfoBody));
      else if (b[1] === 0x51) onData(bncsFrame(0x51, [0, 0, 0, 0]));
      else { onData(bncsFrame(0x46, [0, 0, 0, 0])); onData(bncsFrame(b[1], [0, 0, 0, 0])); }
      return true;
    },
  };
  const protocol = { bncsSelector: () => Uint8Array.of(1), bncsFrame, bncsAuthInfo: () => new Uint8Array(54), bncsAuthCheck: () => ({ body: new Uint8Array(8), clientToken: 1 }) };
  const made = await bncsCreateAccount(transport, protocol, { account: 'ama', password: 'pw', timeoutMs: 500 });
  assert.deepEqual(ids.slice(0, 3), [0x50, 0x51, 0x3d]);
  assert.equal(made.result, 0);
});
