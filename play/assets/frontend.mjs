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
export function createFont({ dc6, tbl, palette, documentObject = globalThis.document }) {
  const glyphs = new Map();
  const glyphFor = (code) => {
    if (glyphs.has(code)) return glyphs.get(code);
    const meta = tbl.chars.get(code);
    const frame = meta && dc6.frames[meta.imageIndex];
    const glyph = frame ? { canvas: frameCanvas(frame, palette, documentObject), advance: meta.width, height: meta.height } : null;
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
    /** Draw `text` with its LEFT edge at x and its TOP at y. */
    draw(ctx, text, x, y) {
      let cursor = x;
      for (const ch of String(text)) {
        const g = glyphFor(ch.codePointAt(0));
        if (!g) continue;
        if (g.canvas.width && g.canvas.height) ctx.drawImage(g.canvas, Math.round(cursor), Math.round(y));
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
  shortButton: 'data\\global\\ui\\CharSelect\\ShortButtonBlank.dc6',
  narrowButton: 'data\\global\\ui\\FrontEnd\\NarrowButtonBlank.dc6',
  font: 'data\\local\\font\\latin\\fontexocet10.dc6',
  fontTbl: 'data\\local\\font\\latin\\fontexocet10.tbl',
  // The big heading face character select/create title their screens with.
  headingFont: 'data\\local\\font\\latin\\font30.dc6',
  headingFontTbl: 'data\\local\\font\\latin\\font30.tbl',
  smallFont: 'data\\local\\font\\latin\\fontridiculous.dc6',
  smallFontTbl: 'data\\local\\font\\latin\\fontridiculous.tbl',
};

// Where retail 1.13c draws each entry on the 800x600 screen (top-left). x and the wide-button tops
// are measured from a retail frame (~/d2-retail under Wine, ) and agree with
// OpenDiablo2's main_menu.go to within 1px (it has EXIT at 535; the retail frame says 534).
// `gateway` is the narrow bar retail hangs under BATTLE.NET naming the selected gateway.
export const TITLE_LAYOUT = {
  'SINGLE PLAYER': { kind: 'wide', x: 264, y: 290 },
  'BATTLE.NET': { kind: 'wide', x: 264, y: 332, gateway: { x: 264, y: 367, label: 'GATEWAY: U.S. EAST' } },
  'OTHER MULTIPLAYER': { kind: 'wide', x: 264, y: 399 },
  'CREDITS': { kind: 'short', x: 264, y: 505 },
  'CINEMATICS': { kind: 'short', x: 401, y: 505 },
  'EXIT DIABLO II': { kind: 'wide', x: 264, y: 534 },
};
// d2ui/button.go LabelColor greyAlpha100: button labels are the font's glyphs modulated by 0x64.
const LABEL_TINT = 0x64 / 0xff;

// The title screen is a 800x600 image cut into a grid of DC6 frames: 4 columns
// (256 + 256 + 256 + 32) by 3 rows (256 + 256 + 88), stored row-major.
export const TITLE_COLUMNS = 4;
export const TITLE_SIZE = { width: 800, height: 600 };

/** Stitch the tiled background DC6 back into one image. */
function composeTiled(dc6, palette, columns, documentObject) {
  const rows = Math.ceil(dc6.frames.length / columns);
  let width = 0, height = 0;
  for (let c = 0; c < columns; c++) width += dc6.frames[c]?.width ?? 0;
  for (let r = 0; r < rows; r++) height += dc6.frames[r * columns]?.height ?? 0;
  const canvas = documentObject.createElement('canvas');
  canvas.width = width; canvas.height = height;
  const ctx = canvas.getContext('2d');
  let y = 0;
  for (let r = 0; r < rows; r++) {
    let x = 0;
    for (let c = 0; c < columns; c++) {
      const frame = dc6.frames[r * columns + c];
      if (!frame) continue;
      ctx.drawImage(frameCanvas(frame, palette, documentObject), x, y);
      x += frame.width;
    }
    y += dc6.frames[r * columns]?.height ?? 0;
  }
  return { canvas, width, height, rows, columns };
}

/**
 * Build the title screen. `items` are the menu entries, each `{ label, action }`.
 *
 * The flaming logo is two DC6s (left and right halves), each 30 frames, positioned by their own
 * frame offsets -- retail animates them over the static black logo underneath.
 */
export function createTitleScreen({ read, decodeDc6, decodeDatPalette, items, documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(FRONTEND.palette));
  const background = composeTiled(decodeDc6(read(FRONTEND.background)), decodeDatPalette(read(FRONTEND.backgroundPalette)),
    TITLE_COLUMNS, documentObject);
  const logos = [...FRONTEND.logoBlack, ...FRONTEND.logoFire].map((path) => {
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
      for (const f of parts) { ctx.drawImage(frameCanvas(f, palette, documentObject), x, 0); x += f.width; }
      return { canvas, width, height };
    });
  };
  const buttons = { wide: buttonKind(FRONTEND.button, 2), short: buttonKind(FRONTEND.shortButton, 1),
    narrow: buttonKind(FRONTEND.narrowButton, 2) };
  const tinted = (p) => { const t = p.slice(); for (let i = 0; i < t.length; i += 4) for (let k = 0; k < 3; k++) t[i + k] = Math.round(t[i + k] * LABEL_TINT); return t; };
  const labelPalette = tinted(palette);
  const font = createFont({ dc6: decodeDc6(read(FRONTEND.font)), tbl: decodeFontTbl(read(FRONTEND.fontTbl)), palette: labelPalette, documentObject });
  const smallFont = createFont({ dc6: decodeDc6(read(FRONTEND.smallFont)), tbl: decodeFontTbl(read(FRONTEND.smallFontTbl)), palette: labelPalette, documentObject });

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
  const drawLabel = (ctx, f, text, x, y, w, h) => f.draw(ctx, text, x + (w - f.measure(text)) / 2, y + (h - f.cellHeight) / 2);

  let hot = -1;
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
    hover(x, y, width, height) { hot = this.hitTest(x, y, width, height); return hot; },
    click(x, y, width, height) {
      const i = this.hitTest(x, y, width, height);
      if (i >= 0) items[i].action?.();
      return i;
    },
    draw(ctx, width, height, tick) {
      ctx.fillStyle = '#000'; ctx.fillRect(0, 0, width, height);
      const v = view(width, height);
      ctx.save();
      ctx.translate(v.ox, v.oy); ctx.scale(v.scale, v.scale);
      ctx.drawImage(background.canvas, 0, 0);
      // Logo: the black halves are static, the fire halves animate over them. A DC6 frame's offset
      // is its BOTTOM-left relative to the anchor; retail anchors the logo at (400, 120).
      for (const logo of logos) {
        const n = logo.canvases.length;
        const frameIndex = logo.path.toLowerCase().includes('fire') ? Math.floor(tick / 2) % n : 0;
        const frame = logo.dc6.frames[frameIndex];
        const canvas = logo.canvases[frameIndex];
        if (!frame || !canvas.width) continue;
        ctx.drawImage(canvas, 400 + frame.offsetX, 120 + frame.offsetY - frame.height);
      }
      for (const b of boxes) {
        const state = items.indexOf(b.item) === hot ? 1 : 0;
        ctx.drawImage(buttons[b.kind][state].canvas, b.x, b.y);
        drawLabel(ctx, b.kind === 'short' ? smallFont : font, b.item.label, b.x, b.y, b.w, b.h);
        if (b.gateway) {
          const g = buttons.narrow[0];
          ctx.drawImage(g.canvas, b.gateway.x, b.gateway.y);
          drawLabel(ctx, smallFont, b.gateway.label, b.gateway.x, b.gateway.y, g.width, g.height);
        }
      }
      ctx.restore();
      return { buttons: boxes.length, hot };
    },
  };
}
