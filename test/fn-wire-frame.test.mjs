// The wire-frame record of the FN capture (the frame-recording directive): the frame a send writes, as the
// hookwlog blob holds it. Per frame: the total dword (len+4), then FF id total16 LE, the payload, padded to 4.
// The blob starts with the frame count dword. The expectations are the protocol's own frames, checked on the encoder.
import { test } from 'node:test';
import assert from 'node:assert/strict';

const u32 = (n) => [n & 0xff, (n >>> 8) & 0xff, (n >>> 16) & 0xff, (n >>> 24) & 0xff];
const str = (s) => [...Buffer.from(s + '\0', 'latin1')];

// one wire frame as a record: total dword, FF id total16 LE, payload, zero-padded to 4
export function frameRecord(id, payload) {
  const tot = payload.length + 4;
  const b = [...u32(tot), 0xff, id, tot & 0xff, (tot >> 8) & 0xff, ...payload];
  while (b.length % 4) b.push(0);
  return Buffer.from(b);
}

test('wire frame: id 0x16 carries 8 payload bytes (value, then flag != 0 as 0/1)', () => {
  const rec = frameRecord(0x16, [0x44, 0x33, 0x22, 0x11, 1, 0, 0, 0]);
  assert.deepEqual([...rec], [0x0c, 0, 0, 0, 0xff, 0x16, 0x0c, 0, 0x44, 0x33, 0x22, 0x11, 1, 0, 0, 0]);
});

test('wire frame: id 0x0C, length is strlen(name)+5 (flags dword, then the name with its NUL)', () => {
  const rec = frameRecord(0x0c, [2, 0, 0, 0, ...str('Monkey')]);
  assert.deepEqual([...rec.subarray(0, 8)], [0x0f, 0, 0, 0, 0xff, 0x0c, 0x0f, 0]);
  assert.equal(rec.length % 4, 0);
});

test('wire frame: a null name is sent as "Diablo II" with the flags dword first', () => {
  const rec = frameRecord(0x0c, [5, 0, 0, 0, ...str('Diablo II')]);
  assert.deepEqual([...rec.subarray(0, 8)], [0x12, 0, 0, 0, 0xff, 0x0c, 0x12, 0]);
  assert.deepEqual([...rec.subarray(8, 12)], [5, 0, 0, 0]);
});
