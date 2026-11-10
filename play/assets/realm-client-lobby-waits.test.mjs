import test from 'node:test';
import assert from 'node:assert/strict';
import { mcpCreateAndJoin, mcpListCharacters, mcpReturnToLobby, mcpCreateCharacter, mcpLogonCharacter, mcpListen, mcpSend, mcpNextRequestId,
  bncsCreateAccount, parseRealmList, realmLogonTitle, charCreateRefusalText } from './realm-client.mjs';
import { createMcpGameList } from './realm-gamelist.mjs';

const mcpFrame = (op, body) => Uint8Array.from([(body.length + 3) & 255, (body.length + 3) >> 8, op, ...body]);
// A stray reply to another request and a queue notice: neither is ever the answer a wait is for.
const strays = [mcpFrame(0x11, [0, 0, 0, 0, 0]), mcpFrame(0x14, [1, 0, 0, 0])];
// One character, "Hero", in the 0x19 reply: [requested u16][total u32][returned u16] then [expires u32][name\0][statstring\0].
const charlist = mcpFrame(0x19, [8, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, ...Buffer.from('Hero\0'), 0x84, 0x80, ...new Array(31).fill(0x01), 0]);
const protocol = {
  mcpSelector: () => Uint8Array.of(1), mcpFrame, mcpStartup: () => [], mcpCharlist: () => [8, 0, 0, 0], mcpCharlogon: (name) => [...Buffer.from(`${name}\0`)],
  mcpCreateGame: ({ requestId }) => [requestId & 255, requestId >> 8], mcpJoinGame: ({ requestId }) => [requestId & 255, requestId >> 8],
};
/** A realm that answers each request with the stray frames first, then the real reply. */
function realm(replies = {}) {
  let onData = () => {}; const sent = [];
  const transport = {
    isOpen: true, close() {}, onClose() {}, onData: (f) => { onData = f; },
    send: (bytes) => {
      if (bytes.length < 3) return;
      const op = bytes[2], id = [bytes[3] ?? 0, bytes[4] ?? 0];
      sent.push([...bytes]);
      const reply = op in replies ? replies[op] : { 0x01: mcpFrame(0x01, [0, 0, 0, 0]), 0x19: charlist, 0x07: mcpFrame(0x07, [0, 0, 0, 0]), 0x02: mcpFrame(0x02, [0, 0, 0, 0]),
        0x03: mcpFrame(0x03, [...id, 6, 0, 0, 0, 0, 0, 0, 0]), 0x04: mcpFrame(0x04, [...id, 7, 0, 10, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0]) }[op];
      if (reply) queueMicrotask(() => { for (const f of strays) onData(f); onData(reply); });
    },
  };
  return { transport, sent, push: (f) => onData(f) };
}

test('the startup, character list and character logon waits pass over frames with other opcodes', async () => {
  const { transport } = realm();
  const roster = await mcpListCharacters(transport, protocol, { realmReply: [1], keepAliveMs: 1e9 });
  assert.deepEqual(roster.characters.map((c) => c.name), ['Hero']);
  roster.session.keepAlive?.stop?.();
  const again = realm();
  const back = await mcpReturnToLobby(again.transport, protocol, { realmReply: [1], character: 'Hero', characters: roster.characters, keepAliveMs: 1e9 });
  assert.equal(back.session.characterLoggedOn, 'Hero');
  back.session.keepAlive?.stop?.();
  const fresh = realm();
  const made = await mcpCreateAndJoin(fresh.transport, protocol, { realmReply: [1], character: 'Hero', gameName: 'x' });
  assert.equal(made.ticket.token, 7);
});

