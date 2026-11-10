// A joined game's objects, end to end: the server's object packets go through the realm world, its records
// through the object renderer, and a click on one through the realm input. The packets are the object part of
// a recorded Rogue Encampment join (stash, waypoint, fire, torches and the camp's props); the classes of the
// first outdoor areas and a town portal are appended in the same packet form.
import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, openSync, readSync, openAsBlob } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { hashString, decrypt, mpqTables } from './mpq-index.mjs';
import { readMpqFile } from './mpq-read.mjs';
import { createObjectRenderer } from './object-renderer.mjs';
import { createRealmWorld } from './realm-world.mjs';
import { createItemTable, createObjectTable } from './realm-tables.mjs';
import { createRealmInput } from './realm-input.mjs';

const JOIN = ['51020d00000025006c0f68140200', '0e020d000000030002000000', '51020c0000002500770f95140200', '0e020c000000030002000000',
  '51020b00000025006f0f7b140200', '0e020b000000030002000000', '5102110000000b01920f6d140000', '51021000000025007a0f5b140200',
  '0e0210000000030002000000', '51020f0000002500880f72140200', '0e020f000000030002000000', '51020e0000002400980f60140000',
  '51020a0000007700b30f59140200', '0e020a000000030102000000', '5102090000002500ab0f51140200', '0e0209000000030002000000',
  '5102080000002500a60f65140200', '0e0208000000030002000000', '5102070000002500bf0f5a140200', '0e0207000000030002000000',
  '5102060000004e008b0f7b140000', '5102050000002500990f9b140200', '0e0205000000030002000000', '51020400000023008c0f98140000',
  '51020300000027009d0f7a140000', '5102020000008101a40f79140000', '5102010000002500b40f7d140200', '0e0201000000030002000000'];
const STASH = 0x11, WAYPOINT = 0x0a, FIRE = 0x03;
const SIZE = { 0x51: 14, 0x0e: 12, 0x0a: 6, 0x77: 2 };
const hex = (text) => Uint8Array.from(text.match(/../g), (b) => parseInt(b, 16));
const feed = (world, ...packets) => { for (const p of packets) world.feed(typeof p === 'string' ? hex(p) : Uint8Array.from(p), (b) => SIZE[b[0]]); };
const u32 = (v) => [v & 255, (v >> 8) & 255, (v >> 16) & 255, (v >>> 24) & 255];
const add = (guid, classId, mode, interact = 0) => [0x51, 2, ...u32(guid), classId & 255, classId >> 8, 0x00, 0x10, 0x00, 0x10, mode, interact];
const setMode = (guid, mode) => [0x0e, 2, ...u32(guid), 3, 0, ...u32(mode)];

const archives = [];
for (const name of ['patch_d2.mpq', 'd2exp.mpq', 'd2data.mpq']) {
  const archive = fileURLToPath(new URL('../../retail/D2/' + name, import.meta.url));
  if (!existsSync(archive)) continue;
  const fd = openSync(archive, 'r'), blob = await openAsBlob(archive);
  const at = (p, l) => { const b = Buffer.alloc(l); readSync(fd, b, 0, l, p); return new Uint8Array(b); };
  const head = at(0, 32), [[hp, hn], [bp, bn]] = mpqTables(head), shift = new DataView(head.buffer).getUint16(14, true);
  const table = (pos, n, key) => { const u = new Uint32Array(n * 4); new Uint8Array(u.buffer).set(at(pos, n * 16)); decrypt(u, hashString(key, 3)); return u; };
  archives.push({ blob, shift, hn, bn, hash: table(hp, hn, '(hash table)'), block: table(bp, bn, '(block table)') });
}
const skip = archives.length < 3 ? 'retail archives absent' : false;
async function fetchFile(name) {
  for (const a of archives) {
    const x = hashString(name, 1), y = hashString(name, 2);
    for (let i = hashString(name, 0) & (a.hn - 1), n = 0; n < a.hn; i = (i + 1) & (a.hn - 1), n++) {
      const bi = a.hash[i * 4 + 3]; if (bi === 0xFFFFFFFF) break;
      if (bi < a.bn && a.hash[i * 4] === x && a.hash[i * 4 + 1] === y) {
        const entry = { pos: a.block[bi * 4], csize: a.block[bi * 4 + 1], size: a.block[bi * 4 + 2], flags: a.block[bi * 4 + 3] };
        const f = await readMpqFile(a.blob, 0, a.shift, entry, name); if (f) return new Uint8Array(await f.arrayBuffer());
      }
    }
  }
  return null;
}
const files = new Map(), wanted = new Set();
const read = (path) => { const key = path.toLowerCase(), f = files.get(key); if (!f) { wanted.add(key); throw new Error('missing ' + path); } return f; };
if (!skip) for (const name of ['objects.txt', 'weapons.txt', 'armor.txt', 'misc.txt'].map((n) => 'data\\global\\excel\\' + n)
  .concat(['data\\global\\palette\\ACT1\\pal.dat', 'data\\global\\palette\\ACT1\\pal.pl2'])) files.set(name.toLowerCase(), await fetchFile(name));

