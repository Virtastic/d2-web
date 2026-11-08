import test from 'node:test';
import assert from 'node:assert/strict';
import { accountCharAllowed, editCharAllowed } from './bnet-login-screen.mjs';

test('edit boxes take 0x20..0x7e and Latin-1 0xA0..0xFE, not DEL or 0xFF', () => {
  for (const c of [' ', 'a', '~', ' ', 'é', 'þ', '‘', '’']) assert.ok(editCharAllowed(c), c);
  for (const c of ['\x7f', 'ÿ', '\x1f', '\t', '\n']) assert.ok(!editCharAllowed(c), JSON.stringify(c));
});

test('the account box refuses the characters the account filter refuses', () => {
  for (const c of '"#%*,/<>?\\ ') assert.ok(!accountCharAllowed(c), c);
  for (const c of ['a', 'Z', '0', '_', '-']) assert.ok(accountCharAllowed(c), c);
  assert.ok(!accountCharAllowed('\x7f'));
});
