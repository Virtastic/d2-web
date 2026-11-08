// The BYO front door's art, cut from the 1.13c archives: the burning logo, the wide button, the hand cursor, the join-game
// loading screen and three bitmap fonts in five colours. One builder for both callers: tools/door-art.mjs
// writes the result as PNGs at stage time, and the door rebuilds it in the browser from the player's own
// archives when the staged copy is missing. `rd(name)` returns a file's bytes (first archive wins) or throws.
import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { decodeFontTbl, textColourPalette } from './frontend.mjs';

const FE = 'data\\global\\ui\\FrontEnd\\', PAL = 'data\\global\\palette\\', FONT = 'data\\local\\font\\latin\\';
const COL = { white: 0, red: 1, gold: 4, grey: 5, tan: 7 };
// The upload popup (upload-modal.mjs) draws the Battle.net connecting popup's fonts: Font30 and Font24 plain, FontFormal12 gold.
export const DOOR_FONTS = [['font16', ['white', 'red', 'gold', 'grey', 'tan'], 'act1'], ['font30', ['gold'], 'act1'], ['fontexocet10', ['white'], 'sky'],
  ['font30', ['white'], 'sky'], ['font24', ['white'], 'sky'], ['fontformal12', ['gold'], 'sky']];
export const DOOR_IMAGES = ['logo_blackL', 'logo_fireL', 'logo_blackR', 'logo_fireR', 'wide', 'loading', 'cursor', 'popup', 'medium',
  ...DOOR_FONTS.flatMap(([n, cols]) => cols.map((c) => `f_${n}_${c}`))];

const surf = (w, h) => ({ w, h, px: new Uint8ClampedArray(w * h * 4) });
function blit(s, f, pal, x0, y0) {
  const rgba = dc6FrameToRgba(f, pal);
  for (let y = 0; y < f.height; y++) for (let x = 0; x < f.width; x++) {
    const i = y * f.width + x, X = x0 + x, Y = y0 + y;
    if (f.opaque[i] && X >= 0 && Y >= 0 && X < s.w && Y < s.h) s.px.set(rgba.subarray(i * 4, i * 4 + 4), (Y * s.w + X) * 4);
  }
}

/** -> { meta, images: { name: { w, h, px } } } */
export async function buildDoorArt(rd) {
  const bytes = async (n) => new Uint8Array(await rd(n));
  const dc6 = async (n) => decodeDc6(await bytes(n));
  const P = async (n) => decodeDatPalette(await bytes(`${PAL}${n}\\pal.dat`));
  const pal = { units: await P('Units'), sky: await P('Sky'), act1: await P('act1'), loading: await P('Loading') };
  const pl2 = { act1: await bytes(`${PAL}ACT1\\pal.pl2`), sky: await bytes(`${PAL}Sky\\pal.pl2`) };
  const meta = { fonts: {} }, images = {};
  // The logo: every frame side by side, each with [x, w, h, offsetX, offsetY].
  for (const [k, n] of [['blackL', 'D2LogoBlackLeft'], ['fireL', 'D2logoFireLeft'], ['blackR', 'D2LogoBlackRight'], ['fireR', 'D2logoFireRight']]) {
    const d = await dc6(FE + n + '.DC6'), s = surf(d.frames.reduce((a, f) => a + f.width, 0), Math.max(...d.frames.map((f) => f.height)));
    let x = 0; meta['logo_' + k] = d.frames.map((f) => { blit(s, f, pal.units, x, 0); const r = [x, f.width, f.height, f.offsetX, f.offsetY]; x += f.width; return r; });
    images['logo_' + k] = s;
  }
  // WideButtonBlank: two halves per state, up above pressed (272x70).
  { const fr = (await dc6(FE + 'WideButtonBlank.dc6')).frames, s = surf(272, 70);
    for (const st of [0, 1]) { blit(s, fr[st * 2], pal.sky, 0, st * 35); blit(s, fr[st * 2 + 1], pal.sky, fr[st * 2].width, st * 35); }
    images.wide = s; }
  // The hand cursor (ohand), frame 0 in the title screen's palette, with [w, h, offsetX, offsetY].
  { const f = (await dc6('data\\global\\ui\\CURSOR\\ohand.DC6')).frames[0], s = surf(f.width, f.height);
    blit(s, f, pal.sky, 0, 0); images.cursor = s; meta.cursor = [f.width, f.height, f.offsetX, f.offsetY]; }
  // The upload popup: PopUp_340x224's two cels and MediumButtonBlank's up and pressed cels, each a strip with [x, w, h, offsetX, offsetY].
  for (const [k, n] of [['popup', 'PopUp_340x224.dc6'], ['medium', 'MediumButtonBlank.dc6']]) {
    const fr = (await dc6(FE + n)).frames.slice(0, 2), s = surf(fr.reduce((a, f) => a + f.width, 0), Math.max(...fr.map((f) => f.height)));
    let x = 0; meta[k] = fr.map((f) => { blit(s, f, pal.sky, x, 0); const r = [x, f.width, f.height, f.offsetX, f.offsetY]; x += f.width; return r; });
    images[k] = s;
  }
  // The join-game loading screen: its frames in a strip of 256x256 cells.
  { const fr = (await dc6('data\\global\\ui\\Loading\\loadingscreen.dc6')).frames, s = surf(256 * fr.length, 256);
    fr.forEach((f, i) => blit(s, f, pal.loading, i * 256, 0)); images.loading = s; meta.loading = fr.length; }
  // Fonts: one atlas per (font, colour), glyph i in cell (i % 16, i / 16) of the largest frame's size.
  for (const [name, cols, p] of DOOR_FONTS) {
    const d = await dc6(`${FONT}${name}.dc6`), tbl = decodeFontTbl(await bytes(`${FONT}${name}.tbl`));
    const cw = Math.max(...d.frames.map((f) => f.width)), ch = Math.max(...d.frames.map((f) => f.height));
    const glyphs = {}; for (const [code, c] of tbl.chars) if (code < 256) glyphs[code] = [c.imageIndex, c.width];
    meta.fonts[name] = { cw, ch, glyphs, frames: d.frames.map((f) => [f.width, f.height, f.offsetX, f.offsetY]) };
    for (const c of cols) {
      const cp = COL[c] ? textColourPalette(pl2[p], pal[p], COL[c]) : pal[p], s = surf(cw * 16, ch * Math.ceil(d.frames.length / 16));
      d.frames.forEach((f, i) => blit(s, f, cp, (i % 16) * cw, ((i / 16) | 0) * ch));
      images[`f_${name}_${c}`] = s;
    }
  }
  return { meta, images };
}
