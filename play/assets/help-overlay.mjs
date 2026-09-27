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
//   * the layout: prior art, `reference/OpenDiablo2/d2game/d2player/help_overlay.go` (its
//     `setupOverlayFrame`, `setupTitleAndButton`, `setupBulletedList`, `setupLabelsWithLines`). A
//     layout is prior art too -- the constants below are that file's, not measured off pixels.
//   * the palette: `PaletteSky` in that file, `data\global\palette\SKY\pal.dat` -- the palette our
//     proven in-game panels use (inventory-panel.mjs, character-panel.mjs, skill-select.mjs).
//     Measured: the border art draws IDENTICALLY under SKY and ACT1 (0 differing bytes over all
//     7 frames), so pixels do not settle this one; the prior art and the sibling panels do.
//
// Records go to the draw list (drawlist.mjs) per blit, with the frame's own bytes hashed, so a row
// can compare the cel DRAWN with the cel the table names instead of counting blits.
import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { createFont, decodeFontTbl } from './frontend.mjs';
import { recordDc6 } from './drawlist.mjs';

export const HELP_OVERLAY_ART = Object.freeze({
  palette: 'data\\global\\palette\\SKY\\pal.dat',
  border: 'data\\global\\ui\\MENU\\800helpborder.dc6',
  yellowBullet: 'data\\global\\ui\\MENU\\helpyellowbullet.dc6',
  whiteBullet: 'data\\global\\ui\\MENU\\helpwhitebullet.dc6',
  font16: { dc6: 'data\\local\\font\\latin\\font16.dc6', tbl: 'data\\local\\font\\latin\\font16.tbl' },
  formal12: { dc6: 'data\\local\\font\\latin\\fontformal12.dc6', tbl: 'data\\local\\font\\latin\\fontformal12.tbl' },
});

// The border is assembled from seven frames of one file, and the frame order is the prior art's:
// top-left corner, the strip below it, the two top strips, the wide top-right piece, the right
// strip, and the strip below that. `magicHelpBorderOffsetX` is the -40 the prior art applies because
// frames 1,3,4,5,6 are 840 px wide together and the screen is 800.
const BORDER_FRAME_ORDER = [0, 1, 2, 3, 4, 5, 6];
export const HELP_BORDER_OFFSET_X = -40;

/**
 * The port of `setupOverlayFrame`: the top-left corner each border frame is drawn at, from the frame
 * SIZES the file actually carries (so a caller can recompute it from the DC6 it decoded itself).
 * Returns `[{ frame, left, top, width, height, anchorX, anchorY }]`, `anchorX/anchorY` being retail's
 * own cel anchor (drawlist.mjs: left edge at x + offsetX, BOTTOM row at y + offsetY).
 */
export function helpBorderPlacements(frames) {
  if (!Array.isArray(frames) || frames.length < BORDER_FRAME_ORDER.length) throw new RangeError('help border needs 7 frames');
  const placed = [];
  let currentX = 0, currentY = 0, prevY = 0, prevWidth = 0, firstFrameWidth = 0;
  for (const frameIndex of BORDER_FRAME_ORDER) {
    const f = frames[frameIndex];
    if (!f || !Number.isInteger(f.width) || !Number.isInteger(f.height)) throw new RangeError(`help border frame ${frameIndex} has no size`);
    const { width: frameWidth, height: frameHeight } = f;
    switch (frameIndex) {
      case 0: currentY += frameHeight; firstFrameWidth = frameWidth; break;   // top-left
      case 1: currentY += frameHeight; break;                                  // below it
      case 2: currentX = firstFrameWidth; currentY = frameHeight; break;
      case 3: currentY = frameHeight; currentX += prevWidth; currentX += HELP_BORDER_OFFSET_X; break;
      case 4: currentY = frameHeight; currentX += prevWidth; break;
      case 5: currentY = frameHeight; currentX += prevWidth; break;
      case 6: currentY = prevY + frameHeight; break;                           // bottom-right
    }
    prevY = currentY; prevWidth = frameWidth;
    // OpenDiablo2's `y` is the sprite's BOTTOM-LEFT corner (its Render translates to y - height), so
    // the drawn top row is `currentY - frameHeight` and retail's own cel anchor -- the bottom row
    // INCLUSIVE, drawlist.mjs -- is that row plus height - 1.
    placed.push({ frame: frameIndex, left: currentX, top: currentY - frameHeight, width: frameWidth, height: frameHeight,
      anchorX: currentX, anchorY: currentY - 1 });
  }
  return placed;
}

