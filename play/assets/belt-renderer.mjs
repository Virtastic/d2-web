import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';

export const BELT_LAYOUT = Object.freeze({ x: 342, y: 561, cells: 4, cell: 31 });

function raster(frame, palette, documentObject) { const c = documentObject.createElement('canvas'); c.width = frame.width; c.height = frame.height;
  c.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0); return c; }

export function createBeltRenderer(read, documentObject = globalThis.document) {
  const palette = decodeDatPalette(read('data\\global\\palette\\SKY\\pal.dat')), cache = new Map();
  const icon = name => { let c = cache.get(name); if (!c) { const d = decodeDc6(read(`data\\global\\items\\${name}.dc6`)); c = raster(d.frames[0], palette, documentObject); cache.set(name, c); } return c; };
  return { itemAt(x, y, items) { const slot = Math.floor((x - BELT_LAYOUT.x) / BELT_LAYOUT.cell); if (slot < 0 || slot >= 4 || y < BELT_LAYOUT.y || y >= BELT_LAYOUT.y + 31) return null;
      return (items || []).find(item => item.mode === 2 && item.gridX === slot) || null; },
    draw(ctx, items) { let count = 0, pixels = 0; for (const item of items || []) { if (item.mode !== 2 || item.gridX < 0 || item.gridX >= 4 || !item.invFile) continue;
        const c = icon(item.invFile); ctx.drawImage(c, BELT_LAYOUT.x + item.gridX * 31 + Math.floor((31 - c.width) / 2), BELT_LAYOUT.y + Math.floor((31 - c.height) / 2)); count++; pixels += c.width * c.height; }
      return { count, pixels }; } };
}
