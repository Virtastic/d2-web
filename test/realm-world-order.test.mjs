import test from 'node:test';
import assert from 'node:assert/strict';
import { createLoadCompleteOrder } from '../play/assets/realm-world-order.mjs';

const size = (b) => ({ 0x03: 12, 0x04: 1, 0x59: 26 }[b[0]] ?? 1);
const p59 = Uint8Array.from({ length: 26 }, (_, i) => (i ? 0xaa : 0x59));
const p03 = Uint8Array.from({ length: 12 }, (_, i) => (i ? 0xbb : 0x03));
const cat = (...a) => Uint8Array.from(a.flatMap((x) => [...x]));

test('0x04 ahead of 0x59 in one frame is delivered after it', () => {
  const o = createLoadCompleteOrder(size);
  const out = o.filter(cat(p03, [4], p59), false);
  assert.deepEqual([...out], [...cat(p03, p59, [4])]);
});

test('0x04 in an earlier frame is held until the assignment frame', () => {
  const o = createLoadCompleteOrder(size);
  assert.equal(o.filter(Uint8Array.of(4), false).length, 0);
  assert.equal(o.held, 1);
  assert.deepEqual([...o.filter(p59, false)], [...cat(p59, [4])]);
  assert.equal(o.held, 0);
});

test('a normal order and an already-active player pass through untouched', () => {
  const o = createLoadCompleteOrder(size);
  const a = cat(p03, p59, [4]);
  assert.equal(o.filter(a, false), a);
  const b = Uint8Array.of(4);
  assert.equal(o.filter(b, true), b);
});
