import test from 'node:test';
import assert from 'node:assert/strict';
import { drawObjectHoverLabel } from '../play/assets/object-hover-label.mjs';
import { PL2_ALPHA_75 } from '../play/assets/frontend.mjs';

function plateResources({ act = 3, duplicatePaletteRgb = false } = {}) {
  const palette = new Uint8Array(768);
  for (let i = 0; i < 256; i++) palette.set([i, i, i], i * 3);
  if (duplicatePaletteRgb) palette.set([5, 5, 5], 6 * 3);
  const pl2 = new Uint8Array(PL2_ALPHA_75 + 65536);
  const outputIndex = 6 + act;
  pl2[PL2_ALPHA_75 + 5 * 256] = outputIndex; // under-index 5, source colour index 0 -> act-specific index
  const memory = new Uint8Array(300_000), assets = new Map([
    [`data\\global\\palette\\ACT${act}\\pal.dat`, palette],
    [`data\\global\\palette\\ACT${act}\\pal.pl2`, pl2],
  ]);
  let current = null; const assetReads = [];
  const Module = { HEAPU8: memory, ccall(name, _returnType, _argTypes, args) {
    if (name === 'd2_asset_read') { assetReads.push(args[0]); current = assets.get(args[0]); if (!current) return 0; memory.set(current, 1024); return 1; }
    if (name === 'd2_asset_len') return current.length;
    if (name === 'd2_asset_ptr') return 1024n;
    throw new Error(`unexpected Module call ${name}`);
  } };
  Module.assetReads = assetReads;
  Module.outputIndex = outputIndex;
  return Module;
}

function hoverLabel(ctx, { act = 3, Module = plateResources({ act }), nameOffset = -40, project = () => [100, 100] } = {}) {
  const boxes = [];
  const font = { measure: (text) => text.length, drawAnchored() {} };
  const plate = drawObjectHoverLabel(ctx,
    { kind: 'object', id: 1, classId: 267, token: 'b6', label: 'Your Private Stash', nameOffset, x: 0, y: 0 },
    font, project, (...box) => boxes.push(box), false, Module, act);
  return { plate, boxes };
}

function fallbackContext({ pixel = [5, 5, 5, 255], getImageData = true } = {}) {
  let fill = null, putCalls = 0;
  const ctx = { canvas: { width: 800, height: 600 }, save() {}, restore() {},
    ...(getImageData ? { getImageData(_x, _y, width, height) {
      const data = new Uint8ClampedArray(width * height * 4);
      for (let i = 0; i < width * height; i++) data.set(pixel, i * 4);
      return { data };
    } } : {}),
    putImageData() { putCalls++; },
    fillRect(x, y, width, height) { fill = { x, y, width, height, style: this.fillStyle }; } };
  return { ctx, get fill() { return fill; }, get putCalls() { return putCalls; } };
}

test('object hover title plate uses the active act palette PL2 DrawMode 2 table', () => {
  const Module = plateResources({ act: 3 });
  const written = [], boxes = [];
  const ctx = { canvas: { width: 800, height: 600 },
    getImageData(x, y, width, height) {
      const data = new Uint8ClampedArray(width * height * 4);
      for (let i = 0; i < width * height; i++) data.set([5, 5, 5, 255], i * 4);
      return { data };
    },
    putImageData(image) { written.splice(0, written.length, ...image.data); },
    save() { throw new Error('PL2 path should not use the canvas fallback'); },
    fillRect() { throw new Error('PL2 path should not use the canvas fallback'); },
    restore() { throw new Error('PL2 path should not use the canvas fallback'); },
  };
  const font = { measure: (text) => text.length, drawAnchored() {} };
  const plate = drawObjectHoverLabel(ctx,
    { kind: 'object', id: 1, classId: 267, token: 'b6', label: 'Your Private Stash', nameOffset: -40, x: 0, y: 0 },
    font, () => [100, 100], (...box) => boxes.push(box), true, Module, 3);

  assert.equal(plate.plateCompositing, 'PL2 AlphaBlend[2][under][0]');
  assert.equal(boxes[0][5], 0);
  assert.equal(boxes[0][6], 2);
  assert.ok(written.length > 0);
  assert.equal(written[0], Module.outputIndex);
  assert.equal(written[1], Module.outputIndex);
  assert.equal(written[2], Module.outputIndex);
  assert.equal(written[3], 255);
});

for (const act of [1, 5]) {
  test(`object hover title plate reads ACT${act} palette and PL2 assets`, () => {
    const Module = plateResources({ act }), canvas = fallbackContext();
    const { plate } = hoverLabel(canvas.ctx, { Module, act });
    assert.equal(plate.plateCompositing, 'PL2 AlphaBlend[2][under][0]');
    assert.deepEqual(Module.assetReads, [
      `data\\global\\palette\\ACT${act}\\pal.dat`,
      `data\\global\\palette\\ACT${act}\\pal.pl2`,
    ]);
    assert.equal(canvas.putCalls, 1);
    assert.equal(canvas.fill, null);
  });
}

test('object hover title plate retains the fallback when retail palette resources are unavailable', () => {
  const canvas = fallbackContext({ getImageData: false });
  const { plate } = hoverLabel(canvas.ctx, { Module: null });
  assert.equal(plate.plateCompositing, 'canvas-half-alpha-fallback');
  assert.deepEqual(canvas.fill, { x: 87, y: 46, width: 26, height: 16, style: 'rgba(0,0,0,.5)' });
});

test('object hover title plate falls back for ambiguous palette indices', () => {
  const canvas = fallbackContext();
  const { plate } = hoverLabel(canvas.ctx, { Module: plateResources({ duplicatePaletteRgb: true }) });
  assert.equal(plate.plateCompositing, 'canvas-half-alpha-fallback');
  assert.equal(canvas.putCalls, 0);
  assert.deepEqual(canvas.fill, { x: 87, y: 46, width: 26, height: 16, style: 'rgba(0,0,0,.5)' });
});

for (const [description, pixel] of [
  ['non-opaque', [5, 5, 5, 254]],
  ['unmapped', [255, 0, 0, 255]],
]) {
  test(`object hover title plate falls back for ${description} under-pixels`, () => {
    const canvas = fallbackContext({ pixel });
    const { plate } = hoverLabel(canvas.ctx);
    assert.equal(plate.plateCompositing, 'canvas-half-alpha-fallback');
    assert.equal(canvas.putCalls, 0);
    assert.deepEqual(canvas.fill, { x: 87, y: 46, width: 26, height: 16, style: 'rgba(0,0,0,.5)' });
  });
}

test('object hover title plate falls back when its rectangle is clipped', () => {
  const canvas = fallbackContext();
  const { plate } = hoverLabel(canvas.ctx, { nameOffset: -200 });
  assert.equal(plate.plateCompositing, 'canvas-half-alpha-fallback');
  assert.equal(canvas.putCalls, 0);
  assert.deepEqual(canvas.fill, { x: 87, y: -5, width: 26, height: 16, style: 'rgba(0,0,0,.5)' });
});
