import { existsSync, readFileSync } from 'node:fs';
import test from 'node:test';
import assert from 'node:assert/strict';
import { createPlugyStash, pageTarget, isDc6, PLUGY_GRID, MAX_PAGE } from '../play/assets/plugy-stash-ui.mjs';

test('prev stops at the first page, next grows without wrapping', () => {
  assert.equal(pageTarget('prev', 0), 0);
  assert.equal(pageTarget('prev', 5), 4);
  assert.equal(pageTarget('next', 5), 6);
  assert.equal(pageTarget('next', MAX_PAGE), MAX_PAGE);
});

test('shift: prev is the first page, next the last used', () => {
  assert.equal(pageTarget('prev', 7, true), 0);
  assert.equal(pageTarget('next', 2, true, 9), 9);
  assert.equal(pageTarget('next', 12, true, 9), 12);
});

test('index buttons jump to the previous / next multiple of 10 (100 with shift)', () => {
  assert.equal(pageTarget('nextIndex', 0), 9);
  assert.equal(pageTarget('nextIndex', 9), 19);
  assert.equal(pageTarget('prevIndex', 25), 19);
  assert.equal(pageTarget('prevIndex', 3), 0);
  assert.equal(pageTarget('nextIndex', 0, true), 99);
  assert.equal(pageTarget('prevIndex', 150, true), 99);
});

test('go() turns pages, grows the count, refuses on a false onPage, and each set keeps its page', () => {
  const calls = []; let refuse = false;
  const s = createPlugyStash({ enabled: true, onPage: (...a) => { calls.push(a); return !refuse; } });
  assert.equal(s.go(3), true);
  assert.equal(s.page, 3); assert.equal(s.pages, 4);
  assert.deepEqual(calls[0], [0, 3, false, false]);
  s.go(5, true);   // to the shared set
  assert.deepEqual([s.page, s.showShared, s.mem[false]], [5, true, 3]);
  refuse = true;
  assert.equal(s.go(1, false), false);
  assert.deepEqual([s.page, s.showShared], [5, true]);
  refuse = false;
  s.go(s.mem[false], false);
  assert.deepEqual([s.page, s.showShared], [3, false]);
});

test('the shared-gold buttons call onGold with put / take and never turn a page', () => {
  const calls = [], pages = [];
  const s = createPlugyStash({ enabled: true, onGold: (k) => calls.push(k), onPage: (...a) => pages.push(a) });
  s.setArt({ stash: Array(12).fill({}), gold: Array(4).fill({}) });
  const g = { left: 0, top: 0 };
  const at = (b) => [s.BTN[b].x + 5, s.BTN[b].y + 5];
  assert.equal(s.click(...at('putGold'), 800, 600, g, true), true);
  assert.equal(s.click(...at('takeGold'), 800, 600, g, true), true);
  assert.deepEqual(calls, ['put', 'take']);
  assert.deepEqual(pages, []);
});

// PlugY's constants (Interface_Stash.cpp get[XY]*Btn at 800x600, RX(v) = 80 + v, RY(v) = 541 - v; D2PrintImage's y is the sprite's bottom edge, 32x32 cels)
// and BigStash.cpp (10x10, gridLeft 16 / gridTop 82 inside the panel). Frames: previous 0, next 2, shared 4 (+2 when showing shared), previousIndex 8, nextIndex 10; gold put 0, take 2.
const PLUGY = {
  prev: { f: 0, v: [0x80, 0x40] }, next: { f: 2, v: [0xA0, 0x40] }, shared: { f: 4, v: [0x10, 0x40] },
  prevIndex: { f: 8, v: [0x60, 0x40] }, nextIndex: { f: 10, v: [0xC0, 0x40] },
  putGold: { f: 0, v: [0x1C, 0x1A8], gold: true }, takeGold: { f: 2, v: [0x105, 0x1A8], gold: true },
};

test('button rects equal PlugY\'s Interface_Stash.cpp constants', () => {
  const s = createPlugyStash({ enabled: true });
  for (const [name, c] of Object.entries(PLUGY)) {
    assert.deepEqual([s.BTN[name].x, s.BTN[name].y + 32, s.BTN[name].f], [80 + c.v[0], 541 - c.v[1], c.f], name);
  }
  assert.deepEqual([s.BTN.close.x, s.BTN.close.y + 32], [80 + 0x110, 541 - 0x40]);
});

