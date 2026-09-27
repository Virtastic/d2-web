// Retail's "Help (H)" label on the in-game HUD -- the last piece of F10 that
// is not the mini-panel's icon row or the stamina bar (those are V8's).
//
// The anchor, the font and the string are retail's OWN, not derived from pixels:
//
//   * `the retail captures` draws `text 'Help (H)' x=714 y=403` and then eight
//     `celcolor DATA\LOCAL\Font\Latin\Font16.dc6` glyphs at x 714/723/730/737/744/752/757/766, every
//     one of them on row 403 -- one glyph per character of "Help (H)", bottom row 403.
//   * `retail/frames/town-start.png` shows it there too, above the mana orb.
//   * The string is NOT in the string tables: `strings` finds no "Help (H)" in string.tbl,
//     expansionstring.tbl or patchstring.tbl, so it is a literal in D2Client, drawn as one here.
//
// WHEN retail draws it, measured rather than guessed -- the label is absent from the three scenes
// whose own draw lists carry no Font16 cel at (714,403): the skill tree (`skilltree.txt`,
// `skilltree-act5.txt`), the stash (`stash-town.txt`) and a shop (`vendor-akara.txt`,
// `vendor-malah-act5.txt`). Every in-game capture has it, and so does the character panel's
// (`charpanel.txt`), where the left half is covered and the right half is not. So the caller gates it
// on the right-hand half being free, not on "a panel is open" -- the F10 lane note that said retail
// draws no armoured HUD over a panel screen was wrong, about this and about the mini-panel: retail
// draws both with the character screen open, and neither with the skill tree open.
import { createFont, decodeFontTbl } from './frontend.mjs';
import { decodeDatPalette, decodeDc6 } from './d2-formats.mjs';

export const HUD_HELP_ART = 'data\\local\\font\\latin\\font16.dc6';
export const HUD_HELP_TBL = 'data\\local\\font\\latin\\font16.tbl';
export const HUD_HELP_TEXT = 'Help (H)';
export const HUD_HELP_ANCHOR = Object.freeze({ x: 714, y: 403 });   // screen 800x600, bottom row

export function createHudHelp(read, documentObject = globalThis.document) {
  const dc6 = decodeDc6(read(HUD_HELP_ART)), tbl = decodeFontTbl(read(HUD_HELP_TBL));
  const perAct = new Map();
  const fontFor = (act) => {
    let f = perAct.get(act);
    if (!f) {
      // `path` makes every glyph a draw-list record (drawlist.mjs), so a row can compare the label
      // draw by draw: path, frame, frame-bytes hash, anchor, mode and colour shift.
      f = createFont({ dc6, tbl, palette: decodeDatPalette(read(`data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\pal.dat`)),
        documentObject, path: HUD_HELP_ART, drawAs: { op: 'celcolor', mode: 5 } });
      perAct.set(act, f);
    }
    return f;
  };
  return {
    text: HUD_HELP_TEXT,
    anchor: HUD_HELP_ANCHOR,
    art: HUD_HELP_ART,
    /** Draw the label at retail's anchor. Returns what was asked for; the blits go to the draw list. */
    draw(ctx, act = 1) {
      const font = fontFor(act);
      font.drawAnchored(ctx, HUD_HELP_TEXT, HUD_HELP_ANCHOR.x, HUD_HELP_ANCHOR.y);
      return { text: HUD_HELP_TEXT, art: HUD_HELP_ART, anchor: { ...HUD_HELP_ANCHOR }, act,
        width: font.measure(HUD_HELP_TEXT),
        glyphs: [...HUD_HELP_TEXT].map((ch) => font.glyph(ch.codePointAt(0))?.index ?? -1) };
    },
  };
}
