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
// WHEN retail draws it -- TWO conditions, both measured, and the caller (`gameplay-shell.mjs`, the
// `hudHelp &&` gate) implements both (F440, s1570):
//
//   1. per MACHINE. D2Client draws the badge only while the Storm registry value "Help Menu" is unset;
//      the `H` key and the button's own click set it, so the FIRST use of Help removes it for good
//      (F210, from D2Client.dll's `6fae7740` draw and `6faeba70` H handler). The
//      controlled pair is `the retail captures` vs `f210-help-after-h.txt`: one
//      machine (`~/.wine-helpbtn`, a prefix with no "Help Menu" value), one scene, one character, `H`
//      pressed between the two takes -- the first frame carries the socket, the gem and the eight
//      Font16 glyphs at (714,403), the second carries none. The caller's store for that value is
//      localStorage `d2.helpMenu`. **This is why most in-game captures carry no label**: 290 of the 464
//      committed draw lists have no cel at (714,403), and they are the ones taken on a prefix that had
//      already used Help (every `cov-a4*` capture -- that session also took the Help screen). The note
//      that used to stand here, "every in-game capture has it", was never measured; F440 was filed on it
//      and it is refuted, and `s1570` measures both arms at retail's own act4 Chaos Sanctuary
//      coordinate against retail's own records.
//   2. per PANEL. On one machine that draws both (`~/.wine-drawlist`, the oracle prefix) the same
//      character draws neither with the skill tree (`skilltree.txt`, `skilltree-act5.txt`), the stash
//      (`stash-town.txt`) or a shop (`vendor-akara.txt`, `vendor-malah-act5.txt`) open, and draws both
//      in the field (`town.txt`) and with the character panel open (`charpanel.txt`), where the left
//      half is covered and the right half is not. So the caller gates it on the right-hand half being
//      free, not on "a panel is open" -- the F10 lane note that said retail draws no armoured HUD over a
//      panel screen was wrong, about this and about the mini-panel: retail draws both with the
//      character screen open, and neither with the skill tree open.
import { createFont, decodeFontTbl } from './frontend.mjs';
import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { recordDc6 } from './drawlist.mjs';

export const HUD_HELP_ART = 'data\\local\\font\\latin\\font16.dc6';
export const HUD_HELP_TBL = 'data\\local\\font\\latin\\font16.tbl';
export const HUD_HELP_TEXT = 'Help (H)';
export const HUD_HELP_ANCHOR = Object.freeze({ x: 714, y: 403 });   // screen 800x600, bottom row

// The button under the label: retail draws `Levelsocket.dc6` frame 0 (bottom row 440) then `Level.dc6`
// frame 0 (bottom row 436, the cross gem) straight after the label's glyphs, in every capture that
// draws the label and no other (the retail captures: 39+3 of 42 with both; the 7 without it are the
// controls overlay covering the spot). Not gated on skill points: the fresh-character town-start
// frame has it. Retail's y is a bottom row, recordDc6 takes the top.
const BUTTON = [['data\\global\\ui\\PANEL\\levelsocket.dc6', 725, 440], ['data\\global\\ui\\PANEL\\level.dc6', 728, 436]];

export function createHudHelp(read, documentObject = globalThis.document) {
  const dc6 = decodeDc6(read(HUD_HELP_ART)), tbl = decodeFontTbl(read(HUD_HELP_TBL));
  const perAct = new Map();
  const button = BUTTON.map(([path, x, bottom]) => {
    const frame = decodeDc6(read(path)).frames[0];
    return { path, x, top: bottom - frame.height + 1, frame };
  });
  let buttonCanvas = null;
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
      buttonCanvas ??= (() => {
        const pal = decodeDatPalette(read('data\\global\\palette\\ACT1\\pal.dat'));
        return button.map(({ frame }) => { const c = documentObject.createElement('canvas'); c.width = frame.width; c.height = frame.height;
          c.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, pal), frame.width, frame.height), 0, 0); return c; });
      })();
      const buttonRecords = button.map(({ path, x, top, frame }, i) => {
        ctx.drawImage(buttonCanvas[i], x, top);
        return recordDc6(path, 0, frame, x, top, { op: 'cel', mode: 5 });
      });
      // The socket's own rect (the button a player presses; retail's Help (H) button), local to the HUD origin.
      const sk = button[0], hit = { x: sk.x, y: sk.top, w: sk.frame.width, h: sk.frame.height };
      return { hit, button: buttonRecords, text: HUD_HELP_TEXT, art: HUD_HELP_ART, anchor: { ...HUD_HELP_ANCHOR }, act,
        width: font.measure(HUD_HELP_TEXT),
        glyphs: [...HUD_HELP_TEXT].map((ch) => font.glyph(ch.codePointAt(0))?.index ?? -1) };
    },
  };
}
