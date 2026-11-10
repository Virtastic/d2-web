import test from 'node:test';
import assert from 'node:assert/strict';
import { mcpGameListBody, mcpGameInfoBody, mcpCancelBody, parseMcpGameListReply, parseMcpGameInfoReply, gameInfoLines, createMcpGameList,
  MCP_GAMELIST, MCP_GAMEINFO, MCP_CANCEL } from './realm-gamelist.mjs';

const ascii = (s) => [...Buffer.from(s, 'latin1')];
const listReply = (id, index, players, status, name, description, tail = []) => Uint8Array.from([id & 255, id >> 8,
  index & 255, (index >> 8) & 255, (index >> 16) & 255, index >>> 24, players, status & 255, (status >>> 8) & 255, (status >>> 16) & 255, status >>> 24,
  ...ascii(name), 0, ...ascii(description), 0, ...tail]);
const infoReply = (id, { status = 0, uptime = 0, level = 0, diff = 0xff, max = 8, classes = [], levels = [], description = '', names = [] }) => {
  const b = new Uint8Array(46); b[0] = id & 255; b[1] = id >> 8;
  new DataView(b.buffer).setUint32(2, status, true); new DataView(b.buffer).setUint32(6, uptime, true);
  b[10] = level; b[11] = diff; b[12] = max; b[13] = names.length; b.set(classes, 14); b.set(levels, 30);
  return Uint8Array.from([...b, ...ascii(description), 0, ...names.flatMap((n) => [...ascii(n), 0])]);
};

test('the game list request is the request id, a flag dword and an empty search text', () => {
  assert.deepEqual([...mcpGameListBody(2)], [0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]);
  assert.deepEqual([...mcpGameListBody(0x1234, { hardcore: true })], [0x34, 0x12, 0x00, 0x08, 0x00, 0x00, 0x00]);
});

test('the game info request is the request id and the game name', () => {
  assert.deepEqual([...mcpGameInfoBody(7, 'Baal')], [0x07, 0x00, 0x42, 0x61, 0x61, 0x6c, 0x00]);
});

test('the cancel request has no body', () => {
  assert.equal(mcpCancelBody().length, 0);
  assert.equal(MCP_CANCEL, 0x13); assert.equal(MCP_GAMELIST, 0x05); assert.equal(MCP_GAMEINFO, 0x06);
});

test('a game list reply reads index, players, status and both strings, and flags a closing frame', () => {
  const body = Uint8Array.from([0x02, 0x00, 0x09, 0x00, 0x00, 0x00, 0x03, 0x04, 0x00, 0x00, 0x00, 0x42, 0x61, 0x61, 0x6c, 0x00, 0x68, 0x69, 0x00]);
  assert.deepEqual(parseMcpGameListReply(body), { requestId: 2, index: 9, players: 3, status: 4, name: 'Baal', description: 'hi', end: false });
  // The closing frame: three empty strings, so a byte follows the description's NUL.
  assert.equal(parseMcpGameListReply(Uint8Array.from([0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])).end, true);
  assert.equal(parseMcpGameListReply(Uint8Array.from([0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x41])), null, 'unterminated');
  assert.equal(parseMcpGameListReply(new Uint8Array(5)), null);
});

test('a game info reply reads the fixed block, the description and one name per character', () => {
  const body = infoReply(5, { uptime: 3725, level: 20, diff: 5, max: 4, classes: [1, 6], levels: [20, 33], description: 'runs', names: ['Sorc', 'Sin'] });
  assert.equal(body.length, 46 + 5 + 5 + 4);
  assert.deepEqual(parseMcpGameInfoReply(body), { requestId: 5, status: 0, uptime: 3725, level: 20, levelDifference: 5, maxPlayers: 4, count: 2, description: 'runs',
    characters: [{ name: 'Sorc', charClass: 1, level: 20 }, { name: 'Sin', charClass: 6, level: 33 }] });
  assert.equal(parseMcpGameInfoReply(new Uint8Array(46)), null);
});

test('the info text: description, elapsed time, level range, player limit, then each character', () => {
  const info = parseMcpGameInfoReply(infoReply(5, { uptime: 3725, level: 20, diff: 5, max: 4, classes: [1, 6, 9], levels: [20, 33, 2], description: 'runs', names: ['Sorc', 'Sin', 'Odd'] }));
  assert.deepEqual(gameInfoLines(info), ['runs', '', 'Elapsed Time: 1:02:05', 'Level 15 to 25', 'Up to 4 Players', '',
    'Sorc', 'Level 20 Sorceress', 'Sin', 'Level 33 Assassin', 'Odd', 'Level 2']);
  // No description, no level limit (difference 100 and up), eight players: only the time.
  assert.deepEqual(gameInfoLines({ description: '', uptime: 59, level: 1, levelDifference: 0xff, maxPlayers: 8, characters: [] }), ['Elapsed Time: 0:00:59', '']);
  // A zero difference names the one level; the range is clamped to 1..99.
  assert.equal(gameInfoLines({ description: '', uptime: 0, level: 7, levelDifference: 0, maxPlayers: 0, characters: [] })[1], 'Level 7');
  assert.equal(gameInfoLines({ description: '', uptime: 0, level: 3, levelDifference: 98, maxPlayers: 0, characters: [] })[1], 'Level 1 to 99');
});

