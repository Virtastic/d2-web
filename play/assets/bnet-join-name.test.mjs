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
const LISTED = [{ name: 'game1' }, { name: 'game2' }];

test('empty name, empty list: Enter sends nothing and shows the Invalid Game Name popup', async () => {
  const { s, joins } = make([]); s.typeKey('Enter'); await tick();
  assert.deepEqual(joins, []); assert.equal(s.evidence.notice.body, 's5137');
});
test('empty name, populated list: Enter sends nothing and shows no popup', async () => {
  const { s, joins } = make(LISTED); s.typeKey('Enter'); await tick();
  assert.deepEqual(joins, []); assert.equal(s.evidence.notice, null);
});
test('empty name, no list callback: Enter sends nothing and shows the popup', async () => {
  const { s, joins } = make([], { onRefreshGames: null }); s.typeKey('Enter'); await tick();
  assert.deepEqual(joins, []); assert.equal(s.evidence.notice.body, 's5137');
});
test('a name matching a listed game joins', async () => {
  const { s, joins } = make(LISTED); type(s, 'game1'); s.typeKey('Enter'); await tick();
  assert.deepEqual(joins, ['game1']); assert.equal(s.evidence.notice, null);
});
test('a name matching no game, empty list: no join, popup', async () => {
  const { s, joins } = make([]); type(s, 'game1'); s.typeKey('Enter'); await tick();
  assert.deepEqual(joins, []); assert.equal(s.evidence.notice.body, 's5137');
});
test('a name matching no game, populated list: no join, no popup', async () => {
  const { s, joins } = make(LISTED); type(s, 'game9'); s.typeKey('Enter'); await tick();
  assert.deepEqual(joins, []); assert.equal(s.evidence.notice, null);
});
