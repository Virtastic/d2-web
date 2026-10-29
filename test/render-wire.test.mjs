import { test } from 'node:test';
import assert from 'node:assert/strict';
import { createRenderWire, renderMode } from '../play/render-wire.mjs';

test('unknown values and webdriver keep the old path', () => {
  for (const q of ['?render=1', '?render=x', '?render=']) assert.equal(renderMode(q, false), '0');
  assert.equal(renderMode(''), '0');   // default-on reverted 2026-10-27
  assert.equal(renderMode('?render=t'), 't');
  assert.equal(renderMode('?render=w'), 'w');   // Stage 5, see render-wire-w.test.mjs
  assert.equal(createRenderWire({ mode: renderMode('?render=1'), load: () => assert.fail('imported') }), null);
});

test('flag off: null wire, no import, no draw-list toggling', () => {
  delete globalThis.__d2DrawList;
  const wire = createRenderWire({ mode: '0', load: () => assert.fail('imported') });
  assert.equal(wire, null);
  assert.equal('__d2DrawList' in globalThis, false);
  assert.equal('__d2RenderHost' in globalThis, false);
});

test('flag on: lazy host, list restored, frame posted before recordMain', async () => {
  const calls = [];
  const host = { init: (w) => calls.push(['init', w]), post: (s, p) => calls.push(['post', s, p.meta.viewCX]), recordMain: (s, l) => calls.push(['rec', s, l.length]) };
  const wire = createRenderWire({ mode: 't', expose: false, load: async () => ({ createRenderHost: () => host, covers: () => true }) });
  const snap = { tiles: [], files: [] };
  const meta = { player: {}, width: 640, height: 480, viewCX: 320, palettePath: 'p' };
  let t = wire.begin(); assert.ok(Array.isArray(globalThis.__d2DrawList));
  wire.end(t, { ready: true, frameSnap: snap, meta });
  assert.equal('__d2DrawList' in globalThis, false);
  assert.equal(wire.host, null);
  await new Promise((r) => setTimeout(r, 5));
  assert.deepEqual(calls, [['init', 640]]);
  t = wire.begin(); wire.end(t, { ready: true, frameSnap: snap, meta });   // no tiles: no post
  assert.deepEqual(calls.length, 1);
  const own = []; globalThis.__d2DrawList = own;
  t = wire.begin(); assert.equal(globalThis.__d2DrawList, own); wire.end(t, { ready: true, frameSnap: snap, meta });
  assert.equal(globalThis.__d2DrawList, own); delete globalThis.__d2DrawList;
});

test('entities pack to units and facings that unpack identically', async () => {
  const { unpackFrame, frameTransferList } = await import('../play/frame-pack.mjs');
  const posted = [];
  const host = { init() {}, post: (s, p) => posted.push(p), recordMain() {} };
  const wire = createRenderWire({ mode: 't', expose: false, load: async () => ({ createRenderHost: () => host, covers: () => true }) });
  const meta = { player: {}, width: 640, height: 480, viewCX: 320, palettePath: 'p' };
  const tile = { room: 1, layer: 2, ordinal: 3, flags: 4, librarySlot: 0, libraryIndex: 0, pop: 0 };
  const snap = { tiles: [tile], files: ['a.dt1'] };
  const a = { id: 7, type: 1, classId: 3, mode: 2, x: 10, y: 20, flags: 5, flagsEx: 0, hp: 9, hpMax: 10, roomOrdinal: 1, preciseX: 10.5, preciseY: 20.25,
    direction: 4, hasDynamicPath: true, lastHitClass: 2, typeFlags: 1, superUniqueId: -1 };
  const b = { id: 8, type: 0, classId: 0, mode: 0, x: 1, y: 2, flags: 0, flagsEx: 0, hp: 1, hpMax: 1, roomOrdinal: 0 };
  let t = wire.begin(); wire.end(t, { ready: true, frameSnap: snap, meta, entities: [a, b] });   // starts the host
  await new Promise((r) => setTimeout(r, 5));
  t = wire.begin(); wire.end(t, { ready: true, frameSnap: snap, meta, entities: [a, b] });
  assert.equal(posted.length, 1);
  assert.ok(frameTransferList(posted[0]).length >= 4);
  const f = unpackFrame(structuredClone(posted[0]));
  assert.equal(f.units.length, 2);
  assert.equal(f.units[0].id, 7); assert.equal(f.units[0].preciseX, 10.5); assert.equal(f.units[1].preciseX, undefined);
  assert.deepEqual(f.facings, [{ id: 7, direction: 4, hasDynamicPath: true, lastHitClass: 2, typeFlags: 1, superUniqueId: -1 }]);
});
