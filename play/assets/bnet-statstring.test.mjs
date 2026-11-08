import test from 'node:test';
import assert from 'node:assert/strict';
import { parseStatstring, CLASS_NAMES } from './bnet-statstring.mjs';

// Builds a statstring in the retail layout (see the parser header).
function build({ product = 'PX2D', realm = 'USEast', name, cls, level, flags = 0, progress = 0, tail = [0xFF, 0xFF, 0xFF], gear0 = 0xFF }) {
  const b = new Array(33).fill(0);
  b[0] = 1; b[1] = 1;
  for (let i = 2; i < 13; i++) b[i] = 0xFF;
  b[2] = gear0;
  b[13] = cls + 1;
  for (let i = 14; i < 25; i++) b[i] = 0xFF;
  b[25] = level; b[26] = flags & 0xFF; b[27] = flags >> 8; b[28] = progress & 0xFF; b[29] = progress >> 8;
  b[30] = tail[0]; b[31] = tail[1]; b[32] = tail[2];
  return product + realm + ',' + name + ',' + String.fromCharCode(...b);
}

// The four users of the retail lobby capture: class and level per slot.
const slots = [['Aelwyn', 0, 87], ['Brightstar', 1, 45], ['Cinder', 2, 12], ['Dorian', 3, 99]];
for (const [name, cls, level] of slots) {
  test(`slot ${name}: ${CLASS_NAMES[cls]} level ${level}`, () => {
    const r = parseStatstring(build({ name, cls, level }));
    assert.equal(r.valid, true);
    assert.equal(r.realm, 'USEast');
    assert.equal(r.name, name);
    assert.equal(r.classId, cls);
    assert.equal(r.className, CLASS_NAMES[cls]);
    assert.equal(r.level, level);
    assert.deepEqual([r.hardcore, r.dead, r.expansion, r.ladder], [false, false, false, false]);
  });
}

test('every class id 0-6 maps to its name', () => {
  for (let c = 0; c < 7; c++) assert.equal(parseStatstring(build({ name: 'x', cls: c, level: 1 })).className, CLASS_NAMES[c]);
});

test('flag bits: hardcore 0x04, dead 0x08, expansion 0x20, ladder 0x40', () => {
  const f = (flags) => { const r = parseStatstring(build({ name: 'x', cls: 4, level: 30, flags })); return [r.hardcore, r.dead, r.expansion, r.ladder]; };
  assert.deepEqual(f(0x04), [true, false, false, false]);
  assert.deepEqual(f(0x08), [false, true, false, false]);
  assert.deepEqual(f(0x20), [false, false, true, false]);
  assert.deepEqual(f(0x40), [false, false, false, true]);
  assert.deepEqual(f(0x6C), [true, true, true, true]);
  assert.deepEqual(f(0x93), [false, false, false, false]);
});

test('progress word and 0xFF tail bytes', () => {
  const r = parseStatstring(build({ name: 'x', cls: 0, level: 5, progress: 0x1234, tail: [0xFF, 7, 0xFF] }));
  assert.equal(r.progress, 0x1234);
  assert.deepEqual(r.extra, [0, 7, 0]);
});

test('accepts Uint8Array and classic product', () => {
  const s = build({ product: 'VD2D', name: 'bytes', cls: 5, level: 9 });
  const r = parseStatstring(Uint8Array.from(s, (c) => c.charCodeAt(0)));
  assert.equal(r.product, 'VD2D');
  assert.equal(r.className, 'Druid');
  assert.equal(r.level, 9);
});

test('a zero first equipment byte means no character', () => {
  const r = parseStatstring(build({ name: 'x', cls: 2, level: 50, gear0: 0 }));
  assert.equal(r.valid, false);
  assert.equal(r.classId, -1);
  assert.equal(r.level, 0);
  assert.equal(r.name, 'x');
});

test('class byte 0 or past the table is not a class', () => {
  assert.equal(parseStatstring(build({ name: 'x', cls: -1, level: 5 })).classId, -1);
  const r = parseStatstring(build({ name: 'x', cls: 9, level: 5 }));
  assert.equal(r.className, '');
  assert.equal(r.level, 5);
});

test('truncated: class present, level absent', () => {
  const s = build({ name: 'x', cls: 3, level: 40 });
  const cut = s.slice(0, s.indexOf(',', s.indexOf(',') + 1) + 1 + 14);
  const r = parseStatstring(cut);
  assert.equal(r.classId, 3);
  assert.equal(r.level, 0);
  assert.equal(r.valid, false);
});

test('truncated before flags keeps level and clears flags', () => {
  const s = build({ name: 'x', cls: 3, level: 40, flags: 0x6C });
  const r = parseStatstring(s.slice(0, s.indexOf(',', s.indexOf(',') + 1) + 1 + 26));
  assert.equal(r.level, 40);
  assert.equal(r.valid, true);
  assert.equal(r.hardcore, false);
});

test('malformed input yields null-safe defaults', () => {
  for (const bad of [undefined, null, 42, {}, '', 'PX', 'PX2D', 'PX2DUSEast', 'PX2DUSEast,name', 'PX2DUSEast,name,', 'JSTR', 'CHAT hello,there,', 'ZZZZa,b,cccccccccccccccccccccccccccc']) {
    const r = parseStatstring(bad);
    assert.equal(r.valid, false, String(bad));
    assert.equal(r.classId, -1);
    assert.equal(r.level, 0);
    assert.deepEqual([r.hardcore, r.dead, r.expansion, r.ladder], [false, false, false, false]);
  }
});
