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

// Retail paths. Kept together so it is obvious this screen is retail data end to end.
export const FRONTEND = {
  palette: 'data\\global\\palette\\Units\\pal.dat',
  background: 'data\\global\\ui\\FrontEnd\\titlescreen.dc6',
  logoFire: ['data\\global\\ui\\FrontEnd\\D2logoFireLeft.DC6', 'data\\global\\ui\\FrontEnd\\D2logoFireRight.DC6'],
  logoBlack: ['data\\global\\ui\\FrontEnd\\D2logoBlackLeft.DC6', 'data\\global\\ui\\FrontEnd\\D2logoBlackRight.DC6'],
  button: 'data\\global\\ui\\FrontEnd\\WideButtonBlank.dc6',
  font: 'data\\local\\font\\latin\\font30.dc6',
  fontTbl: 'data\\local\\font\\latin\\font30.tbl',
};

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
  const background = composeTiled(decodeDc6(read(FRONTEND.background)), palette, TITLE_COLUMNS, documentObject);
  const logos = [...FRONTEND.logoBlack, ...FRONTEND.logoFire].map((path) => {
    const dc6 = decodeDc6(read(path));
    return { path, dc6, canvases: dc6.frames.map((f) => frameCanvas(f, palette, documentObject)) };
  });
  const buttonDc6 = decodeDc6(read(FRONTEND.button));
  const buttonStates = [0, 1].map((state) => {
    // Each state is two frames side by side: a 256px body and a 16px tail.
    const parts = [buttonDc6.frames[state * 2], buttonDc6.frames[state * 2 + 1]].filter(Boolean);
    const width = parts.reduce((n, f) => n + f.width, 0);
    const height = Math.max(...parts.map((f) => f.height));
    const canvas = documentObject.createElement('canvas');
    canvas.width = width; canvas.height = height;
    const ctx = canvas.getContext('2d');
    let x = 0;
    for (const f of parts) { ctx.drawImage(frameCanvas(f, palette, documentObject), x, 0); x += f.width; }
    return { canvas, width, height };
  });
  const font = createFont({
    dc6: decodeDc6(read(FRONTEND.font)),
    tbl: decodeFontTbl(read(FRONTEND.fontTbl)),
    palette, documentObject,
  });

  // Menu buttons stack in the lower half, centred, the way retail lays them out.
  const BUTTON_GAP = 6;
  const layout = (width, height) => {
    const bw = buttonStates[0].width, bh = buttonStates[0].height;
    const total = items.length * bh + (items.length - 1) * BUTTON_GAP;
    // Retail stacks the menu in the lower half, under the logo. Centre the stack on 62% height.
    const top = Math.round(height * 0.62 - total / 2);
    return items.map((item, i) => ({
      item, x: Math.round((width - bw) / 2), y: Math.round(top + i * (bh + BUTTON_GAP)), w: bw, h: bh,
    }));
  };

  let hot = -1;
  return {
    evidence: {
      background: { width: background.width, height: background.height, rows: background.rows, columns: background.columns },
      logoFrames: logos.map((l) => l.dc6.frames.length),
      button: { width: buttonStates[0].width, height: buttonStates[0].height, states: buttonStates.length },
      fontGlyphs: font.measure('SINGLE PLAYER'),
      lineHeight: font.lineHeight,
    },
    hitTest(x, y, width, height) {
      const boxes = layout(width, height);
      for (let i = 0; i < boxes.length; i++) {
        const b = boxes[i];
        if (x >= b.x && x < b.x + b.w && y >= b.y && y < b.y + b.h) return i;
      }
      return -1;
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
      // The art is authored for 800x600; scale it to the viewport rather than cropping it.
      const scale = Math.min(width / background.width, height / background.height);
      const ox = Math.round((width - background.width * scale) / 2);
      const oy = Math.round((height - background.height * scale) / 2);
      ctx.save();
      ctx.translate(ox, oy); ctx.scale(scale, scale);
      ctx.drawImage(background.canvas, 0, 0);
      // Logo: the black halves are static, the fire halves animate over them. Frame offsets are
      // relative to the screen centre, which is how retail anchors them.
      const cx = background.width / 2, cy = 0;
      for (const logo of logos) {
        const n = logo.canvases.length;
        const frameIndex = logo.path.toLowerCase().includes('fire') ? Math.floor(tick / 2) % n : 0;
        const frame = logo.dc6.frames[frameIndex];
        const canvas = logo.canvases[frameIndex];
        if (!frame || !canvas.width) continue;
        ctx.drawImage(canvas, Math.round(cx + frame.offsetX), Math.round(cy + frame.offsetY));
      }
      ctx.restore();

      for (const [i, b] of layout(width, height).entries()) {
        const state = i === hot ? 1 : 0;
        ctx.drawImage(buttonStates[state].canvas, b.x, b.y);
        const label = b.item.label;
        const w = font.measure(label);
        font.draw(ctx, label, b.x + (b.w - w) / 2, b.y + (b.h - font.lineHeight) / 2);
      }
      return { buttons: items.length, hot };
    },
  };
}
