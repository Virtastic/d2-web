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
// THE RULE IS D2CLIENT'S OWN (1.13c), NOT A FIT TO 800x600. The automap overlay draw D2Client+0x62680
// (called from the frame draw 0xC39E0 while the automap is up) sets font 1 (font16) with D2Win
// #10184, resets the line cursor DAT_6fbcc21c to 0x18 (24) and calls, in this order:
//   0x5FFE0  `Game: <name>` (eng string 4181) when the game info's name (+0x1b) is non-empty, then
//            `Password: <pw>` (4182) when +0x241 is non-empty; each advances the cursor when drawn;
//   0x60AA0  the current level's name (D2Common #10826, then 0xBE240); advances when drawn;
//   0x5FE90  `v %d.%02d` from the launcher's version getter; the cursor advances ALWAYS;
//   0x5FD20  `Difficulty: %s` (4183) with `Nightmare` (5155) at difficulty 1, `Hell` (5154) at 2,
//            nothing at Normal (DAT_6fbcc390);
//   0x5FE10  the host address, only for game types 6 and 8 (DAT_6fbcc394);
//   0x5FF70  `EXPANSION` (expansion string 22730), only when DAT_6fbc9854 is set.
// Each line is D2Win #10150(text, x, y, 4, 0) with x = DAT_6fba9e14 - #10028(text) - 16, where
// DAT_6fba9e14 is the screen width (the resolution switch at 0x6fac0e58 copies DAT_6fb8bc48 into it).
// So the block is right-aligned 16px inside the SCREEN's right edge at every resolution (640: 624,
// 800: 784, a 2000px view: 1984), the rows step 16px from y=24, and the automap is the only caller.
// At 800x600 the rule reproduces the four retail records this module was first fitted to (Normal
// single-player expansion games: no Game/Password, Difficulty or address line):
//
//   capture                    record                                  left   advance   right
//   a3-forest-walkin.txt:1033  text 'Spider Forest'    x=684 y=24     684    100       784
//   f5b-automap-town.txt:1156  text 'Rogue Encampment' x=643 y=24     643    141       784
//   a3-forest-walkin.txt:1047  text 'v 1.13'           x=743 y=40     743     41       784
//   a3-forest-walkin.txt:1054  text 'EXPANSION'        x=700 y=56     700     84       784
//
// The module's first version hard-coded right=784, so on any screen wider than 800 the block sat
// short of the right edge (1216px short on a 2000px view).
//
// Prior art: none. D2MOO (1.10f) has no D2Client at all; the rule above is read from the 1.13c
// disassembly of the functions named.
import { createFont, decodeFontTbl, textColourPalette } from './frontend.mjs';
import { decodeDatPalette, decodeDc6 } from './d2-formats.mjs';
import { versionLabel } from './build-version.mjs';

export const AREA_NAME_BLOCK = Object.freeze({
  font: 'data\\local\\font\\latin\\font16.dc6',
  fontTbl: 'data\\local\\font\\latin\\font16.tbl',
  // The game screen's palette is the act's (the same one every other in-game panel module reads).
  palette: (act) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\pal.dat`,
  // D2Win's text colour index: retail's own `color=4` on every record of the block, carried onto each
  // glyph blit as the draw list's `extra` (recordDc6), the field s198 asserts for the trademark screen.
  colour: 4,
  // x = screenWidth - width - 16 (every line function: `sub edx,eax; sub edx,0x10` on DAT_6fba9e14).
  margin: 16,
  // The line cursor DAT_6fbcc21c: 0x18 at the start of 0x62680, += 0x10 per line.
  top: 24,
  step: 16,
  // 0x5FE90 formats `v %d.%02d` from the launcher's version getter: `v 1.13` in retail. Shipped builds
  // show the release version (build-version.mjs); the parity harness keeps `v 1.13`.
  get version() { return versionLabel(); },
  // The eng .tbl texts the other line functions read through D2Lang (indexes in the header).
  strings: Object.freeze({ game: 'Game: ', password: 'Password: ', difficulty: 'Difficulty: %s',
    nightmare: 'Nightmare', hell: 'Hell', expansion: 'EXPANSION' }),
});

/**
 * Where retail starts a right-aligned line: `measure` is the font's own advance sum (the .tbl's byte
 * 3 per code point, which is what D2Win #10028 lays text out with -- the DC6 cell width is not the
 * advance), `screenWidth` the game's screen width (D2Client DAT_6fba9e14).
 */
export function rightAlignedX(measure, text, screenWidth) {
  return (screenWidth | 0) - measure(text) - AREA_NAME_BLOCK.margin;
}

/**
 * D2Client+0x62680's line sequence: which lines show and on which row. `gameName`/`password`: the
 * game info's (+0x1b / +0x241, empty in single player); `difficulty` 0..2 (DAT_6fbcc390);
 * `address` the host address, given only for game types 6 and 8 (DAT_6fbcc394); `expansion`
 * DAT_6fbc9854. The version line takes its row even when it draws nothing (0x5FE90).
 */
export function areaNameBlockLines({ name = '', gameName = '', password = '', difficulty = 0, address = '',
  expansion = true, version = AREA_NAME_BLOCK.version } = {}) {
  const t = AREA_NAME_BLOCK.strings, out = [];
  let y = AREA_NAME_BLOCK.top;
  const put = (key, text) => { out.push({ key, text, y }); y += AREA_NAME_BLOCK.step; };
  if (gameName) put('game', t.game + gameName);
  if (password) put('password', t.password + password);
  if (name) put('name', name);
  if (version) put('version', version); else y += AREA_NAME_BLOCK.step;
  if (difficulty === 1 || difficulty === 2) {
    put('difficulty', t.difficulty.replace('%s', difficulty === 2 ? t.hell : t.nightmare));
  }
  if (address) put('address', address);
  if (expansion) put('expansion', t.expansion);
  return out;
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
    draw(ctx, name, act = 1, { screenWidth = ctx?.canvas?.width ?? 800, ...state } = {}) {
      const text = String(name || '');
      if (!text) throw new Error('the area name block needs the engine\'s own level name');
      const entry = fontFor(act);
      const lines = [];
      for (const { key, text: body, y } of areaNameBlockLines({ ...state, name: text })) {
        const x = rightAlignedX((t) => entry.font.measure(t), body, screenWidth);
        const glyphs = [];
        capture = glyphs;
        try { entry.font.drawAnchored(ctx, body, x, y); } finally { capture = null; }
        lines.push({ key, text: body, x, y, glyphs });
      }
      return { name: text, act, colour: AREA_NAME_BLOCK.colour, right: screenWidth - AREA_NAME_BLOCK.margin,
        font: AREA_NAME_BLOCK.font, palette: entry.palette, paletteShifted: entry.shifted, lines };
    },
  };
}
