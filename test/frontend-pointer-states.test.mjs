import { test } from 'node:test';
import assert from 'node:assert/strict';
import { createFrontendPointerStates } from '../play/assets/frontend-pointer-states.mjs';

test('front-end controls arm only on primary-button press', () => {
  const states = createFrontendPointerStates();
  const calls = [];
  const control = {
    press: () => { calls.push('press'); return 'ok'; },
    release: (token) => { calls.push(`release:${token}`); return token; },
  };

  assert.equal(states.press(7, control, 10, 20, 800, 600, 2), null);
  assert.equal(states.press(7, control, 10, 20, 800, 600, 1), null);
  assert.deepEqual(states.armed, []);
  assert.deepEqual(calls, []);
  assert.equal(states.release(7, control, 10, 20, 800, 600), null);
  assert.deepEqual(calls, []);

  assert.equal(states.press(7, control, 10, 20, 800, 600, 0), 'ok');
  assert.deepEqual(states.armed, [{ pointerId: 7, token: 'ok' }]);
  assert.equal(states.release(7, control, 10, 20, 800, 600), 'ok');
  assert.deepEqual(calls, ['press', 'release:ok']);
});

test('secondary-button chord leaves a primary gesture intact', () => {
  const states = createFrontendPointerStates();
  const calls = [];
  const control = {
    press: () => 'menu',
    release: (token) => { calls.push(token); return token; },
  };

  states.press(4, control, 1, 1, 800, 600, 0);
  assert.equal(states.press(4, control, 1, 1, 800, 600, 2), null);
  assert.deepEqual(states.armed, [{ pointerId: 4, token: 'menu' }]);
  assert.equal(states.release(4, control, 1, 1, 800, 600), 'menu');
  assert.deepEqual(calls, ['menu']);
});

test('secondary-button release cannot complete or cancel an armed primary gesture', () => {
  const states = createFrontendPointerStates();
  const calls = [];
  const control = {
    press: () => 'menu',
    release: (token) => { calls.push(`release:${token}`); return token; },
    cancel: () => calls.push('cancel'),
  };

  states.press(9, control, 1, 1, 800, 600, 0);
  assert.equal(states.release(9, control, 1, 1, 800, 600, 2), null);
  assert.deepEqual(states.armed, [{ pointerId: 9, token: 'menu' }]);
  assert.deepEqual(calls, [], 'right-button release neither acts nor cancels');
  assert.equal(states.release(9, control, 1, 1, 800, 600, 0), 'menu');
  assert.deepEqual(calls, ['release:menu']);
});
