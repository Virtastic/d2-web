// The channel screen against the retail capture (vectors/drawlist/bnet-channel.txt, tick 11 of the headless
// front-end capture): one channel, four users, three chat lines, the caret showing. Skipped without the
// retail archives or the capture. The statstring class/level layout below is the capture's guess, not retail's.
import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { openRetail } from '../../tools/dcc-decode-bench.mjs';
import { decodeDc6, decodeDatPalette } from './d2-formats.mjs';
import { diffDrawLists, parseDrawListVector } from './drawlist.mjs';
import { createBnetLobbyScreen } from './bnet-lobby-screen.mjs';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const vector = join(root, 'vectors', 'drawlist', 'bnet-channel.txt');
const rd = existsSync(vector) ? openRetail() : null;

// string.tbl, expansionstring.tbl and patchstring.tbl as a map from string id to text (the hash node's index field is the id).
function stringTable(read) {
  const map = new Map();
  for (const f of ['string', 'expansionstring', 'patchstring']) {
    const b = read(`data\\local\\lng\\eng\\${f}.tbl`), v = new DataView(b.buffer, b.byteOffset, b.byteLength);
    const n = v.getUint16(2, true), hashN = v.getUint32(4, true), base = 21 + n * 2;
    for (let i = 0; i < hashN; i++) {
      const o = base + i * 17; if (!b[o]) continue;
      const id = v.getUint16(o + 1, true), at = v.getUint32(o + 11, true), len = v.getUint16(o + 15, true);
      map.set(id, new TextDecoder('latin1').decode(b.subarray(at, at + len - 1)));
    }
  }
  return map;
}

globalThis.ImageData ??= class { constructor(data, w, h) { Object.assign(this, { data, width: w, height: h }); } };
const ctx = new Proxy({}, { get: (_, k) => (k === 'getImageData' ? () => ({ data: new Uint8ClampedArray(4) }) : () => {}), set: () => true });
const documentObject = { createElement: () => ({ width: 0, height: 0, getContext: () => ctx }) };

// The capture's statstring: "PX2D" "USEast," name "," 30 bytes, byte 2 = class + 1, byte 13 = level (as the capture builds it).
const stat = (name, cls, level) => {
  const b = new Uint8Array(30).fill(0xff); b[0] = 1; b[1] = 1; b[2] = cls + 1; b[13] = level;
  return `PX2DUSEast,${name},${String.fromCharCode(...b)}`;
};

test('channel screen draws the retail capture record for record', { skip: !rd }, () => {
  const strings = stringTable(rd);
  const screen = createBnetLobbyScreen({ read: rd, decodeDc6, decodeDatPalette, string: (i) => strings.get(i) ?? '', character: { name: 'Hero' },
    documentObject, now: () => 0 });
  screen.pinClock(2750, 2750 - 8 * 40);
  screen.activate('enter-chat'); screen.hover(0, 1, 800, 600);
  const ev = (event, username, text) => ({ event, username, text, raw: text });
  screen.chatEvent(ev(7, '', 'Diablo II Test Channel'));
  [['Aelwyn', 0, 87], ['Brightstar', 1, 45], ['Cinder', 2, 12], ['Dorian', 3, 99]].forEach(([n, c, l]) => screen.chatEvent(ev(1, n, stat(n, c, l))));
  screen.chatEvent(ev(5, 'Aelwyn', 'Hello everyone'));
  screen.chatEvent(ev(5, 'Brightstar', 'Anyone for a Baal run?'));
  screen.chatEvent(ev(5, 'Cinder', 'sure, give me a minute'));
  const ours = [];
  globalThis.__d2DrawList = ours;
  try { screen.draw(ctx, 800, 600); } finally { delete globalThis.__d2DrawList; }
  // The capture's recorder has no frame bytes: hash 0 on its side, so zero ours.
  const retail = parseDrawListVector(readFileSync(vector, 'utf8')).filter((r) => r.op === 'cel' || r.op === 'celcolor');
  const d = diffDrawLists(retail, ours.map((r) => ({ ...r, hash: 0 })));
  console.log(`bnet-channel: ${retail.length} retail draws, ${ours.length} ours, differing records ${d.diffs} (missing ${d.missing.length}, extra ${d.extra.length}, differing ${d.differing.length})`);
  for (const m of d.missing.slice(0, 12)) console.log('  missing', m.op, m.path, m.frame, m.x, m.y, m.mode);
  for (const m of d.extra.slice(0, 12)) console.log('  extra', m.op, m.path, m.frame, m.x, m.y, m.mode);
  for (const m of d.differing.slice(0, 12)) console.log('  differs', m.draw, m.diff);
  assert.equal(d.diffs, 0);
});
