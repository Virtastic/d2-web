// The item tooltip retail draws under the pointer: the item's name, then the engine's property
// lines, in a black box 25 px above the cursor.
//
// The lines are NOT built here. `itemTooltipSpec` only places the fields the caller got from the
// engine (`d2_session_item_title` / `d2_session_item_lines`, surfaced by the shell's `tipFields`),
// which is the same source the shop tooltip uses.
import { createFont, decodeFontTbl, textColourPalette } from './frontend.mjs';
import { decodeDatPalette, decodeDc6 } from './d2-formats.mjs';

export const ITEM_TOOLTIP_ASSETS = {
  font: 'data\\local\\font\\latin\\font16.dc6',
  fontTbl: 'data\\local\\font\\latin\\font16.tbl',
};

// Retail's line order for an item no vendor is selling: the name, then the property lines. P8j's
// colour rule: a magic (quality 4) name is D2Win blue 3, otherwise white; the engine's own
// "\x01<digit>" markers colour each property line (1 red unmet requirement, 3 blue magical).
export function itemTooltipSpec(h) {
  const qc = h?.quality === 4 ? 3 : 0;
  return { lines: [h?.name || h?.code || '', ...(h?.props || [])].filter((line) => line !== ''),
    colours: [qc, ...(h?.propColours || [])] };
}

export function createItemTooltip(read, documentObject = globalThis.document) {
  const fontDc6 = decodeDc6(read(ITEM_TOOLTIP_ASSETS.font));
  const fontTbl = decodeFontTbl(read(ITEM_TOOLTIP_ASSETS.fontTbl));
  const actPath = (act, f) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\${f}`;
  const perAct = new Map();
  function forAct(act) {
    let a = perAct.get(act);
    if (!a) {
      const palette = decodeDatPalette(read(actPath(act, 'pal.dat'))), pl2 = read(actPath(act, 'pal.pl2'));
      a = { font: createFont({ dc6: fontDc6, tbl: fontTbl, palette, documentObject }),
        tint: (c) => (a.tints ||= new Map()).get(c) || (a.tints.set(c, createFont({ dc6: fontDc6, tbl: fontTbl,
          palette: textColourPalette(pl2, palette, c), documentObject })), a.tints.get(c)) };
      perAct.set(act, a);
    }
    return a;
  }
  return {
    // Draws in 800x600 screen coordinates, on top of the panel. Retail centres the box on the
    // pointer, clamps it to the screen and puts its bottom 25 px above the cursor; lines are
    // centred, 16 px apart, top down (the shop tooltip's captured layout, vendor-panel.mjs).
    draw(ctx, act, pointer, width, height, lines, colours) {
      if (!lines.length || !pointer) return null;
      const f = forAct(act);
      const w = Math.max(...lines.map((l) => f.font.measure(l))) + 8, h = lines.length * 16;
      const scale = Math.min(1, width / 800, height / 600), ox = (width - 800 * scale) / 2, oy = (height - 600 * scale) / 2;
      const bx = Math.max(0, Math.min(800 - w, Math.round(pointer.x / scale - ox / scale - w / 2)));
      const by = Math.max(0, Math.min(600 - h, Math.round((pointer.y - oy) / scale) - 25 - h));
      ctx.save(); ctx.translate(ox, oy); ctx.scale(scale, scale);
      ctx.fillStyle = 'rgba(0,0,0,0.65)'; ctx.fillRect(bx, by, w, h);
      lines.forEach((l, k) => {
        const x = bx + Math.floor((w - f.font.measure(l)) / 2), y = by + 13 + 16 * k, c = colours[k];
        if (c === 'v') { const cut = l.indexOf(': ') + 2;
          f.font.drawAnchored(ctx, l.slice(0, cut), x, y);
          f.tint(3).drawAnchored(ctx, l.slice(cut), x + f.font.measure(l.slice(0, cut)), y); }
        else (c ? f.tint(c) : f.font).drawAnchored(ctx, l, x, y);
      });
      ctx.restore();
      return { box: [bx, by, bx + w, by + h], lines, colours };
    },
  };
}
