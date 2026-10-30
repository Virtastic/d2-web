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

test('every 2d context defaults to willReadFrequently (CPU source into CPU target, no GPU readback)', () => {
  const seen = [];
  class HTMLCanvasElement { getContext(type, opts) { seen.push([type, opts]); } }
  const g = { URLSearchParams, navigator: { webdriver: false }, location: { search: '' }, document: undefined, HTMLCanvasElement };
  g.globalThis = g; vm.runInNewContext(src, g);
  const c = new HTMLCanvasElement();
  c.getContext('2d'); c.getContext('2d', { alpha: false }); c.getContext('2d', { willReadFrequently: false }); c.getContext('webgl', { alpha: false });
  assert.deepEqual(JSON.parse(JSON.stringify(seen)), [['2d', { willReadFrequently: true }], ['2d', { willReadFrequently: true, alpha: false }], ['2d', { willReadFrequently: false }], ['webgl', { alpha: false }]]);
});
test('no params: enhanced (fill), retail mounted, player page', () => {
  const e = run('', false);
  assert.equal(e.fill, true); assert.equal(e.parity, false); assert.equal(e.retail, true); assert.equal(e.player, true);
});
test('URL params are never read, by a player or the test runner', () => {
  for (const wd of [false, true]) {
    const e = run('?parity=1&view=800&plugy=1&retail=0&play=1&enhanced=1&assets=byo', wd);
    assert.equal(e.retail, true); assert.equal(e.player, true); assert.equal(e.search, ''); assert.equal(e.assets, '');
  }
  assert.equal(run('?enhanced=1', true).fill, false);   // the test runner always gets the 800x600 parity view
});
