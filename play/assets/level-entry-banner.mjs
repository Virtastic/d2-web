// Retail's level-ENTRY banner: the `ENTERING THE <AREA>` strip a player sees on arriving in a level.
//
// WHAT IT IS, MEASURED (s693). Retail 1.13c's own draw list for a waypoint arrival
// (`the retail captures`, the Great Marsh, level 77) carries exactly two records
// for it and nothing else -- two celcolor cels, one cell file, frames 0 and 1, at (211,160) 256x26
// and (467,160) 120x26, mode 5, colour shift 1:
//
//   celcolor ? frame=0 dir=0 x=211 y=160 mode=5 ... extra=1 ... w=256 h=26 hash=8825d776
//   celcolor ? frame=1 dir=0 x=467 y=160 mode=5 ... extra=1 ... w=120 h=26 hash=89a6f718
//
// The vector cannot name the file (the hook's cels table is read at the capture's first frame, and
// this cell file is opened later), and there is no text record, so the banner is invisible to a row
// that only knows how to read names. It is not invisible to the hashes: FNV-1a over the frame's
// encoded DC6 bytes is the same function the client's decoder feeds (`assets/drawlist.mjs`), so the
// two records name the exact frames. They are `DATA\LOCAL\UI\ENG\ACT3\A3L3.DC6`, frames 0 and 1 --
// the file Levels.txt's own `EntryFile` column names for level 77 ('A3L3'), read out of the retail
// MPQs frame by frame (s693's measurement).
//
// WHERE THE FILE COMES FROM IS THE LEVEL'S OWN TABLE, never a table here. `Levels.txt` gives every
// level an `EntryFile` (`A3L3` for the Great Marsh, `A3L4` for the Flayer Jungle), the engine serves
// it (`d2_level_entry_file`), and the act's own directory completes it. A level retail never captured
// therefore draws its own banner: nothing in this file names a level or an area.
//
// THE ANCHOR IS THE SAME RULE THE DEATH SCREEN'S TWO LINES USE (assets/death-screen.mjs, F17):
// `400 - floor(totalWidth / 2) - 1` for the file's whole width, then frame i at the running sum of
// the widths before it. Measured twice: 376 px -> 211 and 376-256=467/120 for the Great Marsh, and
// 387 px -> 206 and 462/131 for the Flayer Jungle (`the retail captures`, level
// 78, `A3L4.DC6`), which is why it is a rule and not the one level's numbers.
//
// WHEN IT IS DRAWN is the gate the shell owns: 120 frames from the arrival. Measured on a 240-frame
// timed capture of a waypoint arrival -- the two records are present on frames 9..128 and absent from
// 130 on, and the Flayer Jungle's own arrival shows the identical window, so 120 frames is the
// display's, not the area's. See gameplay-shell.mjs for the gate itself.
//
// The art and its colour: the cells are drawn through the act's own screen palette with D2Win's
// colour shift 1 (COL_RED) -- retail's `extra=1` -- which is the PL2 TextColorShifts row the rest of
// the client's coloured text goes through, not an RGB mix (F17, death-screen.mjs).
import { textColourPalette } from './frontend.mjs';
import { recordDc6 } from './drawlist.mjs';

export const ENTRY_BANNER = {
  frames: 120,     // the measured window, in retail game frames from the arrival
  rowY: 160,       // retail's own anchor: the frames' bottom row is y=160 on 800x600
  centre: 400,     // retail centres the whole file on the screen
  mode: 5,         // D2MOO DrawMode 5 (normal), the mode both retail records carry
  colour: 1,       // D2Win COL_RED, retail's `extra=1` on both records
};

/**
 * Where each frame of a level's entry art lands, in 800x600 screen columns: the whole file centred as
 * `400 - floor(totalWidth / 2) - 1`, then every frame at the running sum of the widths before it.
 * Pure and exported so the rule can be pinned against retail's own anchors without a browser
 * (test/level-entry-banner.test.mjs), and so a wrong centring fails a test rather than a screenshot.
 */