globalThis.ImageData ??= class { constructor(data, width, height) { this.data = data; this.width = width; this.height = height; } };
const ctx = { canvas: { width: 800, height: 600 }, drawImage() {}, getImageData: (x, y, w, h) => ({ data: new Uint8ClampedArray(w * h * 4) }), putImageData() {} };
const documentObject = { createElement: () => ({ width: 800, height: 600, getContext: () => ctx }) };
// Draws one object record as the frame loop does, loading the art the renderer asks for, and returns its draw records.
async function draw(object) {
  for (let pass = 0; pass < 8; pass++) {
    const renderer = createObjectRenderer(read, documentObject);
    globalThis.__d2DrawList = [];
    const evidence = renderer.draw(ctx, 400, 300, object, 1), list = globalThis.__d2DrawList;
    globalThis.__d2DrawList = null;
    if (evidence || !wanted.size) { wanted.clear(); return { evidence, list }; }
    for (const key of [...wanted]) { wanted.delete(key); const bytes = await fetchFile(key); if (bytes) files.set(key, bytes); }
  }
  return { evidence: null, list: [] };
}
const world = () => createRealmWorld({ items: createItemTable(read), objects: createObjectTable(read) });
const artOf = (list) => list.filter((r) => r.op === 'dcc').map((r) => r.path.toLowerCase().split('\\').pop());

test('every object of a recorded camp join is recorded and drawn', { skip }, async () => {
  const w = world(); feed(w, ...JOIN);
  const views = w.objectViews();
  assert.equal(views.length, 17);
  const stash = views.find((o) => o.id === STASH);
  assert.deepEqual([stash.classId, stash.token.toLowerCase(), stash.x, stash.y, stash.mode, stash.selectable, stash.componentMask],
    [267, 'b6', 0x0f92, 0x146d, 0, true, 0xFFFF]);
  const drawn = await draw(stash);
  assert.ok(drawn.evidence, 'the stash draws');
  assert.deepEqual(artOf(drawn.list), ['b6trlitnuhth']);
  assert.equal(drawn.list[0].frame, 0);
  const art = new Map();
  for (const view of views) {
    const { evidence, list } = await draw(view);
    assert.equal(Boolean(evidence), artOf(list).length > 0);
    art.set(view.id, artOf(list).join(','));
  }
  // Each guid with the art files of its draw records. Class 78 is the camp's one object with no art in the archives.
  assert.deepEqual([...art].sort((a, b) => a[0] - b[0]), [[1, 'totrlitonhth'], [2, 'sstrlitnuhth'], [3, 'rbtrlitnuhth'], [4, 'n1trlitnuhth'],
    [5, 'totrlitonhth'], [6, ''], [7, 'totrlitonhth'], [8, 'totrlitonhth'], [9, 'totrlitonhth'], [10, 'wptrlitonhth,wps1litonhth'],
    [11, 'totrlitonhth'], [12, 'totrlitonhth'], [13, 'totrlitonhth'], [14, 'n2trlitnuhth'], [15, 'totrlitonhth'], [16, 'totrlitonhth'],
    [17, 'b6trlitnuhth']]);
  assert.equal(views.find((o) => o.id === 6).classId, 78);
  for (const mode of ['nu', 'op', 'on']) assert.equal(await fetchFile(`data\\global\\objects\\TA\\cof\\TA${mode}hth.cof`), null);
  const waypoint = views.find((o) => o.id === WAYPOINT);
  assert.deepEqual([waypoint.classId, waypoint.mode, waypoint.selectable], [119, 2, true]);
});

