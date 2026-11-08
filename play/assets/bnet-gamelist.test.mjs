import test from 'node:test';
import assert from 'node:assert/strict';
import { getGamesPacket, parseGameList, filterGames, encodeGameList } from './bnet-gamelist.mjs';
import { createBnetChat } from './bnet-chat.mjs';

const games = [{ name: 'cows', description: 'cow level', difficulty: 0 }, { name: 'NM', password: 'pw', difficulty: 1, description: 'x' }, { name: 'Hell', difficulty: 2 }, { name: 'HC', difficulty: 3 }];

test('the request is a 0x09 frame with the retail game count and three empty strings', () => {
  const p = getGamesPacket();
  assert.equal(Buffer.from(p).toString('hex'), 'ff091700' + '00'.repeat(12) + '19000000' + '000000');
  assert.equal(p.length, p[2] | (p[3] << 8));
});

test('a reply parses to name, password flag, difficulty digit and description; truncation keeps whole games only', () => {
  const body = encodeGameList(games), got = parseGameList(body);
  assert.deepEqual(got.map((g) => [g.name, g.password, g.difficulty, g.description]), [['cows', false, 0, 'cow level'], ['NM', true, 1, 'x'], ['Hell', false, 2, ''], ['HC', false, 3, '']]);
  assert.equal(got[1].address, '192.0.2.2');
  assert.equal(parseGameList(body.subarray(0, body.length - 5)).length, 3);
  assert.deepEqual(parseGameList(new Uint8Array(2)), []);
});

test('retail filter by progress bits and hardcore', () => {
  const got = parseGameList(encodeGameList(games));
  assert.deepEqual(filterGames(got, { progression: 0 }).map((g) => g.name), ['cows', 'NM']);
  assert.deepEqual(filterGames(got, { progression: 5 }).map((g) => g.name), ['cows', 'NM', 'Hell']);
  assert.deepEqual(filterGames(got, { progression: 0, hardcore: true }).map((g) => g.name), ['HC']);
  assert.deepEqual(filterGames([{ difficulty: 3 }, { difficulty: 4 }, { difficulty: 5 }], { progression: 0, hardcore: true }).map((g) => g.difficulty), [3, 4]);
  assert.deepEqual(filterGames([{ difficulty: 5 }], { progression: 5, hardcore: true }).map((g) => g.difficulty), [5]);
});

test('the chat leg sends the request and hands the 0x09 body to onGames', async () => {
  const sent = []; let pending = null; const body = encodeGameList(games);
  const frame = new Uint8Array([0xff, 0x09, (body.length + 4) & 255, (body.length + 4) >> 8, ...body]); let buf = frame, closed = false;
  const reader = { want: (n) => (buf.length >= n ? Promise.resolve(buf.subarray(0, n)) : new Promise(() => {})), skip: (n) => { buf = buf.subarray(n); }, cancel() { closed = true; } };
  const got = new Promise((r) => { pending = r; });
  const chat = createBnetChat({ transport: { isOpen: true, send: (b) => { sent.push(b); return true; }, close() {} }, reader, character: 'Ama', keepAliveMs: 0, onGames: (b) => pending(b) });
  assert.equal(chat.games(), true); assert.equal(sent[0][1], 0x09);
  assert.deepEqual(parseGameList(await got).map((g) => g.name), games.map((g) => g.name));
  chat.close(); assert.ok(closed);
});