function fixture(options = {}) {
  const sent = [], shown = [], info = [];
  let id = 2, clock = 0;
  const list = createMcpGameList({ send: (op, body) => sent.push([op, ...body]), nextId: () => id++, onGames: (rows) => shown.push(rows),
    onInfo: (lines) => info.push(lines), now: () => clock, ...options });
  return { list, sent, shown, info, at: (ms) => { clock = ms; } };
}

test('rows are gathered a frame at a time and shown when the closing frame arrives', () => {
  const { list, sent, shown } = fixture();
  list.request();
  assert.deepEqual(sent, [[0x05, 2, 0, 0, 0, 0, 0, 0]]);
  assert.equal(list.frame({ op: 0x05, body: listReply(2, 1, 3, 4, 'Baal', 'hi') }), true);
  list.frame({ op: 0x05, body: listReply(2, 2, 1, 4, 'Cows', '') });
  list.frame({ op: 0x05, body: listReply(2, 0x10001, 8, 4, 'Again', 'same low word as Baal') });
  assert.equal(shown.length, 0);
  list.frame({ op: 0x05, body: listReply(2, 0, 0, 0, '', '', [0]) });
  assert.deepEqual(shown, [[{ name: 'Baal', description: 'hi', players: 3, index: 1 }, { name: 'Cows', description: '', players: 1, index: 2 }]]);
});

test('a reply to an older list request is dropped, status -2 shows the list and status -1 empties it', () => {
  const { list, shown } = fixture();
  list.request(); list.request();
  shown.length = 0;
  list.frame({ op: 0x05, body: listReply(2, 1, 1, 4, 'Old', '') });
  list.frame({ op: 0x05, body: listReply(3, 1, 1, 4, 'New', '') });
  list.frame({ op: 0x05, body: listReply(3, 0, 0, 0xfffffffe, '', '') });
  assert.deepEqual(shown.map((r) => r.map((g) => g.name)), [['New']]);
  list.request(); shown.length = 0;
  list.frame({ op: 0x05, body: listReply(4, 1, 1, 4, 'Gone', '') });
  list.frame({ op: 0x05, body: listReply(4, 0, 0, 0xffffffff, '', '') });
  assert.deepEqual(shown, [[]]);
});

test('a hardcore character asks with the hardcore flag, and a request left unanswered is shown empty by the next', () => {
  const { list, sent, shown } = fixture({ hardcore: true });
  list.request();
  assert.deepEqual(sent[0], [0x05, 2, 0, 0x00, 0x08, 0, 0, 0]);
  list.request();
  assert.deepEqual(shown, [[]]);
});

test('names and descriptions are cut to the list entry sizes', () => {
  const { list, shown } = fixture();
  list.request();
  list.frame({ op: 0x05, body: listReply(2, 1, 1, 4, 'a'.repeat(20), 'd'.repeat(40)) });
  list.frame({ op: 0x05, body: listReply(2, 0, 0, 0, '', '', [0]) });
  assert.equal(shown[0][0].name.length, 15); assert.equal(shown[0][0].description.length, 31);
});

test('a picked row asks for its info on the one-second tick, the last pick only, and shows the matching reply', () => {
  const { list, sent, info, at } = fixture();
  at(5000); list.tick();
  list.select('Baal'); list.select('Cows');
  assert.deepEqual(info, [[], []], 'the box empties on each pick');
  at(5500); list.tick();
  assert.equal(sent.length, 0, 'not before the tick');
  at(6000); list.tick();
  assert.deepEqual(sent, [[0x06, 3, 0, 0x43, 0x6f, 0x77, 0x73, 0]]);
  at(7000); list.tick();
  assert.equal(sent.length, 1, 'sent once');
  list.frame({ op: 0x06, body: infoReply(2, { uptime: 1, description: 'stale' }) });
  assert.equal(info.length, 2, 'an answer to the superseded pick is dropped');
  list.frame({ op: 0x06, body: infoReply(3, { uptime: 61, names: ['Ama'], classes: [0], levels: [9] }) });
  assert.deepEqual(info[2], ['Elapsed Time: 0:01:01', '', 'Ama', 'Level 9 Amazon']);
  assert.equal(list.frame({ op: 0x03, body: new Uint8Array(10) }), false, 'other opcodes are left for their own waits');
});
