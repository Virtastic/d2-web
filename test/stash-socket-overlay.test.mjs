import test from 'node:test';
import assert from 'node:assert/strict';
import { createStashPanelRenderer } from '../play/assets/stash-panel.mjs';
function syntheticAssets() {
  const dc6 = (n) => { const F = 38, b = new DataView(new ArrayBuffer(24 + 4 * n + F * n)); b.setInt32(0, 6, true); b.setInt32(4, 1, true); b.setUint32(12, 0xEEEEEEEE, true); b.setInt32(16, 1, true); b.setInt32(20, n, true);
    for (let i = 0; i < n; i++) { const o = 24 + 4 * n + F * i; b.setUint32(24 + 4 * i, o, true); b.setInt32(o + 4, 1, true); b.setInt32(o + 8, 1, true); b.setInt32(o + 28, 3, true);
      [1, 5, 0x80, 0xEE, 0xEE, 0xEE].forEach((v, k) => b.setUint8(o + 32 + k, v)); }
    return new Uint8Array(b.buffer); };
  const pal = new Uint8Array(768).fill(255);   // pal.dat is BGR
  [[10, 128, 0, 0], [20, 0, 128, 0], [30, 0, 0, 128], [40, 128, 128, 0], [50, 128, 64, 64]].forEach(([k, r, g, b]) => pal.set([b, g, r], k * 3));
  const read = (p) => /\.tbl$/i.test(p) ? new Uint8Array([87, 111, 111, 33, 0, 0, 0, 0, 0, 0, 10, 10]) : /pal\.dat$/i.test(p) ? pal : /pal\.pl2$/i.test(p) ? new Uint8Array(13568 + 3 * 65536) : dc6(4);
  globalThis.ImageData ??= class { constructor(d, w, h) { this.data = d; this.width = w; this.height = h; } };
  const doc = { createElement: () => ({ getContext: () => ({ putImageData() {} }) }) };
  const ops = [];
  const ctx = { getTransform: () => ({ a: 1, d: 1, e: 0, f: 0 }), save() {}, restore() {}, translate() {}, scale() {}, setTransform() {}, fillRect() {}, strokeRect() {},
    drawImage: (img, x, y) => ops.push(['cel', x, y]), getImageData: (x, y, w, h) => (ops.push(['box', x, y, w, h]), { data: new Uint8ClampedArray(w * h * 4) }), putImageData() {} };
  return { read, doc, ctx, ops };
}
const STASH = { left: 80, top: 63, recordTop: 63, columns: 6, rows: 8, gridLeft: 153, gridTop: 143, boxW: 29, boxH: 29 };

test('stash: only the hovered socketed item shows its socket cels, at the footprint offsets, gems one cell above', () => {
  const { read, doc, ctx } = syntheticAssets();
  const r = createStashPanelRenderer(read, doc);
  const sockets = { off: [[14, 29], [14, 87]], gem: ['', 'gem1'] };
  const items = [{ id: 7, mode: 0, invPage: 4, invFile: 'a', gridX: 1, gridY: 2, invWidth: 1, invHeight: 4, itemFlags: 0x810, sockets },
    { id: 8, mode: 0, invPage: 4, invFile: 'b', gridX: 4, gridY: 0, invWidth: 1, invHeight: 1, itemFlags: 0x810, sockets: { off: [[1, 1]], gem: [''] } }];
  const hover = (x, y) => ({ pointer: { x, y } });
  let e = r.draw(ctx, 800, 600, STASH, items, true, 1, false, hover(5, 5));
  assert.deepEqual(e.sockets, [], 'nothing hovered, no sockets drawn');
  e = r.draw(ctx, 800, 600, STASH, items, true, 1, false, hover(153 + 29 + 5, 143 + 58 + 5));
  assert.deepEqual(e.sockets.map((s) => [s.id, s.n, s.gem && s.gem.invFile]), [[7, 0, null], [7, 1, 'gem1']]);
  e = r.draw(ctx, 800, 600, STASH, items, true, 1, true, { pointer: { x: 153 + 29 + 5, y: 143 + 58 + 5 }, cursorItem: { id: 1, invWidth: 1, invHeight: 1, invFile: 'h' } });
  assert.deepEqual(e.sockets, [], 'no sockets while an item is held');
});
