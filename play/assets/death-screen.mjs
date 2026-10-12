// What retail draws over the world while the player is dead. The oracle is retail's own frame and
// draw list for a death (retail/drawlists/death.png + the retail captures, the Oracle
// Barbarian -- a softcore save -- dying in the Blood Moor), whose last four celcolor records are
// exactly this screen:
//   frame 0/1 of one cell file at x 261 and 517, y 206, w 256 and 20, mode=5, extra=1
//   frame 0/1 of another  at x 184 and 440, y 254, w 256 and 175, mode=5, extra=1
// Their FNV hashes name the two files, and they are NOT the pair this module used to draw:
//   8f769c17/8640edd7 = youdiedhardcore.dc6, whose art reads "YOU HAVE DIED"
//   14cf7573/cbf0132e = youdiedinst.dc6,     whose art reads "PRESS ESC TO CONTINUE."
// A softcore death therefore draws youdiedhardcore.dc6 -- the first line is not branched on
// hardcore. (youdiedsoftcore.dc6 exists and reads "YOU LOST GOLD", but D2Client 1.13d has only two
// death cells, D2CLIENT_CellDeathInst and D2CLIENT_CellDeathHC -- D2Ex2's D2ExPointers_113D.h,
// 0x11D390 and +4 -- and ExDeathMessage.cpp, the replacement for 1.13d's own draw at 0x6FAC8BB0,
// draws the HC cell unconditionally. Chosen on retail's own frame, which is the stronger oracle.)
// That file also gives the layout: y = ScreenHeight / 2 - 94 = 206, then 48px per line, and the
// colour: D2Win colour 1 (COL_RED, D2Ex2 Constants.h), retail's `extra=1` on both lines.
// Penalty text branches on the live client save flags: softcore can show string 5094
// "Death takes its toll of %d Gold" (stat 175 STAT_GOLDLOST) and string 5095 "You have lost
// experience" on Nightmare/Hell; hardcore shows localized string 5096 "Your deeds of valor" in
// their place. All strings come from the game's own language tables.
//
// Retail's capture also fixes the two off-by-ones this file used to have: the cell file's left edge
// is 400 - floor(width / 2) - 1 over the whole file (261 for 276px, 184 for 431px), not
// 400 - width/2; and retail's y is the frame's BOTTOM ROW inclusive (drawlist.mjs), so the first row
// is y - height + 1.
//
// The art and the lettering are drawn through the SCREEN palette, which in game is the act's:
// retail's record carries pal=00000000 (the screen palette) and every other in-game panel module in
// play/assets reads ACT<n>/pal.dat. This file used the front-end Units palette, which no in-game
// screen uses. Colour 1 is not an RGB mix either: it is the PL2 TextColorShifts row
// (frontend.mjs textColourPalette), the table the rest of the client's coloured text goes through.
import { createFont, decodeFontTbl, textColourPalette } from './frontend.mjs';
import { recordDc6 } from './drawlist.mjs';

