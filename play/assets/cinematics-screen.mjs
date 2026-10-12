// The CINEMATICS screen the title's button opens (F89/F89b). Layout, order and draw modes are read off
// retail's own draw list; nothing here is a stand-in:
// gameselectscreenEXP background, CinematicsSelectionEXP dialog, Font24 heading, seven WideButtonBlank
// rows -- retail draws ALL seven and labels ONE (fresh character: only the first movie is unlocked;
// the other six draw at DrawMode 1, translucent, with no text) -- and a MediumButtonBlank CANCEL.
// Row i plays the i-th cinematic retail's D2Launch.dll names; the first, d2intro, is labelled
// "THE SISTER'S LAMENT". Whether a progressed save unlocks more rows is not captured (F89b).
import { createFont, createBlender, decodeFontTbl, composeTiled, recordTiled, textColourPalette, LABEL_TINT, FRONTEND, TITLE_COLUMNS } from './frontend.mjs';
import { recordDc6 } from './drawlist.mjs';
import { openMovie } from './cinematics.mjs';
import { createCinematicPlayback } from './cinematic-playback.mjs';

const P = 'data\\global\\ui\\FrontEnd\\';
export const CINEMATICS_ASSETS = {
  dialog: P + 'CinematicsSelectionEXP.dc6', row: P + 'WideButtonBlank.dc6', cancel: P + 'MediumButtonBlank.dc6',
  heading: 'data\\local\\font\\latin\\font24.dc6', headingTbl: 'data\\local\\font\\latin\\font24.tbl',
};
// Anchors are retail's (bottom row of each cel); rows sit at these y, the 256px body at x and 16px tail after.
export const CINEMATICS_LAYOUT = {
  dialogAt: [[237, 335], [493, 335], [237, 505], [493, 505]],
  headingX: 287, headingY: 133, headingColour: 4,
  rowX: 262, rowY: [181, 224, 268, 310, 353, 396, 439], cancel: { x: 334, y: 488 },
};
// Retail's seven, in row order (D2Launch.dll: d2intro, Act02start, Act03start, Act04start, Act04end,
// D2x_Intro, D2x_Out). Only the first is unlocked on a fresh character.
export const CINEMATICS = [
  { label: "THE SISTER'S LAMENT", path: 'data\\local\\video\\Eng\\d2intro640x292.bik' },
  { path: 'data\\local\\video\\Eng\\Act02start640x292.bik' },
  { path: 'data\\local\\video\\Eng\\Act03start640x292.bik' },
  { path: 'data\\local\\video\\Eng\\Act04start640x292.bik' },
  { path: 'data\\local\\video\\Eng\\Act04end640x292.bik' },
  { path: 'data\\local\\video\\ENG\\D2x_Intro_640x292.bik' },
  { path: 'data\\local\\video\\ENG\\D2x_Out_640x292.bik' },
];

