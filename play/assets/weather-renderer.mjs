// Retail's weather: the rain, drawn where retail draws it (client-parity F11b).  Measured on retail
// 1.13c's own artefacts, never fitted to pixels:
//
//   * The LEVEL decides.  Levels.txt's `Rain` column (checked against retail/tables-1.13c/Levels.txt,
//     the game's own file) is 1 on Act I's town and wilderness (Rogue Encampment id 1, Blood Moor,
//     Cold Plains .. Tamoe Highland, Burial Grounds, Tristram, Moo Moo Farm), on Act III's town and
//     jungle/Kurast belt (Kurast Docktown id 75 .. Travincal) and on Act V's Harrogath..Rocky Summit
//     -- and 0 on every Act II level and every interior.  The engine hands the level's own byte to the
//     client (`d2_session_level_rain`, src/core/monspawn/monspawn_check.cpp), so this file holds no
//     level list of its own.
//   * The art is `DATA\GLOBAL\UncompOverlays\Rain3.dc6` and `Rain4.dc6`.  Sweeping all 225 retail
//     captures (not only the 40+ converted to vectors): Rain3 and Rain4 are the ONLY overlay art ever
//     drawn, Rain1/Rain2 are never drawn anywhere (they are loaded -- retail/drawlists/act1-cels*.bin
//     opens all four -- and never submitted), and every capture that rains -- the Rogue Encampment's
//     walls13..17, the Blood Moor's run-moor and blood-moor-drink-potion, and Act III's a3-docks /
//     a3-npc / a3-npcmenu / c3 / town3 -- hashes every one of its drops to the same 22 Rain3/Rain4
//     frames the Act III vectors carry.  Rain3 is the near layer (5x3 growing to 33x21) and Rain4 the
//     far one (3x2 to 18x11).
//   * Retail draws the whole rain block AFTER the floors and BEFORE the shadows and walls (a3-docks:
//     96 floor draws, then all 43 rain cels, then the shadow/wall passes) -- a wall or roof therefore
//     covers the rain behind it.  session-terrain.mjs's `afterFloors` hook is that slot.
//   * A particle's screen position is FIXED for its whole lifetime and only changes when its animation
//     wraps: across the a3-docks capture's frames 2169 -> 2171 the 23 particles that did not wrap kept
//     their exact x and y while their frame index advanced by 1, and the 5 that were at index 9 came
//     back at index 0 on new positions.
//   * The index is a 10-frame growth animation and a layer's particles are all on ONE parity: in every
//     capture's every frame the rain indices are {1,3,5,7,9} or {0,2,4,6,8} and nothing else, so the
//     particles carry five phase groups (index = (clock + 2g) mod 10), and a group's particles
//     re-place together when their index wraps.  (The bubble*.dc6 draws beside them in a3-npc,
//     a3-npcmenu and town3 DO mix parities -- a different effect, not part of the rain.)
//   * The animation advances once per TWO retail game frames: the a3-docks capture's frames 2169 and
//     2170 carry byte-identical rain (and differ elsewhere in the frame), while 2169 -> 2171 advances
//     every index by 1.
//
// NOT reproduced, and declared on the row: (1) the anchors -- captures of the same scene share ZERO
// particle positions (a3-npc vs a3-npcmenu 28/21 intersection 0; walls14 vs walls17 87/27 share 1, by
// chance), so retail places them from the client's RNG, which this build has no oracle for; the
// placement below is a seeded, deterministic stand-in.  (2) The EPISODE.  Retail's rain starts and
// stops: in one Rogue Encampment run (32 samples, game frames 7504..10620) 27 of them capture nothing
// and the five wet ones are contiguous, ramping 18 -> 115 -> 114 -> 158 -> 54 drops.  The level's
// `Rain` byte is necessary, not sufficient, and the state that schedules it is NOT the ENVIRONMENT
// time of day -- measured and ruled out: Act III's Docktown rains at day 6.73/11.26/19.16/21.54 degrees
// where the Rogue Encampment, at the same phases, captures nothing (row s561, tools/weather-episode.py).
// It is a client weather cycle recovered from retail's own 1.13c D2Client -- four states (dry, ramp up,
// full, ramp down) with randomized durations, a randomized drop target and a per-tick spawn probability
// -- and NOTHING in this tree has it, so there is no schedule to
// draw from: inventing one would be a stand-in that looks right.  This file draws the peak intensity
// continuously, which over-rains against retail's own dry captures; s561 counts it and F11b carries it
// as the open half.
import { decodeDatPalette, decodeDc6 } from './d2-formats.mjs';
import { recordDc6 } from './drawlist.mjs';

/** The two layers retail's captured frames draw, with the particle count of the Rogue Encampment's own
 *  peak episode (walls14: 45 Rain3 + 42 Rain4 -- its tiles are `Act1\Town\Floor`/`Fence`/`Objects`;
 *  walls15 just after it is 42 + 46, walls17 as it dies is 16 + 11, and Act III's town3 inside the
 *  same kind of episode is 20 + 22 -- the count moves with the intensity, so the peak is the number we
 *  take).  Rain4 is the far layer, Rain3 the near one. */
