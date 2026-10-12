import test from 'node:test';
import assert from 'node:assert/strict';
import { isButtonHeldOverTarget } from '../play/assets/frontend-pointer-states.mjs';

test('a held button draws its down feedback only while the pointer remains over the armed control', () => {
  const armed = 'button-create';
  assert.equal(isButtonHeldOverTarget(armed, armed), true);
  assert.equal(isButtonHeldOverTarget(armed, 'button-exit'), false);
  assert.equal(isButtonHeldOverTarget(armed, null), false);
  assert.equal(isButtonHeldOverTarget(null, armed), false);
});