export function createCinematicsScreen({ read, decodeDc6, decodeDatPalette, onExit = () => {}, string = () => '', documentObject = globalThis.document }) {
  const L = CINEMATICS_LAYOUT;
  const palette = decodeDatPalette(read(FRONTEND.backgroundPalette));
  const bg = composeTiled(decodeDc6(read(FRONTEND.background)), palette, TITLE_COLUMNS, documentObject);
  const canvasOf = (f) => {
    const c = documentObject.createElement('canvas'); c.width = Math.max(1, f.width); c.height = Math.max(1, f.height);
    const rgba = new Uint8ClampedArray(f.width * f.height * 4);
    for (let i = 0; i < f.indices.length; i++) if (f.opaque[i]) rgba.set(palette.subarray(f.indices[i] * 4, f.indices[i] * 4 + 4), i * 4);
    c.getContext('2d').putImageData(new ImageData(rgba, c.width, c.height), 0, 0); return c;
  };
  const cels = (path) => { const dc6 = decodeDc6(read(path)); return dc6.frames.map((frame) => ({ frame, canvas: canvasOf(frame) })); };
  const dialog = cels(CINEMATICS_ASSETS.dialog), row = cels(CINEMATICS_ASSETS.row), cancel = cels(CINEMATICS_ASSETS.cancel);
  const tinted = palette.slice(); for (let i = 0; i < tinted.length; i += 4) for (let k = 0; k < 3; k++) tinted[i + k] = Math.round(tinted[i + k] * LABEL_TINT);
  const blend = createBlender(read('data\\global\\palette\\Sky\\pal.pl2'), palette);
  const label = createFont({ dc6: decodeDc6(read(FRONTEND.font)), tbl: decodeFontTbl(read(FRONTEND.fontTbl)), palette: tinted, documentObject, path: FRONTEND.font, drawAs: { op: 'celcolor', mode: 4, blend } });
  const heading = createFont({ dc6: decodeDc6(read(CINEMATICS_ASSETS.heading)), tbl: decodeFontTbl(read(CINEMATICS_ASSETS.headingTbl)),
    palette: textColourPalette(read('data\\global\\palette\\Sky\\pal.pl2'), palette, L.headingColour), documentObject, path: CINEMATICS_ASSETS.heading });
  const cursor = cels(FRONTEND.cursor)[0];
  const scene = documentObject.createElement('canvas'); scene.width = 800; scene.height = 600;
  const sctx = scene.getContext('2d', { willReadFrequently: true });
  const strip = documentObject.createElement('canvas'); strip.width = 640; strip.height = 292;   // the film's own size, letterboxed
  const view = (w, h) => { const scale = Math.min(w / 800, h / 600); return { scale, ox: Math.round((w - 800 * scale) / 2), oy: Math.round((h - 600 * scale) / 2) }; };
  // Controls: unlocked rows (272x35) and CANCEL. A cel's anchor is its bottom row: top = y - h + 1.
  const rowW = row[0].frame.width + row[1].frame.width, rowH = row[0].frame.height;
  const boxes = [
    ...L.rowY.map((y, i) => ({ id: `row${i}`, i, x: L.rowX, y: y - rowH + 1, w: rowW, h: rowH, on: !!CINEMATICS[i].label })),
    { id: 'cancel', x: L.cancel.x, y: L.cancel.y - cancel[0].frame.height + 1, w: cancel[0].frame.width, h: cancel[0].frame.height, on: true },
  ];
  const hit = (x, y, w, h) => { const v = view(w, h), sx = (x - v.ox) / v.scale, sy = (y - v.oy) / v.scale;
    return boxes.find((b) => b.on && sx >= b.x && sx < b.x + b.w && sy >= b.y && sy < b.y + b.h) ?? null; };
  let pressed = null, pointer = [799, 599];
  const playback = createCinematicPlayback(({ path, isCurrent }) =>
    openMovie({ read, path, documentObject, isCurrent }));
  const stop = () => playback.stop();
  const play = (i) => playback.play(i, CINEMATICS[i].path);
  const activate = (b) => { if (b.id === 'cancel') onExit(); else play(b.i); };
  // Retail's DrawMode 1 (the dimmed rows) is PL2's alpha-blend table over what is underneath, the same
  // path character create's disabled OK button takes (frontend.mjs PL2_ALPHA).
  const drawCel = (c, path, i, x, y, dimmed = false) => {   // (x, y) = retail's anchor: bottom row
    const top = y - c.frame.height + 1, left = x + c.frame.offsetX;
    if (dimmed) blend(sctx, c.frame, left, top, 1); else sctx.drawImage(c.canvas, left, top);
    recordDc6(path, i, c.frame, left, top, { op: 'cel', mode: dimmed ? 1 : 5 });
  };
  const text = (font, s, x, y, w) => font.drawAnchored(sctx, s, x + Math.max(Math.trunc((w - font.measure(s)) / 2), 0), y);
  return {
    get evidence() { const movie = playback.movie; return { boxes: boxes.map(({ id, x, y, w, h, on }) => ({ id, x, y, w, h, on })), rows: CINEMATICS.map((c) => ({ label: c.label ?? null, path: c.path })),
      playing: playback.playing, movie: movie ? movie.evidence() : null, error: playback.error }; },
    targets(w, h) { const v = view(w, h); return Object.fromEntries(boxes.filter((b) => b.on).map((b) => [b.id, [Math.round(v.ox + (b.x + b.w / 2) * v.scale), Math.round(v.oy + (b.y + b.h / 2) * v.scale)]])); },
    hover(x, y, w, h) { const v = view(w, h); pointer = [Math.trunc((x - v.ox) / v.scale), Math.trunc((y - v.oy) / v.scale)]; },
    // While a film plays any press ends it (retail skips a movie on a click); otherwise a press arms a control.
    press(x, y, w, h) { if (playback.playing !== null) return 'movie'; pressed = hit(x, y, w, h); return pressed?.id ?? null; },
    release(token, x, y, w, h) {
      if (token === 'movie') { stop(); return true; }
      const b = hit(x, y, w, h); pressed = null;
      if (b && b.id === token) { activate(b); return true; } return false;
    },
    cancel() { pressed = null; },
    escape() { if (playback.playing !== null) stop(); else onExit(); },
    /** Advance the film; the screen it returns to is this one. */
    async step() {
      const movie = playback.movie, startedAt = playback.startedAt;
      if (!movie || !startedAt) return;
      const t = performance.now() - startedAt; await movie.step(t);
      if (t >= movie.durationMs) stop();
    },
    draw(out, w, h) {
      const v = view(w, h);
      out.fillStyle = '#000'; out.fillRect(0, 0, w, h); out.imageSmoothingEnabled = false;
      const movie = playback.movie;
      if (playback.playing !== null) {   // the film owns the screen, centred at 800 wide (its 640x292 aspect kept)
        if (movie) { movie.draw(strip.getContext('2d'), 640, 292); out.drawImage(strip, v.ox, v.oy + Math.round((600 - 365) / 2 * v.scale), 800 * v.scale, 365 * v.scale); }
        return;
      }
      sctx.fillStyle = '#000'; sctx.fillRect(0, 0, 800, 600); sctx.drawImage(bg.canvas, 0, 0); recordTiled(FRONTEND.background, bg, 0, 0);
      dialog.forEach((c, i) => drawCel(c, CINEMATICS_ASSETS.dialog, i, ...L.dialogAt[i]));
      heading.drawAnchored(sctx, 'SELECT CINEMATICS', L.headingX, L.headingY);
      L.rowY.forEach((y, i) => {
        const on = !!CINEMATICS[i].label, a = !on;
        drawCel(row[0], CINEMATICS_ASSETS.row, 0, L.rowX, y, a); drawCel(row[1], CINEMATICS_ASSETS.row, 1, L.rowX + row[0].frame.width, y, a);
        if (on) text(label, CINEMATICS[i].label, L.rowX, y - 8 - 0, rowW);
      });
      drawCel(cancel[0], CINEMATICS_ASSETS.cancel, 0, L.cancel.x, L.cancel.y);
      text(label, string('strCancel') || 'CANCEL', L.cancel.x, L.cancel.y - 8, cancel[0].frame.width);
      const f = cursor.frame; sctx.drawImage(cursor.canvas, pointer[0] + f.offsetX, pointer[1] - 1 + f.offsetY - f.height + 1);
      recordDc6(FRONTEND.cursor, 0, f, pointer[0] + f.offsetX, pointer[1] - 1 + f.offsetY - f.height + 1);
      out.drawImage(scene, v.ox, v.oy, 800 * v.scale, 600 * v.scale);
    },
  };
}