test('grid geometry is BigStash.cpp\'s 10x10 inside the panel', () => {
  assert.deepEqual([PLUGY_GRID.columns, PLUGY_GRID.rows], [10, 10]);
  assert.equal(PLUGY_GRID.gridLeft, 80 + 16);   // the 800x600 panel's left edge is 80
  assert.equal(PLUGY_GRID.gridTop, 61 + 82);    // its top edge is 61
});

test('the constants match the PlugY source when it is checked out', (t) => {
  const root = process.env.PLUGY_SRC || new URL('../reference/PlugY/PlugY/', import.meta.url).pathname;
  if (!existsSync(root + '/Interface_Stash.cpp')) return t.skip('PlugY source not present');
  const src = readFileSync(root + '/Interface_Stash.cpp', 'utf8');
  const hex = { Previous: '0x80', Next: '0xA0', Shared: '0x10', PreviousIndex: '0x60', NextIndex: '0xC0', PutGold: '0x1C', TakeGold: '0x105' };
  for (const [m, h] of Object.entries(hex)) assert.ok(src.match(new RegExp(`getX${m}Btn\\(\\)[^\\n]*`))[0].includes(h), m);
  assert.ok(src.match(/getYPutGoldBtn\(\)[^\n]*/)[0].includes('0x1A8'));
  const big = readFileSync(root + '/BigStash.cpp', 'utf8');
  for (const re of [/gridX_BB = 10;/, /gridY_BB = 10;/, /gridLeft_BB = 16;/, /gridTop_BB = 82;/]) assert.match(big, re);
});

test('draw lists the PlugY cels at PlugY\'s positions, down cels while pressed, and nothing without art', () => {
  const s = createPlugyStash({ enabled: true });
  const calls = [], ctx = { save() {}, restore() {}, translate() {}, scale() {}, fillRect() {}, drawImage: (img, x, y) => calls.push([img.id, x, y]) };
  const stash = Array.from({ length: 12 }, (_, i) => ({ id: 'S' + i })), gold = Array.from({ length: 4 }, (_, i) => ({ id: 'G' + i }));
  const g = { left: 80, top: 61 };
  assert.equal(s.draw(ctx, 800, 600, g, true), null);   // no art: no invented control
  assert.deepEqual(calls, []);
  s.setArt({ stash, gold });
  const ev = s.draw(ctx, 800, 600, g, true);
  const at = (n) => [80 + PLUGY[n].v[0], 541 - PLUGY[n].v[1] - 32];
  assert.deepEqual(calls, [['S0', ...at('prev')], ['S2', ...at('next')], ['S4', ...at('shared')], ['S8', ...at('prevIndex')], ['S10', ...at('nextIndex')], ['G0', ...at('putGold')], ['G2', ...at('takeGold')]]);
  assert.deepEqual(ev.buttons.map((b) => [b.name, b.art, b.frame]), [['prev', 'StashBtns.dc6', 0], ['next', 'StashBtns.dc6', 2], ['shared', 'StashBtns.dc6', 4], ['prevIndex', 'StashBtns.dc6', 8], ['nextIndex', 'StashBtns.dc6', 10], ['putGold', 'SharedGoldBtns.dc6', 0], ['takeGold', 'SharedGoldBtns.dc6', 2]]);
  calls.length = 0; s.showShared = true; s.down = 'next'; s.draw(ctx, 800, 600, g, true);
  assert.deepEqual(calls.filter((c) => /^S(3|6)$/.test(c[0])).map((c) => c[0]), ['S3', 'S6']);   // next pressed (3), shared toggle showing shared (6)
  assert.equal(s.draw(ctx, 800, 600, g, false), null);   // stash closed
  assert.equal(createPlugyStash({ enabled: true }).click(100, 100, 800, 600, g, true), false);
});

test('isDc6 accepts a DC6 header and refuses a page served for a missing file', () => {
  const dc6 = new Uint8Array(32); dc6[0] = 6; dc6[4] = 1;
  assert.equal(isDc6(dc6), true);
  assert.equal(isDc6(new TextEncoder().encode('<!doctype html><html>..................')), false);
  assert.equal(isDc6(null), false);
});
