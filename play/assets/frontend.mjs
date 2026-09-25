// The retail front end: the title screen a player actually lands on.
//
// Until now `play/` opened straight into a canvas with a developer dropdown above it. There was no
// way INTO the game the way retail has one, which is the first thing anyone notices. Everything
// here is composed from the retail MPQs -- no drawn-by-hand chrome, no CSS pretending to be Diablo.
//
// Prior art used: riiablo `core/src/main/java/com/riiablo/codec/FontTBL.java`
// for the font .tbl record layout (12-byte header, then 256 x 14-byte records). The advance width
// is byte 3 and the DC6 frame index is byte 8 -- the DC6's own frame boxes are a fixed cell (every
// glyph in font30 reports 29x30) and are NOT the advance, so text laid out from the DC6 alone
// comes out monospaced and wrong.

import { recordDc6 } from './drawlist.mjs';

/** 12-byte header, then one 14-byte record per code point. Layout per riiablo's FontTBL. */
export function decodeFontTbl(bytes) {
  if (!(bytes instanceof Uint8Array) || bytes.length < 12) throw new TypeError('font tbl too short');
  const magic = String.fromCharCode(bytes[0], bytes[1], bytes[2], bytes[3]);
  if (magic !== 'Woo!') throw new Error(`font tbl magic ${JSON.stringify(magic)}, expected "Woo!"`);
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  const header = { height: bytes[10], width: bytes[11] };
  const count = Math.floor((bytes.length - 12) / 14);
  const chars = new Map();
  for (let i = 0; i < count; i++) {
    const b = 12 + i * 14;
    chars.set(view.getUint16(b, true), {
      width: bytes[b + 3],        // advance, NOT the DC6 frame width
      height: bytes[b + 4],
      imageIndex: bytes[b + 8],   // which DC6 frame draws this code point
    });
  }
  return { header, chars, count };
}

/**
 * Retail's blended cel draw modes, index-exact (retail's own 8-bit title frame): mode 3
 * writes PL2 AdditiveBlend[under][src], mode 4 (the button labels) MultiplicativeBlend[under][src],
 * the tables of the screen's palette (Sky on the title: 99.96% and 100% of retail's pixels). `under`
 * is read back from the canvas as a palette index, so the canvas must be unscaled 800x600 and hold
 * raw palette colours. Offsets per OpenDiablo2's d2pl2 layout.
 */
export const PL2_ADDITIVE = 1024 + 32 * 256 + 16 * 256 + 256 + 3 * 65536;
export const PL2_MULTIPLICATIVE = PL2_ADDITIVE + 65536;
/** Offset of PL2 TextColorShifts (13 x 256; OpenDiablo2 d2pl2 layout). */
export const PL2_TEXT_SHIFTS = 1024 + 8192 + 4096 + 256 + 5 * 65536 + 111 * 256 + 768 + 14 * 256 + 65536 + 256 + 39;
/** A palette whose entry k is palette[TextColorShifts[colour][k]]: a glyph drawn in D2Win colour `colour`. */
export function textColourPalette(pl2, palette, colour) {
  const out = palette.slice();
  for (let k = 0; k < 256; k++) { const j = pl2[PL2_TEXT_SHIFTS + colour * 256 + k]; out.set(palette.subarray(j * 4, j * 4 + 3), k * 4); }
  return out;
}
export function createBlender(pl2, palette) {
  const index = new Map();
  for (let j = 255; j >= 0; j--) index.set((palette[j * 4] << 16) | (palette[j * 4 + 1] << 8) | palette[j * 4 + 2], j);
  const tables = { 3: pl2.subarray(PL2_ADDITIVE, PL2_ADDITIVE + 65536), 4: pl2.subarray(PL2_MULTIPLICATIVE, PL2_MULTIPLICATIVE + 65536) };
  return (ctx, frame, left, top, mode) => {
    const table = tables[mode];
    if (!frame.width || !frame.height) return;
    const img = ctx.getImageData(left, top, frame.width, frame.height), d = img.data;
    for (let k = 0; k < frame.indices.length; k++) {
      if (!frame.opaque[k]) continue;
      const under = index.get((d[k * 4] << 16) | (d[k * 4 + 1] << 8) | d[k * 4 + 2]);
      const c = under === undefined ? frame.indices[k] : table[under * 256 + frame.indices[k]];
      d[k * 4] = palette[c * 4]; d[k * 4 + 1] = palette[c * 4 + 1]; d[k * 4 + 2] = palette[c * 4 + 2]; d[k * 4 + 3] = 255;
    }
    ctx.putImageData(img, left, top);
  };
}

