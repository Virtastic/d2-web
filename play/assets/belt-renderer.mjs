import { beltCelAt } from './item-cel-place.mjs';
import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { POPUP_BG } from './belt-popup.mjs';
import { createFont, decodeFontTbl, textColourPalette } from './frontend.mjs';
import { BELT_FONT } from './belt-draw-engine.mjs';

// Rasterises the belt's draw list. WHAT the belt draws and where (which popup frames, which potions, which hotkey labels,
// how a label is trimmed) is D2Client+0x28DC0 run by the engine (belt-draw-engine.mjs); this file turns those ops into pixels.
// The boxes themselves are Belts.txt through D2Common #10638, handed over by the engine.

function raster(frame, palette, documentObject) { const c = documentObject.createElement('canvas'); c.width = frame.width; c.height = frame.height;
  c.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0); return c; }

export function createBeltRenderer(read, documentObject = globalThis.document) {
  const palette = decodeDatPalette(read('data\\global\\palette\\SKY\\pal.dat')), cache = new Map();
  const icon = name => { let c = cache.get(name); if (!c) { const d = decodeDc6(read(`data\\global\\items\\${name}.dc6`)); c = raster(d.frames[0], palette, documentObject); cache.set(name, c); } return c; };
  let bg;
  const fonts = new Map();
  // The labels are Font16, colour shift = the op's colour (retail: `text '1' color=4`, gold).
  const font = (color) => {
    let f = fonts.get(color);
    if (!f) fonts.set(color, f = createFont({ dc6: decodeDc6(read(BELT_FONT + '.dc6')), tbl: decodeFontTbl(read(BELT_FONT + '.tbl')),
      palette: textColourPalette(read('data\\global\\palette\\SKY\\pal.pl2'), palette, color), documentObject, path: BELT_FONT + '.dc6', drawAs: { op: 'celcolor', mode: 5, extra: color } }));
    return f;
  };
  return {
    // ops: the engine's draw list; items: the belt items the 'item' ops index ({ invFile }); boxes: the record's boxes (evidence only).
    draw(ctx, ops, items, shown) {
      const boxes = shown ?? [];   // the shell passes null until the engine hands the boxes over
      let count = 0, pixels = 0;
      const bgCalls = [];
      for (const op of ops || []) {
        if (op.op === 'bg') {
          bgCalls.push({ x: op.x, y: op.y });
          bg ??= (() => { try { const d = decodeDc6(read(POPUP_BG + '.dc6')); return raster(d.frames[0], palette, documentObject); } catch { return null; } })();
          if (bg) ctx.drawImage(bg, op.x, op.y - bg.height);   // DC6 cels are placed by their bottom-left
        } else if (op.op === 'item') {
          const it = items?.[op.item];
          if (!it?.invFile) continue;
          const c = icon(it.invFile);
          // The cel is 28x28 in a 29x29 Belts.txt box and sits one row BELOW the box's top row (item-cel-place.mjs).
          const at = beltCelAt({ left: op.x, top: op.y });
          ctx.drawImage(c, at.left, at.top);
          count++; pixels += c.width * c.height;
        } else if (op.op === 'text') font(op.color).drawAnchored(ctx, op.text, op.x, op.y);
      }
      return { count, pixels, bg: bgCalls, boxes: boxes.length, cell: boxes.length ? boxes[0].right - boxes[0].left : 0 };
    },
  };
}
