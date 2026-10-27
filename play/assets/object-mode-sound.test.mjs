// D25: entering an object mode plays the class record's row[mode] (D2Client+0x237E0 -> 0x22570, table 0xF6C58).
// Reads the retail 1.13c D2Client.dll directly (independent of the generated module); skipped without it.
// Every object class x every (previous, new) mode pair is compared against the DLL's own rule: 0 diffs.
import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import { createEventSounds } from './event-sounds.mjs';

const dllPath = process.env.D2_CLIENT_DLL || 'retail/D2/D2SE/CORES/1.13c/D2Client.dll';
const none = { skip: !existsSync(dllPath) && 'no retail D2Client.dll' };
const files = { 'data\\global\\excel\\Sounds.txt': new TextEncoder().encode('Sound\tIndex\n') };
const rule = (dll, secs, cls, prev, mode) => { // 0x22570 + the 0x237E0 gate
  const off = (r) => { for (const [va, vs, rp] of secs) if (r >= va && r < va + vs) return r - va + rp; };
  const p = dll.readUInt32LE(off(0xf6c58 + cls * 4)); if (!p) return 0;
  if (p - 0x6fab0000 === 0xf6988 && prev > mode) return 0;
  return dll.readUInt32LE(off(p - 0x6fab0000 + mode * 4));
};
test('object mode entry sound: every class and mode pair equals the DLL rule', none, () => {
  const dll = readFileSync(dllPath), pe = dll.readUInt32LE(0x3c), so = dll.readUInt16LE(pe + 20);
  const secs = Array.from({ length: dll.readUInt16LE(pe + 6) }, (_, i) => { const o = pe + 24 + so + i * 40; return [dll.readUInt32LE(o + 12), dll.readUInt32LE(o + 8), dll.readUInt32LE(o + 20)]; });
  let pairs = 0, diffs = 0, played = 0;
  for (let cls = 0; cls < 573; cls++) for (let prev = 0; prev < 8; prev++) for (let mode = 0; mode < 8; mode++) {
    if (prev === mode) continue;
    const got = [], s = createEventSounds((f) => files[f], (row) => got.push(row));
    s.update({ objects: [{ id: 1, classId: cls, mode: prev }] }, 0);
    got.length = 0; // class 0x3B also plays on first sight (0x237E0); that arm is covered by the rule check below
    s.update({ objects: [{ id: 1, classId: cls, mode }] }, 0);
    const want = rule(dll, secs, cls, prev, mode) ? [rule(dll, secs, cls, prev, mode)] : [];
    pairs++; played += want.length;
    if (JSON.stringify(got) !== JSON.stringify(want)) diffs++;
  }
  assert.ok(played > 1000, 'control: the corpus plays sounds'); // a vacuous table would pass with 0
  assert.equal(diffs, 0, `${diffs}/${pairs} diffs`);
});
test('chest 181 (Mode1=1): opening plays object_chest_large (2547) once, at OP entry', none, () => {
  const got = [], s = createEventSounds((f) => files[f], (row) => got.push(row));
  for (const m of [0, 0, 1, 1, 1, 2, 2]) s.update({ objects: [{ id: 1, classId: 181, mode: m }] }, 0);
  assert.deepEqual(got, [2547]);
});
test('chest 5 (Mode1=0): the single 0 -> 2 step plays object_chest_large (2547)', none, () => {
  const got = [], s = createEventSounds((f) => files[f], (row) => got.push(row));
  for (const m of [0, 0, 2, 2]) s.update({ objects: [{ id: 1, classId: 5, mode: m }] }, 0);
  assert.deepEqual(got, [2547]);
});
test('control: a wrong expectation fails', none, () => {
  const got = [], s = createEventSounds((f) => files[f], (row) => got.push(row));
  for (const m of [0, 1]) s.update({ objects: [{ id: 1, classId: 181, mode: m }] }, 0);
  assert.throws(() => assert.deepEqual(got, [2546]));
});
