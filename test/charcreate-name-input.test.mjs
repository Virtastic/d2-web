import { test } from 'node:test';
import assert from 'node:assert/strict';
import { isRetailEditBoxCharacter } from '../play/assets/charcreate-name-input.mjs';

test('character-create name field accepts bytes selected by the native D2Win bitmap', () => {
  for (const key of [' ', '0', '9', 'A', 'z', '-', '_', '!', '~', '\u0091', '\u0092', '\u00a0', '\u00fe']) {
    assert.equal(isRetailEditBoxCharacter(key), true, `expected ${JSON.stringify(key)} to be accepted`);
  }
});

test('character-create name field rejects bytes excluded by the D2Win bitmap and multi-character input', () => {
  for (const key of ['', '\x00', '\x1f', '\x7f', '\u0080', '\u0090', '\u0093', '\u009f', '\u00ff', 'Ā', '🙂', 'ab']) {
    assert.equal(isRetailEditBoxCharacter(key), false, `expected ${JSON.stringify(key)} to be rejected`);
  }
});

test('the browser key adapter does not infer an ANSI code-page conversion', () => {
  assert.equal(isRetailEditBoxCharacter('A'), true);
  assert.equal(isRetailEditBoxCharacter('\u00e9'), true);
  assert.equal(isRetailEditBoxCharacter('\u2018'), false);
  assert.equal(isRetailEditBoxCharacter('\u2019'), false);
  assert.equal(isRetailEditBoxCharacter('\u0091'), true);
});
