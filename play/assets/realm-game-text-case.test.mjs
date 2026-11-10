import test from 'node:test';
import assert from 'node:assert/strict';
import { titleCaseGameText } from './realm-client.mjs';

test('game name and password: first character of each word upper-case, the rest lower-case, spaces kept', () => {
  assert.equal(titleCaseGameText('hELLO wORLD'), 'Hello World');
  assert.equal(titleCaseGameText(' a  b '), ' A  B ');
  assert.equal(titleCaseGameText(' '), ' ');
  assert.equal(titleCaseGameText(''), '');
  assert.equal(titleCaseGameText('1ABC'), '1abc');
  assert.equal(titleCaseGameText('ab1CD-EF'), 'Ab1cd-ef');
});
