// The pieces retail draws around the in-game side panels, from its own draw list and 8-bit frame
// buffer: the 800borderframe halves, the close button
// (buysellbtn frame 10), the gold coin button and Font16 text. Every cel is placed as retail anchors
// it: left edge x, BOTTOM row y (drawlist.mjs), through the act palette.
import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { createFont, decodeFontTbl } from './frontend.mjs';

export const CHROME_ART = {
  border: 'data\\global\\ui\\PANEL\\800BorderFrame.dc6',
  close: 'data\\global\\ui\\PANEL\\buysellbtn.dc6',
  gold: 'data\\global\\ui\\PANEL\\goldcoinbtn.dc6',
};
// 800borderframe: frames 0-4 frame the left panel, 5-9 the right one (retail's anchors).
export const BORDER_LEFT = [[0, 0, 253], [1, 256, 63], [2, 0, 484], [3, 0, 553], [4, 256, 553]];
export const BORDER_RIGHT = [[5, 400, 63], [6, 544, 253], [7, 713, 484], [8, 544, 553], [9, 400, 553]];

const actPalette = (act) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\pal.dat`;

export function createPanelChrome(read, documentObject = globalThis.document) {
  const files = {};
  const dc6 = (path) => (files[path] ??= decodeDc6(read(path)));
  const tbl = decodeFontTbl(read('data\\local\\font\\latin\\font16.tbl')), fontDc6 = decodeDc6(read('data\\local\\font\\latin\\font16.dc6'));
  const perAct = new Map();
  const forAct = (act) => {
    let a = perAct.get(act);
    if (!a) {
      const palette = decodeDatPalette(read(actPalette(act)));
      a = { palette, canvases: new Map(), font: createFont({ dc6: fontDc6, tbl, palette, documentObject }) };
      perAct.set(act, a);
    }
    return a;
  };
  const canvasOf = (a, path, i) => {
    const key = path + '#' + i;
    let c = a.canvases.get(key);
    if (!c) {
      const f = dc6(path).frames[i];
      c = documentObject.createElement('canvas'); c.width = Math.max(1, f.width); c.height = Math.max(1, f.height);
      if (f.width && f.height) c.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(f, a.palette), f.width, f.height), 0, 0);
      a.canvases.set(key, c);
    }
    return c;
  };
  return {
    /** Draw frame i of `path` anchored as retail does (left x, bottom row y). */
    cel(ctx, act, path, i, x, y) {
      const f = dc6(path).frames[i]; if (!f) return;
      ctx.drawImage(canvasOf(forAct(act), path, i), x + f.offsetX, y + f.offsetY - f.height + 1);
    },
    /** Font16 white text anchored at its bottom row. */
    text(ctx, act, s, x, y) { forAct(act).font.drawAnchored(ctx, s, x, y); },
    measure(act, s) { return forAct(act).font.measure(s); },
    border(ctx, act, pieces) { for (const [i, x, y] of pieces) this.cel(ctx, act, CHROME_ART.border, i, x, y); },
  };
}