const LAYERS = [
  { path: 'DATA\\GLOBAL\\UncompOverlays\\Rain4.dc6', particles: 42 },
  { path: 'DATA\\GLOBAL\\UncompOverlays\\Rain3.dc6', particles: 45 },
];
const FRAMES = 10;          // the growth animation's length, read off retail's own index range
const TICKS_PER_FRAME = 2;  // measured above: 2169 and 2170 are the same animation step

/** The vector writes a cel path lower case with no `data\` and no extension. */
const MPQ_PATH = (path) => path.toLowerCase();

/**
 * A decoded palette (RGBA, 256 * 4) -> the additive grey each cel byte contributes. THE RAIN'S RULE,
 * and the one this file got wrong: the operand is the PALETTE'S VALUE for the byte, never the byte.
 * `ACT1\pal.dat` maps the rain cels' indices 172..179 to (4,4,4) .. (32,32,32); using the index adds
 * 172..179 instead, saturates every channel and drew the rain as solid white splats over the whole
 * town. Exported so test/weather-intensity.test.mjs can pin it against the real palette.
 */
export function intensityTable(rgba) {
  const table = new Uint8Array(256);
  for (let i = 0; i < 256; i++) table[i] = Math.max(rgba[i * 4], rgba[i * 4 + 1], rgba[i * 4 + 2]);
  return table;
}

/**
 * Every act palette maps the rain cels' indices 172..179 IDENTICALLY, so this renderer does not need
 * to be told which act it is drawing. MEASURED over reference/riiablo's vendored copies of retail's
 * own files, `pal.dat` 172..179: ACT1, ACT2, ACT3, ACT4, ACT5 and Sky are all
 * (4,4,4) (8,8,8) (12,12,12) (16,16,16) (20,20,20) (24,24,24) (24,28,36) (32,32,32), one distinct
 * mapping across six files. The other palettes in that set (menu, EndGame, Trademark, fechar) differ,
 * and none of them is an act.
 *
 * This is why the palette is read HERE rather than plumbed in from the caller: `belt-renderer.mjs`,
 * `character-panel.mjs` and `death-screen.mjs` all read their own `pal.dat` the same way, and the
 * caller-side plumbing this replaced is what the held attempt is suspected of breaking `s126` with.
 * The `act` argument stays on `setLevel` so a caller that knows the act can select it, but nothing
 * has to.
 */
