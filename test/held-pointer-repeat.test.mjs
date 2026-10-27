import test from 'node:test';
import assert from 'node:assert/strict';
import { queueHeldPointerRepeat } from '../play/assets/held-pointer-repeat.mjs';

test('held world movement refreshes the current pointer once per simulation advance', () => {
  const commands = [];
  const input = { pointer: (...args) => commands.push(args) };
  const canRetarget = () => true;

  assert.equal(queueHeldPointerRepeat(input, true, false, 10, 20, canRetarget), false,
    'render frames without an engine advance do not queue duplicate commands');
  assert.equal(queueHeldPointerRepeat(input, false, true, 10, 20, canRetarget), false,
    'released input does not repeat');
  assert.equal(queueHeldPointerRepeat(input, true, true, 10, 20, canRetarget), true);
  assert.equal(queueHeldPointerRepeat(input, true, true, 30, 40, canRetarget), true,
    'the next simulation advance retargets to the latest screen point');
  assert.deepEqual(commands, [[10, 20, 0, true], [30, 40, 0, true]]);
});

test('held world movement does not repeat when the pointer is owned by a panel', () => {
  let calls = 0;
  const input = { pointer: () => calls++ };
  assert.equal(queueHeldPointerRepeat(input, true, true, 10, 20, () => false), false);
  assert.equal(calls, 0);
});

test('held right button repeats the 0x0F aim every simulation advance, like the left', () => {
  const cmds = [];
  const input = { pointer: (...a) => cmds.push(a) };
  for (let i = 0; i < 3; i++) queueHeldPointerRepeat(input, true, true, 5, 6, () => true, 2);
  assert.deepEqual(cmds, [[5, 6, 2, true], [5, 6, 2, true], [5, 6, 2, true]]);
});
