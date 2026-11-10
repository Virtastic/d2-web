// A joined game's objects in the scene: the engine's object units (the server's object packets are applied there,
// src/core/realm/d2gs_client_object.cpp and its host test) are shaped by the realm world into the records the
// object renderer and the realm input read. The units are those of a recorded Rogue Encampment join (stash,
// waypoint, fire, torches and the camp's props) as the engine reports them; the classes of the first outdoor
// areas and a town portal are appended in the same form.
import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, openSync, readSync, openAsBlob } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { hashString, decrypt, mpqTables } from './mpq-index.mjs';
import { readMpqFile } from './mpq-read.mjs';
import { createObjectRenderer } from './object-renderer.mjs';
import { readFileSync } from 'node:fs';
import { createRealmWorld, parseRealmObjectUnits, parseRealmObjectEvents, OBJECT_UNIT_WORDS } from './realm-world.mjs';
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

const objects = createObjectTable(read);
// One engine unit record (14 words) as the engine makes it for a class in a mode: the mode's cel count, start cel
// and step are the class's, it can be picked if the class says so for the mode, and it stands in a revealed room.
const unit = (id, classId, mode, { x = 0x1000, y = 0x1000, interact = 0, clock, selectable, inRoom = true, overlays = [] } = {}) => {
  const cls = objects.byClass(classId);
  return [id, classId, x, y, mode, clock ?? (cls.start?.[mode] ?? 0) << 8, (cls.frameCount?.[mode] ?? 0) << 8, cls.frameDelta?.[mode] ?? 0,
    ((selectable ?? Boolean(cls.selectable?.[mode])) ? 1 : 0) | (inRoom ? 2 : 0), interact, overlays.length, ...overlays, ...[-1, -1, -1].slice(overlays.length)];
};
const parse = (records) => parseRealmObjectUnits(Int32Array.from(records.flat()), records.length);
// The join's units: each assign packet's fields, in the mode the last mode-change packet for it names.
const joinUnits = () => {
  const byId = new Map();
  for (const text of JOIN) {
    const p = hex(text), id = p[2];
    if (p[0] === 0x51) byId.set(id, { classId: p[6] | (p[7] << 8), x: p[8] | (p[9] << 8), y: p[10] | (p[11] << 8), mode: p[12], interact: p[13] });
    else byId.get(id).mode = p[8];
  }
  return parse([...byId].map(([id, o]) => unit(id, o.classId, o.mode, o)));
};

test('every object of a recorded camp join is drawn from its engine unit', { skip }, async () => {
  const w = world();
  const views = w.objectViews(joinUnits());
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
  const torch = views.find((o) => o.id === 0x08);
  assert.deepEqual([torch.classId, torch.mode, torch.framePrecise, torch.frameCountPrecise, torch.animSpeed], [37, 2, 0, 20 << 8, 200]);
});

test('the classes of the first outdoor areas draw in the modes the server puts them in', { skip }, async () => {
  const w = world(); let guid = 100; const cases = [];
  // barrel, chests, casket, shrine, doors, well, cairn stone: idle, operating, opened; a town portal opening and open
  for (const classId of [11, 5, 6, 1, 3, 2, 13, 14, 15, 130, 17]) for (const mode of [0, 1, 2]) cases.push([classId, mode]);
  cases.push([59, 1], [59, 2]);
  const views = w.objectViews(parse(cases.map(([classId, mode]) => unit(guid++, classId, mode, { interact: classId === 59 ? 1 : 0 }))));
  assert.equal(views.length, cases.length);
  const failed = [];
  for (const view of views) if (!(await draw(view)).evidence) failed.push(`${view.classId}:${view.mode}`);
  assert.deepEqual(failed, []);
  assert.deepEqual(views.filter((o) => o.classId === 59).map((o) => o.portalLevel), [1, 1]);
});

