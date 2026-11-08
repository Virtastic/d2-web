import test from 'node:test';
import assert from 'node:assert/strict';
import { wrapBubble, bubbleLayout, liveBubbles, BUBBLE_MS } from './game-bubbles.mjs';

const m = (s) => s.length * 10;
test('wraps at 180 px on word boundaries', () => {
  assert.deepEqual(wrapBubble('hello world foo bar baz', m), ['hello world foo', 'bar baz']);
  assert.deepEqual(wrapBubble('a', m), ['a']);
  assert.deepEqual(wrapBubble('abcdefghijklmnopqrstuvwxyz', m).every((l) => m(l) <= 180), true);
});
test('layout: centred, 50 + lines above the player', () => {
  const L = bubbleLayout(['hi'], m, 400, 300, 800, 600, true);
  assert.equal(L.w, 20 + 20); assert.equal(L.h, 21);
  assert.equal(L.left, 380); assert.equal(L.top, 300 - 30 - 50 - 15 - 3);
  assert.deepEqual(L.box, [380, L.top + 4, 420, L.top + 4 + 16]);
  assert.deepEqual(L.lines[0], { text: 'hi', x: 390, y: L.top + 0x12 });
});
test('other units lift 10; off-screen gives null', () => {
  assert.equal(bubbleLayout(['hi'], m, 400, 300, 800, 600, false).top, 300 - 10 - 50 - 15 - 3);
  assert.equal(bubbleLayout(['hi'], m, 400, 10, 800, 600, true), null);
  assert.equal(bubbleLayout(['hi'], m, 1000, 300, 800, 600, true), null);
});
test('bubbles expire', () => {
  const map = new Map([['0:1', { text: 'a', at: 0 }], ['0:2', { text: 'b', at: BUBBLE_MS }]]);
  liveBubbles(map, BUBBLE_MS + 1); assert.deepEqual([...map.keys()], ['0:2']);
});
