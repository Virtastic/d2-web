// F115 -- renamed from F109 at the cycle-273 merge, because act IV's F109 (the
// River of Flame waypoint) landed first. The AREA NAME block: the level's own name above `v 1.13` and
// `EXPANSION`, in font16 at colour 4, at the top right. Nothing under `play/` drew any of it before
// this module; the only place the client ever drew `v 1.13` was the title screen's bottom-left corner
// (frontend.mjs TITLE_VERSION).
//
// WHOSE ELEMENT IT IS: THE AUTOMAP OVERLAY'S, NOT A TIMED PLAY-SCREEN WINDOW. F109's premise -- that
// the play screen draws this block for a window after a level is entered -- was a MISREAD REFERENCE:
// the two captures it was derived from are automap-UP captures, and the block is retail's automap
// label. Measured over the whole draw-list corpus: the only three vectors that carry any
// of the three text records are the only three that carry automap cells -- `a3-forest-walkin.txt`
// (826 maximap draws, the record at :1033, after that capture's last automap cell at :1020),
// `f5b-automap-town.txt` (890) and `v6-automap-town.txt` (885) -- while their automap-OFF twins
// `f5b-automap-town-off.txt` and `v6-automap-town-off.txt` have 0 automap draws AND none of the three
// lines, in the same scene, standing in the same place -- with the belt digits (`1`..`4`) and the
// `Mana:` line still drawn in both, the `Help (H)` label in `f5b`'s twin. No other vector under
// the retail captures carries any of the three.
// So the client draws the block exactly while the automap is open (gameplay-shell.mjs gates on the
// automap's own state), and the old `windowTicks` constant is GONE rather than replaced: there is no
// timed window to measure. `a3-spider-forest-seam.txt` and `a3-great-marsh-wp.txt` -- later frames of
// the same levels, which F109 read as the window having closed -- are automap-OFF captures.
//
// THE ELEMENT RETAIL DOES DRAW ON ARRIVAL IS A DIFFERENT ONE, and this module is not it: two unnamed
// celcolor cels (`(211,160) 256x26` and `(467,160) 120x26`, mode 5) in `a3-marsh-wp-timed.txt:427-428`,
// held for 120 frames from the arrival (that frame count is the timed capture run's --
// `retail-capture-rc77` `e01f80a3`; the two cel records themselves are in the vector). They are an
// image with no text record and no nameable cel
// source, so nothing under `play/` draws them and nothing may until their cels are identified. That is
// a gap; the maintainer owns its row id.
//
// THE ANCHORS ARE RETAIL'S OWN RECORDS, NOT PIXELS. The same captures carry the whole block, and
// between them the right edge is DERIVED rather than guessed -- retail right-aligns the block on
// x=784 (= 800 - 16), which is exactly where all four records end when they are laid out with
// font16's own .tbl advances:
//
//   capture                    record                                  left   advance   right
//   a3-forest-walkin.txt:1033  text 'Spider Forest'    x=684 y=24     684    100       784
//   f5b-automap-town.txt:1156  text 'Rogue Encampment' x=643 y=24     643    141       784
//   a3-forest-walkin.txt:1047  text 'v 1.13'           x=743 y=40     743     41       784
//   a3-forest-walkin.txt:1054  text 'EXPANSION'        x=700 y=56     700     84       784
//
// Four records, two different name lengths (13 and 16 characters) and two fixed lines, one single
// right edge: the rule has no free parameter left, so `x = right - measure(text)` reproduces every
// one of retail's own anchors. The block's first write-up said the alignment rule was NOT measured and
// the two name anchors were unrelated left edges (x=684 and x=643); the arithmetic above refutes that
// and is why this module can draw a name retail never captured (Flayer Jungle, also 13 characters,
// lands on 684 like the Spider Forest). The anchors survive the re-scoping of the row: they are
// retail's own records, and they are the automap's own anchors.
//
// Prior art: none. D2MOO (1.10f) has no D2Client at all, and no other catalogued repo draws this
// block, so the anchors above are the whole specification and retail's own records are the oracle.
import { createFont, decodeFontTbl, textColourPalette } from './frontend.mjs';
import { decodeDatPalette, decodeDc6 } from './d2-formats.mjs';

