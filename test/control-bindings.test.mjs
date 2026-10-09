import test from 'node:test';
import assert from 'node:assert/strict';
import { CONFIGURE_CONTROLS_SLOTS, controlActionForKey, controlBindingMatches, restoreControlBindings } from '../play/assets/configure-controls.mjs';
import { DROPPED_GAMEPLAY_KEYS } from '../play/assets/retail-key-bindings.mjs';

test('Configure Controls committed keys are resolved by function and physical key code', () => {
  const bindings = CONFIGURE_CONTROLS_SLOTS.map((slot) => slot ? [slot[1], slot[2]] : null);

  assert.equal(controlBindingMatches(bindings, 'Inventory Screen', 'KeyI'), true);
  assert.equal(controlBindingMatches(bindings, 'Inventory Screen', 'KeyB'), true);
  assert.equal(controlBindingMatches(bindings, 'Inventory Screen', 'KeyK'), false);
  assert.equal(controlBindingMatches(bindings, 'Inventory Screen', 'ArrowLeft'), false);

  bindings[1][0] = 'K';
  bindings[1][1] = null;
  assert.equal(controlBindingMatches(bindings, 'Inventory Screen', 'KeyI'), false);
  assert.equal(controlBindingMatches(bindings, 'Inventory Screen', 'KeyB'), false);
  assert.equal(controlBindingMatches(bindings, 'Inventory Screen', 'KeyK'), true);
  assert.equal(controlBindingMatches(bindings, 'Missing action', 'KeyK'), false);
});

test('the shared action resolver covers the implemented retail keyboard consumers', () => {
  const bindings = CONFIGURE_CONTROLS_SLOTS.map((slot) => slot ? [slot[1], slot[2]] : null);
  for (const [code, action] of [
    ['KeyA', 'Character Screen'], ['KeyC', 'Character Screen'],
    ['KeyI', 'Inventory Screen'], ['KeyB', 'Inventory Screen'],
    ['KeyQ', 'Quest Log'], ['KeyM', 'Message Log'], ['KeyH', 'Help Screen'],
    ['KeyT', 'Skill Tree'], ['KeyS', 'Skill Speed Bar'], ['KeyW', 'Swap Weapons'], ['KeyR', 'Toggle Run/Walk'],
    ['ControlLeft', 'Run'], ['ShiftLeft', 'Stand Still'], ['AltLeft', 'Show Items'],
    ['Space', 'Clear Screen'], ['Digit1', 'Use Belt 1'], ['Digit4', 'Use Belt 4'],
    ['F1', 'Skill 1'], ['F8', 'Skill 8'],
  ]) assert.equal(controlActionForKey(bindings, code), action, `${code} should resolve to ${action}`);

  bindings[1][0] = 'K';
  bindings[1][1] = null;
  assert.equal(controlActionForKey(bindings, 'KeyI'), null);
  assert.equal(controlActionForKey(bindings, 'KeyB'), null);
  assert.equal(controlActionForKey(bindings, 'KeyK'), 'Inventory Screen');
});

test('stored Configure Controls data rejects corruption and duplicate key ownership to retail defaults', () => {
  const defaults = CONFIGURE_CONTROLS_SLOTS.map((slot) => slot ? [slot[1], slot[2]] : null);
  const saved = defaults.map((binding) => binding ? [...binding] : null);
  saved[1][0] = 'K';
  saved[1][1] = null;
  assert.deepEqual(restoreControlBindings(JSON.stringify(saved)), saved, 'a valid accepted table survives restore');
  assert.deepEqual(restoreControlBindings('{not json'), defaults, 'malformed JSON uses captured retail defaults');

  const duplicate = defaults.map((binding) => binding ? [...binding] : null);
  duplicate[0][0] = 'I'; // Inventory Screen already owns I in slot 1.
  assert.deepEqual(restoreControlBindings(JSON.stringify(duplicate)), defaults, 'duplicate ownership rejects the whole table');

  const badGap = defaults.map((binding) => binding ? [...binding] : null);
  badGap[7] = ['X', null];
  assert.deepEqual(restoreControlBindings(JSON.stringify(badGap)), defaults, 'a populated spacer slot rejects the whole table');
});

test('old fixed DOM gameplay shortcuts cannot bypass a Configure Controls rebind', () => {
  for (const code of [
    'KeyW', 'KeyA', 'KeyS', 'KeyD', 'KeyE', 'KeyT', 'KeyR', 'Space',
    'F1', 'F2', 'F3', 'F4', 'F5', 'F6', 'F7', 'F8',
    'Digit1', 'Digit2', 'Digit3', 'Digit4', 'ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight',
  ]) assert.equal(DROPPED_GAMEPLAY_KEYS.has(code), true, `${code} must not fall through to a fixed CommandInput action`);
  assert.equal(DROPPED_GAMEPLAY_KEYS.has('Escape'), false, 'Escape remains the explicit shell cancel/menu path');
});