// The prior art's constants, verbatim (help_overlay.go). `x` is the label's horizontal anchor --
// every label on this screen except the title is centred on it (`HorizontalAlignCenter`); `y` is the
// label's TOP row, which is the semantics `GetTextMetrics`' height is used with when the callout's
// leader line starts at `LabelY + hh + lineOffset`.
export const HELP_LAYOUT = Object.freeze({
  title: { x: 363, y: 0, font: 'font16' },              // 800/2 - 37; the label is centred on it
  close: { labelX: 702, labelY: 60, font: 'font16', button: { x: 685, y: 25, w: 20, h: 20 } },
  listRootX: 100, listRootY: 59, listItemStep: 20, listBulletX: 88, listBulletY: 83,
  lineOffset: 5,                                        // between a label's last row and its line
  white: 255,                                           // `color.White` in the SKY palette
});

/** The bulleted list, in the prior art's order: label key, then the key the `%s` stands for. */
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

/**
 * The labelled leader lines (`createCallout`) and the plain labels (`createLabel`), both in the
 * prior art's order. A callout draws its label centred at (x, y), a 1 px vertical white line from
 * `y + textHeight + lineOffset` down to `dotY`, and a white bullet sprite whose BOTTOM row is `dotY`.
 */
export const HELP_CALLOUTS = Object.freeze([
  { key: 'strlvlup', x: 222, y: 355, dotX: 217, dotY: 574, line: true, order: 1 },
  { key: 'strnewskl', x: 578, y: 355, dotX: 573, dotY: 574, line: true, order: 2 },
  { key: 'StrHelp10', x: 135, y: 382, line: false, order: 3 },
  { key: 'StrHelp11', x: 135, y: 397, line: false, order: 4 },
  { key: 'StrHelp12', x: 135, y: 412, dotX: 130, dotY: 565, line: true, order: 5 },
  { key: 'StrHelp13', x: 675, y: 381, line: false, order: 6 },
  { key: 'StrHelp11', x: 675, y: 396, line: false, order: 7 },
  { key: 'StrHelp12', x: 675, y: 411, dotX: 670, dotY: 562, line: true, order: 8 },
  { key: 'StrHelp17', x: 450, y: 371, line: false, order: 9 },
  { key: 'StrHelp18', x: 450, y: 386, line: false, order: 10 },
  { key: 'StrHelp19', x: 450, y: 401, line: false, order: 11 },
  { key: 'StrHelp20', x: 450, y: 417, dotX: 445, dotY: 539, line: true, order: 12 },
  { key: 'StrHelp9', x: 65, y: 451, dotX: 60, dotY: 538, line: true, order: 13 },
  { key: 'StrHelp15', x: 315, y: 450, dotX: 310, dotY: 583, line: true, order: 14 },
  { key: 'StrHelp22', x: 745, y: 451, dotX: 740, dotY: 538, line: true, order: 15 },
  { key: 'StrHelp14', x: 264, y: 480, line: false, order: 16 },
  { key: 'StrHelp14a', x: 264, y: 495, dotX: 259, dotY: 583, line: true, order: 17 },
  { key: 'StrHelp16', x: 370, y: 476, line: false, order: 18 },
  { key: 'StrHelp16a', x: 370, y: 493, dotX: 365, dotY: 565, line: true, order: 19 },
  { key: 'StrHelp21', x: 535, y: 490, dotX: 530, dotY: 568, line: true, order: 20 },
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

  // Every label on the screen, in draw order: the title and Close first, then the bulleted list,
  // then the callouts and plain labels -- the prior art's own construction order.
  const labelPlan = [
    { key: 'Strhelp1', ...HELP_LAYOUT.title, align: 'center', where: 'title' },
    { key: 'strClose', x: HELP_LAYOUT.close.labelX, y: HELP_LAYOUT.close.labelY, font: 'font16', align: 'center', where: 'close' },
    ...HELP_BULLETS.map((b, i) => ({ key: b.key, x: HELP_LAYOUT.listRootX, y: HELP_LAYOUT.listRootY + i * HELP_LAYOUT.listItemStep,
      font: 'formal12', align: 'left', where: 'bullet', index: i })),
    ...HELP_CALLOUTS.map((c) => ({ key: c.key, x: c.x, y: c.y, font: 'formal12', align: 'center', where: c.line ? 'callout' : 'label', order: c.order })),
  ];

  return {
    layout: HELP_LAYOUT,
    art: HELP_OVERLAY_ART,
    borderPlacements: placements,
    /** Screen (800x600) coordinates for a canvas point, the inverse of the draw transform. */
    toScreen(cx, cy, width, height) { const o = origin(width, height); return [(cx - o.x) / o.scale, (cy - o.y) / o.scale]; },
    /**
   * Retail's Close box: the button the prior art places (`closeButtonX/Y` = 685,25), 20x20
   * square-close. The border art's OWN black square is a few pixels up and left of it (682..702 x
   * 21..41, read off frame 4), so the clickable rect is the prior art's button and the drawn square
   * sits inside it -- clicking the square's centre hits, clicking its outermost 3 px rim does not.
   * Declared in F3c rather than widened to a union of the two.
   */
    hitClose(cx, cy, width, height) {
      const [x, y] = this.toScreen(cx, cy, width, height), b = HELP_LAYOUT.close.button;
      return x >= b.x && x < b.x + b.w && y >= b.y && y < b.y + b.h;
    },
    draw(ctx, width, height) {
      const o = origin(width, height);
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      const evidence = { palette: HELP_OVERLAY_ART.palette, border: [], bullets: [], labels: [], lines: [] };
      // 1. the border frames (prior-art order), each recorded as a cel draw of its own file.
      for (const p of placements) {
        ctx.drawImage(borderCanvases[p.frame], p.left, p.top);
        evidence.border.push({ path: HELP_OVERLAY_ART.border, cel: p.frame, left: p.left, top: p.top, width: p.width, height: p.height,
          blit: recordDc6(HELP_OVERLAY_ART.border, p.frame, borderFrames[p.frame], p.left, p.top) });
      }
      // 2. the bullet sprites: eight yellow beside the list, one white at each callout's dot. The
      //    sprite's own anchor is its BOTTOM row (OpenDiablo2's Sprite.Render), so top = dotY - height.
      const dot = (kind, x, y, where) => {
        const b = bullets[kind], top = y - b.frame.height;
        ctx.drawImage(b.canvas, x, top);
        evidence.bullets.push({ kind, where, path: b.path, cel: b.cel, left: x, top, blit: recordDc6(b.path, b.cel, b.frame, x, top) });
      };
      HELP_BULLETS.forEach((_, i) => dot('yellow', HELP_LAYOUT.listBulletX, HELP_LAYOUT.listBulletY + i * HELP_LAYOUT.listItemStep, `list${i}`));
      for (const c of HELP_CALLOUTS) if (c.line) dot('white', c.dotX, c.dotY, `callout${c.order}`);
      // 3. the labels. `x` is the label's own anchor (centred, except the bulleted list which the
      //    prior art leaves left-aligned); the glyphs are drawn with their BOTTOM row at
      //    `y + lineHeight - 1`, which puts the text block's first row at `y`.
      const glyphsOf = (f, text) => [...String(text)].map((ch) => f.glyph(ch.codePointAt(0))?.index ?? -1);
      for (const plan of labelPlan) {
        const f = fonts[plan.font], text = helpText(plan.key, string);
        const anchorX = plan.align === 'center' ? Math.round(plan.x - f.measure(text) / 2) : Math.round(plan.x);
        const anchorY = plan.y + f.lineHeight - 1;
        const width_ = f.drawAnchored(ctx, text, anchorX, anchorY);
        evidence.labels.push({ key: plan.key, text, where: plan.where, index: plan.index ?? null, order: plan.order ?? null,
          font: plan.font, fontPath: HELP_OVERLAY_ART[plan.font].dc6, x: plan.x, y: plan.y, align: plan.align, anchorX, anchorY,
          width: width_, lineHeight: f.lineHeight, glyphs: glyphsOf(f, text) });
      }
      // 4. the leader lines, last, as the prior art renders them: from under the label's text down
      //    to the dot. `color.White` is index 255 of the screen's palette.
      ctx.fillStyle = '#ffffff';
      for (const c of HELP_CALLOUTS) {
        if (!c.line) continue;
        const f = fonts.formal12, hh = f.lineHeight, y0 = c.y + hh + HELP_LAYOUT.lineOffset, length = c.dotY - y0;
        ctx.fillRect(Math.round(c.x), Math.round(y0), 1, Math.round(length));
        evidence.lines.push({ key: c.key, x: Math.round(c.x), y0: Math.round(y0), y1: Math.round(c.dotY), length: Math.round(length), colour: HELP_LAYOUT.white });
      }
      ctx.restore();
      return evidence;
    },
  };
}