test('the character create wait passes over other opcodes, and a refusal carries the realm\'s result', async () => {
  const { transport } = realm();
  const roster = await mcpListCharacters(transport, protocol, { realmReply: [1], keepAliveMs: 1e9 });
  roster.session.keepAlive?.stop?.();
  await mcpCreateCharacter(roster.session, { name: 'Ama', charClass: 0 });
  const taken = realm({ 0x02: mcpFrame(0x02, [0x14, 0, 0, 0]) });
  const session = (await mcpListCharacters(taken.transport, protocol, { realmReply: [1], keepAliveMs: 1e9 })).session;
  session.keepAlive?.stop?.();
  const error = await mcpCreateCharacter(session, { name: 'Ama', charClass: 0 }).catch((e) => e);
  assert.equal(error.result, 0x14);
});

test('a refused character is told in the game\'s own words: the name in use, or rejected by the server', () => {
  const string = (i) => ({ 11059: "A character named '%s' already exists in this Realm.", 5214: 'Name ', 5215: ' rejected by Server' })[i];
  assert.equal(charCreateRefusalText(0x14, 'Ama', string), "A character named 'Ama' already exists in this Realm.");
  assert.equal(charCreateRefusalText(0x15, 'Ama', string), 'Name Ama rejected by Server');
  assert.equal(charCreateRefusalText(0x01, 'Ama', string), 'Name Ama rejected by Server');
});

test('logging the character on at lobby entry sends 0x07 once, and a create on that session does not send it again', async () => {
  const { transport, sent } = realm();
  const { session } = await mcpListCharacters(transport, protocol, { realmReply: [1], keepAliveMs: 1e9 });
  session.keepAlive?.stop?.();
  const [a, b] = await Promise.all([mcpLogonCharacter(session, 'hero'), mcpLogonCharacter(session, 'Hero')]);
  assert.equal(a.name, 'Hero'); assert.equal(b.name, 'Hero');
  await mcpCreateAndJoin(transport, protocol, { realmReply: [1], character: 'Hero', gameName: 'x', session });
  assert.deepEqual(sent.map((b) => b[2]).filter((op) => op === 0x07), [0x07]);
  assert.deepEqual(sent.find((b) => b[2] === 0x07), [8, 0, 0x07, 0x48, 0x65, 0x72, 0x6f, 0]);
});

test('Cancel on the create wait sends the realm the bodyless 0x13 and ends the create as cancelled; the session stays usable', async () => {
  const { transport, sent } = realm({ 0x03: null });
  const { session } = await mcpListCharacters(transport, protocol, { realmReply: [1], keepAliveMs: 1e9 });
  session.keepAlive?.stop?.();
  const abort = new AbortController();
  const run = mcpCreateAndJoin(transport, protocol, { realmReply: [1], character: 'Hero', gameName: 'x', session, signal: abort.signal }).catch((e) => e);
  await new Promise((r) => setTimeout(r, 10));
  abort.abort();
  const error = await run;
  assert.equal(error.cancelled, true); assert.equal(error.mcpStage, undefined);
  assert.deepEqual(sent.at(-1), [3, 0, 0x13]);
  assert.equal(session.joined, false);
  // The join wait has no Cancel: an aborted signal does not touch it.
  const joined = await mcpCreateAndJoin(transport, protocol, { realmReply: [1], character: 'Hero', gameName: 'x', session, join: true, signal: abort.signal });
  assert.equal(joined.ticket.token, 7);
  assert.equal(sent.filter((b) => b[2] === 0x13).length, 1);
});

