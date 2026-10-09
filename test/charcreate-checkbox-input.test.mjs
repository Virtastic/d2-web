import test from 'node:test';
import assert from 'node:assert/strict';
import { createCharacterCreateCheckboxInput } from '../play/assets/charcreate-checkboxes.mjs';

test('Character Create clickboxes commit their toggle on pointer-down', () => {
  const states = { expansion: true, hardcore: false };
  const input = createCharacterCreateCheckboxInput((key) => {
    if (!(key in states)) return null;
    states[key] = !states[key];
    return key;
  });

  assert.equal(input.press('checkbox-expansion'), 'expansion');
  assert.deepEqual(states, { expansion: false, hardcore: false });
  assert.equal(input.release('checkbox-expansion'), null);
  assert.deepEqual(states, { expansion: false, hardcore: false });

  assert.equal(input.press('checkbox-hardcore'), 'hardcore');
  assert.deepEqual(states, { expansion: false, hardcore: true });
  input.cancel();
  assert.deepEqual(states, { expansion: false, hardcore: true });

  assert.equal(input.press('checkbox-hardcore'), 'hardcore');
  assert.deepEqual(states, { expansion: false, hardcore: false });
  assert.equal(input.release('checkbox-hardcore'), null);
  assert.deepEqual(states, { expansion: false, hardcore: false });
});

test('non-checkbox controls do not change checkbox state on pointer-down', () => {
  const states = { expansion: true, hardcore: false };
  const input = createCharacterCreateCheckboxInput((key) => {
    states[key] = !states[key];
    return key;
  });

  assert.equal(input.press('ok'), null);
  assert.equal(input.press(null), null);
  assert.deepEqual(states, { expansion: true, hardcore: false });
});
