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
    assert.equal(e.retail, true); assert.equal(e.player, true); assert.equal(e.search, wd ? '?parity=1&view=800&plugy=1&retail=0&play=1&enhanced=1&assets=byo' : ''); assert.equal(e.assets, '');
  }
  assert.equal(run('?enhanced=1', true).fill, false);   // the test runner always gets the 800x600 parity view
});
test('as=player: the test runner gets the player page with its URL options; a player never does', () => {
  const run2 = (search, webdriver) => {
    class Navigator {} const nav = new Navigator(); Object.defineProperty(nav, 'webdriver', { get() { return webdriver; }, configurable: true });
    Object.setPrototypeOf(nav, Navigator.prototype); Object.defineProperty(Navigator.prototype, 'webdriver', { get() { return webdriver; }, configurable: true });
    delete nav.webdriver;
    const g = { URLSearchParams, Navigator, navigator: nav, location: { search }, document: undefined };
    g.globalThis = g; vm.runInNewContext(src, g);
    return { e: g.__d2Entry, wd: nav.webdriver };
  };
  const r = run2('?play=1&as=player', true);
  assert.equal(r.wd, false); assert.equal(r.e.fill, true); assert.equal(r.e.test, false); assert.equal(r.e.search, '?play=1&as=player'); assert.equal(r.e.asPlayer, true);
  const p = run2('?play=1&as=player', false);
  assert.equal(p.wd, false); assert.equal(p.e.search, ''); assert.equal(p.e.asPlayer, undefined);
});
