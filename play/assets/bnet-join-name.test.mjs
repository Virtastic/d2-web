import test from 'node:test';
import assert from 'node:assert/strict';
import { createBnetLobbyScreen } from './bnet-lobby-screen.mjs';

globalThis.ImageData ??= class { constructor(data, w, h) { Object.assign(this, { data, width: w, height: h }); } };
const ctx = new Proxy({}, { get: (_, k) => (k === 'getImageData' ? () => ({ data: new Uint8ClampedArray(4) }) : () => {}), set: () => true });
const frame = { width: 10, height: 10, offsetX: 0, offsetY: 0, indices: new Uint8Array(100), opaque: new Uint8Array(100).fill(1) };
const tbl = new Uint8Array(12 + 95 * 14); tbl.set([0x57, 0x6f, 0x6f, 0x21]); tbl[10] = 10;
for (let c = 32; c < 127; c++) { const b = 12 + (c - 32) * 14; tbl[b] = c; tbl[b + 3] = 6; tbl[b + 4] = 10; }
const tick = () => new Promise((r) => setImmediate(r));
const make = (games, hooks = {}) => {
  const joins = [];
  const doc = { createElement: () => ({ width: 0, height: 0, style: {}, setAttribute() {}, addEventListener() {}, getContext: () => ctx }), body: { appendChild() {} } };
  const s = createBnetLobbyScreen({ read: (p) => (p.endsWith('.tbl') ? tbl : new Uint8Array(1 << 22)), decodeDc6: () => ({ frames: Array(6).fill(frame) }),
    decodeDatPalette: () => new Uint8Array(1024), now: () => 60000, string: (i) => `s${i}`, character: { name: 'Ama', charClass: 0, level: 3 },
    documentObject: doc, onJoinGame: async (g) => { joins.push(g.gameName); }, ...hooks });
  s.activate('join'); s.setGames(games);
  return { s, joins };
};
const type = (s, t) => { for (const c of t) s.typeKey(c === ' ' ? 'Space' : /\d/.test(c) ? `Digit${c}` : `Key${c.toUpperCase()}`, c); };
const LISTED = [{ name: 'Game1' }, { name: 'Game2' }];
const ALL = [{ name: 'Baal Runs 24-7' }, { name: 'Cows' }, { name: 'NM Mephisto', password: true }, { name: 'lower case' }, { name: 'Tristram' }];
const HC = { name: 'HC Normal', difficulty: 3 };
// A refreshed table lists every entry; a table not yet filled lists none but the raw reply still holds them.
const run = async (typed, { table = true, hcFirst = false } = {}) => {
  const raw = hcFirst ? [HC, ...ALL] : ALL;
  const { s, joins } = make([]); s.setGames(table ? ALL : [], raw, hcFirst);
  type(s, typed); s.typeKey('Enter'); await tick();
  return { joins, notice: s.evidence.notice?.body ?? null };
};
const POPUP = 's5137';

test('empty name, empty list: Enter sends nothing and shows the Invalid Game Name popup', async () => {
  const { s, joins } = make([]); s.typeKey('Enter'); await tick();
  assert.deepEqual(joins, []); assert.equal(s.evidence.notice.body, POPUP);
});
test('empty name, populated list: Enter sends nothing and shows no popup', async () => {
  const { s, joins } = make(LISTED); s.typeKey('Enter'); await tick();
  assert.deepEqual(joins, []); assert.equal(s.evidence.notice, null);
});
test('empty name, no list callback: Enter sends nothing and shows the popup', async () => {
  const { s, joins } = make([], { onRefreshGames: null }); s.typeKey('Enter'); await tick();
  assert.deepEqual(joins, []); assert.equal(s.evidence.notice.body, POPUP);
});
test('a name matching no game, empty list and no reply: no join, popup', async () => {
  const { s, joins } = make([]); type(s, 'game1'); s.typeKey('Enter'); await tick();
  assert.deepEqual(joins, []); assert.equal(s.evidence.notice.body, POPUP);
});
test('refreshed table: first entry typed in lower case joins the title-cased name', async () => {
  assert.deepEqual(await run('baal runs 24-7'), { joins: ['Baal Runs 24-7'], notice: null });
});
test('refreshed table: lower, same and upper case all join "Cows"', async () => {
  for (const t of ['cows', 'Cows', 'COWS']) assert.deepEqual(await run(t), { joins: ['Cows'], notice: null }, t);
});
test('refreshed table: stored names not in title case cannot be joined by typing', async () => {
  for (const t of ['NM Mephisto', 'lower case']) assert.deepEqual(await run(t), { joins: [], notice: null }, t);
});
test('refreshed table: an unknown name is silent', async () => {
  assert.deepEqual(await run('zzz'), { joins: [], notice: null });
});
test('refreshed table: an empty name is silent', async () => {
  assert.deepEqual(await run(''), { joins: [], notice: null });
});
test('empty table: only the first raw entry is compared', async () => {
  assert.deepEqual(await run('baal runs 24-7', { table: false }), { joins: ['Baal Runs 24-7'], notice: null });
  assert.deepEqual(await run('cows', { table: false }), { joins: [], notice: null });
});
test('empty table: an empty name is silent', async () => {
  assert.deepEqual(await run('', { table: false }), { joins: [], notice: null });
});
test('a hidden hardcore first entry gives the popup and no join, with or without a filled table', async () => {
  for (const table of [true, false]) assert.deepEqual(await run('hc normal', { table, hcFirst: true }), { joins: [], notice: POPUP }, String(table));
});

test('a hidden first entry only raises the popup when the typed name is not in the table', async () => {
  assert.deepEqual(await run('cows', { hcFirst: true }), { joins: ['Cows'], notice: null });
  for (const t of ['zzz', '', 'NM Mephisto']) assert.deepEqual(await run(t, { hcFirst: true }), { joins: [], notice: POPUP }, t);
  assert.deepEqual(await run('cows', { table: false, hcFirst: true }), { joins: [], notice: POPUP });
});
test('double-clicking a row joins through the title-cased name box: a name not in title case cannot be joined', async () => {
  for (const [y, want] of [[220 + 19 + 5, ['Cows']], [220 + 38 + 5, []], [220 + 57 + 5, []]]) {
    const { s, joins } = make([]); s.setGames(ALL, ALL, false); s.pinClock(60000, 0);
    s.press(440, y, 800, 600); s.press(440, y, 800, 600); await tick();
    assert.deepEqual(joins, want, String(y));
  }
});