export const DEATH_SCREEN = {
  // The first line, whatever the difficulty: retail's captured death draws these exact bytes.
  death: 'data\\local\\ui\\eng\\youdiedhardcore.dc6',
  instructions: 'data\\local\\ui\\eng\\youdiedinst.dc6',
  font: 'data\\local\\font\\latin\\font30.dc6',
  fontTbl: 'data\\local\\font\\latin\\font30.tbl',
  goldString: 5094, experienceString: 5095,
  firstLineFromCentre: -94, lineStep: 48,
  colour: 1,        // D2Win COL_RED: retail's `extra=1` on both cell lines
  cellMode: 5,      // DRAW_MODE_NORMAL, the mode retail's draw list records for both lines
};
const centre = 400;
const actPalette = (act) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\pal.dat`;

/** Native death-text branch: hardcore gets its own line; softcore gets gold and difficulty loss. */
export function deathScreenPenaltyRows({ hardcore = false, goldLost = 0, difficulty = 0 } = {}) {
  let y = 300 + DEATH_SCREEN.firstLineFromCentre + 2 * DEATH_SCREEN.lineStep;
  if (hardcore) return [{ stringIndex: 5096, y }];
  const rows = [];
  if (goldLost) { rows.push({ stringIndex: DEATH_SCREEN.goldString, y }); y += DEATH_SCREEN.lineStep; }
  if (difficulty > 0) rows.push({ stringIndex: DEATH_SCREEN.experienceString, y });
  return rows;
}

function frameCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, frame.width); canvas.height = Math.max(1, frame.height);
  if (frame.width && frame.height) {
    const rgba = new Uint8ClampedArray(frame.width * frame.height * 4);
    for (let i = 0; i < frame.indices.length; i++) if (frame.opaque[i]) rgba.set(palette.subarray(frame.indices[i] * 4, frame.indices[i] * 4 + 4), i * 4);
    canvas.getContext('2d').putImageData(new ImageData(rgba, frame.width, frame.height), 0, 0);
  }
  return canvas;
}

export function createDeathScreen({ read, decodeDc6, decodeDatPalette, string, documentObject = globalThis.document }) {
  const dc6 = { death: decodeDc6(read(DEATH_SCREEN.death)), instructions: decodeDc6(read(DEATH_SCREEN.instructions)) };
  const frames = Object.fromEntries(Object.entries(dc6).map(([name, file]) => [name, file.frames]));
  const widths = Object.fromEntries(Object.entries(frames).map(([name, list]) => [name, list.reduce((w, f) => w + f.width, 0)]));
  const fontDc6 = decodeDc6(read(DEATH_SCREEN.font)), fontTbl = decodeFontTbl(read(DEATH_SCREEN.fontTbl));
  const acts = new Map();
  // One entry per act: the screen palette in its colour-1 translation, the lettering font through it,
  // and one canvas per cell frame of the two art files. Bounded by the five acts.
  const actArt = (act) => {
    const key = Math.min(5, Math.max(1, act | 0));
    if (!acts.has(key)) {
      const palette = decodeDatPalette(read(actPalette(key)));
      let pl2 = null; try { pl2 = read(actPalette(key).replace(/pal\.dat$/i, 'pal.pl2')); } catch { pl2 = null; }
      const red = pl2 ? textColourPalette(pl2, palette, DEATH_SCREEN.colour) : palette;
      const canvases = Object.fromEntries(Object.entries(frames).map(([name, list]) => [name, list.map((f) => frameCanvas(f, red, documentObject))]));
      acts.set(key, { palette: actPalette(key), font: createFont({ dc6: fontDc6, tbl: fontTbl, palette: red, documentObject }), canvases });
    }
    return acts.get(key);
  };
  let evidence = null;
  return {
    get evidence() { return evidence; },
    /** `goldLost` is stat 175; `difficulty` 0-2; `act` picks the screen palette. Draws in 800x600
     *  screen space, scaled to fit. */
    draw(ctx, width, height, { hardcore = false, goldLost = 0, difficulty = 0, act = 1 } = {}) {
      const art = actArt(act), font = art.font;
      const scale = Math.min(width / 800, height / 600), ox = (width - 800 * scale) / 2, oy = (height - 600 * scale) / 2;
      ctx.save(); ctx.translate(ox, oy); ctx.scale(scale, scale);
      const lines = [];
      let bottom = 300 + DEATH_SCREEN.firstLineFromCentre;
      // Each frame is its own blit, recorded with retail's own coordinates (play/assets/drawlist.mjs):
      // the record's x is retail's anchor and its y the frame's bottom row, so this list diffs against
      // the vector record for record, hash included.
      const cellLine = (name) => {
        const path = DEATH_SCREEN[name], list = frames[name], canvases = art.canvases[name];
        let left = centre - Math.floor(widths[name] / 2) - 1;
        const blits = [];
        for (let i = 0; i < list.length; i++) {
          const frame = list[i], drawLeft = left + frame.offsetX, drawTop = bottom - frame.height + 1 + frame.offsetY;
          ctx.drawImage(canvases[i], drawLeft, drawTop);
          blits.push(recordDc6(path, i, frame, drawLeft, drawTop,
            { op: 'celcolor', mode: DEATH_SCREEN.cellMode, extra: DEATH_SCREEN.colour }));
          left += frame.width;
        }
        lines.push({ kind: 'cell', art: name, path, y: bottom, left: blits[0]?.x ?? null, mode: DEATH_SCREEN.cellMode,
          colour: DEATH_SCREEN.colour, palette: art.palette, frames: blits });
      };
      cellLine('death');
      bottom += DEATH_SCREEN.lineStep;
      cellLine('instructions');
      const text = (index, s, y) => {
        const w = font.measure(s), x = centre - Math.floor(w / 2);
        let glyphs = 0;
        font.draw(ctx, s, x, y - font.cellHeight, () => { glyphs++; });
        lines.push({ kind: 'text', stringIndex: index, text: s, x, y, glyphs, colour: DEATH_SCREEN.colour,
          font: DEATH_SCREEN.font, palette: art.palette });
      };
      for (const row of deathScreenPenaltyRows({ hardcore, goldLost, difficulty })) {
        const template = String(string(row.stringIndex) || '');
        const value = row.stringIndex === DEATH_SCREEN.goldString ? template.replace('%d', goldLost) : template;
        text(row.stringIndex, value, row.y);
      }
      ctx.restore();
      evidence = { lines };
      return evidence;
    },
  };
}
