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
import { drawBg, present } from './widebg.mjs';
import { isButtonHeldOverTarget } from './frontend-pointer-states.mjs';
import { mixTable } from './mix-palette.mjs';
import { createGatewaySelector, DEFAULT_GATEWAY_LABEL, GATEWAY_LAYOUT } from './bnet-servers.mjs';

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
 * mode 1 AlphaBlend[under][src] (a control drawn dimmed), the tables of the screen's palette (Sky on
 * the title: 99.96% and 100% of retail's pixels). `under`
 * is read back from the canvas as a palette index, so the canvas must be unscaled 800x600 and hold
 * raw palette colours. Offsets per OpenDiablo2's d2pl2 layout.
 */
export const PL2_ADDITIVE = 1024 + 32 * 256 + 16 * 256 + 256 + 3 * 65536;
export const PL2_MULTIPLICATIVE = PL2_ADDITIVE + 65536;
/** Offset of PL2 AlphaBlend (retail's DrawMode 1, the disabled/dimmed control): index 1 of the four
 *  tables, two 65536-byte steps below the additive one. Retail's own draw list draws character
 *  creation's OK button through it while the name is under two characters
 *  (`cel MediumSelButtonBlank.dc6 frame=0 x=627 y=572 mode=1`, the retail captures),
 *  and character select's disabled CONVERT TO EXPANSION the same way. */
export const PL2_ALPHA = PL2_ADDITIVE - 2 * 65536;
/** The other two PL2 alpha banks used by retail DrawMode 0 and 2 (PL2 AlphaBlend[0] and [2]). */
export const PL2_ALPHA_25 = PL2_ALPHA - 65536;
export const PL2_ALPHA_75 = PL2_ALPHA + 65536;
/** Offset of PL2 TextColorShifts (13 x 256; OpenDiablo2 d2pl2 layout). */
export const PL2_TEXT_SHIFTS = 1024 + 8192 + 4096 + 256 + 5 * 65536 + 111 * 256 + 768 + 14 * 256 + 65536 + 256 + 39;
/**
 * Offset of PL2 MaxComponentBlend (256 x 256; OpenDiablo2-pl2 pkg/pl2.go `maxComponentBlends = 256`):
 * the 65536 bytes right before DarkenedColorShift in PL2_TEXT_SHIFTS's own derivation, so this is that
 * same proven offset arithmetic, not a second one -- the file's total size (443,175 bytes, every 1.13c
 * palette but Loading) is exactly PL2_TEXT_SHIFTS + 13*256, confirming the layout end to end.
 *
 * Client-parity C3o: a saved character's component colour byte (D2SaveHeaderStrc 0x98..0xA7) is
 * `1 + index` into this bank -- the same packed value `ITEMS_GetColor` writes at save time
 * (`(colourField & 31) + 32 * pItemsTxtRecord->nTransform`, src/core/d2moo/src/Items/Items.cpp) before
 * the save-writer increments it (0xFF/-1 = no transform). Reading it back needs only the byte, not the
 * item identity that produced it.
 */
export const PL2_MAX_COMPONENT_BLEND = PL2_TEXT_SHIFTS - 65536 - 256 - 39;

/**
 * Client-parity C3o: retail's dyed-component transform is D2CMP_MixPalette, not a PL2 slice: the
 * colour byte selects nTrans in {1,2,5,6,7,8} and nColor in [0,20], and the table is row nColor of the
 * matching data\global\items\palette\*.dat file (mix-palette.mjs). A byte with no table falls back to
 * the PL2 MaxComponentBlend slice so the layer still draws.
 */
