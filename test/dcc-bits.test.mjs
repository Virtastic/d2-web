import test from 'node:test';
import assert from 'node:assert/strict';
import { Bits } from '../play/assets/dcc-bits.mjs';

// BigInt defines the whole little-endian bit string independently of the reader.
function expected(bytes, offset, count) {
  let word = 0n;
  for (let i = bytes.length - 1; i >= 0; i--) word = (word << 8n) | BigInt(bytes[i]);
  return Number((word >> BigInt(offset)) & ((1n << BigInt(count)) - 1n));
}
const patterns = [new Uint8Array(7), new Uint8Array(7).fill(255), Uint8Array.of(0x80, 0x01, 0xfe, 0x7f, 0xaa, 0x55, 0x93)];
test('DCC bit reads cover every alignment and width, including unsigned 32-bit', () => {
  for (const bytes of patterns) for (let offset = 0; offset < 16; offset++) for (let count = 0; count <= 32; count++) {
    const reader = new Bits(bytes, offset, offset + count);
    assert.equal(reader.read(count, 'fixture'), expected(bytes, offset, count), `offset ${offset}, width ${count}`);
    assert.equal(reader.bit, offset + count);
  }
});
test('DCC signed fields preserve zero width and two-complement sign extension', () => {
  for (const bytes of patterns) for (let offset = 0; offset < 8; offset++) for (let count = 0; count <= 32; count++) {
    const unsigned = expected(bytes, offset, count);
    const signed = count && unsigned >= 2 ** (count - 1) ? unsigned - 2 ** count : unsigned;
    assert.equal(new Bits(bytes, offset).signed(count, 'signed'), signed);
  }
});
test('DCC substreams reject overruns without consuming the next stream', () => {
  const reader = new Bits(Uint8Array.of(0xff, 0xff), 3, 9);
  assert.equal(reader.read(6, 'mask'), 63);
  assert.equal(reader.read(0, 'mask'), 0);
  assert.throws(() => reader.read(1, 'mask'), { name: 'RangeError', message: 'truncated DCC mask at bit 9 reading 1, limit 9' });
  assert.equal(reader.bit, 9);
  for (const count of [-1, 33]) assert.throws(() => reader.read(count, 'bad'), RangeError);
  assert.equal(reader.bit, 9);
});
test('DCC sequential mixed-width reads maintain the bit cursor across byte edges', () => {
  const bytes = Uint8Array.of(0xf3, 0x7a, 0x80, 0xfe, 0x12, 0xac, 0xff);
  const reader = new Bits(bytes, 1);
  for (const count of [1, 2, 4, 0, 8, 32, 7, 1]) {
    const offset = reader.bit;
    assert.equal(reader.read(count, 'sequence'), expected(bytes, offset, count));
    assert.equal(reader.bit, offset + count);
  }
});
