import test from 'node:test';
import assert from 'node:assert/strict';
import { createGameCursorClock, NATIVE_CURSOR_IDLE_MS } from '../play/assets/game-cursor.mjs';

test('native idle cursor waits 5000 ms after input, then advances by render calls', () => {
  let time = 100;
  const clock = createGameCursorClock({ now: () => time });
  const hand = { kind: 'hand' };
  assert.equal(NATIVE_CURSOR_IDLE_MS, 5000);
  assert.deepEqual(clock.next(hand), { file: 'protate', frame: 0, framesPerCel: null,
    since: 0, elapsedMs: 0, state: 'protate-sequence-unresolved' });

  time += 5000;
  assert.equal(clock.next(hand).file, 'protate'); // native comparison is strictly greater-than
  time += 1;
  assert.equal(clock.next(hand).file, 'protate'); // cursor update follows this draw
  const handDraws = Array.from({ length: 32 }, () => clock.next(hand));
  assert.deepEqual(handDraws.slice(0, 8).map((draw) => draw.frame), [0, 0, 0, 0, 1, 1, 1, 1]);
  assert.deepEqual(handDraws.slice(28).map((draw) => draw.frame), [7, 7, 7, 7]);
  assert.ok(handDraws.every((draw) => draw.file === 'ohand' && draw.framesPerCel === 4));

  const rotate = Array.from({ length: 17 }, () => clock.next(hand));
  assert.deepEqual(rotate.slice(0, 9).map((draw) => draw.frame), [0, 0, 0, 0, 0, 0, 0, 0, 1]);
  assert.equal(rotate[16].frame, 2);
  assert.ok(rotate.every((draw) => draw.file === 'orotate' && draw.framesPerCel === 8));
});

test('new pointer input restarts the millisecond timeout and hand sequence', () => {
  let time = 0;
  const clock = createGameCursorClock({ now: () => time });
  const hand = { kind: 'hand' };
  time = 5001;
  assert.equal(clock.next(hand).file, 'protate');
  assert.equal(clock.next(hand).file, 'ohand');
  for (let i = 0; i < 12; i++) clock.next(hand);

  clock.noteInput();
  assert.equal(clock.next(hand).state, 'protate-sequence-unresolved');
  time += 5000;
  assert.equal(clock.next(hand).file, 'protate');
  time += 1;
  assert.equal(clock.next(hand).file, 'protate');
  assert.equal(clock.next(hand).file, 'ohand');
  assert.equal(clock.next(hand).frame, 0);
});

test('talk uses native orotate draw cadence and protate timing remains explicitly unresolved', () => {
  let time = 0;
  const clock = createGameCursorClock({ now: () => time });
  const talk = { kind: 'talk' }, attack = { kind: 'attack' };
  const talkDraws = Array.from({ length: 17 }, () => clock.next(talk));
  assert.deepEqual(talkDraws.map((draw) => draw.frame), [0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2]);
  const attackDraw = clock.next(attack);
  assert.equal(attackDraw.file, 'protate');
  assert.equal(attackDraw.framesPerCel, null);
  assert.equal(attackDraw.state, 'attack-rate-unresolved');
});
