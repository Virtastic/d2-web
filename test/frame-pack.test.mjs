import { test } from 'node:test';
import assert from 'node:assert/strict';
import { packTerrain, unpackTerrain, transferList, TERRAIN_BYTES, packUnits, unpackUnits, packFacings, unpackFacings,
  packLightmap, unpackLightmap, packFrame, unpackFrame, frameTransferList } from '../play/frame-pack.mjs';

const U = 0xffffffff, I = 2147483647;
const mk = (i, over = {}) => ({ room: i, layer: i % 3, ordinal: i, orientation: -i - 1, mainIndex: i * 7, subIndex: -1, frameIndex: 0,
  worldX: -1000 - i, worldY: 1000 + i, screenX: -5, screenY: 0, width: 160, height: 80, flags: 0x80000001, librarySlot: 3, libraryIndex: i,
  entryHint: -1, pop: 2, entry: i, ...over });
const fix = (a) => { for (const r of a) if (r.entry === undefined) delete r.entry; return a; };
const sample = () => fix([
  mk(0), mk(1, { layer: 1, coord: { nIndex: 4, x0: -9, y0: 0, x1: I, y1: -I - 1 }, hidesPlayer: true }),
  mk(2, { layer: 1, coord: null, hidesPlayer: false }), mk(3, { seam: true, room: U, flags: U, pop: U }),
  mk(4, { worldX: -I - 1, worldY: I, entry: undefined }), mk(5, { layer: 1, coord: { nIndex: 0, x0: 0, y0: 0, x1: 0, y1: 0 } }),
]);

test('round trip, edge values', () => {
  const recs = sample();
  assert.deepStrictEqual(unpackTerrain(packTerrain(recs)), recs);
});
test('empty list', () => {
  const p = packTerrain([]);
  assert.equal(p.buf.byteLength, 0);
  assert.deepStrictEqual(unpackTerrain(p), []);
});
test('survives structuredClone with transfer', () => {
  const recs = sample(), p = packTerrain(recs, ['a.dt1']);
  const q = structuredClone(p, { transfer: transferList(p) });
  assert.equal(p.buf.byteLength, 0);   // detached
  assert.deepStrictEqual(unpackTerrain(q), recs);
  assert.deepStrictEqual(q.files, ['a.dt1']);
});
test('size: bytes per record', () => {
  assert.equal(TERRAIN_BYTES, 104);
  assert.equal(packTerrain(sample()).buf.byteLength, 6 * 104);
});
test('timing (printed, not asserted)', () => {
  const recs = Array.from({ length: 3300 }, (_, i) => mk(i, { layer: 1, coord: { nIndex: i, x0: 1, y0: 2, x1: 3, y1: 4 }, hidesPlayer: false }));
  const t0 = performance.now(); const p = packTerrain(recs); const t1 = performance.now();
  structuredClone(p); const t2 = performance.now(); structuredClone(recs); const t3 = performance.now();
  unpackTerrain(p); const t4 = performance.now();
  console.log(`3300 recs: pack ${(t1 - t0).toFixed(2)} ms, clone packed ${(t2 - t1).toFixed(2)} ms, clone plain ${(t3 - t2).toFixed(2)} ms, unpack ${(t4 - t3).toFixed(2)} ms`);
});

const kindOf = (t) => ['player', 'monster', 'object', 'missile', 'item'][t] ?? 'tile';
const unit = (i, o = {}) => { const type = o.type ?? i % 6; return { id: U - i, type, classId: -i - 1, mode: i, x: -5 - i, y: I, flags: U, flagsEx: 0, hp: -1, hpMax: I,
  roomOrdinal: U, kind: kindOf(type), hostile: type === 1, ...o }; };
const units = () => [unit(0), unit(1, { preciseX: 12.5, preciseY: -3.25 }), unit(2, { preciseX: 0, preciseY: 0 }), unit(7, { type: 7 }), unit(5)];
const facings = () => [{ id: U, direction: 255, hasDynamicPath: true, lastHitClass: 7, typeFlags: 255, superUniqueId: -32768 },
  { id: 0, direction: 0, hasDynamicPath: false, lastHitClass: 0, typeFlags: 0, superUniqueId: 32767 }];
