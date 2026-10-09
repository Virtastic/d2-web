/** A small FIFO of immutable per-layer canvases used by blended-object fallback layers. */
export function createObjectLayerRasterCache(documentObject, { limit = 256, byteLimit = 64 * 1024 * 1024, makeImageData = (rgba, width, height) =>
  new ImageData(rgba, width, height) } = {}) {
  if (!Number.isInteger(limit) || limit < 1) throw new RangeError('object layer raster cache limit must be positive');
  if (!Number.isSafeInteger(byteLimit) || byteLimit < 1) throw new RangeError('object layer raster cache byte limit must be positive');
  const entries = new Map();
  let bytes = 0;
  return {
    get(key, width, height, rgba) {
      let canvas = entries.get(key);
      if (canvas) return canvas;
      const rasterBytes = width * height * 4;
      canvas = documentObject.createElement('canvas'); canvas.width = width; canvas.height = height;
      if (width && height) canvas.getContext('2d').putImageData(makeImageData(typeof rgba === 'function' ? rgba() : rgba, width, height), 0, 0);
      if (!Number.isSafeInteger(rasterBytes) || rasterBytes < 0 || rasterBytes > byteLimit) return canvas;
      entries.set(key, canvas);
      bytes += rasterBytes;
      while (entries.size > limit || bytes > byteLimit) {
        const oldestKey = entries.keys().next().value;
        bytes -= entries.get(oldestKey).width * entries.get(oldestKey).height * 4;
        entries.delete(oldestKey);
      }
      return canvas;
    },
    get size() { return entries.size; },
    get bytes() { return bytes; },
  };
}

/** Keep native PL2 blending live against the destination; cache only the immutable raster fallback. */
export function drawObjectLayer(ctx, cache, { key, width, height, rgba, layer, left, top, mode, blend }) {
  if (mode !== 5 && blend) {
    blend(ctx, layer, left, top, mode);
    return 'pl2';
  }
  ctx.drawImage(cache.get(key, width, height, rgba), left, top);
  return 'canvas';
}
