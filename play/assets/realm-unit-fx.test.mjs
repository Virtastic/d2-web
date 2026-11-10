import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmUnitFx, parseRealmUnitFx } from './realm-unit-fx.mjs';

const OVERLAY = ['Overlay\tFilename\tFrames\tPreDraw\tTrans\tAnimRate', 'null\tnull\t1\t0\t0\t16', 'cast\tcast_a\t4\t0\t3\t16', '', 'hit\thit_b\t2\t1\t0\t8'].join('\r\n');
const read = () => new TextEncoder().encode(OVERLAY);
const record = (...words) => { const b = new Uint8Array(32); new DataView(b.buffer).setUint32(0, 1, true); words.forEach((w, i) => new DataView(b.buffer).setUint32(4 + i * 4, w >>> 0, true)); return b; };
const join = (...parts) => { const out = new Uint8Array(parts.length * 32); parts.forEach((p, i) => out.set(p, i * 32)); return out; };

test('engine unit effect records decode as eight little-endian words', () => {
  const bytes = join(record(10, 1, 0, 77, 251, 3, 0), record(11, 2, 1, 9, 1, 0, 0xffffffff));
  assert.deepEqual(parseRealmUnitFx(bytes, 2), [
    { version: 1, tick: 10, kind: 1, unitType: 0, unitGuid: 77, row: 251, delay: 3, skill: 0 },
    { version: 1, tick: 11, kind: 2, unitType: 1, unitGuid: 9, row: 1, delay: 0, skill: 0xffffffff }]);
  assert.throws(() => parseRealmUnitFx(bytes, 3), RangeError);
  assert.throws(() => parseRealmUnitFx(record(1, 7, 0, 0, 0, 0, 0), 1), /invalid unit effect/);
});

test('a start sound plays once, on the tick its delay runs out', () => {
  const got = [], fx = createRealmUnitFx(read, (row, e) => got.push([row, e.unitType, e.unitId, e.skill]));
  const events = parseRealmUnitFx(join(record(10, 1, 0, 77, 500, 0, 36), record(10, 1, 0, 77, 251, 3, 36)), 2);
  fx.step(events, 10);
  assert.deepEqual(got, [[500, 0, 77, 36]]);
  fx.step([], 12);
  assert.equal(got.length, 1);
  fx.step([], 13); fx.step([], 14);
  assert.deepEqual(got, [[500, 0, 77, 36], [251, 0, 77, 36]]);
});

test('a muted sound is dropped and its overlay is kept', () => {
  const got = [], fx = createRealmUnitFx(read, (row) => got.push(row));
  fx.step(parseRealmUnitFx(join(record(5, 1, 0, 77, 500, 0, 36), record(5, 2, 0, 77, 1, 0, 36), record(5, 1, 0, 8, 501, 0, 36)), 3), 5, (e) => e.unitGuid === 77);
  assert.deepEqual(got, [501]);
  assert.equal(fx.apply({ entities: [] }, 77).length, 1);
});

test('a one-shot overlay rides its unit for its frames and then ends', () => {
  const fx = createRealmUnitFx(read, () => {});
  // Row 1: four frames at one frame a tick, over the body. Row 3 (after a blank row): two frames at half speed, behind it.
  // Row 0 has no file and draws nothing.
  fx.step(parseRealmUnitFx(join(record(20, 2, 1, 9, 1, 0, 40), record(20, 2, 0, 77, 3, 0, 0xffffffff), record(20, 2, 1, 9, 0, 0, 40)), 3), 20);
  const frame = () => { const s = { entities: [{ type: 1, id: 9 }, { type: 0, id: 5 }, { type: 4, id: 9 }] }; const hero = fx.apply(s, 77); return { s, hero }; };
  let f = frame();
  assert.deepEqual(f.s.entities[0].stateOverlays, [{ kind: 'overlay', overlay: 1, file: 'cast_a', frames: 4, rate: 256, trans: 3, predraw: false, age: 1 }]);
  assert.equal(f.s.entities[1].stateOverlays, undefined);
  assert.equal(f.s.entities[2].stateOverlays, undefined);
  assert.deepEqual(f.hero, [{ kind: 'overlay', overlay: 3, file: 'hit_b', frames: 2, rate: 128, trans: 0, predraw: true, age: 1 }]);
  fx.step([], 22); f = frame();
  assert.equal(f.s.entities[0].stateOverlays[0].age, 3);
  assert.deepEqual(f.hero, []);
  fx.step([], 23); f = frame();
  assert.equal(f.s.entities[0].stateOverlays, undefined);
});

test('an overlay is added after the overlays the frame already carries', () => {
  const fx = createRealmUnitFx(read, () => {});
  fx.step(parseRealmUnitFx(record(1, 2, 1, 9, 1, 0, 40), 1), 1);
  const s = { entities: [{ type: 1, id: 9, stateOverlays: [{ kind: 'overlay', file: 'aura' }] }] };
  fx.apply(s);
  assert.deepEqual(s.entities[0].stateOverlays.map((o) => o.file), ['aura', 'cast_a']);
});