test('a view is the engine unit as it stands: its mode, cel clock, pick flag, room and overlays', { skip }, () => {
  const w = world();
  // A locked chest mid-way through opening and not pickable; an idle shrine with its two overlays; a torch whose
  // room is not revealed; a unit of a class the tables do not know.
  const words = Int32Array.from([
    200, 5, 0x1234, 0x5678, 1, 0x180, 0x300, 0x80, 2, 0x80, 0, -1, -1, -1,
    201, 2, 0x1000, 0x1001, 0, 0, 0x100, 0x100, 3, 6, 2, 0x3B, 0x39, -1,
    202, 37, 0x1002, 0x1003, 2, 1400, 20 << 8, 200, 0, 0, 0, -1, -1, -1,
    203, 0x7fff, 1, 2, 0, 0, 0x100, 0x100, 3, 0, 0, -1, -1, -1]);
  assert.equal(words.length, 4 * OBJECT_UNIT_WORDS);
  const views = w.objectViews(parseRealmObjectUnits(words, 4));
  assert.deepEqual(views.map((o) => o.id), [200, 201, 202]);
  const [chest, shrine, torch] = views;
  assert.deepEqual([chest.x, chest.y, chest.mode, chest.framePrecise, chest.frameCountPrecise, chest.animSpeed, chest.selectable, chest.locked, chest.inClientRoom],
    [0x1234, 0x5678, 1, 0x180, 0x300, 0x80, false, true, true]);
  assert.deepEqual([shrine.selectable, shrine.overlays, shrine.portalLevel, shrine.locked], [true, [0x3B, 0x39], 0, false]);
  assert.deepEqual([torch.framePrecise, torch.selectable, torch.inClientRoom, torch.overlays], [1400, false, false, []]);
  // An id above 2^31 is unsigned.
  assert.equal(parseRealmObjectUnits(Int32Array.from([-2, 5, 0, 0, 0, 0, 0x100, 0, 3, 0, 0, -1, -1, -1]), 1)[0].id, 0xFFFFFFFE);
});

test('an object activation carries the sound row and the player it plays at', () => {
  const events = parseRealmObjectEvents(Int32Array.from([1, 0x20, 1, 6, 0xA68, 0, 1, 0xA0, 2, 16, 0xA76, 1]), 2);
  assert.deepEqual(events, [{ objectId: 0x20, playerId: 1, shrine: 6, soundRow: 0xA68, effect: 0 },
    { objectId: 0xA0, playerId: 2, shrine: 16, soundRow: 0xA76, effect: 1 }]);
  assert.throws(() => parseRealmObjectEvents(Int32Array.from([2, 0, 0, 0, 0, 0]), 1));
});

test('a click on the stash walks to it, uses it on arrival, and the server reply opens the panel', { skip }, () => {
  const w = world(); const units = joinUnits();
  let player = { x: 0x0f92 + 30, y: 0x146d + 30 }; const out = [];
  const scene = () => ({ ready: true, player, run: false, selected: { left: 0, right: 0 }, monsters: [], objects: w.objectViews(units), items: [],
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
});

// The engine's object unit is the one source: the page keeps no object state of its own and steps no object clock.
test('the page reads objects from the engine and keeps no copy of their state', () => {
  const at = (name) => readFileSync(new URL(name, import.meta.url), 'utf8');
  const worldSource = at('./realm-world.mjs'), shell = at('../gameplay-shell.mjs'), client = at('./realm-client.mjs'), link = at('../../wasm-build/link-d2.sh');
  assert.doesNotMatch(worldSource, /case 0x51|case 0x0e|objectsByGuid|tickObjects/);
  assert.doesNotMatch(shell, /tickObjects/);
  assert.match(shell, /objects: protocol\?\.d2gsClientObjects\?\.\(\) \?\? \[\]/);
  assert.match(shell, /d2gsClientTakeObjectEvents\(\)\) if \(used\.soundRow\) unitSounds\?\.playRow\(used\.soundRow/);
  for (const name of ['d2_realm_client_object_units', 'd2_realm_client_take_object_events']) {
    assert.ok(client.includes(`'${name}'`), name);
    assert.ok(link.includes(`_${name},`), name);
  }
  const w = createRealmWorld({ items: { byCode: () => null }, objects: { byClass: () => ({ token: 'CH' }) } });
  feed(w, '5102110000000b01920f6d140000', '0e0211000000030002000000');
  assert.deepEqual(w.objectViews(), []);
});