/** Paint one palette-indexed DC6 frame into a canvas. Transparent pixels stay transparent. */
function frameCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, frame.width);
  canvas.height = Math.max(1, frame.height);
  if (!frame.width || !frame.height) return canvas;
  const rgba = new Uint8ClampedArray(frame.width * frame.height * 4);
  for (let i = 0; i < frame.indices.length; i++) {
    if (!frame.opaque[i]) continue;
    const s = frame.indices[i] * 4;
    rgba.set(palette.subarray(s, s + 4), i * 4);
  }
  canvas.getContext('2d').putImageData(new ImageData(rgba, frame.width, frame.height), 0, 0);
  return canvas;
}

/**
 * A retail bitmap font: the DC6 supplies the glyph images, the .tbl the advance widths and the
 * code-point -> frame mapping.
 */
export function createFont({ dc6, tbl, palette, documentObject = globalThis.document, path = null, drawAs = { op: 'celcolor', mode: 5 } }) {
  const glyphs = new Map();
  const glyphFor = (code) => {
    if (glyphs.has(code)) return glyphs.get(code);
    const meta = tbl.chars.get(code);
    const frame = meta && dc6.frames[meta.imageIndex];
    const glyph = frame ? { canvas: frameCanvas(frame, palette, documentObject), advance: meta.width, height: meta.height, frame, index: meta.imageIndex } : null;
    glyphs.set(code, glyph);
    return glyph;
  };
  return {
    lineHeight: tbl.header.height,
    // Every glyph frame is the same fixed cell; retail centres THAT box in a button, not the .tbl
    // line height (the retail title frame puts exocet10's ink 3px higher than line-height centring).
    cellHeight: Math.max(...dc6.frames.map((f) => f.height)),
    measure(text) {
      let w = 0;
      for (const ch of String(text)) w += glyphFor(ch.codePointAt(0))?.advance ?? 0;
      return w;
    },
    /**
     * Draw `text` as D2Win does: glyph by glyph from x (whole pixels, the .tbl advances), each glyph
     * a cel drawn with its BOTTOM row at `anchorY` (inclusive).
     */
    drawAnchored(ctx, text, x, anchorY) {
      let cursor = Math.trunc(x);
      for (const ch of String(text)) {
        const g = glyphFor(ch.codePointAt(0));
        if (!g) continue;
        if (g.canvas.width && g.canvas.height) {
          const top = anchorY + g.frame.offsetY - g.frame.height + 1;
          if (drawAs.blend) drawAs.blend(ctx, g.frame, cursor + g.frame.offsetX, top, drawAs.mode);
          else ctx.drawImage(g.canvas, cursor + g.frame.offsetX, top);
          if (path) recordDc6(path, g.index, g.frame, cursor + g.frame.offsetX, top, drawAs);
        }
        cursor += g.advance;
      }
      return cursor - x;
    },
    /** Draw `text` with its LEFT edge at x and its TOP at y. */
    draw(ctx, text, x, y) {
      let cursor = x;
      for (const ch of String(text)) {
        const g = glyphFor(ch.codePointAt(0));
        if (!g) continue;
        if (g.canvas.width && g.canvas.height) {
          ctx.drawImage(g.canvas, Math.round(cursor), Math.round(y));
          // A glyph is a cel draw of the font's DC6 frame (drawlist.mjs); `path` names the font.
          if (path) recordDc6(path, g.index, g.frame, Math.round(cursor), Math.round(y), drawAs);
        }
        cursor += g.advance;
      }
      return cursor - x;
    },
  };
}

