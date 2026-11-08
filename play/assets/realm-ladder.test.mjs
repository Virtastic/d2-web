import test from 'node:test';
import assert from 'node:assert/strict';
import { parseLadderReply, ladderRequestBody, ladderMode, ladderRows } from './realm-ladder.mjs';
import { mcpLadderRequest } from './realm-client.mjs';
import { createBnetLobbyScreen } from './bnet-lobby-screen.mjs';

const le32 = (v) => [v & 255, (v >> 8) & 255, (v >> 16) & 255, (v >>> 24) & 255];
const stats = (cls, { died = 0, exp = 0, hc = 0, prog = 0, level }) => (cls & 15) | (died << 4) | (exp << 5) | (hc << 6) | (prog << 8) | (level << 16);
const entry = (name, xp, st) => [...le32(xp), ...le32(0), ...le32(st), ...[...name].map((c) => c.charCodeAt(0)), ...new Array(16 - name.length).fill(0)];
const reply = (mode, entries) => {
  const data = [...le32(0), ...le32(entries.length), ...le32(16), ...entries.flat()];
  return Uint8Array.from([mode, data.length & 255, data.length >> 8, data.length & 255, data.length >> 8, 0, 0, ...data]);
};
const rows = [entry('Zed', 900000, stats(1, { exp: 1, level: 80 })), entry('Hardy', 500000, stats(3, { exp: 1, hc: 1, level: 70 })),
  entry('Olde', 400000, stats(6, { level: 60 })), entry('Dead', 100, stats(0, { exp: 1, died: 1, level: 5 }))];

test('request bytes: mode is the ladder base plus the class tab, then a request id', () => {
  assert.deepEqual([...ladderRequestBody(ladderMode(2), 1)], [0x1b, 1, 0]);
  assert.deepEqual([0, 1, 2, 3].map((k) => ladderMode(k)), [0x09, 0x00, 0x1b, 0x13]);
  assert.equal(ladderMode(2, 3), 0x1e);
});

test('a reply decodes to ranked entries with their stat bits', () => {
  const parsed = parseLadderReply(reply(0x1b, rows));
  assert.equal(parsed.flag, 0x1b); assert.equal(parsed.entries.length, 4);
  assert.deepEqual(parsed.entries[0], { rank: 1, name: 'Zed', experience: 900000, charClass: 1, died: false, expansion: true, hardcore: false, progression: 0, level: 80 });
  assert.equal(parsed.entries[1].hardcore, true); assert.equal(parsed.entries[3].died, true);
});

test('the empty ladder is 14 zero bytes and gives no entries', () => {
  assert.deepEqual(parseLadderReply(new Uint8Array(14)).entries, []);
});

test('a truncated or overrunning reply is refused', () => {
  assert.throws(() => parseLadderReply(new Uint8Array(3)), /short/);
  const bad = reply(0, rows); bad[3] = 0xff; bad[4] = 0xff;
  assert.throws(() => parseLadderReply(bad), /past its buffer/);
});

test('the four ladders and the class tab filter the unfiltered list the realm sends', () => {
  const { entries } = parseLadderReply(reply(0, rows));
  assert.deepEqual(ladderRows(entries, 2).map((r) => [r.rank, r.name]), [[1, 'Zed'], [2, 'Dead']]);
  assert.deepEqual(ladderRows(entries, 3).map((r) => r.name), ['Hardy']);
  assert.deepEqual(ladderRows(entries, 0).map((r) => r.name), ['Olde']);
  assert.deepEqual(ladderRows(entries, 1), []);
  assert.deepEqual(ladderRows(entries, 2, 2).map((r) => r.name), ['Zed']);
  assert.deepEqual(ladderRows(entries, 2, 1).map((r) => r.name), ['Dead']);
});

test('mcpLadderRequest sends op 0x11 and reads the 0x11 reply past other frames', async () => {
  const sent = [], frames = [[0x12, []], [0x11, [...reply(0x1b, rows)]]];
  const chunks = frames.flatMap(([op, body]) => [(body.length + 3) & 255, (body.length + 3) >> 8, op, ...body]);
  let pos = 0, waiter = null;
  const reader = { want: async (n) => { assert.ok(pos + n <= chunks.length); return Uint8Array.from(chunks.slice(pos, pos + n)); }, skip: (n) => { pos += n; } };
  const session = { reader, transport: { send: (b) => sent.push([...b]) }, protocol: { mcpFrame: (op, body) => [op, ...body] } };
  const result = await mcpLadderRequest(session, 0x1b);
  assert.deepEqual(sent, [[0x11, 0x1b, 1, 0]]); assert.equal(result.entries.length, 4);
});

// The screen: a LADDER button opens the list for that kind.
globalThis.ImageData ??= class { constructor(data, w, h) { Object.assign(this, { data, width: w, height: h }); } };
const ctx = new Proxy({}, { get: (_, k) => (k === 'getImageData' ? () => ({ data: new Uint8ClampedArray(4) }) : () => {}), set: () => true });
const documentObject = { createElement: () => ({ width: 0, height: 0, getContext: () => ctx }) };
const frame = { width: 1, height: 1, offsetX: 0, offsetY: 0, indices: new Uint8Array(1), opaque: new Uint8Array([1]) };
const tbl = new Uint8Array(12 + 95 * 14); tbl.set([0x57, 0x6f, 0x6f, 0x21]); tbl[10] = 10;
for (let c = 32; c < 127; c++) { const b = 12 + (c - 32) * 14; tbl[b] = c; tbl[b + 3] = 6; tbl[b + 4] = 10; }
const lobby = (onLadder) => createBnetLobbyScreen({ read: (p) => (p.endsWith('.tbl') ? tbl : new Uint8Array(1 << 22)), decodeDc6: () => ({ frames: [frame, frame, frame, frame, frame, { ...frame, width: 10 }] }),
  decodeDatPalette: () => new Uint8Array(1024), string: () => 'x', character: { name: 'Ama', charClass: 0, level: 7 }, documentObject, now: () => 0, onLadder });
const tick = () => new Promise((r) => setImmediate(r));

test('LADDER then a category asks for that kind and shows the rows; an empty ladder shows the empty state; a failure shows a notice', async () => {
  const asked = [];
  const s = lobby(async (kind) => { asked.push(kind); return kind === 3 ? [] : [{ rank: 1, name: 'Zed', level: 80, experience: 5 }]; });
  s.activate('ladder'); assert.equal(s.ladderState().panel, 'ladder');
  s.activate('ladder-expansion'); assert.equal(s.ladderState().panel, 'ladder-list'); assert.equal(s.ladderState().rows, null);
  await tick(); assert.deepEqual(asked, [2]); assert.equal(s.ladderState().rows.length, 1);
  s.activate('ladder-list-back'); assert.equal(s.ladderState().panel, 'ladder');
  s.activate('ladder-expansion-hardcore'); await tick(); assert.deepEqual(s.ladderState().rows, []);
});

test('a failed ladder request goes back to the LADDER choice with a notice', async () => {
  const s = lobby(async () => { throw new Error('gone'); });
  s.activate('ladder'); s.activate('ladder-standard'); await tick(); await tick();
  assert.equal(s.ladderState().panel, 'ladder');
  assert.equal(s.activate('notice-ok'), true);
});
