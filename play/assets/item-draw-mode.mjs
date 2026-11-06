// The draw mode retail's inventory-style item blit passes to the cel draw: an item carrying the
// ethereal flag (0x400000) is drawn with mode 1 (the PL2 AlphaBlend table, over whatever is under it),
// every other item with mode 5 (opaque). Every item grid (inventory, stash, cube, belt) goes through the
// same blit (and so does the worn-item slot), so they share this rule. Ground items go through the unit draw instead.
import { createBlender } from './frontend.mjs';
import { decodeDatPalette } from './d2-formats.mjs';

export const ETHEREAL_ITEM_FLAG = 0x400000;
export const ITEM_DRAW_MODE_NORMAL = 5;
export const ITEM_DRAW_MODE_ETHEREAL = 1;

export const itemDrawMode = (itemFlags) => ((itemFlags | 0) & ETHEREAL_ITEM_FLAG ? ITEM_DRAW_MODE_ETHEREAL : ITEM_DRAW_MODE_NORMAL);

// Per-act blender over that act's palette and PL2; null when the act has no PL2 (the item then stays opaque).
export function createItemBlendCache(read) {
  const byAct = new Map();
  return (act) => {
    const a = Math.min(5, Math.max(1, act | 0));
    if (!byAct.has(a)) {
      const base = `data\\global\\palette\\ACT${a}\\`;
      let blend = null;
      try { blend = createBlender(read(base + 'pal.pl2'), decodeDatPalette(read(base + 'pal.dat'))); } catch { blend = null; }
      byAct.set(a, blend);
    }
    return byAct.get(a);
  };
}

// Draw one item cel. `frame` is the decoded DC6 frame, `remap(index)` the item's colour remap (or null),
// `image` the already-rendered opaque canvas used for mode 5. Returns the mode used.
export function drawItemCel(ctx, image, frame, left, top, itemFlags, blend, remap = null) {
  const mode = itemDrawMode(itemFlags);
  if (mode === ITEM_DRAW_MODE_NORMAL || !blend) { ctx.drawImage(image, left, top); return ITEM_DRAW_MODE_NORMAL; }
  const indices = remap ? Uint8Array.from(frame.indices, remap) : frame.indices;
  blendThroughTransform(ctx, blend, { width: frame.width, height: frame.height, indices, opaque: frame.opaque }, left, top, mode);
  return mode;
}

// The blender reads and writes pixels with getImageData/putImageData, which ignore the context's
// transform, while every panel draws its items under translate(base) + scale(view). Blending at the
// raw (left, top) put an ethereal item's cel at the wrong device position: its own cell was left
// empty and a dark smear appeared elsewhere on the canvas. A pure integer translation moves the
// coordinates. Otherwise the blend runs at 800x600 resolution on a scratch canvas that holds what is
// under the cel, and the result is drawn back through the transform (only the cel's own pixels).
function blendThroughTransform(ctx, blend, cel, left, top, mode) {
  const m = typeof ctx.getTransform === 'function' ? ctx.getTransform() : null;
  if (!m) { blend(ctx, cel, left, top, mode); return; }
  const x0 = m.a * left + m.c * top + m.e, y0 = m.b * left + m.d * top + m.f;
  if (m.a === 1 && m.d === 1 && m.b === 0 && m.c === 0) { blend(ctx, cel, Math.round(x0), Math.round(y0), mode); return; }
  const doc = ctx.canvas?.ownerDocument ?? globalThis.document;
  const scratch = doc.createElement('canvas'); scratch.width = cel.width; scratch.height = cel.height;
  const s = scratch.getContext('2d'); s.imageSmoothingEnabled = false;
  s.drawImage(ctx.canvas, x0, y0, m.a * cel.width, m.d * cel.height, 0, 0, cel.width, cel.height);
  blend(s, cel, 0, 0, mode);
  const img = s.getImageData(0, 0, cel.width, cel.height);
  for (let k = 0; k < cel.opaque.length; k++) if (!cel.opaque[k]) img.data[k * 4 + 3] = 0;
  s.putImageData(img, 0, 0);
  ctx.drawImage(scratch, left, top);
}
