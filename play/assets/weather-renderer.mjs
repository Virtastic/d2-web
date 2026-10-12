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
import { createBlender } from './frontend.mjs';
import { recordDc6, recordPrim } from './drawlist.mjs';
import { createWeatherCycle } from './weather-cycle.mjs';
import { plotRainLine } from './weather-line.mjs';

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

/** The renderer needs the active act because DrawMode 3 maps indices through that act's palette/PL2. */
const ACT_PALETTE = (act) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, (act | 0) + 1))}\\pal.dat`;

export function createWeatherRenderer({ read }) {
  const layers = [];
  const errors = [];
  let prepared = false, levelRain = 0;
  let actN = 0, rgbaPal = null, rippleBlend = null;
  const cycle = createWeatherCycle(), clock = { tick: null };
  const parity = new URLSearchParams(globalThis.location?.search ?? '').get('enhanced') !== '1' && new URLSearchParams(globalThis.location?.search ?? '').get('view') !== 'fill';
  let live = [];
  const streaks = { drops: [], w: 0, h: 0, tick: null, camX: null, camY: null, seed: 0x2545f491 };

  /** Load the active act's palette for source cels and PL2-composited ripple output. */
  const palette = () => {
    if (rgbaPal) return rgbaPal;
    try {
      rgbaPal = decodeDatPalette(read(ACT_PALETTE(actN)));
    } catch (error) {
      errors.push(`palette for act ${actN + 1}: ${error}`);
      return null;
    }
    return rgbaPal;
  };
  const blender = () => {
    if (rippleBlend) return rippleBlend;
    const pal = palette();
    if (!pal) return null;
    try {
      rippleBlend = createBlender(read(ACT_PALETTE(actN).replace(/pal\.dat$/i, 'pal.pl2')), pal);
    } catch (error) {
      errors.push(`PL2 for act ${actN + 1}: ${error}`);
      return null;
    }
    return rippleBlend;
  };

  // ---- The falling drops: retail's LINE primitives (renderer slot 48, mode 127), ~250 a frame ----------
  // Measured on the retail captures{town-anim,run-by-click-anim}.txt (4 and 24 consecutive whole frames) --
  // the drops were never the Rain3/4 cels (those are the ground ripples), they are these lines:
  //   * drawn AFTER the whole world (floors, walls, units) and before the HUD, in the act palette's grey
  //     (colour idx 22 25 27 115 185 188 192 198 200 206 231); 1 px wide, ~7 in 10 are 3..10 px streaks
  //     and ~1 in 4 a 1 px dot that sits ~3 frames;
  //   * a streak falls in a straight line at its OWN speed, 15..27 px per game frame, with a per-frame
  //     wind slope dx/dy in about -0.28..+0.41 that every drop shares; its length is fixed until it lands,
  //     then it shortens to the landing row, shows as a dot for one frame and is gone;
  //   * drops are WORLD-anchored in x/y: while the camera scrolls (floors shift 6-7 px a frame in
  //     run-by-click-anim) the drops' chains shift by the same amount, so they are advanced by the
  //     camera's own screen delta -- and they cover the whole viewport, so a wider window gets more.
  // NOT recovered: retail's RNG (spawn x, landing rows, speeds) -- seeded stand-ins with the measured
  // ranges -- and the wind's schedule (a slow sine over the measured range).
  const LINE_COLOURS = [115, 115, 115, 27, 27, 22, 25, 185, 188, 192, 198, 200, 206, 231];
  const rnd = (n) => { streaks.seed = (Math.imul(streaks.seed, 1664525) + 1013904223) >>> 0; return (streaks.seed >>> 8) % n; };
  const spawn = (drop, first) => {
    const dot = rnd(4) === 0;
    drop.dot = dot; drop.colour = LINE_COLOURS[rnd(LINE_COLOURS.length)];
    drop.vy = 15 + rnd(13); drop.len = dot ? 0 : 3 + rnd(8);
    drop.x = rnd(streaks.w + 200) - 100; drop.land = Math.round(streaks.h * 0.2) + rnd(Math.round(streaks.h * 0.75));
    drop.y = first ? rnd(drop.land) : -drop.len - rnd(drop.vy);
    drop.ttl = 3 + rnd(4); drop.state = 0;
    if (dot) drop.y = drop.land;
  };
  const stepDrops = (wind, scrollX, scrollY) => {
    for (const d of streaks.drops) {
      d.x -= scrollX; d.y -= scrollY;
      if (d.dot) { if (--d.ttl < 0) spawn(d, false); continue; }
      if (d.state === 1) { spawn(d, false); continue; }
      d.x += wind * d.vy; d.y += d.vy;
      if (d.y + d.len >= d.land) { d.len = Math.max(0, d.land - d.y); d.state = d.len === 0 ? 1 : 0; if (d.y >= d.land) { d.len = 0; d.state = 1; } }
      if (d.x < -100 || d.x > streaks.w + 100) spawn(d, false);
    }
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
      for (let i = 0; i < layer.particles; i++) particles.push({ seed: next(0x1000000), phase: 2 * (i % 5) });
      layers.push({ ...layer, dc6, particles });
    }
  };

  return {
    /** The level's own Rain byte, straight from the engine's Levels.txt read. */
    /**
     * The level's own Rain byte, straight from the engine's Levels.txt read, and optionally the act
     * that colours it. A change of act invalidates the palette and blend table. `act` is optional: every act palette agrees on the rain indices (ACT_PALETTE above), so
     * a caller that does not know the act still gets the right colours.
     */
    setLevel(rain, act) {
      levelRain = Number(rain) > 0 ? 1 : 0;
      const next = Number.isFinite(act) ? Math.min(4, Math.max(0, act | 0)) : actN;
      if (next !== actN) {
        actN = next; rgbaPal = null; rippleBlend = null;
      }
    },


    /**
     * The falling drops, drawn after the world (retail's line records).  `cx, cy` are the camera's screen
     * position, so the drops follow the scroll; returns the line records drawn ({x0,y0,x1,y1,colour}).
     */
    drawStreaks(ctx, width, height, tick, cx, cy) {
      if (!levelRain) return [];
      if (!palette()) return [];
      // The retail count IS the cycle's intensity on 800x600; a wider or taller viewport gets the same
      // DENSITY (?parity=1: exactly retail's count, whatever the viewport).
      const want = Math.round(cycle.intensity * (parity ? 1 : (width * height) / 480000));
      if (streaks.w !== width || streaks.h !== height || streaks.drops.length !== want) {
        streaks.w = width; streaks.h = height;
        while (streaks.drops.length > want) streaks.drops.pop();
        while (streaks.drops.length < want) { const d = {}; spawn(d, true); streaks.drops.push(d); }
      }
      const t = Math.floor(Number(tick));
      if (streaks.tick === null) { streaks.tick = t; streaks.camX = cx; streaks.camY = cy; }
      const steps = Math.min(5, Math.max(0, t - streaks.tick));
      if (steps) {
        const wind = 0.33 * Math.sin(t / 1500);
        for (let i = 0; i < steps; i++) stepDrops(wind, (cx - streaks.camX) / steps, (cy - streaks.camY) / steps);
        streaks.tick = t; streaks.camX = cx; streaks.camY = cy;
      } else if (t < streaks.tick) { streaks.tick = t; streaks.camX = cx; streaks.camY = cy; }
      const wind = 0.33 * Math.sin(t / 1500), out = [];
      ctx.save();
      for (const d of streaks.drops) {
        const x0 = Math.round(d.x), y0 = Math.round(d.y), x1 = Math.round(d.x + wind * d.len), y1 = Math.round(d.y + d.len);
        const o = d.colour * 4;
        ctx.fillStyle = `rgb(${rgbaPal[o]},${rgbaPal[o + 1]},${rgbaPal[o + 2]})`;
        plotRainLine(ctx, x0, y0, x1, y1);
        recordPrim('line', x0, y0, x1, y1, d.colour, 127);
        out.push({ x0, y0, x1, y1, colour: d.colour });
      }
      ctx.restore();
      return out;
    },

    /** One weather tick per game frame (retail: one per rendered frame), on every level. */
    advance(tick) {
      const t = Math.floor(Number(tick));
      if (clock.tick !== null && t > clock.tick) cycle.tick(t - clock.tick);
      clock.tick = t;
    },
    get cycle() { return cycle; },
    /** Live ripples of the last draw: world anchor (wx, wy) and screen anchor (sx, sy). */
    get ripples() { return live.map((r) => ({ ...r, waterCell: r.waterCell && { ...r.waterCell } })); },

    active() { return levelRain === 1; },

    /** Why a layer did not load, if one did not -- read by the row rather than swallowed here. */
    get errors() { return errors.slice(); },

    /**
     * Draw this frame's rain.  `tick` is the engine's own game tick (`engineTicks` in
     * gameplay-shell.mjs), so the animation is a function of game time, not of frames rendered.
     * Returns one identity per blit -- `{ path, frame, x, y, w, h, hash }` in the draw list's own
     * conventions (play/assets/drawlist.mjs), the same record retail's hook writes.
     */
    draw(ctx, width, height, tick, cx = 0, cy = 0, waterAnchors = []) {
      live = [];
      if (!levelRain || cycle.intensity === 0 || !waterAnchors.length) return [];   // retail draws none while dry or without water
      prepare();
      const step = Math.floor(Number(tick) / TICKS_PER_FRAME);
      const blits = [];
      const blend = blender();
      if (!blend) return [];
      for (const layer of layers) {
        for (const particle of layer.particles) {
          const index = ((step + particle.phase) % FRAMES + FRAMES) % FRAMES;
          // Particles live in WORLD (camera-origin) space: wx/wy = screen position + the camera's screen
          // position when spawned; drawn at wx - cx, so they scroll with the ground (F11b).
          if (particle.wx === undefined) {
            const anchor = waterAnchors[particle.seed % waterAnchors.length];
            particle.waterCell = { x: anchor.x, y: anchor.y };
            particle.wx = anchor.sx + width / 2; particle.wy = anchor.sy + height / 2 - 8;
          }
          if (particle.last === undefined) particle.last = index;
          // The animation wrapped (9 -> 0): retail gives the particle a NEW position, and keeps it for
          // the whole of the next pass -- the captured particles that did not wrap kept theirs exactly.
          else if (index < particle.last) {
            particle.seed = (Math.imul(particle.seed ^ step, 1664525) + 1013904223) >>> 0;
            const anchor = waterAnchors[particle.seed % waterAnchors.length];
            particle.waterCell = { x: anchor.x, y: anchor.y };
            particle.wx = anchor.sx + width / 2; particle.wy = anchor.sy + height / 2 - 8;
          }
          particle.x = particle.wx - cx; particle.y = particle.wy - cy;
          particle.last = index;
          // D2Client 1.13c FUN_6FB2FC70 gates the anchor before applying cel offsets.
          // Both Perspective branches require 0 <= y < screenHeight - 47; candidate
          // water-floor eligibility does not replace this later submission check.
          if (particle.y < 0 || particle.y >= height - 47) continue;
          const frame = layer.dc6.frames[index];
          if (!frame || !blend) continue;
          // Retail's anchor: a cel frame lands with its left edge at x + offsetX and its BOTTOM row at
          // y + offsetY, so the particle's (x, y) is the anchor above the streak.
          const left = particle.x + frame.offsetX;
          const top = particle.y + frame.offsetY - frame.height + 1;
          // Blend directly against the preexisting canvas. A source-over draw here would replace
          // the terrain under opaque cel pixels before the PL2 lookup and blend the cel against itself.
          blend(ctx, frame, left, top, 3);
          const record = recordDc6(layer.path, index, frame, left, top, { op: 'cel', mode: 3, extra: 0 });
          if (record && record.hash) blits.push(record);
          live.push({ layer: layer.path, wx: particle.wx, wy: particle.wy, sx: particle.x, sy: particle.y, frame: index, waterCell: particle.waterCell });
        }
      }
      return blits;
    },
  };
}
