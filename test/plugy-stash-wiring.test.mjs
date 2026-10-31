// D64: the live client drew retail's stash (no page buttons, no 10x10 grid) although the PlugY mode was on.
// Cause: the shell applied PlugY's panel art to ONE stash renderer, after waiting at most 300 s for it to exist, while every
// game start builds a new renderer; when the wait ran out the TypeError also skipped the page-button art. And the
// 10x10 grid was laid over the engine record only on the read that filled the layout cache.
// These tests replay the shell's sequence on synthetic assets and check the draw records and the input results.
import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { createStashPanelRenderer } from '../play/assets/stash-panel.mjs';
import { createPlugyStash, createPlugyPanelArt, createStashLayoutReader, PLUGY_GRID } from '../play/assets/plugy-stash-ui.mjs';

const dc6 = (n) => { const F = 38, b = new DataView(new ArrayBuffer(24 + 4 * n + F * n)); b.setInt32(0, 6, true); b.setInt32(4, 1, true); b.setUint32(12, 0xEEEEEEEE, true); b.setInt32(16, 1, true); b.setInt32(20, n, true);
  for (let i = 0; i < n; i++) { const o = 24 + 4 * n + F * i; b.setUint32(24 + 4 * i, o, true); b.setInt32(o + 4, 1, true); b.setInt32(o + 8, 1, true); b.setInt32(o + 28, 3, true);
    [1, 5, 0x80, 0xEE, 0xEE, 0xEE].forEach((v, k) => b.setUint8(o + 32 + k, v)); }
  return new Uint8Array(b.buffer); };
const read = (p) => /\.tbl$/i.test(p) ? new Uint8Array([87, 111, 111, 33, 0, 0, 0, 0, 0, 0, 10, 10]) : /pal\.dat$/i.test(p) ? new Uint8Array(768) : /pal\.pl2$/i.test(p) ? new Uint8Array(13568 + 3 * 65536) : dc6(4);
globalThis.ImageData ??= class { constructor(d, w, h) { this.data = d; this.width = w; this.height = h; } };
const doc = { createElement: () => ({ getContext: () => ({ putImageData() {} }) }) };
const ctx = () => { const ops = []; return { ops, getTransform: () => ({ a: 1, d: 1, e: 0, f: 0 }), save() {}, restore() {}, translate() {}, scale() {}, setTransform() {}, fillRect() {}, strokeRect() {},
  drawImage: (img, x, y) => ops.push(['cel', img.tag ?? null, x, y]), getImageData: (x, y, w, h) => ({ data: new Uint8ClampedArray(w * h * 4) }), putImageData() {} }; };
// The engine record of BIG_BANK_PAGE_1 at 800x600 as retail's rows give it (6x8).
const RECORD = { left: 80, top: 61, recordTop: 280, columns: 6, rows: 8, gridLeft: 153, gridTop: 143, boxW: 29, boxH: 29 };
const PLUGY_TRADE = dc6(1);   // PlugY's TradeStash.dc6 stand-in: one frame where retail's has four, so the draw record tells them apart

// The shell's own sequence: renderer per game, art bound on the draw path, the layout through the reader.
function shell() {
  const art = createPlugyPanelArt(), state = { renderer: null, setArtCalls: 0 };
  const reader = createStashLayoutReader(() => ({ ...RECORD }), () => art.apply(state.renderer));
  const newGame = () => { reader.reset(); const r = createStashPanelRenderer(read, doc); const s = r.setArt.bind(r); r.setArt = (b) => { state.setArtCalls++; s(b); }; state.renderer = r; };
  const frame = (items = []) => { art.apply(state.renderer); return state.renderer.draw(ctx(), 800, 600, reader.read(), items, true, 1); };
  return { art, state, reader, newGame, frame };
}

test('the PlugY panel art reaches a renderer built AFTER the art loaded (the expired 300 s wait)', () => {
  const s = shell();
  s.art.setTrade(PLUGY_TRADE);
  assert.equal(s.art.apply(null), false);   // no renderer yet: the old loader threw here and never set the button art
  s.newGame();
  const e = s.frame();
  assert.deepEqual([e.frames, e.grid.columns, e.grid.rows, e.grid.left, e.grid.top, e.grid.box], [1, 10, 10, 96, 143, [29, 29]]);
  assert.equal(s.state.renderer.plugy.on, true);
});

test('a second game in the same tab keeps the PlugY panel and grid (a fresh renderer is bound too)', () => {
  const s = shell();
  s.newGame(); s.art.setTrade(PLUGY_TRADE);
  assert.equal(s.frame().frames, 1);
  s.newGame();   // Save and Exit, then play again: bootstrapSession builds a new renderer with retail's art
  const e = s.frame(), e2 = s.frame();
  assert.deepEqual([e.frames, e.grid.columns, e.grid.rows], [1, 10, 10]);
  assert.deepEqual(e2.grid, e.grid, 'the second read of the cached record is 10x10 too');
  assert.equal(s.state.setArtCalls, 2, 'setArt once per renderer, not per frame');
});

test('without the PlugY art the panel stays retail (6x8, four frames)', () => {
  const s = shell();
  s.newGame();
  const e = s.frame();
  assert.deepEqual([e.frames, e.grid.columns, e.grid.rows, e.grid.left], [4, 6, 8, 153]);
});

