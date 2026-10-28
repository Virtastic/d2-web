import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import vm from 'node:vm';

const src = readFileSync(new URL('../play/entry.js', import.meta.url), 'utf8');
const run = (search, webdriver) => {
  const g = { URLSearchParams, navigator: { webdriver }, location: { search }, document: undefined };
  g.globalThis = g; vm.runInNewContext(src, g);
  return g.__d2Entry;
};

test('no params: enhanced (fill), retail mounted, player page', () => {
  const e = run('', false);
  assert.equal(e.fill, true); assert.equal(e.parity, false); assert.equal(e.retail, true); assert.equal(e.player, true);
});
test('player URL params are ignored: ?parity=1 ?view=800 ?plugy=1 ?retail=0', () => {
  const e = run('?parity=1&view=800&plugy=1&retail=0&play=1', false);
  assert.equal(e.fill, true); assert.equal(e.retail, true); assert.equal(e.search, '');
});
test('test path (webdriver): bare page stays 800x600 parity; ?enhanced=1 opts in', () => {
  assert.equal(run('', true).fill, false);
  assert.equal(run('?enhanced=1', true).fill, true);
  assert.equal(run('?play=1&parity=1', true).fill, false);
});