const ACT_PALETTE = (act) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, (act | 0) + 1))}\\pal.dat`;

export function createWeatherRenderer({ read, documentObject = globalThis.document }) {
  const layers = [];
  const errors = [];
  let prepared = false, levelRain = 0;
  let actN = 0, grey = null;

  /**
   * 256-entry byte -> the additive grey that byte contributes, from the act's own palette. A palette
   * that will not load is recorded on `errors` (which the row reads) and yields a table of ZEROES, so
   * the frame draws no rain rather than a false one. Falling back to the raw index here would restore
   * the exact defect this replaced: 172..179 added to every channel is solid white.
   */
  const greyTable = () => {
    if (grey) return grey;
    try {
      // decodeDatPalette returns RGBA (256 * 4), not one byte per entry: index by 4, not 3.
      grey = intensityTable(decodeDatPalette(read(ACT_PALETTE(actN))));
    } catch (error) {
      errors.push(`palette for act ${actN + 1}: ${error}`);
      grey = new Uint8Array(256);                     // all zero: no false rain to look at
    }
    return grey;
  };

  /**
   * The rain's own bytes are the INTENSITY, not a palette index.  Every act palette maps the cels'
   * dominant indices 172..179 to near-black -- `ACT1\\pal.dat` gives (4,4,4), (8,8,8) .. (32,32,32),
   * and `ACT1\\pal.pl2`, `Units\\pal.dat` agree entry for entry -- while retail's own finished frame of
   * the same draws (retail/drawlists/town3.png) has them as grey streaks: pixels 25 or more brighter
   * than their neighbours 3 px above and below sit inside a captured rain blit's rect at luminance
   * 74..179.  Additive drawing of the source byte over the ground reproduces that range; the palette
   * cannot (it would brighten by 4..32).  Which D2DDraw path mode 3 takes is NOT recovered, so this is
   * a model, stated here and on F11b -- the identity a row checks (cel, frame, frame bytes, geometry,
   * mode) is retail's either way.
   */
  const bitmapOf = (layer, index) => {
    let bitmap = layer.bitmaps.get(index);
    if (bitmap === undefined) {
      bitmap = null;
      const frame = layer.dc6.frames[index];
      if (frame && frame.width && frame.height) {
        const rgba = new Uint8ClampedArray(frame.indices.length * 4);
        for (let i = 0; i < frame.indices.length; i++) {
          if (!frame.opaque[i]) continue;
          // The intensity is the ACT PALETTE's value for this byte, NOT the byte itself. The cels'
          // dominant indices are 172..179 and `ACT1\pal.dat` maps them to (4,4,4) .. (32,32,32); retail's
          // own frame of the same draws (retail/drawlists/town3.png) has the streaks "25 or more brighter
          // than their neighbours", which is that palette value. Drawing the raw index adds 172..179,
          // saturates every channel, and is why the rain rendered as solid white splats over the town
          // (seen on :8950 2026-09-27: 6,395 near-white px on the ground band against retail's 631, 96%
          // of them moving -- the particle layer). Which D2DDraw path mode 3 takes is not recovered, so
          // the additive model is still a model; the OPERAND is what the measured delta settles.
          const value = greyTable()[frame.indices[i]];
          rgba[i * 4] = value; rgba[i * 4 + 1] = value; rgba[i * 4 + 2] = value; rgba[i * 4 + 3] = 255;
        }
        const canvas = documentObject.createElement('canvas');
        canvas.width = frame.width; canvas.height = frame.height;
        canvas.getContext('2d').putImageData(new ImageData(rgba, frame.width, frame.height), 0, 0);
        bitmap = canvas;
      }
      layer.bitmaps.set(index, bitmap);
    }
    return bitmap;
  };

  const prepare = () => {
    if (prepared) return;
    prepared = true;
    for (const layer of LAYERS) {
      let dc6 = null;
      try { dc6 = decodeDc6(read(MPQ_PATH(layer.path))); } catch (error) { errors.push(`${layer.path}: ${error}`); continue; }
      if (!dc6 || dc6.frames.length < FRAMES) { errors.push(`${layer.path}: ${dc6 ? dc6.frames.length : 0} frames, need ${FRAMES}`); continue; }
      // One particle per (position, phase group).  Positions are the declared stand-in: a uniform
      // spread over the 800x600 screen from a fixed seed, so two runs of a row draw the same frame.
      let seed = 0x9e3779b9 ^ (layer.particles * 2654435761);
      const next = (n) => { seed = (Math.imul(seed, 1664525) + 1013904223) >>> 0; return (seed >>> 8) % n; };
      const particles = [];
      for (let i = 0; i < layer.particles; i++) particles.push({ x: next(800), y: next(600), phase: 2 * (i % 5) });
      layers.push({ ...layer, dc6, particles, bitmaps: new Map() });
    }
  };

  return {
    /** The level's own Rain byte, straight from the engine's Levels.txt read. */
    /**
     * The level's own Rain byte, straight from the engine's Levels.txt read, and optionally the act
     * that colours it. A change of act invalidates the palette table AND the per-frame bitmaps built
     * from it. `act` is optional: every act palette agrees on the rain indices (ACT_PALETTE above), so
     * a caller that does not know the act still gets the right colours.
     */
    setLevel(rain, act) {
      levelRain = Number(rain) > 0 ? 1 : 0;
      const next = Number.isFinite(act) ? Math.min(4, Math.max(0, act | 0)) : actN;
      if (next !== actN) {
        actN = next; grey = null;
        for (const layer of layers) layer.bitmaps.clear();
      }
    },

    active() { return levelRain === 1; },

    /** Why a layer did not load, if one did not -- read by the row rather than swallowed here. */
    get errors() { return errors.slice(); },

    /**
     * Draw this frame's rain.  `tick` is the engine's own game tick (`engineTicks` in
     * gameplay-shell.mjs), so the animation is a function of game time, not of frames rendered.
     * Returns one identity per blit -- `{ path, frame, x, y, w, h, hash }` in the draw list's own
     * conventions (play/assets/drawlist.mjs), the same record retail's hook writes.
     */
    draw(ctx, width, height, tick) {
      if (!levelRain) return [];
      prepare();
      const step = Math.floor(Number(tick) / TICKS_PER_FRAME);
      const blits = [];
      ctx.save();
      // Retail's rain cels are drawn with DrawMode 3 (modulate/add), not the normal 5 -- the streaks
      // brighten what is under them.
      ctx.globalCompositeOperation = 'lighter';
      for (const layer of layers) {
        for (const particle of layer.particles) {
          const index = ((step + particle.phase) % FRAMES + FRAMES) % FRAMES;
          if (particle.last === undefined) particle.last = index;
          // The animation wrapped (9 -> 0): retail gives the particle a NEW position, and keeps it for
          // the whole of the next pass -- the captured particles that did not wrap kept theirs exactly.
          else if (index < particle.last) {
            const seed = (Math.imul(particle.x * 31 + particle.y * 17 + step, 1664525) + 1013904223) >>> 0;
            particle.x = (seed >>> 8) % width;
            particle.y = (seed >>> 20) % height;
          }
          particle.last = index;
          const frame = layer.dc6.frames[index];
          const bitmap = bitmapOf(layer, index);
          if (!bitmap || !frame) continue;
          // Retail's anchor: a cel frame lands with its left edge at x + offsetX and its BOTTOM row at
          // y + offsetY, so the particle's (x, y) is the anchor above the streak.
          const left = particle.x + frame.offsetX;
          const top = particle.y + frame.offsetY - frame.height + 1;
          ctx.drawImage(bitmap, left, top);
          const record = recordDc6(layer.path, index, frame, left, top, { op: 'cel', mode: 3, extra: 0 });
          if (record && record.hash) blits.push(record);
        }
      }
      ctx.restore();
      return blits;
    },
  };
}