test('the layout reader lays the 10x10 grid over the cached record on every read', () => {
  let reads = 0, on = true;
  const r = createStashLayoutReader(() => (reads++, { ...RECORD }), () => on);
  const a = r.read(), b = r.read();
  assert.deepEqual([a.columns, a.rows, a.gridLeft, a.gridTop, b.columns, b.rows, b.gridLeft, b.gridTop], [10, 10, PLUGY_GRID.gridLeft, PLUGY_GRID.gridTop, 10, 10, PLUGY_GRID.gridLeft, PLUGY_GRID.gridTop]);
  assert.equal(reads, 1);
  on = false;
  assert.deepEqual([r.read().columns, r.read().rows], [6, 8]);
  r.reset(); r.read();
  assert.equal(reads, 2);
});

test('items on the far row and column draw, are hit and accept a drop on the 10x10 grid', () => {
  const s = shell();
  s.newGame(); s.art.setTrade(PLUGY_TRADE);
  const items = [{ id: 5, mode: 0, invPage: 4, invFile: 'x', gridX: 9, gridY: 9, invWidth: 1, invHeight: 1, itemFlags: 0x10 },
    { id: 6, mode: 0, invPage: 4, invFile: 'y', gridX: 7, gridY: 0, invWidth: 2, invHeight: 3, itemFlags: 0x10 }];
  const e = s.frame(items);
  assert.deepEqual(e.drawn.map((d) => [d.id, d.cell]), [[5, [9, 9]], [6, [7, 0]]]);
  const g = s.reader.read(), r = s.state.renderer, mid = (cx, cy) => [g.gridLeft + cx * 29 + 14, g.gridTop + cy * 29 + 14];
  assert.equal(r.itemAt(...mid(9, 9), 800, 600, g, items)?.id, 5);
  assert.equal(r.itemAt(...mid(8, 2), 800, 600, g, items)?.id, 6);
  assert.deepEqual(r.cellAt(...mid(9, 9), 800, 600, g, { invWidth: 1, invHeight: 1 }), { x: 9, y: 9 });
  assert.deepEqual(r.cellAt(...mid(9, 5), 800, 600, g, { invWidth: 1, invHeight: 1 }), { x: 9, y: 5 });
  // On retail's 6x8 record the same point is off the grid (x 96+9*29 > 153+6*29 is not, so check a cell the old grid lacks).
  assert.equal(r.cellAt(g.gridLeft + 2, g.gridTop + 9 * 29 + 14, 800, 600, RECORD, { invWidth: 1, invHeight: 1 }), null);
});

test('the page control draws its buttons and pages on clicks (input -> onPage)', () => {
  const calls = [];
  const p = createPlugyStash({ enabled: true, onPage: (...a) => { calls.push(a); return true; }, lastUsed: () => 4 });
  const tag = (n, k) => Array.from({ length: n }, (_, i) => ({ tag: `${k}${i}` }));
  p.setArt({ stash: tag(12, 's'), gold: tag(4, 'g') });
  p.text = { measure: () => 10, draw() {} };
  const c = ctx(), g = { ...RECORD, ...PLUGY_GRID };
  const ev = p.draw(c, 800, 600, g, true);
  assert.deepEqual(ev.buttons.map((b) => [b.name, b.art, b.frame, b.x, b.y]), [
    ['prev', 'StashBtns.dc6', 0, 208, 445], ['next', 'StashBtns.dc6', 2, 240, 445], ['shared', 'StashBtns.dc6', 4, 96, 445],
    ['prevIndex', 'StashBtns.dc6', 8, 176, 445], ['nextIndex', 'StashBtns.dc6', 10, 272, 445],
    ['putGold', 'SharedGoldBtns.dc6', 0, 108, 85], ['takeGold', 'SharedGoldBtns.dc6', 2, 341, 85]]);
  assert.deepEqual(c.ops.map((o) => o[1]), ['s0', 's2', 's4', 's8', 's10', 'g0', 'g2']);
  const at = (n) => [p.BTN[n].x + 16, p.BTN[n].y + 16];
  assert.equal(p.click(...at('prev'), 800, 600, g, true), true);   // page 0: prev stays, no swap
  assert.equal(p.click(...at('next'), 800, 600, g, true), true);
  assert.equal(p.click(...at('next'), 800, 600, g, true, true), true);   // shift: last non-empty page (4)
  assert.equal(p.click(...at('shared'), 800, 600, g, true), true);
  assert.equal(p.click(...at('shared'), 800, 600, g, true), true);   // back to the personal set at its page
  assert.equal(p.click(400, 300, 800, 600, g, true), false);
  assert.deepEqual(calls, [[0, 1, false, false], [1, 4, false, false], [4, 0, false, true], [0, 4, true, false]]);
  p.go(0, true);
  assert.equal(p.draw(ctx(), 800, 600, g, true).buttons.find((b) => b.name === 'shared').frame, 6, 'shared set: the toggle shows its other face');
});

test('the shell binds the art on the draw path and reads the stash layout through the reader', () => {
  const src = readFileSync(new URL('../play/gameplay-shell.mjs', import.meta.url), 'utf8');
  assert.doesNotMatch(src, /!stashPanelRenderer && t < 600/, 'no deadline on the renderer');
  assert.match(src, /if \(plugyStash\.enabled\) plugyPanelArt\.apply\(stashPanelRenderer\);\n\s+if \(stashPanelRenderer\?\.plugy\.on\)/);
  assert.match(src, /function readStashLayout\(\) \{ return \(stashLayout = stashLayoutReader\.read\(\)\); \}/);
  assert.match(src, /plugyStash\.setArt\(\{ stash: canv\(btns\), gold: canv\(gold\) \}\);\n\s+plugyPanelArt\.setTrade\(trade\)/);
});