// Retail paths. Kept together so it is obvious this screen is retail data end to end. These are the
// Lord of Destruction front end's (1.13c boots to it): the EXP background under the Sky palette,
// exocet labels on the blank buttons (reference/OpenDiablo2 main_menu.go + d2ui/button.go).
export const FRONTEND = {
  palette: 'data\\global\\palette\\Units\\pal.dat',
  backgroundPalette: 'data\\global\\palette\\Sky\\pal.dat',
  background: 'data\\global\\ui\\FrontEnd\\gameselectscreenEXP.dc6',
  logoFire: ['data\\global\\ui\\FrontEnd\\D2logoFireLeft.DC6', 'data\\global\\ui\\FrontEnd\\D2logoFireRight.DC6'],
  logoBlack: ['data\\global\\ui\\FrontEnd\\D2logoBlackLeft.DC6', 'data\\global\\ui\\FrontEnd\\D2logoBlackRight.DC6'],
  button: 'data\\global\\ui\\FrontEnd\\WideButtonBlank.dc6',
  // BATTLE.NET's button is its own DC6 in 1.13c (retail's draw list, the retail captures).
  button02: 'data\\global\\ui\\FrontEnd\\WideButtonBlank02.dc6',
  shortButton: 'data\\global\\ui\\CharSelect\\ShortButtonBlank.dc6',
  narrowButton: 'data\\global\\ui\\FrontEnd\\NarrowButtonBlank.dc6',
  font: 'data\\local\\font\\latin\\fontexocet10.dc6',
  fontTbl: 'data\\local\\font\\latin\\fontexocet10.tbl',
  // The big heading face character select/create title their screens with.
  headingFont: 'data\\local\\font\\latin\\font30.dc6',
  headingFontTbl: 'data\\local\\font\\latin\\font30.tbl',
  smallFont: 'data\\local\\font\\latin\\fontridiculous.dc6',
  smallFontTbl: 'data\\local\\font\\latin\\fontridiculous.tbl',
  // The version label in the bottom-left corner and the menu pointer.
  versionFont: 'data\\local\\font\\latin\\font16.dc6',
  versionFontTbl: 'data\\local\\font\\latin\\font16.tbl',
  cursor: 'data\\global\\ui\\CURSOR\\ohand.DC6',
};

// Where retail 1.13c draws each entry on the 800x600 screen (top-left), from retail's own draw list
//.
// `gateway` is the narrow bar retail hangs under BATTLE.NET naming the selected gateway. `order` is
// the order retail draws them in (its control list): gateway, BATTLE.NET, OTHER MULTIPLAYER, SINGLE
// PLAYER, CREDITS, CINEMATICS, EXIT.
export const TITLE_LAYOUT = {
  'SINGLE PLAYER': { kind: 'wide', x: 264, y: 290, order: 3 },
  'BATTLE.NET': { kind: 'wide02', x: 264, y: 332, order: 2, gateway: { x: 264, y: 367, label: 'GATEWAY: U.S. EAST' } },
  'OTHER MULTIPLAYER': { kind: 'wide', x: 264, y: 399, order: 2.5 },
  'CREDITS': { kind: 'short', x: 264, y: 504, order: 4 },
  'CINEMATICS': { kind: 'short', x: 402, y: 504, order: 5 },
  'EXIT DIABLO II': { kind: 'wide', x: 264, y: 534, order: 6 },
};
// Retail's version label: Font16, colour 0, anchored at (79, 569) (title draw list).
export const TITLE_VERSION = { text: 'v 1.13', x: 79, y: 569 };
// d2ui/button.go LabelColor greyAlpha100: button labels are the font's glyphs modulated by 0x64.
const LABEL_TINT = 0x64 / 0xff;

// The title screen is a 800x600 image cut into a grid of DC6 frames: 4 columns
// (256 + 256 + 256 + 32) by 3 rows (256 + 256 + 88), stored row-major.
export const TITLE_COLUMNS = 4;
export const TITLE_SIZE = { width: 800, height: 600 };

