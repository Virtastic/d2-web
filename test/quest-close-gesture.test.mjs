import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { createQuestCloseGesture, questCloseFrame } from '../play/assets/quest-close-gesture.mjs';

test('Quest Log close stays visibly pressed until its owning pointer releases', () => {
  const gesture = createQuestCloseGesture();
  assert.equal(questCloseFrame(gesture.pressed), 10);
  assert.equal(gesture.press(7), true);
  assert.equal(gesture.pressed, true);
  assert.equal(questCloseFrame(gesture.pressed), 11);
  assert.equal(gesture.release(8, true), false, 'another pointer cannot finish the gesture');
  assert.equal(gesture.pressed, true);
  assert.equal(gesture.release(7, true), true, 'release over close activates it');
  assert.equal(gesture.pressed, false);
  assert.equal(questCloseFrame(gesture.pressed), 10);
});

test('Quest Log close release elsewhere clears feedback without closing the panel', () => {
  const gesture = createQuestCloseGesture();
  gesture.press(3);
  assert.equal(gesture.release(3, false), false);
  assert.equal(gesture.pressed, false);
  assert.equal(questCloseFrame(gesture.pressed), 10);
});

test('Quest Log close cancellation and lost pointer capture clear only the owning press', () => {
  for (const event of ['pointercancel', 'lostpointercapture']) {
    const gesture = createQuestCloseGesture();
    assert.equal(gesture.press(12), true);
    assert.equal(gesture.pressed, true);
    assert.equal(questCloseFrame(gesture.pressed), 11);

    // Both shell events route through cancel(pointerId); an unrelated pointer cannot
    // clear the held visual or release the close action.
    assert.equal(gesture.cancel(13), false, `${event}: unrelated pointer keeps ownership`);
    assert.equal(gesture.pressed, true);
    assert.equal(gesture.cancel(12), true, `${event}: owning pointer ends the gesture`);
    assert.equal(gesture.pressed, false);
    assert.equal(questCloseFrame(gesture.pressed), 10);
    assert.equal(gesture.release(12, true), false, `${event}: later up cannot activate a cancelled press`);
  }
});

test('Quest Log close press cannot be re-armed or transferred by the same pointer', () => {
  const gesture = createQuestCloseGesture();
  assert.equal(gesture.press(21), true);
  assert.equal(gesture.press(21), false, 'duplicate down does not replace the active gesture');
  assert.equal(gesture.pointerId, 21);
  assert.equal(gesture.pressed, true);
  assert.equal(gesture.release(21, true), true, 'the original owning pointer still activates on release');
  assert.equal(gesture.pressed, false);
});

test('Quest Log shell routes pointer cancellation and lost capture through the gesture owner', () => {
  const shell = readFileSync(new URL('../play/gameplay-shell.mjs', import.meta.url), 'utf8');
  for (const event of ['pointercancel', 'lostpointercapture']) {
    const handler = shell.match(new RegExp(`canvas\\.addEventListener\\('${event}', \\(e\\) => \\{([\\s\\S]*?)\\n\\}\\);`));
    assert.ok(handler, `${event} handler exists`);
    assert.match(handler[1], /questCloseGesture\.cancel\(e\.pointerId\)/,
      `${event} cancels only the matching pointer's Quest Log close press`);
  }
});