const light = (n = 3) => { const cells = new Uint8Array(2304).map((_, i) => i & 255), blocked = new Uint8Array(2304).map((_, i) => i % 2);
  return { origin: [-4, 9], ambient: -1, sources: n, list: Array.from({ length: n }, (_, i) => [-i - 1, i, 5, I, -I - 1, U]), cells, blocked,
    at() {}, grid() {} }; };
const strip = ({ at, grid, ...r }) => r;

test('units round trip incl. precise and unsigned', () => assert.deepStrictEqual(unpackUnits(packUnits(units())), units()));
test('facings round trip', () => assert.deepStrictEqual(unpackFacings(packFacings(facings())), facings()));
test('lightmap round trip, closures rebuilt', () => {
  const lm = light(), u = unpackLightmap(packLightmap(lm));
  assert.deepStrictEqual(strip(u), strip(lm));
  assert.equal(u.at(-4, 9), 0); assert.equal(u.at(100, 100), lm.cells[2303]); assert.equal(u.grid(-3, 10).length, 64);
  assert.equal(u.grid(0, 0)[9], u.at(0, 0));
});
test('empty inputs and null lightmap', () => {
  assert.deepStrictEqual(unpackUnits(packUnits([])), []); assert.deepStrictEqual(unpackFacings(packFacings([])), []);
  assert.equal(packLightmap(null), null); assert.equal(unpackLightmap(null), null);
  assert.deepStrictEqual(strip(unpackLightmap(packLightmap(light(0)))), strip(light(0)));
  const f = unpackFrame(packFrame({})); assert.deepStrictEqual(f, { tiles: [], files: [], units: [], facings: [], lightmap: null });
});
test('packFrame round trip and structuredClone transfer detaches every buffer', () => {
  const src = { tiles: sample(), files: ['a.dt1', ''], units: units(), facings: facings(), lightmap: light() };
  const p = packFrame(src), list = frameTransferList(p);
  assert.equal(list.length, 6);
  const c = structuredClone(p, { transfer: list });
  for (const b of list) assert.equal(b.byteLength, 0);
  const o = unpackFrame(c);
  assert.deepStrictEqual({ ...o, lightmap: strip(o.lightmap) }, { ...src, lightmap: strip(src.lightmap) });
});

test('packFrame carries monster animation, missile data and objects; old frames still decode', async () => {
  const { packFrame, unpackFrame } = await import('../play/frame-pack.mjs');
  const mk = (id, type) => ({ id, type, classId: 1, mode: 1, x: 5, y: 6, flags: 0, flagsEx: 0, hp: 1, hpMax: 1, roomOrdinal: 0 });
  const animation = { id: 2, code: 'zz', mode: 1, direction: 0, components: ['tr'], frame: 0 }, missile = { direction: 3, framePrecise: 512, trans: 1 };
  const p = structuredClone(packFrame({ units: [mk(2, 1), mk(3, 3)].map((u) => ({ ...u, ...(u.id === 2 ? { animation } : { missile }) })), objects: [{ id: 9, classId: 7, mode: 0, x: 1, y: 2, token: 'ab' }] }));
  const f = unpackFrame(p);
  assert.deepEqual(f.units.find((u) => u.id === 2).animation, animation);
  assert.deepEqual(f.units.find((u) => u.id === 3).missile, missile);
  assert.deepEqual(f.units.find((u) => u.kind === 'object'), { id: 9, classId: 7, mode: 0, x: 1, y: 2, token: 'ab', kind: 'object' });
  const old = packFrame({ units: [mk(2, 1)] }); delete old.extra; delete old.objects;
  assert.equal(unpackFrame(old).units.length, 1);
});

test('extra (animation) follows the unit, not its id: the hero and a monster can both be id 1', () => {
  const u = (type, id, animation) => ({ id, type, classId: 1, mode: 0, x: 1, y: 1, flags: 0, flagsEx: 0, hp: 1, hpMax: 1, roomOrdinal: 0, ...(animation ? { animation } : {}) });
  const f = unpackFrame(packFrame({ units: [u(0, 1), u(1, 1, { code: 'ci' })] }));
  assert.equal(f.units[0].animation, undefined); assert.equal(f.units[1].animation.code, 'ci');
});