/** The 256-entry palette-index remap a saved component colour byte selects, or null for 0xFF/absent. */
export function componentColourTransform(pl2, colourByte, read) {
  if (colourByte === undefined || colourByte === null || colourByte === 0xFF) return null;
  const mixed = mixTable(read, colourByte);
  if (mixed) return mixed;
  const offset = PL2_MAX_COMPONENT_BLEND + (colourByte - 1) * 256;
  return pl2.subarray(offset, offset + 256);
}
/** A palette whose entry k is palette[TextColorShifts[colour][k]]: a glyph drawn in D2Win colour `colour`. */
export function textColourPalette(pl2, palette, colour) {
  const out = palette.slice();
  for (let k = 0; k < 256; k++) { const j = pl2[PL2_TEXT_SHIFTS + colour * 256 + k]; out.set(palette.subarray(j * 4, j * 4 + 3), k * 4); }
  return out;
}
export function createBlender(pl2, palette) {
  const index = new Map();
  for (let j = 255; j >= 0; j--) index.set((palette[j * 4] << 16) | (palette[j * 4 + 1] << 8) | palette[j * 4 + 2], j);
  const tables = {
    0: pl2.subarray(PL2_ALPHA_25, PL2_ALPHA_25 + 65536),
    1: pl2.subarray(PL2_ALPHA, PL2_ALPHA + 65536),
    2: pl2.subarray(PL2_ALPHA_75, PL2_ALPHA_75 + 65536),
    3: pl2.subarray(PL2_ADDITIVE, PL2_ADDITIVE + 65536),
    4: pl2.subarray(PL2_MULTIPLICATIVE, PL2_MULTIPLICATIVE + 65536),
  };
  // World art normally reaches this compositor as an exact palette colour. UI scaling and an
  // earlier blend do not, though. Source-index fallback makes an effect-3 cel opaque in that
  // case; choose the nearest palette entry, as the other PL2 consumer (the NPC menu box) does.
  const paletteIndex = (r, g, b) => {
    const key = (r << 16) | (g << 8) | b;
    let value = index.get(key);
    if (value !== undefined) return value;
    let best = 0, bestDistance = Infinity;
    for (let j = 0; j < 256; j++) {
      const dr = palette[j * 4] - r, dg = palette[j * 4 + 1] - g, db = palette[j * 4 + 2] - b;
      const distance = dr * dr + dg * dg + db * db;
      if (distance < bestDistance) { best = j; bestDistance = distance; }
    }
    index.set(key, best); return best;
  };
  // PERF: the Map.get above ran once per blended pixel (campfire, torches, rain, blended monster layers). A
  // direct-mapped cache in front of it (as session-terrain's indexOf32), keyed by the pixel's low 24 bits as a
  // little-endian Uint32 reads them, returns paletteIndex's own answer: a key's index never changes once set.
  const ck = new Int32Array(4096).fill(-1), cj = new Uint8Array(4096);
  const rgba = Uint32Array.from({ length: 256 }, (_, j) => palette[j * 4] | (palette[j * 4 + 1] << 8) | (palette[j * 4 + 2] << 16) | 0xff000000);
  return (ctx, frame, left, top, mode) => {
    const table = tables[mode];
    if (!frame.width || !frame.height) return;
    // A cel entirely outside the target canvas paints nothing: getImageData would read transparent
    // black and putImageData would clip every pixel, so the round trip is pure cost. Most town
    // objects (torches, the waypoint) are blended at anchors far off the 800x600 frame.
    const canvas = ctx.canvas;
    if (canvas && (left >= canvas.width || top >= canvas.height || left + frame.width <= 0 || top + frame.height <= 0)) return;
    const img = ctx.getImageData(left, top, frame.width, frame.height);
    const d = new Uint32Array(img.data.buffer, img.data.byteOffset, img.data.length >>> 2);
    for (let k = 0; k < frame.indices.length; k++) {
      if (!frame.opaque[k]) continue;
      const key = d[k] & 0xffffff, h = Math.imul(key, 0x9E3779B1) >>> 20;
      let under;
      if (ck[h] === key) under = cj[h];
      else { under = paletteIndex(key & 0xff, (key >>> 8) & 0xff, key >>> 16); ck[h] = key; cj[h] = under; }
      d[k] = rgba[table[under * 256 + frame.indices[k]]];
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
// `onGlyph` is bound as onFontGlyph: it is the FONT-level option, and `draw` also takes a per-call
// callback. The two are handed different payloads (see draw), so they cannot share a name.
export function createFont({ dc6, tbl, palette, documentObject = globalThis.document, path = null, drawAs = { op: 'celcolor', mode: 5 }, onGlyph: onFontGlyph = null }) {
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
    /** The glyph a code point resolves to (null when the font has none) -- identity evidence. */
    glyph(code) { return glyphFor(code); },
    measure(text) {
      let w = 0;
      for (const ch of String(text)) w += glyphFor(ch.codePointAt(0))?.advance ?? 0;
      return w;
    },
    /**
     * Draw `text` as D2Win does: glyph by glyph from x (whole pixels, the .tbl advances), each glyph
     * a cel drawn with its BOTTOM row at `anchorY` (inclusive).
     */
    drawAnchored(ctx, text, x, anchorY, onGlyph = null) {
      let cursor = Math.trunc(x);
      for (const ch of String(text)) {
        const g = glyphFor(ch.codePointAt(0));
        if (!g) continue;
        if (g.canvas.width && g.canvas.height) {
          const top = anchorY + g.frame.offsetY - g.frame.height + 1;
          if (drawAs.blend) drawAs.blend(ctx, g.frame, cursor + g.frame.offsetX, top, drawAs.mode);
          else ctx.drawImage(g.canvas, cursor + g.frame.offsetX, top);
          const blit = path && recordDc6(path, g.index, g.frame, cursor + g.frame.offsetX, top, drawAs);
          if (blit && onFontGlyph) onFontGlyph({ char: ch, ...blit });
          onGlyph?.({ character: ch, index: g.index, path, x: cursor, y: anchorY });
        }
        cursor += g.advance;
      }
      return cursor - x;
    },
    /** Draw `text` with its LEFT edge at x and its TOP at y. */
    draw(ctx, text, x, y, onGlyph = null) {
      let cursor = x;
      for (const ch of String(text)) {
        const g = glyphFor(ch.codePointAt(0));
        if (!g) continue;
        if (g.canvas.width && g.canvas.height) {
          const left = Math.round(cursor), top = Math.round(y);
          ctx.drawImage(g.canvas, left, top);
          // A glyph is a cel draw of the font's DC6 frame (drawlist.mjs); `path` names the font.
          const blit = path && recordDc6(path, g.index, g.frame, left, top, drawAs);
          // Two consumers want identity evidence and they cannot share one `path`. The per-call
          // callback (gameplay-shell's hover labels) is handed the FONT path, which its row compares
          // against the requested font identity. The font-level option (charcreate's description
          // lines) is handed the RECORDED cel row, whose `path` is the DC6 draw path the draw list
          // uses. Both fire only for a glyph that was really blitted, so requested text stays
          // distinct from painted identity in either shape.
          onGlyph?.({ character: ch, index: g.index, path, x: left, y: top });
          if (blit && onFontGlyph) onFontGlyph({ char: ch, ...blit });
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
  // The phase BEFORE the menu, which 1.13c boots into (retail's own draw list,
  // the retail captures): its own background art, the same four logo DC6s, and the two
  // copyright lines -- no buttons, no version label, no pointer.
  trademark: 'data\\global\\ui\\FrontEnd\\trademarkscreenEXP.dc6',
  trademarkFont: 'data\\local\\font\\latin\\fontformal12.dc6',
  trademarkFontTbl: 'data\\local\\font\\latin\\fontformal12.tbl',
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
  gatewayPanel: 'data\\global\\ui\\FrontEnd\\GWListBox.dc6',
  gatewayButton: 'data\\global\\ui\\FrontEnd\\CancelButtonBlank.dc6',
  gatewayHeadingFont: 'data\\local\\font\\latin\\font24.dc6',
  gatewayHeadingFontTbl: 'data\\local\\font\\latin\\font24.tbl',
  gatewayBodyFont: 'data\\local\\font\\latin\\font8.dc6',
  gatewayBodyFontTbl: 'data\\local\\font\\latin\\font8.tbl',
  gatewayRegionFont: 'data\\local\\font\\latin\\fontformal12.dc6',
  gatewayRegionFontTbl: 'data\\local\\font\\latin\\fontformal12.tbl',
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
  'BATTLE.NET': { kind: 'wide02', x: 264, y: 332, order: 2, gateway: { x: 264, y: 367, label: DEFAULT_GATEWAY_LABEL } },
  'OTHER MULTIPLAYER': { kind: 'wide', x: 264, y: 399, order: 2.5 },
  'CREDITS': { kind: 'short', x: 264, y: 504, order: 4 },
  'CINEMATICS': { kind: 'short', x: 402, y: 504, order: 5 },
  'EXIT DIABLO II': { kind: 'wide', x: 264, y: 534, order: 6 },
};
// Retail's version label: Font16, colour 0, anchored at (79, 569) (title draw list).
export const TITLE_VERSION = { text: 'v 1.13', x: 79, y: 569 };
// d2ui/button.go LabelColor greyAlpha100: button labels are the font's glyphs modulated by 0x64.
export const LABEL_TINT = 0x64 / 0xff;

/**
 * The trademark screen: the copyright notice retail shows for ten seconds before the menu (its own
 * pre-menu phase), on the trademark art.
 *
 * Where everything comes from: retail's premenu draw list (`the retail captures`) gives the
 * font (FontFormal12), the two bottom-row anchors (520 and 544), the draw mode (celcolor, mode 5),
 * the colour shift (extra 4 -- gold; OpenD2 `Modcode/Client/UI/Menus/Trademark.cpp`) and the text
 * itself -- a font's DC6 frame index is its code point, so the 59 glyph records decode to
 * "© Copyright 2001 Blizzard Entertainment" and "All Rights Reserved.". The x is D2Win's centre
 * alignment in the 800-wide screen: `trunc((800 - width) / 2)` puts the glyph cursors at retail's own
 * 222 and 314 exactly (advances 356 and 172 in fontformal12.tbl).
 *
 * The indices are measured, not taken from OpenD2: 5104 is the CLASSIC line
 * ("(c) 2000 Blizzard Entertainment") and drawing it would not be the LoD screen; 1.13c's
 * ExpansionString.tbl carries the 2001 line at 21882 (scanned with the engine's own
 * d2_lang_string_by_index, not read off a table file) and the classic table's "All Rights Reserved."
 * at 5105 -- the
 * expansion table has no second line of its own. D2MOO's StringIds.h names the same two classic
 * indices and leaves 21882 unnamed, i.e. the same layout on 1.10f.
 */
export const TRADEMARK_LAYOUT = {
  screenWidth: 800,
  colour: 4,                                        // D2Win TextColorShifts index (gold)
  lines: [{ string: 21882, y: 520 },                // ExpansionString.tbl "© Copyright 2001 Blizzard Entertainment"
           { string: 5105, y: 544 }],               // string.tbl "All Rights Reserved."
};

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
export function recordTiled(path, tiled, ox, oy) {
  for (const p of tiled.placements) recordDc6(path, p.i, p.frame, ox + p.left, oy + p.top);
}

/**
 * Build the title screen. `items` are the menu entries, each `{ label, action }`.
 *
 * Two phases, in retail's order: `trademark` (the copyright notice on the trademark art, no buttons)
 * and `menu`. `string` is the engine's string-table lookup by index (`d2_lang_string_by_index`), which
 * is where the copyright text comes from -- the glyph frames retail draws are the text, so a wrong or
 * missing string shows up as a wrong draw list rather than as a blank line nobody notices.
 *
 * The flaming logo is two DC6s (left and right halves), each 30 frames, positioned by their own
 * frame offsets -- retail animates them over the static black logo underneath, in both phases.
 */
export function createTitleScreen({ read, decodeDc6, decodeDatPalette, items, string = null, documentObject = globalThis.document, bnet = null, onGateway = null }) {
  const palette = decodeDatPalette(read(FRONTEND.palette));
  // The whole title is ONE 8-bit frame shown through Sky (retail's frame buffer, s194), the buttons
  // too: Units differs from Sky in a few entries, the button gems among them. The trademark phase is
  // the same screen with earlier art, so it is the same palette.
  const skyPalette = decodeDatPalette(read(FRONTEND.backgroundPalette));
  const background = { ...composeTiled(decodeDc6(read(FRONTEND.background)), skyPalette,
    TITLE_COLUMNS, documentObject), path: FRONTEND.background };
  const trademarkBackground = { ...composeTiled(decodeDc6(read(FRONTEND.trademark)), skyPalette,
    TITLE_COLUMNS, documentObject), path: FRONTEND.trademark };
  // Retail's order: black left, fire left, black right, fire right (title draw list).
  // A frame's canvas is painted the first time it draws: the fire halves blend from the frame itself and
  // never need one, and painting all 120 up front was most of the title build's first task.
  const logos = [FRONTEND.logoBlack[0], FRONTEND.logoFire[0], FRONTEND.logoBlack[1], FRONTEND.logoFire[1]].map((path) => {
    const dc6 = decodeDc6(read(path)), canvases = [];
    return { path, dc6, canvas: (i) => canvases[i] ??= frameCanvas(dc6.frames[i], palette, documentObject) };
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
  // Draw-list records for a composed button drawn with its top-left at (x, y). Returns them, so the
  // Battle.net region list can carry the identity of what it actually painted.
  const recordButton = (b, x, y) => { const out = []; for (const p of b.parts) { out.push(recordDc6(b.path, p.index, p.frame, x, y)); x += p.frame.width; } return out; };
  const buttons = { wide: buttonKind(FRONTEND.button, 2), wide02: buttonKind(FRONTEND.button02, 2),
    short: buttonKind(FRONTEND.shortButton, 1), narrow: buttonKind(FRONTEND.narrowButton, 2) };
  const tinted = (p) => { const t = p.slice(); for (let i = 0; i < t.length; i += 4) for (let k = 0; k < 3; k++) t[i + k] = Math.round(t[i + k] * LABEL_TINT); return t; };
  const labelPalette = tinted(palette);
  const skyPl2 = read('data\\global\\palette\\Sky\\pal.pl2');
  // Labels are recorded as retail's glyph path draws them (drawlist: pfCelDrawColor, mode 4); the
  // 0x64 tint is this client's rendering of that mode, measured against retail's frame (s174).
  const blend = createBlender(skyPl2, palette);
  const logoBlend = createBlender(skyPl2, skyPalette);
  const labelDraw = { op: 'celcolor', mode: 4, blend };
  // Composed unscaled (the blends read pixels back), then scaled onto the page.
  const scene = documentObject.createElement('canvas'); scene.width = 800; scene.height = 600;
  const sceneCtx = scene.getContext('2d', { willReadFrequently: true });
  const font = createFont({ dc6: decodeDc6(read(FRONTEND.font)), tbl: decodeFontTbl(read(FRONTEND.fontTbl)), palette: labelPalette, documentObject, path: FRONTEND.font, drawAs: labelDraw });
  const smallFont = createFont({ dc6: decodeDc6(read(FRONTEND.smallFont)), tbl: decodeFontTbl(read(FRONTEND.smallFontTbl)), palette: labelPalette, documentObject, path: FRONTEND.smallFont, drawAs: labelDraw });
  // The local gateway screen is the same retail screen measured by F233. Keep its art and typography
  // on the same 800x600 Sky surface as the menu, with the exact 1.13c files, colour shifts and anchors.
  const gatewayPanelDc6 = decodeDc6(read(FRONTEND.gatewayPanel));
  const gatewayButtonDc6 = decodeDc6(read(FRONTEND.gatewayButton));
  const gatewayButton = gatewayButtonDc6.frames[0];
  const gatewayButtonCanvas = frameCanvas(gatewayButton, skyPalette, documentObject);
  const gatewayHeadingFont = createFont({ dc6: decodeDc6(read(FRONTEND.gatewayHeadingFont)),
    tbl: decodeFontTbl(read(FRONTEND.gatewayHeadingFontTbl)),
    palette: textColourPalette(skyPl2, palette, GATEWAY_LAYOUT.heading.shift), documentObject,
    path: FRONTEND.gatewayHeadingFont, drawAs: { op: 'celcolor', mode: 5, extra: GATEWAY_LAYOUT.heading.shift } });
  const gatewayBodyFont = createFont({ dc6: decodeDc6(read(FRONTEND.gatewayBodyFont)),
    tbl: decodeFontTbl(read(FRONTEND.gatewayBodyFontTbl)),
    palette: textColourPalette(skyPl2, palette, GATEWAY_LAYOUT.body.shift), documentObject,
    path: FRONTEND.gatewayBodyFont, drawAs: { op: 'celcolor', mode: 5, extra: GATEWAY_LAYOUT.body.shift } });
  const gatewayRegionFont = (shift) => createFont({ dc6: decodeDc6(read(FRONTEND.gatewayRegionFont)),
    tbl: decodeFontTbl(read(FRONTEND.gatewayRegionFontTbl)),
    // Retail's selected row is FontFormal12 color token 0: the unshifted Units font palette. Passing
    // it through TextColorShifts[0] maps its white glyph pixels to black and reverses the highlight.
    palette: shift === 0 ? palette : textColourPalette(skyPl2, palette, shift), documentObject,
    path: FRONTEND.gatewayRegionFont, drawAs: { op: 'celcolor', mode: 5, extra: shift } });
  const gatewayRegionFonts = { selected: gatewayRegionFont(GATEWAY_LAYOUT.rows.selectedShift), other: gatewayRegionFont(GATEWAY_LAYOUT.rows.shift) };
  const drawGatewayFont = (fontObject, ctx, text, x, y) => {
    const glyphs = [];
    fontObject.drawAnchored(ctx, text, x, y, (g) => glyphs.push(g));
    return { text: glyphs.map((g) => g.character).join(''), glyphs: glyphs.map((g) => ({ character: g.character, index: g.index, path: g.path, x: g.x, y: g.y })) };
  };
  const drawGatewayButtonLabel = (ctx, text, x) => {
    const glyphs = [];
    const start = x + Math.trunc((GATEWAY_LAYOUT.button.w - smallFont.measure(text)) / 2);
    smallFont.drawAnchored(ctx, text, start, GATEWAY_LAYOUT.button.anchorY, (g) => glyphs.push(g));
    return { text: glyphs.map((g) => g.character).join(''), glyphs: glyphs.map((g) => ({ character: g.character, index: g.index, path: g.path, x: g.x, y: g.y })) };
  };
  const gatewayScreen = {
    drawPanel(ctx) {
      return GATEWAY_LAYOUT.panel.tiles.map((tile) => {
        const frame = gatewayPanelDc6.frames[tile.frame];
        const x = GATEWAY_LAYOUT.panel.x + tile.x, y = GATEWAY_LAYOUT.panel.y + tile.y;
        ctx.drawImage(frameCanvas(frame, skyPalette, documentObject), x, y);
        const blit = recordDc6(FRONTEND.gatewayPanel, tile.frame, frame, x, y, { op: 'cel', mode: 5 });
        return { ...blit, path: FRONTEND.gatewayPanel, frame: tile.frame, x, y };
      });
    },
    drawHeading(ctx) {
      const { text, font, x, y } = GATEWAY_LAYOUT.heading;
      return { ...drawGatewayFont(gatewayHeadingFont, ctx, text, x, y), font, x, y };
    },
    drawBody(ctx) {
      return GATEWAY_LAYOUT.body.lines.map((line) => ({ ...drawGatewayFont(gatewayBodyFont, ctx, line.text, GATEWAY_LAYOUT.body.x, line.y), font: GATEWAY_LAYOUT.body.font, x: GATEWAY_LAYOUT.body.x, y: line.y }));
    },
    drawSelection(ctx, row) {
      const rect = { x: GATEWAY_LAYOUT.rows.x, y: row.anchorY - 22, w: GATEWAY_LAYOUT.rows.w, h: GATEWAY_LAYOUT.rows.h };
      ctx.fillStyle = GATEWAY_LAYOUT.rows.selectedColor;
      ctx.fillRect(rect.x, rect.y, rect.w, rect.h);
      return { ...rect, color: 190, rgb: [99, 99, 99] };
    },
    drawRegion(ctx, row, selected) {
      const fontObject = selected ? gatewayRegionFonts.selected : gatewayRegionFonts.other;
      return { ...drawGatewayFont(fontObject, ctx, row.region, GATEWAY_LAYOUT.rows.textX, row.anchorY), anchorY: row.anchorY,
        font: FRONTEND.gatewayRegionFont, shift: selected ? GATEWAY_LAYOUT.rows.selectedShift : GATEWAY_LAYOUT.rows.shift,
        x: GATEWAY_LAYOUT.rows.textX, y: row.anchorY, selected };
    },
    drawButton(ctx, key, pressed) {
      const button = GATEWAY_LAYOUT.button[key];
      ctx.drawImage(gatewayButtonCanvas, button.x, GATEWAY_LAYOUT.button.y);
      const cel = recordDc6(FRONTEND.gatewayButton, 0, gatewayButton, button.x, GATEWAY_LAYOUT.button.y, { op: 'cel', mode: 5 });
      const label = drawGatewayButtonLabel(ctx, button.label, button.x);
      return { key, path: FRONTEND.gatewayButton, frame: 0, x: button.x, y: GATEWAY_LAYOUT.button.y,
        w: gatewayButton.width, h: gatewayButton.height, pressed, cel, label };
    },
  };
  const versionFont = createFont({ dc6: decodeDc6(read(FRONTEND.versionFont)), tbl: decodeFontTbl(read(FRONTEND.versionFontTbl)), palette, documentObject, path: FRONTEND.versionFont, drawAs: { op: 'celcolor', mode: 5 } });
  // The copyright lines, in the font's own colour 4 (gold) through Sky's shift table -- the palette
  // entry each glyph pixel is remapped to before the palette lookup (textColourPalette), which is
  // what retail's celcolour draw with extra=4 does.
  const goldPalette = textColourPalette(skyPl2, skyPalette, TRADEMARK_LAYOUT.colour);
  const trademarkFont = createFont({ dc6: decodeDc6(read(FRONTEND.trademarkFont)),
    tbl: decodeFontTbl(read(FRONTEND.trademarkFontTbl)), palette: goldPalette, documentObject,
    path: FRONTEND.trademarkFont, drawAs: { op: 'celcolor', mode: 5, extra: TRADEMARK_LAYOUT.colour } });
  // Resolved on first use, not at construction: the string tables are the engine's, and a failure
  // here must be visible in the evidence rather than silently drawing nothing.
  let trademarkError = '';
  const trademarkLines = TRADEMARK_LAYOUT.lines.map((line) => ({ ...line, text: null, x: null }));
  const resolveTrademark = () => {
    for (const line of trademarkLines) {
      if (line.text !== null) return;
      try { line.text = String(string ? string(line.string) : ''); }
      catch (error) { trademarkError = String(error); line.text = ''; }
      line.x = Math.trunc((TRADEMARK_LAYOUT.screenWidth - trademarkFont.measure(line.text)) / 2);
    }
  };
  // `glyph` takes a code point (the .tbl's key); a line of text counts its characters' code points.
  const glyphCount = (text) => [...String(text)].filter((ch) => trademarkFont.glyph(ch.codePointAt(0))).length;
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
  // `dx`/`dy` carry retail's pressedButtonOffset: a button held down draws its label 2 px left and
  // 2 px down, the same displacement its own cel moves. Retail's title frame with SINGLE PLAYER held
  // is the evidence: the glyphs move, the cels do not.
  const drawLabel = (ctx, text, x, y, w, h, dx = 0, dy = 0, onGlyph = null) => {
    const f = h >= 35 ? font : smallFont;
    const offY = h >= 35 ? 4 : h === 32 ? 3 : h > 20 ? 2 : 1;
    const bottom = y + h - 1;
    f.drawAnchored(ctx, text, x + dx + Math.max(Math.trunc((w - f.measure(text)) / 2), 0), bottom + dy + offY - Math.trunc((h - f.lineHeight) / 2), onGlyph);
  };
  const drawOrder = [...boxes].sort((a, b) => TITLE_LAYOUT[a.item.label].order - TITLE_LAYOUT[b.item.label].order);

  // Battle.net is a CONFIG FILE the project owns (`play/assets/bnet-servers.json` through
  // `bnet-servers.mjs`), not a player setting. This is the whole of it: with the shipped empty file
  // `enabled` is false, nothing below exists, BATTLE.NET keeps its inert action, the bar keeps
  // retail's own text and the frame is byte-for-byte retail's. `bnet` is the boot fetch's promise (or
  // a plain config); it resolves long before a player can reach the menu, and until it does the
  // evidence says `pending` rather than pretending Battle.net is off.
  let gatewaySelector = null;
  let bnetState = { pending: true, enabled: false, serverCount: 0, error: '', url: null, configured: [] };
  const applyBnet = (cfg) => {
    bnetState = { pending: false, enabled: Boolean(cfg?.enabled), serverCount: cfg?.servers?.length ?? 0,
      error: cfg?.error ?? '', url: cfg?.url ?? null, configured: (cfg?.servers ?? []).map((s) => ({ region: s.region, host: s.host })) };
    if (!bnetState.enabled) return;
    gatewaySelector = createGatewaySelector({
      servers: cfg.servers, screen: gatewayScreen, width: 800, height: 600, onCommit: onGateway,
    });
  };
  if (bnet) Promise.resolve(bnet).then(applyBnet, (error) => applyBnet({ enabled: false, servers: [], error: String(error) }));

  let hot = -1, pressed = -1, pointer = null;
  // The glyphs the gateway bar ACTUALLY blitted last frame -- identity evidence for the bar text,
  // which is retail's own until the player picks a configured region.
  let barDrawn = null;
  // Retail's logo animation, both phases: black left, fire left, black right, fire right, one frame a
  // tick at 25 Hz, the DC6's last frame never shown (title draw list: frames 8, 9, 10, 11 on four
  // consecutive frames; the creation loops' rule). A frame's anchor is its bottom row.
  const drawLogos = (ctx, tick) => {
    for (const logo of logos) {
      const n = logo.dc6.frames.length;
      const frameIndex = Math.floor(tick * 25 / 60) % (n - 1);
      const frame = logo.dc6.frames[frameIndex];
      if (!frame) continue;
      const fire = logo.path.toLowerCase().includes('fire');
      const left = 400 + frame.offsetX, top = 120 + frame.offsetY - frame.height + 1;
      if (fire) logoBlend(ctx, frame, left, top, 3);
      else ctx.drawImage(logo.canvas(frameIndex), left, top);
      recordDc6(logo.path, frameIndex, frame, left, top, { op: 'cel', mode: fire ? 3 : 5 });
    }
  };
  return {
    // F91: the front end's own font for a notice drawn by the client rather than by retail (the
    // EXIT DIABLO II notice, play/assets/exit-sequence.mjs) -- the same gold FontFormal12 the two
    // copyright lines use, so the one screen the browser leaves behind still looks like this client.
    noticeFont: trademarkFont,
    get evidence() {
      resolveTrademark();
      return {
        background: { width: background.width, height: background.height, rows: background.rows, columns: background.columns },
        logoFrames: logos.map((l) => l.dc6.frames.length),
        button: { width: buttons.wide[0].width, height: buttons.wide[0].height, states: buttons.wide.length },
        boxes: boxes.map((b) => ({ label: b.item.label, kind: b.kind, x: b.x, y: b.y, w: b.w, h: b.h })),
        fontGlyphs: font.measure('SINGLE PLAYER'),
        lineHeight: font.lineHeight,
        pointer: { hot: hot >= 0 ? items[hot]?.label ?? null : null, pressed: pressed >= 0 ? items[pressed]?.label ?? null : null },
        // The Battle.net config as the page read it, and -- when it listed servers -- the identity of
        // what the region dialog actually painted. Empty/absent file: `{ enabled: false }`, and the
        // gateway bar is retail's own text.
        gateways: { ...bnetState, bar: gatewaySelector ? gatewaySelector.gatewayBar() : DEFAULT_GATEWAY_LABEL,
          barDrawn, ...(gatewaySelector ? { dialog: gatewaySelector.evidence } : { dialog: null }) },
        trademark: {
          background: { width: trademarkBackground.width, height: trademarkBackground.height,
            rows: trademarkBackground.rows, columns: trademarkBackground.columns, path: FRONTEND.trademark },
          font: FRONTEND.trademarkFont, colour: TRADEMARK_LAYOUT.colour, error: trademarkError,
          lines: trademarkLines.map((l) => ({ string: l.string, text: l.text, x: l.x, y: l.y,
            glyphs: glyphCount(l.text) })),
        },
      };
    },
    hitTest(x, y, width, height) {
      // The region dialog is modal over the menu: while it is up, no menu entry is under the pointer.
      if (gatewaySelector?.open) return -1;
      const v = view(width, height);
      const sx = (x - v.ox) / v.scale, sy = (y - v.oy) / v.scale;
      const i = boxes.findIndex((b) => sx >= b.x && sx < b.x + b.w && sy >= b.y && sy < b.y + b.h);
      return i < 0 ? -1 : items.indexOf(boxes[i].item);
    },
    labelAt(index) { return items[index]?.label ?? null; },
    /** Canvas-pixel centres of the SAME boxes hitTest() and click() use. */
    targets(width, height) {
      const v = view(width, height);
      return Object.fromEntries(boxes.map((b) => [b.item.label, [
        Math.round(v.ox + (b.x + b.w / 2) * v.scale), Math.round(v.oy + (b.y + b.h / 2) * v.scale),
      ]]));
    },
    /** Whether the Battle.net config listed servers, and the region dialog's row centres if so. */
    gatewaysOpen() { return Boolean(gatewaySelector?.open); },
    key(code) { return gatewaySelector?.key(code) ?? false; },
    // The retail gateway panel (4 GWListBox tiles) and a button blank, as CSS urls for the realm login dialog.
    gatewayArt() {
      const c = documentObject.createElement('canvas'); c.width = GATEWAY_LAYOUT.panel.w; c.height = GATEWAY_LAYOUT.panel.h;
      const ctx = c.getContext('2d');
      for (const t of GATEWAY_LAYOUT.panel.tiles) ctx.drawImage(frameCanvas(gatewayPanelDc6.frames[t.frame], skyPalette, documentObject), t.x, t.y);
      return { panelImage: `url(${c.toDataURL()})`, buttonImage: `url(${gatewayButtonCanvas.toDataURL()})` };
    },
    openGateways() { return gatewaySelector ? gatewaySelector.openDialog() : false; },
    gatewayTargets(width, height) { return gatewaySelector ? gatewaySelector.targets(width, height) : {}; },
    hover(x, y, width, height) {
      const v = view(width, height);
      const sx = (x - v.ox) / v.scale, sy = (y - v.oy) / v.scale;
      pointer = [Math.trunc(sx), Math.trunc(sy)];
      if (gatewaySelector?.open) return gatewaySelector.hover(sx, sy);
      hot = this.hitTest(x, y, width, height); return hot;
    },
    click(x, y, width, height) {
      if (gatewaySelector?.open) {
        const v = view(width, height), sx = (x - v.ox) / v.scale, sy = (y - v.oy) / v.scale;
        const token = gatewaySelector.press(sx, sy);
        return token === null ? -1 : gatewaySelector.release(token, sx, sy);
      }
      const i = this.hitTest(x, y, width, height);
      if (i >= 0) items[i].action?.();
      return i;
    },
    // Press arms the button under the pointer and shows its down cel; the release fires only when
    // it is still over the same button. Hover alone never draws the down cel (retail's mouse-up).
    press(x, y, width, height) {
      if (gatewaySelector?.open) { const v = view(width, height); return gatewaySelector.press((x - v.ox) / v.scale, (y - v.oy) / v.scale); }
      hot = this.hitTest(x, y, width, height);
      pressed = hot; return pressed >= 0 ? pressed : null;
    },
    release(token, x, y, width, height) {
      if (gatewaySelector?.open) { const v = view(width, height); return gatewaySelector.release(token, (x - v.ox) / v.scale, (y - v.oy) / v.scale); }
      const i = this.hitTest(x, y, width, height); pressed = -1;
      if (i >= 0 && i === token) items[i].action?.();
      return i >= 0 && i === token ? i : -1;
    },
    /** A cancelled gesture drops the down cel without activating the button. */
    cancel() { if (gatewaySelector?.open) gatewaySelector.cancel(); pressed = -1; },
    draw(outCtx, width, height, tick, phase = 'menu') {
      outCtx.fillStyle = '#000'; outCtx.fillRect(0, 0, width, height);
      const v = view(width, height);
      const ctx = sceneCtx;
      ctx.fillStyle = '#000'; ctx.fillRect(0, 0, 800, 600);
      const art = phase === 'trademark' ? trademarkBackground : background;
      drawBg(ctx, art.path, art.canvas);
      recordTiled(art.path, art, 0, 0);
      drawLogos(ctx, tick);
      if (phase === 'trademark') {
        // Retail's pre-menu frame is the art, the logo and the two copyright lines -- nothing else:
        // no buttons, no `v 1.13`, and no pointer cel.
        resolveTrademark();
        let glyphs = 0;
        for (const line of trademarkLines) {
          if (!line.text) continue;
          trademarkFont.drawAnchored(ctx, line.text, line.x, line.y);
          glyphs += glyphCount(line.text);
        }
        present(outCtx, scene, v, art.path);
        return { phase, buttons: 0, glyphs, hot: -1 };
      }
      // The region dialog is a MODAL SCREEN, not an overlay on the menu: the menu entries are not drawn
      // at all while it is up, so a player cannot read a region row as a relabelled menu button.
      if (gatewaySelector?.open) {
        gatewaySelector.draw(ctx);
      } else for (const b of drawOrder) {
        if (b.gateway) {
          const g = buttons.narrow[0];
          ctx.drawImage(g.canvas, b.gateway.x, b.gateway.y);
          recordButton(g, b.gateway.x, b.gateway.y);
          // Retail's own text until the player picks a region (`gatewayBar`); either way the glyphs we
          // blit are recorded, so a scenario compares the PAINTED bar to the configured region.
          const glyphs = [];
          const bar = gatewaySelector ? gatewaySelector.gatewayBar() : b.gateway.label;
          drawLabel(ctx, bar, b.gateway.x, b.gateway.y, g.width, g.height, 0, 0, (gl) => glyphs.push(gl));
          barDrawn = { text: glyphs.map((gl) => gl.character).join(''), glyphs: glyphs.map((gl) => ({ character: gl.character, index: gl.index, path: gl.path })) };
        }
        // Hover keeps retail's up cel; only a held pointer gets the down cel.
        const down = isButtonHeldOverTarget(pressed, hot) && items.indexOf(b.item) === pressed;
        const state = down ? 1 : 0;
        ctx.drawImage(buttons[b.kind][state].canvas, b.x, b.y);
        recordButton(buttons[b.kind][state], b.x, b.y);
        drawLabel(ctx, b.item.label, b.x, b.y, b.w, b.h, down ? -2 : 0, down ? 2 : 0);
      }
      if (!gatewaySelector?.open) versionFont.drawAnchored(ctx, TITLE_VERSION.text, TITLE_VERSION.x, TITLE_VERSION.y);
      // Retail draws the pointer last (D2WinMain.cpp), so the region dialog goes under the pointer cel.
      if (pointer) {
        const f = cursorDc6.frames[0];
        // Like every other retail screen (charselect.mjs), D2Win anchors the ohand cel one pixel
        // above the pointer; title.txt captures this at pointer (799,599), blit (799,598).
        const left = pointer[0] + f.offsetX, top = pointer[1] - 1 + f.offsetY - f.height + 1;
        ctx.drawImage(cursorCanvas, left, top);
        recordDc6(FRONTEND.cursor, 0, f, left, top);
      }
      present(outCtx, scene, v, art.path);
      return { phase, buttons: gatewaySelector?.open ? 0 : boxes.length, hot };
    },
  };
}
