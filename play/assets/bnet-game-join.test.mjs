import test from 'node:test';
import assert from 'node:assert/strict';
import { mcpCreateAndJoin } from './realm-client.mjs';

// A realm may hand the joining client a fresh token distinct from the one CREATE GAME returned
// (the real local realm does); the join reply's token is the one the game server is dialled with.
test('creating a game follows the join reply token even when it differs from the create reply token', async () => {
  let onData = () => {};
  const frame = (op, body) => Uint8Array.from([(body.length + 3) & 255, (body.length + 3) >> 8, op, ...body]);
  const transport = {
    onData: (f) => { onData = f; }, onClose: () => {},
    send: (bytes) => {
      const op = bytes[2], id = bytes[3] | (bytes[4] << 8);
      if (op === 0x03) onData(frame(0x03, [id & 255, id >> 8, 6, 0, 0, 0, 0, 0, 0, 0]));
      if (op === 0x04) onData(frame(0x04, [id & 255, id >> 8, 7, 0, 10, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0]));
    }
  };
  const protocol = {
    mcpFrame: (op, body) => Uint8Array.from([0, 0, op, (body[0] ?? 0) & 255, (body[1] ?? 0) & 255]),
    mcpCreateGame: ({ requestId }) => [requestId & 255, requestId >> 8],
    mcpJoinGame: ({ requestId }) => [requestId & 255, requestId >> 8]
  };
  const session = { transport, protocol, joined: false, list: { characters: [{ name: 'Hero' }] }, characterLoggedOn: 'Hero', received: [], nextRequestId: 2 };
  const made = await mcpCreateAndJoin(transport, protocol, { realmReply: [1], character: 'Hero', gameName: 'x', session, keepAliveMs: 0 });
  assert.equal(made.ticket.token, 7);
  assert.equal(made.game.createToken, 6);
});

import { mcpCreateCharacter } from './realm-client.mjs';
test('a Druid or Assassin is created as an expansion character even with the expansion box off', async () => {
  let sent;
  let onData = () => {};
  const reader = { want: async () => Uint8Array.from([7, 0, 2, 0, 0, 0, 0]), skip: () => {} };
  const session = { reader, list: { characters: [], total: 0, returned: 0 }, protocol: { mcpFrame: (op, body) => body },
    transport: { send: (b) => { sent = b; } } };
  await mcpCreateCharacter(session, { name: 'Dru', charClass: 5, expansion: false });
  assert.equal(sent[4] & 0x20, 0x20);
  await mcpCreateCharacter(session, { name: 'Ama', charClass: 0, expansion: false });
  assert.equal(sent[4] & 0x20, 0);
});

test('character creation sends the name as UTF-8 with the ladder and hardcore bits, and a non-ASCII name is not an error', async () => {
  let sent;
  const reader = { want: async () => Uint8Array.from([7, 0, 2, 0, 0, 0, 0]), skip: () => {} };
  const session = { reader, list: { characters: [], total: 0, returned: 0 }, protocol: { mcpFrame: (op, body) => body },
    transport: { send: (b) => { sent = b; } } };
  await mcpCreateCharacter(session, { name: 'Ça\u0092', charClass: 6, hardcore: true, ladder: true });
  assert.equal(sent[0], 6);
  assert.equal(sent[4], 0x20 | 0x04 | 0x40);
  const name = new TextEncoder().encode('Ça’');
  assert.deepEqual([...sent.subarray(6, 6 + name.length)], [...name]);
  assert.equal(sent.length, 6 + name.length + 1);
  assert.equal(sent[sent.length - 1], 0);
});

test('character creation leaves the ladder bit clear unless asked, and flag bytes are class,0,0,0,flags,0,name', async () => {
  let sent;
  const reader = { want: async () => Uint8Array.from([7, 0, 2, 0, 0, 0, 0]), skip: () => {} };
  const session = { reader, list: { characters: [], total: 0, returned: 0 }, protocol: { mcpFrame: (op, body) => body },
    transport: { send: (b) => { sent = b; } } };
  await mcpCreateCharacter(session, { name: 'Ab', charClass: 1 });
  assert.deepEqual([...sent], [1, 0, 0, 0, 0x20, 0, 0x41, 0x62, 0]);
  await mcpCreateCharacter(session, { name: 'Ab', charClass: 1, ladder: true });
  assert.deepEqual([...sent], [1, 0, 0, 0, 0x60, 0, 0x41, 0x62, 0]);
});

import { gameErrorText } from './bnet-lobby-screen.mjs';
test('a join the realm never answers fails as Join Failed (5141) in the lobby, not as a lost session', async () => {
  const transport = { onData: () => {}, onClose: () => {}, send: () => {} };
  const protocol = { mcpFrame: (op, body) => Uint8Array.from([0, 0, op]), mcpJoinGame: () => [0, 0] };
  const session = { transport, protocol, joined: false, list: { characters: [{ name: 'Hero' }] }, characterLoggedOn: 'Hero', received: [], nextRequestId: 2 };
  const error = await mcpCreateAndJoin(transport, protocol, { realmReply: [1], character: 'Hero', gameName: 'x', session, join: true, gameReplyMs: 20 }).catch((e) => e);
  assert.equal(error.mcpStage, 'join');
  assert.equal(gameErrorText(error, (id) => `S${id}`), 'S5141');
});
