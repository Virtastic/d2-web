// Retail's in-game Help screen (`H`, CfgHelp / retail's `ToggleHelpScreen`): a full-screen border
// with the panel's title, an eight-line bulleted list of the game's own key callouts, and a set of
// labelled leader lines pointing at the HUD controls each one describes.
//
// Every input here is retail's OWN data, named by the asset, never invented:
//
//   * the art: `data\global\ui\MENU\800helpborder.dc6` (7 frames), `helpyellowbullet.dc6` and
//     `helpwhitebullet.dc6` (one 8x8 frame each) -- all three listed in `retail/listfiles/d2.lst`.
//   * the text: the engine's string table (`d2_lang_string`), keys `Strhelp1`, `StrHelp2`..`StrHelp22`,
//     `strClose`, `strlvlup`, `strnewskl` -- all in `retail/lang/string.tbl`. The four keys whose text
//     carries a `%s` substitute the ENGINE's own name for the bound key (`KeyControl` -> "Ctrl",
//     `KeyMenu` -> "Alt", `KeyShift` -> "Shift", `KeyTab` -> "Tab"), so even the hotkey word in the
//     sentence is retail's string, not a literal here.
//   * the layout: RETAIL'S OWN draw list of this screen (the retail captures, captured from the running
//     1.13c game): every anchor below is a record of that frame. OpenDiablo2's help_overlay.go was the
//     first cut of it and is refuted where the two differ (its border strip 3 sat at 280, retail's is 322; its
//     bullets and labels were centred, retail's are left-anchored at other columns).
//   * the palette: `PaletteSky` in that file, `data\global\palette\SKY\pal.dat` -- the palette our
//     proven in-game panels use (inventory-panel.mjs, character-panel.mjs, skill-select.mjs).
//     Measured: the border art draws IDENTICALLY under SKY and ACT1 (0 differing bytes over all
//     7 frames), so pixels do not settle this one; the prior art and the sibling panels do.
//
// Records go to the draw list (drawlist.mjs) per blit, with the frame's own bytes hashed, so a row
// can compare the cel DRAWN with the cel the table names instead of counting blits.
import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { createFont, decodeFontTbl } from './frontend.mjs';
import { recordDc6, recordPrim } from './drawlist.mjs';

export const HELP_OVERLAY_ART = Object.freeze({
  palette: 'data\\global\\palette\\SKY\\pal.dat',
  border: 'data\\global\\ui\\MENU\\800helpborder.dc6',
  yellowBullet: 'data\\global\\ui\\MENU\\helpyellowbullet.dc6',
  whiteBullet: 'data\\global\\ui\\MENU\\helpwhitebullet.dc6',
  closeButton: 'data\\global\\ui\\panel\\buysellbtn.dc6',
  font16: { dc6: 'data\\local\\font\\latin\\font16.dc6', tbl: 'data\\local\\font\\latin\\font16.tbl' },
  formal12: { dc6: 'data\\local\\font\\latin\\fontformal12.dc6', tbl: 'data\\local\\font\\latin\\fontformal12.tbl' },
});

// The border is seven frames of one file at retail's own cel anchors (left x, BOTTOM row y).
export const HELP_BORDER_ANCHORS = Object.freeze([[0, 256], [0, 512], [66, 20], [322, 20], [535, 256], [780, 256], [780, 512]]);

/** `[{ frame, left, top, width, height, anchorX, anchorY }]` from the frame SIZES the file carries. */
export function helpBorderPlacements(frames) {
  if (!Array.isArray(frames) || frames.length < HELP_BORDER_ANCHORS.length) throw new RangeError('help border needs 7 frames');
  return HELP_BORDER_ANCHORS.map(([anchorX, anchorY], frame) => {
    const f = frames[frame];
    if (!f || !Number.isInteger(f.width) || !Number.isInteger(f.height)) throw new RangeError(`help border frame ${frame} has no size`);
    return { frame, left: anchorX, top: anchorY - f.height + 1, width: f.width, height: f.height, anchorX, anchorY };
  });
}

