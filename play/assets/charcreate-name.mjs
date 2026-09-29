// Retail's character-creation name field. This is separate from the hero chooser because the field
// has its own font, bottom-row anchor and blinking glyph, measured from 1.13c `named.bin` (s730).
import { FRONTEND } from './frontend.mjs';

export const NAME_FONT = { font: FRONTEND.trademarkFont, fontTbl: FRONTEND.trademarkFontTbl };
export const NAME_FIELD = { x: 326, anchorY: 514 };
export const CARET_GLYPH = '_';
export const CARET_BLINK_MS = 250;

/** Build the renderer using this screen's retail asset reader. */
export function createCharacterNameRenderer({ read, decodeDc6, decodeFontTbl, createFont, palette, documentObject }) {
  let glyphs = null;
  const font = createFont({
    dc6: decodeDc6(read(NAME_FONT.font)), tbl: decodeFontTbl(read(NAME_FONT.fontTbl)),
    palette, documentObject, path: NAME_FONT.font,
    onGlyph: (blit) => { if (glyphs) glyphs.push(blit); },
  });
  const caretLit = (clock) => Math.floor(clock * 40 / CARET_BLINK_MS) % 2 === 0;
  return {
    caretLit,
    // The returned cels are the ones that reached the canvas, and are s730's identity evidence.
    draw(ctx, text, clock) {
      const drawn = { font: NAME_FONT.font, x: NAME_FIELD.x, anchorY: NAME_FIELD.anchorY, text, glyphs: [], caret: null };
      glyphs = drawn.glyphs;
      const pen = NAME_FIELD.x + font.drawAnchored(ctx, text, NAME_FIELD.x, NAME_FIELD.anchorY);
      glyphs = null;
      if (caretLit(clock)) {
        drawn.caretAt = pen;
        glyphs = [];
        font.drawAnchored(ctx, CARET_GLYPH, pen, NAME_FIELD.anchorY);
        drawn.caret = glyphs[0] ?? null;
        glyphs = null;
      }
      return drawn;
    },
  };
}
