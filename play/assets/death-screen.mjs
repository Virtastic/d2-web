// What retail draws over the world while the player is dead: two cell files of lettering, then the
// gold the death cost. Retail art and strings, not text of our own:
//   data\local\ui\eng\youdiedsoftcore.dc6  "YOU LOST GOLD"            (youdiedhardcore.dc6 for hardcore)
//   data\local\ui\eng\youdiedinst.dc6      "PRESS ESC TO CONTINUE."
//   string 5094 "Death takes its toll of %d Gold" (stat 175, the gold lost), string 5095 "You have
//   lost experience" on Nightmare and Hell.
// Layout from reference/D2Ex2/ExDeathMessage.cpp, which replaces 1.13d D2Client's own draw
// (0x6FAC8BB0): the first cell centred at screen height / 2 - 94, each later line 48px lower, text in
// size 2 (font30), colour 1. Each cell file is two 256px-wide frames laid side by side.
// TODO(client-parity F17): no retail frame of a death yet (level 1); whether the cell y is the
// frame's bottom (as DC6 draws elsewhere) is taken from the DC6 convention, and the red is
// OpenDiablo2's colorRed100Alpha (0xFF7777), not D2Win's colour-1 translation.
import { createFont, decodeFontTbl, FRONTEND } from './frontend.mjs';

export const DEATH_SCREEN = {
  softcore: 'data\\local\\ui\\eng\\youdiedsoftcore.dc6',
  hardcore: 'data\\local\\ui\\eng\\youdiedhardcore.dc6',
  instructions: 'data\\local\\ui\\eng\\youdiedinst.dc6',
  font: 'data\\local\\font\\latin\\font30.dc6',
  fontTbl: 'data\\local\\font\\latin\\font30.tbl',
  goldString: 5094, experienceString: 5095,
  firstLineFromCentre: -94, lineStep: 48,
  red: [0xff, 0x77, 0x77],
};

function cellCanvas(dc6, palette, documentObject) {
  const frames = dc6.frames;
  const width = frames.reduce((w, f) => w + f.width, 0), height = Math.max(...frames.map((f) => f.height));
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, width); canvas.height = Math.max(1, height);
  const ctx = canvas.getContext('2d');
  let x = 0;
  for (const f of frames) {
    if (f.width && f.height) {
      const rgba = new Uint8ClampedArray(f.width * f.height * 4);
      for (let i = 0; i < f.indices.length; i++) if (f.opaque[i]) rgba.set(palette.subarray(f.indices[i] * 4, f.indices[i] * 4 + 4), i * 4);
      ctx.putImageData(new ImageData(rgba, f.width, f.height), x, height - f.height);
    }
    x += f.width;
  }
  return canvas;
}

export function createDeathScreen({ read, decodeDc6, decodeDatPalette, string, documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(FRONTEND.palette));
  const soft = cellCanvas(decodeDc6(read(DEATH_SCREEN.softcore)), palette, documentObject);
  const hard = cellCanvas(decodeDc6(read(DEATH_SCREEN.hardcore)), palette, documentObject);
  const inst = cellCanvas(decodeDc6(read(DEATH_SCREEN.instructions)), palette, documentObject);
  const red = palette.map((v, i) => (i % 4 === 3 ? v : Math.round(v * DEATH_SCREEN.red[i % 4] / 0xff)));
  const font = createFont({ dc6: decodeDc6(read(DEATH_SCREEN.font)), tbl: decodeFontTbl(read(DEATH_SCREEN.fontTbl)), palette: red, documentObject });
  let evidence = null;
  return {
    get evidence() { return evidence; },
    /** `goldLost` is stat 175; `difficulty` 0-2. Draws in 800x600 screen space, scaled to fit. */
    draw(ctx, width, height, { hardcore = false, goldLost = 0, difficulty = 0 } = {}) {
      const scale = Math.min(width / 800, height / 600), ox = (width - 800 * scale) / 2, oy = (height - 600 * scale) / 2;
      ctx.save(); ctx.translate(ox, oy); ctx.scale(scale, scale);
      const lines = [];
      let bottom = 300 + DEATH_SCREEN.firstLineFromCentre;
      const cell = hardcore ? hard : soft;
      ctx.drawImage(cell, Math.round(400 - cell.width / 2), bottom - cell.height);
      lines.push({ art: hardcore ? 'hardcore' : 'softcore', bottom });
      bottom += DEATH_SCREEN.lineStep;
      ctx.drawImage(inst, Math.round(400 - inst.width / 2), bottom - inst.height);
      lines.push({ art: 'instructions', bottom });
      const text = (s, y) => { const w = font.measure(s); font.draw(ctx, s, Math.round(400 - w / 2), y - font.cellHeight); lines.push({ text: s, bottom: y }); };
      if (!hardcore && goldLost) { bottom += DEATH_SCREEN.lineStep; text(String(string(DEATH_SCREEN.goldString) || '').replace('%d', goldLost), bottom); }
      if (!hardcore && difficulty > 0) text(String(string(DEATH_SCREEN.experienceString) || ''), bottom + DEATH_SCREEN.lineStep);
      ctx.restore();
      evidence = { lines };
      return evidence;
    },
  };
}