// Retail's anchors: every label is (left x, BOTTOM row y) of its first glyph; the Close box is the panel's buysellbtn
// frame 10 at its own anchor.
export const HELP_LAYOUT = Object.freeze({
  title: { x: 328, y: 17, font: 'font16' },
  close: { labelX: 681, labelY: 73, font: 'font16', button: { x: 685, y: 25, w: 32, h: 32 }, cel: { frame: 10, x: 685, y: 56 } },
  listTextX: 117, listTextY: 78, listBulletX: 104, listBulletY: 73, listItemStep: 20,
  dim: [0, 0, 800, 551],                                // box, colour 0, mode 1 (50%), drawn before the HUD chrome
  white: 255,                                           // the leader lines' colour index
});

/** The bulleted list, in retail's order: label key, then the key the `%s` stands for. */
export const HELP_BULLETS = Object.freeze([
  { key: 'StrHelp2', sub: 'KeyControl' },
  { key: 'StrHelp3', sub: 'KeyMenu' },
  { key: 'StrHelp4', sub: 'KeyShift' },
  { key: 'StrHelp5', sub: 'KeyTab' },
  { key: 'StrHelp6' },
  { key: 'StrHelp7' },
  { key: 'StrHelp8' },
  { key: 'StrHelp8a', sub: 'KeyH' },
]);

// `strlvlup`/`strnewskl` carry no `%s`. `KeyH` has no string-table entry (a single letter is a
// literal in retail's own control mapping), so its substitution is the letter itself.
export const HELP_SUBSTITUTIONS = Object.freeze({
  StrHelp2: { string: 'KeyControl' }, StrHelp3: { string: 'KeyMenu' },
  StrHelp4: { string: 'KeyShift' }, StrHelp5: { string: 'KeyTab' }, StrHelp8a: { literal: 'H' },
});

/** The white dots: retail's bullet cel anchor and the 2 px leader line above it, in draw order. */
export const HELP_DOTS = Object.freeze([
  { x: 55, y: 535, lineX: 58, y0: 486, y1: 532 }, { x: 130, y: 564, lineX: 133, y0: 445, y1: 561 },
  { x: 217, y: 575, lineX: 220, y0: 375, y1: 572 }, { x: 260, y: 585, lineX: 263, y0: 526, y1: 582 },
  { x: 310, y: 585, lineX: 313, y0: 486, y1: 582 }, { x: 365, y: 565, lineX: 368, y0: 525, y1: 562 },
  { x: 445, y: 540, lineX: 448, y0: 451, y1: 537 }, { x: 530, y: 569, lineX: 533, y0: 525, y1: 566 },
  { x: 572, y: 575, lineX: 575, y0: 375, y1: 572 }, { x: 670, y: 564, lineX: 673, y0: 445, y1: 561 },
  { x: 740, y: 535, lineX: 743, y0: 486, y1: 532 },
]);

/** The callout labels in retail's draw order, each at (left x, bottom row y). */
export const HELP_CALLOUTS = Object.freeze([
  { key: 'StrHelp9', x: 25, y: 481 }, { key: 'StrHelp10', x: 84, y: 410 }, { key: 'StrHelp11', x: 85, y: 425 }, { key: 'StrHelp12', x: 61, y: 440 },
  { key: 'strlvlup', x: 179, y: 375 }, { key: 'StrHelp14', x: 224, y: 506 }, { key: 'StrHelp14a', x: 236, y: 521 }, { key: 'StrHelp15', x: 261, y: 481 },
  { key: 'StrHelp16', x: 322, y: 505 }, { key: 'StrHelp16a', x: 354, y: 520 }, { key: 'StrHelp17', x: 405, y: 401 }, { key: 'StrHelp18', x: 372, y: 416 },
  { key: 'StrHelp19', x: 386, y: 431 }, { key: 'StrHelp20', x: 388, y: 446 }, { key: 'StrHelp21', x: 518, y: 520 }, { key: 'strnewskl', x: 534, y: 375 },
  { key: 'StrHelp13', x: 619, y: 410 }, { key: 'StrHelp11', x: 625, y: 425 }, { key: 'StrHelp12', x: 601, y: 440 }, { key: 'StrHelp22', x: 701, y: 481 },
]);

