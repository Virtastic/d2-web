import test from 'node:test';
import assert from 'node:assert/strict';
import { wrapSpaces } from './bnet-login-screen.mjs';

const measure = (t) => t.length * 10;

test('message box text wraps at spaces and cuts a word wider than the box instead of running past it', () => {
  const url = 'www.battle.net/support/errormessages/';
  const lines = wrapSpaces(`please visit ${url}`, 150, measure);
  assert.ok(lines.every((l) => measure(l) <= 150), JSON.stringify(lines));
  assert.equal(lines.join('').replace(/ /g, ''), `pleasevisit${url}`);
  assert.deepEqual(wrapSpaces('one two three', 100, measure), ['one two', 'three']);
  assert.deepEqual(wrapSpaces('a', 100, measure), ['a']);
});
