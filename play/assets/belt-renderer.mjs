import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { createFont, decodeFontTbl, textColourPalette } from './frontend.mjs';

// The belt's boxes are TABLE DATA, not a layout this file gets to choose. Retail computes them in
// DATATBLS_GetInvRectFromBeltsTxt (D2Common #10638) from Belts.txt, at the requested resolution,
// and the engine hands them over through `d2_session_belt_boxes`.
//
// This used to be `{ x: 342, y: 561, cells: 4, cell: 31 }` -- typed in, uncited, and wrong: retail's
// 800x600 belt is 29x29 boxes with its first column at x=423, so the belt was drawn 81px left of
// where the game puts it. 
//
// Kept only as the fallback for when the engine is not up (the panel still has to draw something
// during boot), and marked as such rather than presented as the layout.
export const BELT_FALLBACK = Object.freeze({ x: 423, y: 562, cells: 4, cell: 29, pitch: 31 });

function raster(frame, palette, documentObject) { const c = documentObject.createElement('canvas'); c.width = frame.width; c.height = frame.height;
  c.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0); return c; }

/**
 * `boxes` is the engine's Belts.txt rectangles: [{ left, right, top, bottom }, ...] in the belt's
 * own order, which is bottom row first. A belt with no record falls back to the four-box row.
 */
export function beltBoxes(boxes) {
  if (Array.isArray(boxes) && boxes.length) return boxes;
  const { x, y, cells, cell, pitch } = BELT_FALLBACK;
  return Array.from({ length: cells }, (_, i) =>
    ({ left: x + i * pitch, right: x + i * pitch + cell, top: y, bottom: y + cell }));
}

export function createBeltRenderer(read, documentObject = globalThis.document) {
  const palette = decodeDatPalette(read('data\\global\\palette\\SKY\\pal.dat')), cache = new Map();
  const icon = name => { let c = cache.get(name); if (!c) { const d = decodeDc6(read(`data\\global\\items\\${name}.dc6`)); c = raster(d.frames[0], palette, documentObject); cache.set(name, c); } return c; };
  // Retail labels the belt's four columns '1'..'4': `text '1' x=425 y=589 color=4` = Font16, gold shift
  // (extra=4), 2 px in from each column's left and 27 px below its top.
  const FONT = 'data\\local\\font\\latin\\font16';
  let numFont = null;
  const numbers = (ctx, rects) => {
    numFont ??= createFont({ dc6: decodeDc6(read(FONT + '.dc6')), tbl: decodeFontTbl(read(FONT + '.tbl')),
      palette: textColourPalette(read('data\\global\\palette\\SKY\\pal.pl2'), palette, 4), documentObject, path: FONT + '.dc6', drawAs: { op: 'celcolor', mode: 5, extra: 4 } });
    rects.slice(0, 4).forEach((b, i) => numFont.drawAnchored(ctx, String(i + 1), b.left + 2, b.top + 27));
  };
  return {
    itemAt(x, y, items, boxes) {
      const rects = beltBoxes(boxes);
      const slot = rects.findIndex((b) => x >= b.left && x < b.right && y >= b.top && y < b.bottom);
      if (slot < 0) return null;
      return (items || []).find(item => item.mode === 2 && item.gridX === slot) || null;
    },
    draw(ctx, items, boxes, labels = false) {   // labels: the HUD passes true; item-pixel rows count the sprites alone
      const rects = beltBoxes(boxes);
      let count = 0, pixels = 0;
      for (const item of items || []) {
        if (item.mode !== 2 || item.gridX < 0 || item.gridX >= rects.length || !item.invFile) continue;
        const b = rects[item.gridX], c = icon(item.invFile);
        const w = b.right - b.left, h = b.bottom - b.top;
        // The cel is 28x28 in a 29x29 Belts.txt box and sits one row BELOW the box's top row: retail
        // draws the HUD belt poticon with its bottom row on the box's bottom row (cel ? 28x28 at
        // y=590 against a box of 562..591), which is the same +1
        // the inventory places a 28x28 cel in a 29x29 cell with (play/assets/inventory-panel.mjs:229
        // grid, :234 body). Measured: F228/s813 reads our canvas 1 px up and finds 889 of 948 belt
        // pixels wrong; at the +1 they are retail's.
        ctx.drawImage(c, b.left + Math.floor((w - c.width) / 2), b.top + Math.floor((h - c.height) / 2) + 1);
        count++; pixels += c.width * c.height;
      }
      if (labels) numbers(ctx, rects);
      return { count, pixels, boxes: rects.length, cell: rects.length ? rects[0].right - rects[0].left : 0 };
    },
  };
}