/** The string this screen draws for `key`: retail's own text with its `%s` filled from retail's own
 *  name string for the bound key. Throws when the engine has no string -- a label that silently
 *  draws nothing is the failure this screen must not have. */
export function helpText(key, string, substitutions = HELP_SUBSTITUTIONS) {
  const raw = string(key);
  if (!raw) throw new Error(`help: no string table entry for ${key}`);
  const sub = substitutions[key];
  if (!sub) return raw;
  const value = sub.string ? string(sub.string) : sub.literal;
  if (!value) throw new Error(`help: no substitution for ${key}`);
  return raw.replace('%s', value);
}

function frameCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, frame.width); canvas.height = Math.max(1, frame.height);
  if (frame.width && frame.height) {
    canvas.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0);
  }
  return canvas;
}

export function createHelpOverlayRenderer(read, string, documentObject = globalThis.document) {
  if (typeof read !== 'function' || typeof string !== 'function') throw new TypeError('help overlay needs a reader and a string lookup');
  const palette = decodeDatPalette(read(HELP_OVERLAY_ART.palette));
  const borderFrames = decodeDc6(read(HELP_OVERLAY_ART.border)).frames;
  const borderCanvases = borderFrames.map((f) => frameCanvas(f, palette, documentObject));
  const placements = helpBorderPlacements(borderFrames);
  const bullet = (path) => {
    const frames = decodeDc6(read(path)).frames, cel = frames[0];
    return { path, cel: 0, frame: cel, canvas: frameCanvas(cel, palette, documentObject) };
  };
  const bullets = { yellow: bullet(HELP_OVERLAY_ART.yellowBullet), white: bullet(HELP_OVERLAY_ART.whiteBullet) };
  const font = (spec) => createFont({ dc6: decodeDc6(read(spec.dc6)), tbl: decodeFontTbl(read(spec.tbl)), palette, documentObject,
    path: spec.dc6, drawAs: { op: 'celcolor', mode: 5 } });
  const fonts = { font16: font(HELP_OVERLAY_ART.font16), formal12: font(HELP_OVERLAY_ART.formal12) };
  const origin = (width, height) => { const scale = Math.min(1, width / 800, height / 600); return { scale, x: (width - 800 * scale) / 2, y: (height - 600 * scale) / 2 }; };

  const closeDc6 = decodeDc6(read(HELP_OVERLAY_ART.closeButton)).frames[HELP_LAYOUT.close.cel.frame];
  const closeCanvas = frameCanvas(closeDc6, palette, documentObject);
  // Every label on the screen, in retail's draw order: the title and Close, the bulleted list, then the callouts.
  const labelPlan = [
    { key: 'Strhelp1', ...HELP_LAYOUT.title, where: 'title' },
    { key: 'strClose', x: HELP_LAYOUT.close.labelX, y: HELP_LAYOUT.close.labelY, font: 'font16', where: 'close' },
    ...HELP_BULLETS.map((b, i) => ({ key: b.key, x: HELP_LAYOUT.listTextX, y: HELP_LAYOUT.listTextY + i * HELP_LAYOUT.listItemStep, font: 'formal12', where: 'bullet', index: i })),
    ...HELP_CALLOUTS.map((c, i) => ({ key: c.key, x: c.x, y: c.y, font: 'formal12', where: 'callout', order: i + 1 })),
  ];

  return {
    layout: HELP_LAYOUT,
    art: HELP_OVERLAY_ART,
    borderPlacements: placements,
    /** Screen (800x600) coordinates for a canvas point, the inverse of the draw transform. */
    toScreen(cx, cy, width, height) { const o = origin(width, height); return [(cx - o.x) / o.scale, (cy - o.y) / o.scale]; },
    /** Retail's Close box: the panel's 32x32 buysellbtn cel (frame 10) drawn at (685, 56) by its bottom row. */
    hitClose(cx, cy, width, height) {
      const [x, y] = this.toScreen(cx, cy, width, height), b = HELP_LAYOUT.close.button;
      return x >= b.x && x < b.x + b.w && y >= b.y && y < b.y + b.h;
    },
    /** Retail dims the world (box 0,0,800,551, mode 1) BEFORE it draws the HUD chrome, so the chrome stays bright. */
    dim(ctx, width, height) {
      const o = origin(width, height), [x1, y1, x2, y2] = HELP_LAYOUT.dim;
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      ctx.fillStyle = 'rgba(0,0,0,0.5)'; ctx.fillRect(x1, y1, x2 - x1, y2 - y1);
      recordPrim('box', x1, y1, x2, y2, 0, 1);
      ctx.restore();
    },
    draw(ctx, width, height) {
      const o = origin(width, height);
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      const evidence = { palette: HELP_OVERLAY_ART.palette, border: [], bullets: [], labels: [], lines: [] };
      for (const p of placements) {
        ctx.drawImage(borderCanvases[p.frame], p.left, p.top);
        evidence.border.push({ path: HELP_OVERLAY_ART.border, cel: p.frame, left: p.left, top: p.top, width: p.width, height: p.height,
          blit: recordDc6(HELP_OVERLAY_ART.border, p.frame, borderFrames[p.frame], p.left, p.top) });
      }
      const cl = HELP_LAYOUT.close.cel, closeTop = cl.y - closeDc6.height + 1;
      ctx.drawImage(closeCanvas, cl.x, closeTop);
      evidence.close = { blit: recordDc6(HELP_OVERLAY_ART.closeButton, cl.frame, closeDc6, cl.x, closeTop) };
      // A bullet's anchor is its BOTTOM row (drawlist.mjs), as everything retail draws.
      const dot = (kind, x, y, where) => {
        const b = bullets[kind], top = y - b.frame.height + 1;
        ctx.drawImage(b.canvas, x, top);
        evidence.bullets.push({ kind, where, path: b.path, cel: b.cel, left: x, top, blit: recordDc6(b.path, b.cel, b.frame, x, top) });
      };
      HELP_BULLETS.forEach((_, i) => dot('yellow', HELP_LAYOUT.listBulletX, HELP_LAYOUT.listBulletY + i * HELP_LAYOUT.listItemStep, `list${i}`));
      // Each white dot is followed by its leader line: two 1 px columns, colour index 255 of the screen's palette.
      ctx.fillStyle = '#ffffff';
      HELP_DOTS.forEach((d, i) => {
        dot('white', d.x, d.y, `dot${i + 1}`);
        ctx.fillRect(d.lineX, d.y0, 2, d.y1 - d.y0 + 1);
        for (const x of [d.lineX, d.lineX + 1]) recordPrim('line', x, d.y0, x, d.y1, HELP_LAYOUT.white, 255);
        evidence.lines.push({ x: d.lineX, y0: d.y0, y1: d.y1 });
      });
      const glyphsOf = (f, text) => [...String(text)].map((ch) => f.glyph(ch.codePointAt(0))?.index ?? -1);
      for (const plan of labelPlan) {
        const f = fonts[plan.font], text = helpText(plan.key, string);
        const width_ = f.drawAnchored(ctx, text, plan.x, plan.y);
        evidence.labels.push({ key: plan.key, text, where: plan.where, index: plan.index ?? null, order: plan.order ?? null,
          font: plan.font, fontPath: HELP_OVERLAY_ART[plan.font].dc6, x: plan.x, y: plan.y, anchorX: plan.x, anchorY: plan.y,
          width: width_, lineHeight: f.lineHeight, glyphs: glyphsOf(f, text) });
      }
      ctx.restore();
      return evidence;
    },
  };
}