export const AREA_NAME_BLOCK = Object.freeze({
  font: 'data\\local\\font\\latin\\font16.dc6',
  fontTbl: 'data\\local\\font\\latin\\font16.tbl',
  // The game screen's palette is the act's (the same one every other in-game panel module reads).
  palette: (act) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\pal.dat`,
  // D2Win's text colour index: retail's own `color=4` on every record of the block, carried onto each
  // glyph blit as the draw list's `extra` (recordDc6), the field s198 asserts for the trademark screen.
  colour: 4,
  // 800 - 16: retail's right edge for the whole block, fitted exactly by the four records in the
  // header. Not a margin this file chose -- it is what the four anchors have in common.
  right: 784,
  // The block's three rows, retail's own bottom-row anchors (font16 is 16 rows tall: 24, 40, 56).
  rows: Object.freeze({ name: 24, version: 40, expansion: 56 }),
  // Retail draws these two as literals under the name (they are the build's, not a level's): the
  // title screen already draws `v 1.13` from its own constant (frontend.mjs TITLE_VERSION), and
  // neither string is a .tbl entry in retail's three tables.
  version: 'v 1.13',
  expansion: 'EXPANSION',
});

/**
 * Where retail starts a right-aligned line: `measure` is the font's own advance sum (the .tbl's byte
 * 3 per code point, which is what D2Win lays text out with -- the DC6 cell width is not the advance).
 */
export function rightAlignedX(measure, text, right = AREA_NAME_BLOCK.right) {
  return right - measure(text);
}

export function createAreaNameBlockRenderer(read, documentObject = globalThis.document) {
  const perAct = new Map();
  // createFont's glyph callback is set once per font, so the capture list is swapped per line: each
  // blit reaches it in call order, which is what makes the evidence per-blit rather than per-line.
  let capture = null, dc6 = null, tbl = null;
  const fontFor = (act) => {
    let entry = perAct.get(act);
    if (!entry) {
      // Lazy, like the other frame-loop renderers: a font the MPQs do not carry must cost the block,
      // not the session (the caller's draw is wrapped, and the shell's bootstrap is not).
      if (!dc6) { dc6 = decodeDc6(read(AREA_NAME_BLOCK.font)); tbl = decodeFontTbl(read(AREA_NAME_BLOCK.fontTbl)); }
      const path = AREA_NAME_BLOCK.palette(act);
      const palette = decodeDatPalette(read(path));
      let shifted = false, glyphPalette = palette;
      try {
        // Colour 4 is not an RGB mix: it is the PL2 TextColorShifts row, the same table the death
        // screen's red and character creation's button labels go through. A missing PL2 must never
        // be reported as a colour shift -- it draws the act palette unchanged and says so.
        glyphPalette = textColourPalette(read(path.replace(/pal\.dat$/i, 'pal.pl2')), palette, AREA_NAME_BLOCK.colour);
        shifted = true;
      } catch { shifted = false; }
      entry = { palette: path, shifted, font: createFont({ dc6, tbl, palette: glyphPalette, documentObject,
        path: AREA_NAME_BLOCK.font, drawAs: { op: 'celcolor', mode: 5, extra: AREA_NAME_BLOCK.colour },
        onGlyph: (blit) => { if (capture) capture.push(blit); } }) };
      perAct.set(act, entry);
    }
    return entry;
  };
  return {
    block: AREA_NAME_BLOCK,
    /**
     * Draw the block for `name` -- the CURRENT level's own name, as the engine gives it
     * (`d2_level_name`, already resolved through `d2_lang_string` by the caller). A name is required:
     * this module never invents, caches or defaults a level name, because the browser holding a second
     * copy of the level table is exactly the divergence F115 is about.
     */
    draw(ctx, name, act = 1) {
      const text = String(name || '');
      if (!text) throw new Error('the area name block needs the engine\'s own level name');
      const entry = fontFor(act);
      const lines = [];
      const line = (key, body, y) => {
        const x = rightAlignedX((t) => entry.font.measure(t), body);
        const glyphs = [];
        capture = glyphs;
        try { entry.font.drawAnchored(ctx, body, x, y); } finally { capture = null; }
        lines.push({ key, text: body, x, y, glyphs });
      };
      line('name', text, AREA_NAME_BLOCK.rows.name);
      line('version', AREA_NAME_BLOCK.version, AREA_NAME_BLOCK.rows.version);
      line('expansion', AREA_NAME_BLOCK.expansion, AREA_NAME_BLOCK.rows.expansion);
      return { name: text, act, colour: AREA_NAME_BLOCK.colour, right: AREA_NAME_BLOCK.right,
        font: AREA_NAME_BLOCK.font, palette: entry.palette, paletteShifted: entry.shifted, lines };
    },
  };
}