test('the classes of the first outdoor areas draw in the modes the server puts them in', { skip }, async () => {
  const w = world(); let guid = 100; const cases = [];
  // barrel, chests, casket, shrine, doors, well, cairn stone: idle, operating, opened; a town portal opening and open
  for (const classId of [11, 5, 6, 1, 3, 2, 13, 14, 15, 130, 17]) for (const mode of [0, 1, 2]) cases.push([classId, mode]);
  cases.push([59, 1], [59, 2]);
  for (const [classId, mode] of cases) feed(w, add(guid++, classId, mode, classId === 59 ? 1 : 0));
  const views = w.objectViews();
  assert.equal(views.length, cases.length);
  const failed = [];
  for (const view of views) if (!(await draw(view)).evidence) failed.push(`${view.classId}:${view.mode}`);
  assert.deepEqual(failed, []);
  assert.deepEqual(views.filter((o) => o.classId === 59).map((o) => o.portalLevel), [1, 1]);
});

test('an object animates on the client clock and an opened chest stops being selectable', { skip }, () => {
  const w = world(); feed(w, ...JOIN, add(200, 5, 0, 0x80), add(201, 5, 0));
  const fire = () => w.objectViews().find((o) => o.id === FIRE);
  const before = fire(); assert.ok(before.frameCountPrecise > 256 && before.animSpeed > 0);
  const seen = new Set();
  for (let tick = 0; tick < 400; tick++) { w.tickObjects(); const f = fire(); assert.ok(f.framePrecise < f.frameCountPrecise); seen.add(f.framePrecise >>> 8); }
  assert.equal(seen.size, before.frameCountPrecise >>> 8, 'the fire shows every cel of its loop');
  const chest = (id) => w.objectViews().find((o) => o.id === id);
  assert.deepEqual([chest(200).selectable, chest(200).locked, chest(201).locked], [true, true, false]);
  feed(w, setMode(200, 2));                                  // the server opens it: one cel a mode, no run of cels
  assert.deepEqual([chest(200).mode, chest(200).selectable], [2, false]);
  feed(w, add(202, 11, 0)); const barrel = () => chest(202);
  assert.equal(barrel().selectable, true);
  feed(w, setMode(202, 1));                                  // a barrel breaks over its cels, then lies broken
  w.tickObjects(); assert.deepEqual([barrel().mode, barrel().framePrecise > 0, barrel().selectable], [1, true, false]);
  for (let tick = 0; tick < 100; tick++) w.tickObjects();
  assert.deepEqual([barrel().mode, barrel().selectable], [2, false], 'the breaking cels run out into the broken mode');
});

test('a click on the stash walks to it, uses it on arrival, and the server reply opens the panel', { skip }, () => {
  const w = world(); feed(w, ...JOIN);
  let player = { x: 0x0f92 + 30, y: 0x146d + 30 }; const out = [];
  const scene = () => ({ ready: true, player, run: false, selected: { left: 0, right: 0 }, monsters: [], objects: w.objectViews(), items: [],
    isNpc: () => false, hoverBox: () => false, toScreen: (x, y) => ({ x: x * 2, y }), pointerWorld: (px, py) => ({ x: px / 2, y: py }) });
  const input = createRealmInput({ transmit: (b) => out.push(Array.from(b)), scene, now: () => 0 });
  input.leftClick(0x0f92 * 2, 0x146d);
  assert.deepEqual(out, [[0x02, 2, 0, 0, 0, STASH, 0, 0, 0]]);
  input.tick(); assert.equal(out.length, 1);
  player = { x: 0x0f92 + 1, y: 0x146d + 1 };
  input.tick();
  assert.deepEqual(out.at(-1), [0x13, 2, 0, 0, 0, STASH, 0, 0, 0]);
  feed(w, [0x77, 16]);
  assert.deepEqual(w.takeUiEvents(), [16]);
  feed(w, [0x0a, 2, ...u32(STASH)]);
  assert.equal(w.objectViews().some((o) => o.id === STASH), false);
});
