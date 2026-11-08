import test from 'node:test';
import assert from 'node:assert/strict';
import { chatLine } from './bnet-chat.mjs';
import { d2gsDenyString, readD2gsDeny } from './d2gs-deny.mjs';

test('chat fallback lines follow the retail event table', () => {
  const l = (event, username = 'Bob', text = 'hi') => chatLine({ event, username, text });
  assert.deepEqual(l(5), { text: '<Bob> hi', colour: 0 });
  assert.deepEqual(l(4), { text: 'Bob whispers: hi', colour: 2 });
  assert.deepEqual(l(0x0a), { text: 'You whisper to Bob: hi', colour: 2 });
  assert.deepEqual(l(0x17), { text: '*Bob hi*', colour: 5 });
  for (const e of [6, 0x12, 0x15, 0x16]) assert.deepEqual(l(e), { text: 'hi', colour: 3 });
  assert.deepEqual(l(7, '', 'Diablo II'), { text: 'You have joined channel: Diablo II', colour: 5 });
  assert.deepEqual(l(2), { text: 'Bob has joined the channel.', colour: 5 });
  assert.deepEqual(l(3), { text: 'Bob has left the channel.', colour: 5 });
  assert.deepEqual(l(0x13), { text: 'hi', colour: 1 });
  assert.match(l(0x0d, '', 'X').text, /^The channel "X" is full\. /);
  assert.match(l(0x0f, '', 'X').text, /"X" is restricted\./);
  assert.equal(l(1), null);
  assert.equal(l(9), null);
});

test('0xB4 wire codes map as the retail jump table; out of range is reason 9', () => {
  assert.equal(d2gsDenyString(27), d2gsDenyString(22));
  assert.equal(d2gsDenyString(0), 5372);
  assert.deepEqual(readD2gsDeny(Uint8Array.of(0xb4, 4, 0, 0, 0)), { code: 4 });
});
