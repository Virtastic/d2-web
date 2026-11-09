import test from 'node:test';
import assert from 'node:assert/strict';
import { PL2_ALPHA } from './frontend.mjs';
import { createJoinLoading, createJoinLoadingArt, JOIN_LOADING_TIMEOUT_MS } from './bnet-join-loading.mjs';

test('click -> loading shown -> world ready -> game scene', () => {
  let t = 0; const l = createJoinLoading({ now: () => t });
  assert.equal(l.shown, false);
  l.start(); assert.equal(l.shown, true);
  assert.equal(l.steps, 0); l.step(); l.step(); assert.equal(l.steps, 2);
  assert.equal(l.ready(), true); assert.equal(l.shown, false);
  assert.equal(l.ready(), false);
});
test('failure returns to the lobby', () => {
  const l = createJoinLoading({ now: () => 0 });
  l.start(); assert.equal(l.fail(), true); assert.equal(l.shown, false); assert.equal(l.fail(), false);
});
test('a join whose world never comes up leaves no loading screen behind', () => {
  let t = 0; const l = createJoinLoading({ now: () => t });
  l.start(); assert.equal(l.fail(), true);
  t = JOIN_LOADING_TIMEOUT_MS * 2;
  assert.equal(l.shown, false); assert.equal(l.expired(), false); assert.equal(l.ready(), false);
  l.start(); t += 1000; assert.equal(l.shown, true);
});
test('the wait ends at three minutes', () => {
  let t = 0; const l = createJoinLoading({ now: () => t });
  l.start(); t = JOIN_LOADING_TIMEOUT_MS - 1; assert.equal(l.expired(), false);
  t = JOIN_LOADING_TIMEOUT_MS; assert.equal(l.expired(), true);
});
test('a realm join (mode 3) draws no bar; a TCP/IP address join (mode 7) does', () => {
  const wasm = { rects: (mode) => (mode >= 6 && mode <= 9 ? [0, 0, 1, 1, 0, 1] : []), width: () => 0, chunks: () => 0, finalWidth: () => 0, walkCalls: (n) => n };
  const realm = createJoinLoading({ now: () => 0, wasm }); realm.start(); assert.deepEqual(realm.bar(), []);
  const addr = createJoinLoading({ now: () => 0, wasm, mode: 7 }); addr.start(); assert.equal(addr.bar().length, 1);
});
test('a mode-1 rectangle takes the PL2 alpha table entry over black; a mode-5 one the plain colour', () => {
  const palette = new Uint8Array(1024); for (let i = 0; i < 256; i++) palette.set([i, 0, 0, 255], i * 4);
  const pl2 = new Uint8Array(PL2_ALPHA + 65536); for (let i = 0; i < 256; i++) pl2[PL2_ALPHA + i] = i >> 1;   // [under 0][src] = half
  const files = { 'data\\global\\ui\\Loading\\loadingscreen.dc6': 'dc6', 'data\\global\\palette\\Loading\\pal.dat': 'dat', 'data\\global\\palette\\Loading\\Pal.PL2': pl2 };
  const art = createJoinLoadingArt({ read: (n) => files[n], decodeDc6: () => ({ frames: [] }), decodeDatPalette: () => palette, documentObject: {} });
  const fills = [], ctx = { set fillStyle(v) { this.s = v; }, fillRect() { fills.push(this.s); } };
  art.draw(ctx, 800, 600, 0, [[0, 0, 1, 1, 0, 1], [0, 0, 1, 1, 0, 5]]);
  assert.deepEqual(fills.slice(1), ['rgb(127,0,0)', 'rgb(255,0,0)']);   // near(255,255,0) is index 255; mode 1 gives 255 >> 1
});
