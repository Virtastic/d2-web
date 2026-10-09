import test from 'node:test';
import assert from 'node:assert/strict';
import { createObjectLayerRasterCache, drawObjectLayer } from '../play/assets/object-layer-raster-cache.mjs';

function fakeDocument() {
  let created = 0;
  return { get created() { return created; }, createElement(tag) {
    assert.equal(tag, 'canvas'); created++;
    const canvas = { width: 0, height: 0, pixels: null, getContext() { return {
      putImageData(image) { canvas.pixels = [...image.data]; },
    }; } };
    return canvas;
  } };
}
const imageData = (data) => ({ data });

test('object layer raster cache reuses immutable fallback pixels while preserving draw order and PL2 calls', () => {
  const layers = [
    { key: 'obj:3:HD:act0', width: 2, height: 1, pixels: [10, 11], mode: 5, x: 4, y: 7 },
    { key: 'obj:3:TR:act0', width: 1, height: 2, pixels: [20, 21], mode: 3, x: 4, y: 6 },
    { key: 'obj:3:LG:act0', width: 1, height: 1, pixels: [30], mode: 5, x: 5, y: 7 },
    { key: 'obj:3:RA:act0:highlight', width: 1, height: 1, pixels: [40], mode: 7, x: 6, y: 7 },
  ];
  const renderTwice = (limit) => {
    const documentObject = fakeDocument(), cache = createObjectLayerRasterCache(documentObject, { limit, makeImageData: imageData });
    const calls = [], evidence = [], rgbaBuilds = { count: 0 }, destination = [];
    const ctx = { drawImage(canvas, x, y) { calls.push(['canvas', x, y]); destination.push({ x, y, pixels: [...canvas.pixels] }); } };
    const blend = (_ctx, layer, x, y, mode) => {
      calls.push(['pl2', x, y, mode, [...layer.indices]]);
      destination.push({ x, y, pixels: ['pl2', mode, ...layer.indices] });
    };
    for (let frame = 0; frame < 2; frame++) for (const layer of layers) {
      evidence.push({ component: layer.key.split(':')[2], mode: layer.mode, x: layer.x, y: layer.y });
      const result = drawObjectLayer(ctx, cache, { key: layer.key, width: layer.width, height: layer.height,
        rgba: () => { rgbaBuilds.count++; return layer.pixels.flatMap((p) => [p, 0, 0, 255]); },
        layer: { width: layer.width, height: layer.height, indices: layer.pixels, opaque: new Uint8Array(layer.pixels.length).fill(1) },
        left: layer.x, top: layer.y, mode: layer.mode, blend: layer.mode === 7 ? null : blend });
      assert.equal(result, layer.mode === 3 ? 'pl2' : 'canvas');
    }
    return { calls, evidence, output: destination.map(({ x, y, pixels }) => [x, y, pixels]),
      created: documentObject.created, converted: rgbaBuilds.count, size: cache.size };
  };

  const cached = renderTwice(8), evictionForced = renderTwice(1);
  assert.deepEqual(cached.calls, evictionForced.calls, 'draw order and PL2 arguments are identical');
  assert.deepEqual(cached.output, evictionForced.output, 'raster pixels and blend results are identical');
  assert.deepEqual(cached.evidence, evictionForced.evidence, 'per-layer evidence order and values are identical');
  assert.equal(cached.created, 3, 'fallback and highlight raster layers allocate canvases once each');
  assert.equal(cached.converted, 3, 'pixel conversion occurs once per cached raster');
  assert.equal(cached.size, 3);
  assert.equal(evictionForced.created, 6, 'one-entry cache forces a baseline raster rebuild per frame');
  assert.equal(evictionForced.converted, 6);
});

test('object layer raster cache evicts oldest entries at its fixed bound', () => {
  const doc = fakeDocument(), cache = createObjectLayerRasterCache(doc, { limit: 2, makeImageData: imageData });
  const get = (key) => cache.get(key, 1, 1, [key.charCodeAt(0), 0, 0, 255]);
  const first = get('a'); get('b'); get('c');
  assert.equal(cache.size, 2);
  assert.notEqual(get('a'), first, 'the evicted entry is rebuilt on demand');
  assert.equal(cache.size, 2);
});

test('object layer raster cache evicts oldest variable-sized rasters to satisfy byte and count bounds', () => {
  const doc = fakeDocument(), cache = createObjectLayerRasterCache(doc, { limit: 4, byteLimit: 20, makeImageData: imageData });
  // Composed-object rasters and fallback layer rasters use the same FIFO, so their
  // distinct key families must compete for the same byte budget.
  const get = (key, width) => cache.get(key, width, 1, new Array(width * 4).fill(key.charCodeAt(0)));
  const a = get('composed:object:frame:act:normal', 1), b = get('layer:object:frame:component:act:normal', 2);
  assert.equal(cache.bytes, 12);
  const c = get('composed:other-object:frame:act:normal', 4);
  assert.equal(cache.size, 1, 'adding 16 bytes evicts both older entries');
  assert.equal(cache.bytes, 16);
  assert.notEqual(get('composed:object:frame:act:normal', 1), a, 'the older composed entry was evicted');
  assert.equal(cache.size, 2);
  assert.equal(cache.bytes, 20);
  assert.equal(get('composed:other-object:frame:act:normal', 4), c, 'the newer raster remains cached');
  assert.notEqual(get('layer:object:frame:component:act:normal', 2), b, 'the older layer entry was evicted');
  assert.equal(cache.size, 2);
  assert.equal(cache.bytes, 12);
});

test('object layer raster cache draws but does not retain a raster larger than its byte cap', () => {
  const doc = fakeDocument(), cache = createObjectLayerRasterCache(doc, { limit: 2, byteLimit: 8, makeImageData: imageData });
  const first = cache.get('large', 3, 1, new Array(12).fill(7));
  const second = cache.get('large', 3, 1, new Array(12).fill(7));
  assert.notEqual(second, first, 'oversized raster is rebuilt instead of cached');
  assert.equal(cache.size, 0);
  assert.equal(cache.bytes, 0);
  assert.deepEqual(first.pixels, new Array(12).fill(7), 'the uncached raster remains drawable');
});
