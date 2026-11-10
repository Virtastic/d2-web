// A joined game's objects in the scene: the server's object packets are applied by the engine's object units
// (src/core/realm/d2gs_client_object.cpp), and the page reads the units back as the records the object renderer
// and the realm input take. Each test here feeds packets to those units (built natively, with the rows of the
// game's own Objects table) and reads them through the page's own wrapper. The packets are those of a recorded
// Rogue Encampment join (stash, waypoint, fire, torches and the camp's props); the classes of the first outdoor
// areas and a town portal are assigned in the same form.
import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, openSync, readSync, openAsBlob } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { hashString, decrypt, mpqTables } from './mpq-index.mjs';
import { readMpqFile } from './mpq-read.mjs';
import { createObjectRenderer } from './object-renderer.mjs';
import { readFileSync } from 'node:fs';
import { spawnSync } from 'node:child_process';
import { mkdtempSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { createRealmWorld } from './realm-world.mjs';
import { parseRealmObjectEvents, OBJECT_UNIT_BYTES } from './realm-objects.mjs';
import { bindEngineProtocol } from './realm-client.mjs';
import { realmViewSnapshot } from './realm-client-view.mjs';
import { createItemTable } from './realm-tables.mjs';
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
const root = fileURLToPath(new URL('../../', import.meta.url));
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
const world = () => createRealmWorld({ items: createItemTable(read) });
const artOf = (list) => list.filter((r) => r.op === 'dcc').map((r) => r.path.toLowerCase().split('\\').pop());

// The engine's object unit code, built natively (realm-objects.test.cpp), with the Objects table rows it
// looks classes up in given from the game's own file.
const dir = mkdtempSync(join(tmpdir(), 'd2-realm-objects-'));
test.after(() => rmSync(dir, { recursive: true, force: true }));
writeFileSync(join(dir, 'emscripten.h'), '#define EMSCRIPTEN_KEEPALIVE\n');
const core = join(root, 'src/core/d2moo');
const build = spawnSync('g++', ['-std=c++17', '-w', '-UNDEBUG', `-I${dir}`, `-I${core}/compat`, `-I${core}/include`, `-I${core}/include/D2Game`,
  join(root, 'play/assets/realm-objects.test.cpp'), '-o', join(dir, 'probe')], { encoding: 'utf8' });
const tableLines = [];
if (!skip) {
  const lines = new TextDecoder('latin1').decode(read('data\\global\\excel\\objects.txt')).replace(/\r/g, '').split('\n').filter((l) => l.length);
  const header = lines.shift().split('\t').map((name) => name.trim().toLowerCase());
  for (const line of lines) {
    const values = line.split('\t'), cell = (name) => (values[header.indexOf(name)] ?? '').trim(), n = (name) => Number(cell(name)) || 0;
    if (cell('id') === '' || !cell('token')) continue;
    const modes = Array.from({ length: 8 }, (_, m) => ['selectable', 'framecnt', 'framedelta', 'cycleanim', 'start', 'hascollision'].map((c) => n(c + m)).join(' '));
    tableLines.push(`txt ${n('id')} ${cell('token')} ${n('orientation')} ${n('subclass')} ${n('lockable')} ${n('isdoor')} ${modes.join(' ')}`);
  }
}
const pkt = (p) => `p ${typeof p === 'string' ? p : Array.from(p, (b) => b.toString(16).padStart(2, '0')).join('')}`;
const assign = (guid, classId, mode, { x = 0x1000, y = 0x1000, interact = 0 } = {}) => pkt([0x51, 2, ...u32(guid), classId & 255, classId >> 8, x & 255, x >> 8, y & 255, y >> 8, mode, interact]);
const setMode = (guid, mode) => pkt([0x0e, 2, ...u32(guid), 3, 0, ...u32(mode)]);
const remove = (guid) => pkt([0x0a, 2, ...u32(guid)]);
// The object records the page holds after the engine took the commands: the engine's read-back through the
// page's own wrapper, exactly as the frame loop takes them.
function readBack(commands) {
  assert.equal(build.status, 0, build.stderr);
  const run = spawnSync(join(dir, 'probe'), [], { encoding: 'utf8', maxBuffer: 1 << 26, input: [...tableLines, ...commands, 'dump', ''].join('\n') });
  assert.equal(run.status, 0, run.stderr);
  const [, count, text = ''] = run.stdout.trim().split(' ');
  const records = hex(text);
  assert.equal(records.length, Number(count) * OBJECT_UNIT_BYTES);
  const HEAPU8 = new Uint8Array(1 << 18); let top = 16;
  const Module = { HEAPU8, _malloc: (n) => { const p = top; top += n; return BigInt(p); },
    _d2_realm_client_object_units() {},
    ccall: (name, _rt, _args, values) => { if (name !== 'd2_realm_client_object_units') return 0;
      const n = Math.min(Number(count), values[1]); HEAPU8.set(records.subarray(0, n * OBJECT_UNIT_BYTES), Number(values[0])); return n; } };
  return bindEngineProtocol(Module, 16384).d2gsClientObjects();
}
const join17 = () => readBack(JOIN.map(pkt));

test('every object of a recorded camp join is drawn from its engine unit', { skip }, async () => {
  const views = join17();
  assert.equal(views.length, 17);
  const stash = views.find((o) => o.id === STASH);
  assert.deepEqual([stash.classId, stash.token.toLowerCase(), stash.x, stash.y, stash.mode, stash.selectable, stash.componentMask, stash.inClientRoom],
    [267, 'b6', 0x0f92, 0x146d, 0, true, 0xFFFF, true]);
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
  // The scene's objects are these records themselves.
  const snapshot = realmViewSnapshot({ base: null, resources: null, monsters: [], missiles: [], players: [], level: 1, actor: () => null, frame: 0, world: world(), objects: views });
  assert.equal(snapshot.objects, views);
});

test('the classes of the first outdoor areas draw in the modes the server puts them in', { skip }, async () => {
  let guid = 100; const cases = [];
  // barrel, chests, casket, shrine, doors, well, cairn stone: idle, operating, opened; a town portal opening and open
  for (const classId of [11, 5, 6, 1, 3, 2, 13, 14, 15, 130, 17]) for (const mode of [0, 1, 2]) cases.push([classId, mode]);
  cases.push([59, 1], [59, 2]);
  const views = readBack(cases.map(([classId, mode]) => assign(guid++, classId, mode, { interact: classId === 59 ? 1 : 0 })));
  assert.equal(views.length, cases.length);
  const failed = [];
  for (const view of views) if (!(await draw(view)).evidence) failed.push(`${view.classId}:${view.mode}`);
  assert.deepEqual(failed, []);
  assert.deepEqual(views.filter((o) => o.classId === 59).map((o) => o.portalLevel), [1, 1]);
  assert.deepEqual(views.filter((o) => o.classId !== 59).map((o) => o.portalLevel).filter(Boolean), []);
});

test('a record is the engine unit as it stands: its mode, cel clock, pick flag, lock, room and overlays', { skip }, () => {
  // A locked chest the server set opening, three ticks on; an idle shrine of a glowing kind, seven ticks on; a
  // torch assigned into a room not yet revealed; a class the table does not have, which makes no unit.
  const by = (views) => new Map(views.map((o) => [o.id, o]));
  const commands = ['shrine 6 6', assign(200, 5, 0, { x: 0x1234, y: 0x5678, interact: 0x80 }), assign(201, 2, 0, { interact: 6 }),
    'room 0', assign(202, 37, 2), 'room 1', assign(203, 0x7fff, 0)];
  let got = by(readBack(commands));
  assert.deepEqual([...got.keys()].sort(), [200, 201, 202]);
  let chest = got.get(200), shrine = got.get(201), torch = got.get(202);
  assert.deepEqual([chest.token, chest.x, chest.y, chest.mode, chest.framePrecise, chest.selectable, chest.locked, chest.inClientRoom, chest.overlays],
    ['L1', 0x1234, 0x5678, 0, 0, true, true, true, []]);
  assert.deepEqual([shrine.selectable, shrine.overlays, shrine.overlayAges, shrine.portalLevel, shrine.locked], [true, [0x3B, 0x39], [0, 0], 0, false]);
  assert.deepEqual([torch.mode, torch.inClientRoom, torch.selectable, torch.overlays, torch.overlayAges], [2, false, false, [], []]);

  // The chest's opening is one cel and holds; a barrel's breaking cels run on the client clock and end in the
  // opened mode with no packet; the shrine's overlays age a frame a tick; the torch's cels loop.
  const later = (ticks) => by(readBack([...commands, assign(204, 1, 0), setMode(200, 1), setMode(204, 1), `tick ${ticks}`]));
  got = later(1);
  chest = got.get(200); torch = got.get(202);
  const barrel = got.get(204);
  assert.deepEqual([chest.mode, chest.selectable, chest.frameCountPrecise], [1, false, 256]);
  assert.deepEqual([barrel.mode, barrel.selectable, barrel.framePrecise], [1, false, barrel.animSpeed]);
  assert.ok(barrel.animSpeed > 0 && barrel.frameCountPrecise > 256);
  assert.equal(torch.framePrecise, 200);
  got = later(7);
  assert.deepEqual(got.get(201).overlayAges, [7, 7]);
  assert.equal(got.get(204).framePrecise, Math.min(7 * barrel.animSpeed, barrel.frameCountPrecise - 256));
  got = later(200);
  assert.deepEqual([got.get(200).mode, got.get(204).mode], [1, 2]);
  assert.equal(got.get(202).framePrecise, (200 * 200) % (20 << 8));

  // A used shrine loses its overlays; a removed object is gone; an assign for a known id replaces the unit.
  got = by(readBack([...commands, setMode(201, 2), remove(202), assign(200, 11, 0)]));
  assert.deepEqual([...got.keys()].sort(), [200, 201]);
  assert.deepEqual([got.get(201).mode, got.get(201).overlays], [2, []]);
  assert.deepEqual([got.get(200).classId, got.get(200).locked], [11, false]);
  // An id above 2^31 is unsigned.
  assert.equal(readBack([assign(0xFFFFFFFE, 5, 0)])[0].id, 0xFFFFFFFE);
});

test('an object activation carries the sound row and the player it plays at', () => {
  const events = parseRealmObjectEvents(Int32Array.from([1, 0x20, 1, 6, 0xA68, 0, 1, 0xA0, 2, 16, 0xA76, 1]), 2);
  assert.deepEqual(events, [{ objectId: 0x20, playerId: 1, shrine: 6, soundRow: 0xA68, effect: 0 },
    { objectId: 0xA0, playerId: 2, shrine: 16, soundRow: 0xA76, effect: 1 }]);
  assert.throws(() => parseRealmObjectEvents(Int32Array.from([2, 0, 0, 0, 0, 0]), 1));
});

test('a click on the stash walks to it, uses it on arrival, and the server reply opens the panel', { skip }, () => {
  const w = world(); const objects = join17();
  let player = { x: 0x0f92 + 30, y: 0x146d + 30 }; const out = [];
  const scene = () => ({ ready: true, player, run: false, selected: { left: 0, right: 0 }, monsters: [], objects, items: [],
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

// The engine's object unit is the one model. These fail if the page regains an object packet parser, an object
// record store, an object clock or its own reading of the Objects table, or reads the engine through a path that
// can come back empty without an error.
test('the page keeps no object model of its own and cannot read the engine silently empty', () => {
  const at = (name) => readFileSync(new URL(name, import.meta.url), 'utf8');
  const worldSource = at('./realm-world.mjs'), shell = at('../gameplay-shell.mjs'), client = at('./realm-client.mjs'), link = at('../../wasm-build/link-d2.sh');
  const code = worldSource.split('\n').filter((line) => !/^\s*(\/\/|\*|\/\*)/.test(line)).join('\n');
  assert.doesNotMatch(code, /case 0x51|case 0x0e|case 0x4d|objectsByGuid|tickObjects|objectViews|objectTable|enterMode|stepObject|object/);
  assert.doesNotMatch(at('./realm-tables.mjs'), /objects\.txt|createObjectTable/i);
  for (const name of ['./realm-objects.mjs', './realm-client-view.mjs'])
    assert.doesNotMatch(at(name), /objects\.txt|byClass|case 0x51|case 0x0e|new Map\(/i, name);
  assert.doesNotMatch(shell, /tickObjects|createObjectTable|objectViews|d2gsClientObjects\?\./);
  assert.match(shell, /objects: protocol \? protocol\.d2gsClientObjects\(\) : \[\]/);
  assert.match(shell, /d2gsClientTakeObjectEvents\(\)\) if \(used\.soundRow\) unitSounds\?\.playRow\(used\.soundRow/);
  for (const name of ['d2_realm_client_object_units', 'd2_realm_client_take_object_events']) {
    assert.ok(client.includes(`'${name}'`), name);
    assert.ok(link.includes(`_${name},`), name);
  }
  // The world is fed object packets and nothing of it answers for an object.
  const w = createRealmWorld({ items: { byCode: () => null } });
  feed(w, '5102110000000b01920f6d140000', '0e0211000000030002000000');
  assert.deepEqual(Object.keys(w).filter((key) => /object/i.test(key)), []);
  // An engine built without the read-back is an error at the first frame, not a scene without objects.
  const HEAPU8 = new Uint8Array(1 << 16); let top = 16;
  const bare = { HEAPU8, _malloc: (n) => { const p = top; top += n; return BigInt(p); }, ccall: () => 0 };
  assert.throws(() => bindEngineProtocol(bare, 4096).d2gsClientObjects(), /d2_realm_client_object_units/);
});