export function entryFrameAnchors(widths, centre = ENTRY_BANNER.centre) {
  const total = widths.reduce((w, x) => w + x, 0);
  let left = centre - Math.floor(total / 2) - 1;
  return widths.map((w) => { const at = left; left += w; return at; });
}

/** The MPQ path of a level's entry art: Levels.txt's EntryFile under the level's own act. */
export function entryFilePath(act, entryFile) {
  const actNo = Math.min(5, Math.max(1, act | 0));
  return `data\\local\\ui\\eng\\act${actNo}\\${String(entryFile).toLowerCase()}.dc6`;
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

const actPalette = (act) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\pal.dat`;

export function createEntryBanner({ read, decodeDc6, decodeDatPalette, documentObject = globalThis.document }) {
  const arts = new Map();
  let evidence = null;
  // One entry per level: the frames decoded from ITS EntryFile and one canvas per frame. Cached by
  // level id, because the file is a property of the level and a session walks back and forth.
  const artFor = (levelId, entryFile, act) => {
    const key = `${levelId}`;
    if (arts.has(key)) return arts.get(key);
    const path = entryFilePath(act, entryFile);
    let art;
    try {
      const file = decodeDc6(read(path));
      const palette = decodeDatPalette(read(actPalette(act)));
      let pl2 = null; try { pl2 = read(actPalette(act).replace(/pal\.dat$/i, 'pal.pl2')); } catch { pl2 = null; }
      const shift = pl2 ? textColourPalette(pl2, palette, ENTRY_BANNER.colour) : palette;
      art = { path, palette: actPalette(act), levels: null,
        width: file.frames.reduce((w, f) => w + f.width, 0),
        frames: file.frames.map((f) => ({ frame: f, canvas: frameCanvas(f, shift, documentObject) })), error: '' };
    } catch (error) { art = { path, error: String(error), frames: [], width: 0, palette: actPalette(act) }; }
    arts.set(key, art);
    return art;
  };
  return {
    get evidence() { return evidence; },
    /** Drop every cached level's art (the engine reloaded its tables; a fresh page never needs this). */
    reset() { arts.clear(); evidence = null; },
    /**
     * Draw the banner for the level the session just arrived in. One blit per frame of the level's own
     * EntryFile, in the records retail's hook writes, so a row can compare them record for record.
     */
    draw(ctx, width, height, { levelId = 0, entryFile = '', act = 1 } = {}) {
      const art = artFor(levelId, entryFile, act);
      if (art.error) { evidence = { level: levelId, entryFile, path: art.path, error: art.error, blits: [] }; return evidence; }
      const scale = Math.min(width / 800, height / 600), ox = (width - 800 * scale) / 2, oy = (height - 600 * scale) / 2;
      ctx.save(); ctx.translate(ox, oy); ctx.scale(scale, scale);
      const blits = [];
      const anchors = entryFrameAnchors(art.frames.map(({ frame }) => frame.width));
      for (let i = 0; i < art.frames.length; i++) {
        const { frame, canvas } = art.frames[i];
        const drawLeft = anchors[i] + frame.offsetX, drawTop = ENTRY_BANNER.rowY - frame.height + 1 + frame.offsetY;
        ctx.drawImage(canvas, drawLeft, drawTop);
        blits.push(recordDc6(art.path, i, frame, drawLeft, drawTop,
          { op: 'celcolor', mode: ENTRY_BANNER.mode, extra: ENTRY_BANNER.colour }));
      }
      ctx.restore();
      evidence = { level: levelId, entryFile, path: art.path, totalWidth: art.width, y: ENTRY_BANNER.rowY,
        mode: ENTRY_BANNER.mode, colour: ENTRY_BANNER.colour, palette: art.palette,
        left: blits[0]?.x ?? null, blits, error: '' };
      return evidence;
    },
  };
}
