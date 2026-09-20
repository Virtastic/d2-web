import { decodeDatPalette, decodeDt1 } from './d2-formats.mjs';

export const ACT_TILE_ASSETS = Object.freeze([
  Object.freeze({ palette: 'data\\global\\palette\\ACT1\\pal.dat', dt1: 'data\\global\\tiles\\ACT1\\TOWN\\floor.dt1' }),
  Object.freeze({ palette: 'data\\global\\palette\\ACT2\\pal.dat', dt1: 'data\\global\\tiles\\ACT2\\Town\\ground.dt1' }),
  Object.freeze({ palette: 'data\\global\\palette\\ACT3\\pal.dat', dt1: 'data\\global\\tiles\\ACT3\\Docktown\\ruinflr.dt1' }),
  Object.freeze({ palette: 'data\\global\\palette\\ACT4\\pal.dat', dt1: 'data\\global\\tiles\\ACT4\\Mesa\\floor.dt1' }),
  Object.freeze({ palette: 'data\\global\\palette\\ACT5\\pal.dat', dt1: 'data\\global\\tiles\\expansion\\Town\\ground.dt1' }),
]);

export function tilePixelsToRgba(pixels, palette) {
  if (!pixels?.indices || !pixels?.opaque || pixels.indices.length !== pixels.opaque.length) {
    throw new TypeError('DT1 tile pixels require equal index and opacity planes');
  }
  if (!(palette instanceof Uint8Array) || palette.length !== 1024) {
    throw new RangeError('decoded palette must contain 256 RGBA colors');
  }
  const rgba = new Uint8ClampedArray(pixels.indices.length * 4);
  for (let i = 0; i < pixels.indices.length; i++) {
    if (!pixels.opaque[i]) continue;
    const source = pixels.indices[i] * 4;
    rgba.set(palette.subarray(source, source + 4), i * 4);
  }
  return rgba;
}

export function representativeTiles(dt1, limit = 12) {
  if (!dt1?.tiles || !Number.isSafeInteger(limit) || limit < 1) return [];
  return dt1.tiles
    .filter((tile) => tile.pixels.width > 0 && tile.pixels.height > 0 && tile.pixels.opaque.some(Boolean))
    .sort((a, b) => (b.rarity - a.rarity) || (b.pixels.indices.length - a.pixels.indices.length))
    .slice(0, limit);
}

export function decodeActTileAssets(read, act0) {
  const asset = ACT_TILE_ASSETS[act0];
  if (!asset) throw new RangeError(`unsupported act ${act0 + 1}`);
  const palette = decodeDatPalette(read(asset.palette));
  const dt1 = decodeDt1(read(asset.dt1));
  const tiles = representativeTiles(dt1);
  if (!tiles.length) throw new Error(`DT1 has no drawable tiles: ${asset.dt1}`);
  return { act0, asset, palette, tiles };
}

export function createActTileAtlas(decoded, documentObject = globalThis.document) {
  if (!documentObject?.createElement) throw new Error('canvas document is unavailable');
  const cellWidth = 192, cellHeight = 128, columns = 4;
  const rows = Math.ceil(decoded.tiles.length / columns);
  const canvas = documentObject.createElement('canvas');
  canvas.width = cellWidth * columns; canvas.height = cellHeight * rows;
  const context = canvas.getContext('2d');
  context.imageSmoothingEnabled = false;
  for (let i = 0; i < decoded.tiles.length; i++) {
    const pixels = decoded.tiles[i].pixels;
    const rgba = tilePixelsToRgba(pixels, decoded.palette);
    const tileCanvas = documentObject.createElement('canvas');
    tileCanvas.width = pixels.width; tileCanvas.height = pixels.height;
    tileCanvas.getContext('2d').putImageData(new ImageData(rgba, pixels.width, pixels.height), 0, 0);
    const x = i % columns * cellWidth + Math.floor((cellWidth - pixels.width) / 2);
    const y = Math.floor(i / columns) * cellHeight + cellHeight - pixels.height;
    context.drawImage(tileCanvas, x, y);
  }
  return canvas;
}