/**
 * Where D2Win draws a background cel file's frames (D2MOO D2WinMain.cpp, confirmed by retail's title
 * draw list): 4 per row at x = 0, 256, 512, 768, the rows anchored (bottom row, inclusive) at
 * y = 256 and 512, the last row at the screen's bottom row, 599. So row 0 of the screen stays black
 * and the last two rows overlap by one line.
 */
function tiledPlacements(dc6, columns, screenHeight = 600) {
  const rows = Math.ceil(dc6.frames.length / columns), out = [];
  for (let r = 0; r < rows; r++) for (let c = 0; c < columns; c++) {
    const i = r * columns + c, frame = dc6.frames[i];
    if (!frame) continue;
    const anchorY = r < rows - 1 ? 256 * (r + 1) : screenHeight - 1;
    out.push({ i, frame, left: 256 * c + frame.offsetX, top: anchorY + frame.offsetY - frame.height + 1 });
  }
  return { rows, out };
}

/** The tiled background as retail places it, flattened into one 800x600 image. */
export function composeTiled(dc6, palette, columns, documentObject) {
  const { rows, out } = tiledPlacements(dc6, columns);
  const canvas = documentObject.createElement('canvas');
  canvas.width = TITLE_SIZE.width; canvas.height = TITLE_SIZE.height;
  const ctx = canvas.getContext('2d');
  for (const p of out) ctx.drawImage(frameCanvas(p.frame, palette, documentObject), p.left, p.top);
  return { canvas, width: canvas.width, height: canvas.height, rows, columns, dc6, placements: out };
}

/** Draw-list records for a composed background drawn at (ox, oy). */
function recordTiled(path, tiled, ox, oy) {
  for (const p of tiled.placements) recordDc6(path, p.i, p.frame, ox + p.left, oy + p.top);
}

/**
 * Build the title screen. `items` are the menu entries, each `{ label, action }`.
 *
 * The flaming logo is two DC6s (left and right halves), each 30 frames, positioned by their own
 * frame offsets -- retail animates them over the static black logo underneath.
 */
