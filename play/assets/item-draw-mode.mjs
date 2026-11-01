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
  blend(ctx, { width: frame.width, height: frame.height, indices, opaque: frame.opaque }, left, top, mode);
  return mode;
}
