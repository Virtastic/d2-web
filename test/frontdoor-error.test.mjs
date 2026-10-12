import test from 'node:test';
import assert from 'node:assert/strict';
import { savedCharacterReadError, setFrontdoorError } from '../play/assets/frontdoor-error.mjs';

function alertElement() {
  return { hidden: true, textContent: '' };
}

test('character-select errors are shown in the player viewport alert', () => {
  const alert = alertElement();
  setFrontdoorError(alert, savedCharacterReadError(new Error('IndexedDB read failed')));
  assert.equal(alert.hidden, false);
  assert.equal(alert.textContent, 'Character select: Unable to read saved characters: IndexedDB read failed');
});

test('an empty character-select status leaves the error alert hidden', () => {
  const alert = alertElement();
  setFrontdoorError(alert, '');
  assert.equal(alert.hidden, true);
  assert.equal(alert.textContent, '');
});