export function createTitleScreen({ read, decodeDc6, decodeDatPalette, items, documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(FRONTEND.palette));
  // The whole title is ONE 8-bit frame shown through Sky (retail's frame buffer, s194), the buttons
  // too: Units differs from Sky in a few entries, the button gems among them.
  const skyPalette = decodeDatPalette(read(FRONTEND.backgroundPalette));
  const background = composeTiled(decodeDc6(read(FRONTEND.background)), skyPalette,
    TITLE_COLUMNS, documentObject);
  // Retail's order: black left, fire left, black right, fire right (title draw list).
  const logos = [FRONTEND.logoBlack[0], FRONTEND.logoFire[0], FRONTEND.logoBlack[1], FRONTEND.logoFire[1]].map((path) => {
    const dc6 = decodeDc6(read(path));
    return { path, dc6, canvases: dc6.frames.map((f) => frameCanvas(f, palette, documentObject)) };
  });
  // A button DC6 holds its states in order; a wide state is two frames side by side (256px body +
  // 16px tail), a short state one frame.
  const buttonKind = (path, framesPerState) => {
    const dc6 = decodeDc6(read(path));
    return [0, 1].map((state) => {
      const parts = dc6.frames.slice(state * framesPerState, (state + 1) * framesPerState);
      const width = parts.reduce((n, f) => n + f.width, 0);
      const height = Math.max(...parts.map((f) => f.height));
      const canvas = documentObject.createElement('canvas');
      canvas.width = width; canvas.height = height;
      const ctx = canvas.getContext('2d');
      let x = 0;
      for (const f of parts) { ctx.drawImage(frameCanvas(f, skyPalette, documentObject), x, 0); x += f.width; }
      return { canvas, width, height, path, parts: parts.map((f, k) => ({ frame: f, index: state * framesPerState + k })) };
    });
  };
  // Draw-list records for a composed button drawn with its top-left at (x, y).
  const recordButton = (b, x, y) => { for (const p of b.parts) { recordDc6(b.path, p.index, p.frame, x, y); x += p.frame.width; } };
  const buttons = { wide: buttonKind(FRONTEND.button, 2), wide02: buttonKind(FRONTEND.button02, 2),
    short: buttonKind(FRONTEND.shortButton, 1), narrow: buttonKind(FRONTEND.narrowButton, 2) };
  const tinted = (p) => { const t = p.slice(); for (let i = 0; i < t.length; i += 4) for (let k = 0; k < 3; k++) t[i + k] = Math.round(t[i + k] * LABEL_TINT); return t; };
  const labelPalette = tinted(palette);
  // Labels are recorded as retail's glyph path draws them (drawlist: pfCelDrawColor, mode 4); the
  // 0x64 tint is this client's rendering of that mode, measured against retail's frame (s174).
  const blend = createBlender(read('data\\global\\palette\\Sky\\pal.pl2'), palette);
  const labelDraw = { op: 'celcolor', mode: 4, blend };
  // Composed unscaled (the blends read pixels back), then scaled onto the page.
  const scene = documentObject.createElement('canvas'); scene.width = 800; scene.height = 600;
  const sceneCtx = scene.getContext('2d', { willReadFrequently: true });
  const font = createFont({ dc6: decodeDc6(read(FRONTEND.font)), tbl: decodeFontTbl(read(FRONTEND.fontTbl)), palette: labelPalette, documentObject, path: FRONTEND.font, drawAs: labelDraw });
  const smallFont = createFont({ dc6: decodeDc6(read(FRONTEND.smallFont)), tbl: decodeFontTbl(read(FRONTEND.smallFontTbl)), palette: labelPalette, documentObject, path: FRONTEND.smallFont, drawAs: labelDraw });
  const versionFont = createFont({ dc6: decodeDc6(read(FRONTEND.versionFont)), tbl: decodeFontTbl(read(FRONTEND.versionFontTbl)), palette, documentObject, path: FRONTEND.versionFont, drawAs: { op: 'celcolor', mode: 5 } });
  const cursorDc6 = decodeDc6(read(FRONTEND.cursor));
  const cursorCanvas = frameCanvas(cursorDc6.frames[0], palette, documentObject);

  // Every entry sits where retail puts it on the 800x600 screen; an entry retail does not have
  // gets no box (so it cannot be clicked into existence).
  const layout = () => items.flatMap((item) => {
    const at = TITLE_LAYOUT[item.label];
    if (!at) return [];
    const b = buttons[at.kind][0];
    return [{ item, kind: at.kind, x: at.x, y: at.y, w: b.width, h: b.height, gateway: at.gateway }];
  });

  // The art is authored for 800x600; the viewport scales it rather than cropping it, and pointer
  // coordinates come back through the same transform.
  const view = (width, height) => {
    const scale = Math.min(width / background.width, height / background.height);
    return { scale, ox: Math.round((width - background.width * scale) / 2), oy: Math.round((height - background.height * scale) / 2) };
  };
  const boxes = layout();
  // D2Win's button label (D2MOO D2WinButton.cpp BUTTON_Draw): exocet10 on buttons 35+ tall, else
  // fontridiculous; x = left + max((w - textWidth) / 2, 0), anchor y = bottom + offY - (h - font
  // line height) / 2 with offY 4 (h >= 35) / 3 (h == 32) / 2 (h > 20) / 1, integer division.
  const drawLabel = (ctx, text, x, y, w, h) => {
    const f = h >= 35 ? font : smallFont;
    const offY = h >= 35 ? 4 : h === 32 ? 3 : h > 20 ? 2 : 1;
    const bottom = y + h - 1;
    f.drawAnchored(ctx, text, x + Math.max(Math.trunc((w - f.measure(text)) / 2), 0), bottom + offY - Math.trunc((h - f.lineHeight) / 2));
  };
  const drawOrder = [...boxes].sort((a, b) => TITLE_LAYOUT[a.item.label].order - TITLE_LAYOUT[b.item.label].order);

  let hot = -1, pointer = null;
  return {
    evidence: {
      background: { width: background.width, height: background.height, rows: background.rows, columns: background.columns },
      logoFrames: logos.map((l) => l.dc6.frames.length),
      button: { width: buttons.wide[0].width, height: buttons.wide[0].height, states: buttons.wide.length },
      boxes: boxes.map((b) => ({ label: b.item.label, kind: b.kind, x: b.x, y: b.y, w: b.w, h: b.h })),
      fontGlyphs: font.measure('SINGLE PLAYER'),
      lineHeight: font.lineHeight,
    },
    hitTest(x, y, width, height) {
      const v = view(width, height);
      const sx = (x - v.ox) / v.scale, sy = (y - v.oy) / v.scale;
      const i = boxes.findIndex((b) => sx >= b.x && sx < b.x + b.w && sy >= b.y && sy < b.y + b.h);
      return i < 0 ? -1 : items.indexOf(boxes[i].item);
    },
    labelAt(index) { return items[index]?.label ?? null; },
    hover(x, y, width, height) {
      const v = view(width, height);
      pointer = [Math.trunc((x - v.ox) / v.scale), Math.trunc((y - v.oy) / v.scale)];
      hot = this.hitTest(x, y, width, height); return hot;
    },
    click(x, y, width, height) {
      const i = this.hitTest(x, y, width, height);
      if (i >= 0) items[i].action?.();
      return i;
    },
    draw(outCtx, width, height, tick) {
      outCtx.fillStyle = '#000'; outCtx.fillRect(0, 0, width, height);
      const v = view(width, height);
      const ctx = sceneCtx;
      ctx.fillStyle = '#000'; ctx.fillRect(0, 0, 800, 600);
      ctx.drawImage(background.canvas, 0, 0);
      recordTiled(FRONTEND.background, background, 0, 0);
      // Logo: retail animates the black halves in step with the fire (same frame index), the fire in
      // DrawMode 3 -- PL2 AdditiveBlend over what is below (createBlender). A frame's anchor is its bottom row.
      for (const logo of logos) {
        const n = logo.canvases.length;
        // Retail's 25 Hz front-end clock, one logo frame a tick, the DC6's last frame never shown
        // (title draw list: frames 8, 9, 10, 11 on four consecutive frames; the creation loops' rule).
        const frameIndex = Math.floor(tick * 25 / 60) % (n - 1);
        const frame = logo.dc6.frames[frameIndex];
        const canvas = logo.canvases[frameIndex];
        if (!frame || !canvas.width) continue;
        const fire = logo.path.toLowerCase().includes('fire');
        const left = 400 + frame.offsetX, top = 120 + frame.offsetY - frame.height + 1;
        if (fire) blend(ctx, frame, left, top, 3);
        else ctx.drawImage(canvas, left, top);
        recordDc6(logo.path, frameIndex, frame, left, top, { op: 'cel', mode: fire ? 3 : 5 });
      }
      for (const b of drawOrder) {
        if (b.gateway) {
          const g = buttons.narrow[0];
          ctx.drawImage(g.canvas, b.gateway.x, b.gateway.y);
          recordButton(g, b.gateway.x, b.gateway.y);
          drawLabel(ctx, b.gateway.label, b.gateway.x, b.gateway.y, g.width, g.height);
        }
        const state = items.indexOf(b.item) === hot ? 1 : 0;
        ctx.drawImage(buttons[b.kind][state].canvas, b.x, b.y);
        recordButton(buttons[b.kind][state], b.x, b.y);
        drawLabel(ctx, b.item.label, b.x, b.y, b.w, b.h);
      }
      versionFont.drawAnchored(ctx, TITLE_VERSION.text, TITLE_VERSION.x, TITLE_VERSION.y);
      // D2Win draws the pointer last, as a cel at (mouse x, mouse y - 1) (D2MOO D2WinMain.cpp).
      if (pointer) {
        const f = cursorDc6.frames[0];
        const left = pointer[0] + f.offsetX, top = pointer[1] - 1 + f.offsetY - f.height + 1;
        ctx.drawImage(cursorCanvas, left, top);
        recordDc6(FRONTEND.cursor, 0, f, left, top);
      }
      outCtx.save(); outCtx.imageSmoothingEnabled = false;
      outCtx.drawImage(scene, v.ox, v.oy, 800 * v.scale, 600 * v.scale);
      outCtx.restore();
      return { buttons: boxes.length, hot };
    },
  };
}