test('the queue place reaches the caller while the game list shares the socket and the request counter', async () => {
  const { transport, sent, push } = realm({ 0x03: null });
  const { session } = await mcpListCharacters(transport, protocol, { realmReply: [1], keepAliveMs: 1e9 });
  session.keepAlive?.stop?.();
  await mcpLogonCharacter(session, 'Hero');
  const shown = [];
  const list = createMcpGameList({ send: (op, body) => mcpSend(session, op, body), nextId: () => mcpNextRequestId(session), onGames: (rows) => shown.push(rows.map((r) => r.name)) });
  mcpListen(session, (frame) => list.frame(frame));
  const listId = list.request();
  assert.deepEqual(sent.at(-1), [10, 0, 0x05, listId, 0, 0, 0, 0, 0, 0]);
  const places = [];
  const run = mcpCreateAndJoin(transport, protocol, { realmReply: [1], character: 'Hero', gameName: 'x', session, onQueue: (n) => places.push(n) });
  await new Promise((r) => setTimeout(r, 5));
  const createId = sent.find((b) => b[2] === 0x03)[3];
  assert.equal(createId, listId + 1, 'one counter for list and create requests');
  push(mcpFrame(0x14, [9, 0, 0, 0]));
  push(mcpFrame(0x05, [listId, 0, 1, 0, 0, 0, 2, 4, 0, 0, 0, ...Buffer.from('Baal\0\0')]));
  push(mcpFrame(0x05, [listId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]));
  push(mcpFrame(0x03, [createId, 0, 6, 0, 0, 0, 0, 0, 0, 0]));
  const made = await run;
  assert.equal(places[0], 9); assert.deepEqual(shown, [['Baal']]);
  assert.equal(made.game.createToken, 6);
});

test('the request counter never hands out 0', () => {
  const session = { nextRequestId: 0xffff };
  assert.equal(mcpNextRequestId(session), 0xffff); assert.equal(mcpNextRequestId(session), 1);
});

test('the realm named in the realm logon is the title the server listed, the configured name only when it lists none', () => {
  const body = Uint8Array.from([0, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, ...Buffer.from('Olympus\0Realm for Europe\0'), 1, 0, 0, 0, ...Buffer.from('Asgard\0\0')]);
  const list = parseRealmList(body);
  assert.deepEqual(list, [{ title: 'Olympus', description: 'Realm for Europe' }, { title: 'Asgard', description: '' }]);
  assert.equal(realmLogonTitle(list, 'configured'), 'Olympus');
  assert.equal(realmLogonTitle(parseRealmList(Uint8Array.from([0, 0, 0, 0, 0, 0, 0, 0])), 'configured'), 'configured');
});

const bncsFrame = (id, body) => Uint8Array.from([0xff, id, (body.length + 4) & 255, (body.length + 4) >> 8, ...body]);
test('bytes ahead of a Battle.net frame are skipped up to the next 0xFF, and a marker with an impossible length is passed', async () => {
  const authInfoBody = [...Buffer.alloc(20), ...Buffer.from('ver-IX86-1.mpq\0A=1 B=2 C=3 4 A=A+S B=B-C C=C^A A=A^B\0')];
  let onData = () => {}; const ids = [];
  const transport = {
    isOpen: true, close() {}, onClose() {}, onData: (cb) => { onData = cb; },
    send: (b) => {
      if (b[0] !== 0xff) return true;
      ids.push(b[1]);
      if (b[1] === 0x50) { onData(Uint8Array.from([0x01, 0x02, 0x03])); onData(bncsFrame(0x50, authInfoBody)); }           // garbage, then the frame
      else if (b[1] === 0x51) { onData(Uint8Array.from([0x00, 0xff, 0x51, 0x02, 0x00])); onData(bncsFrame(0x51, [0, 0, 0, 0])); }   // a marker whose length is 2
      else { const f = bncsFrame(b[1], [0, 0, 0, 0]); onData(Uint8Array.from([0x7f, ...f.subarray(0, 3)])); onData(f.subarray(3)); }   // garbage and a frame split across reads
      return true;
    },
  };
  const proto = { bncsSelector: () => Uint8Array.of(1), bncsFrame, bncsAuthInfo: () => new Uint8Array(54), bncsAuthCheck: () => ({ body: new Uint8Array(8), clientToken: 1 }) };
  const made = await bncsCreateAccount(transport, proto, { account: 'ama', password: 'pw', timeoutMs: 500 });
  assert.deepEqual(ids.slice(0, 3), [0x50, 0x51, 0x3d]);
  assert.equal(made.result, 0);
});
