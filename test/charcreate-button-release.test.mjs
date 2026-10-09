import { test } from 'node:test';
import assert from 'node:assert/strict';
import { releaseCharacterCreateButton } from '../play/assets/charcreate.mjs';

test('armed Character Create OK and EXIT callbacks run on mouse-up outside their hitboxes', () => {
  const calls = [];
  const handlers = {
    submit: () => calls.push('submit'),
    cancel: () => calls.push('cancel'),
  };

  assert.equal(releaseCharacterCreateButton('ok', handlers), 'ok');
  assert.equal(releaseCharacterCreateButton('exit', handlers), 'exit');
  assert.deepEqual(calls, ['submit', 'cancel']);
});

test('unrelated armed controls keep their existing release path', () => {
  const calls = [];
  assert.equal(releaseCharacterCreateButton('class-amazon', {
    submit: () => calls.push('submit'), cancel: () => calls.push('cancel'),
  }), null);
  assert.deepEqual(calls, []);
});
