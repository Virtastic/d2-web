// The real map, drawn from the engine's own tile list.
import { terrainDrawOp } from './terrain-draw-identity.mjs';
import { wallHidesPlayer } from './roof-hide.mjs';
import { createRoofFades } from './roof-fade.mjs';
import { perfT0, perfT, perfSample } from './frame-perf.mjs';
//
// What this replaces: retail-tile-view.mjs picked ONE hardcoded DT1 per act, took a dozen
// "representative" tiles out of it, and tiled them as a backdrop. Every level in an act therefore
// drew the same ground -- Andariel's Lair rendered the Rogue Encampment.
//
// What the engine actually provides, and this uses:
//   * d2_session_level_tile_files -> the DT1 libraries this LEVEL's type uses (LvlTypes.txt), all
//     32 slots in order, so a tile's librarySlot indexes the list directly.
//   * d2_session_tiles -> one record per placed tile straight out of the DRLG: which library and
//     which tile in it, its layer, and its absolute isometric screen position.
//
// Projection. Diablo II is isometric: 80px per tile on X, 40 on Y, and five subtiles to a tile. The
// engine hands out absolute screen coordinates in that space, which is where the constants below
// come from -- they are not invented here, they are what `screenX/screenY` in the tile records
// already satisfy (verified against a dumped level: tile (4516,1924) -> (207280,257680), and the
// player's subtile position maps through the same formula).
export const SUBTILES_PER_TILE = 5;
export const TILE_SCREEN_W = 80;
export const TILE_SCREEN_H = 40;
// Per SUBTILE: 80/5 = 16 across, 40/5 = 8 down. The -80/+80 match the engine's own origin.
export const SUBTILE_SCREEN_X = TILE_SCREEN_W / SUBTILES_PER_TILE;
export const SUBTILE_SCREEN_Y = TILE_SCREEN_H / SUBTILES_PER_TILE;

/** Absolute isometric screen position of a world (subtile) coordinate, in the engine's own frame. */
export function worldToScreen(x, y) {
  return { x: (x - y) * SUBTILE_SCREEN_X - 80, y: (x + y) * SUBTILE_SCREEN_Y + 80 };
}

/** Inverse: a screen offset from the player, back to a world (subtile) delta. */
export function screenDeltaToWorld(dx, dy) {
  return {
    x: dx / (2 * SUBTILE_SCREEN_X) + dy / (2 * SUBTILE_SCREEN_Y),
    y: dy / (2 * SUBTILE_SCREEN_Y) - dx / (2 * SUBTILE_SCREEN_X),
  };
}

/**
 * The light map cell (subtile) each of a wall's SIX light entries reads, for a wall tile whose corner is
 * (A, B) = the client's tile corner (worldX/Y * 5), by orientation. D2DDraw+0x7320 (read)
 * lights a wall block at DT1 x from entry (x >> 5) to entry (x >> 5) + 1 of a six-entry array (blocks
 * reach x 128, so entries 0..5 are all used); the hook recorded only the first four until an earlier change.
 * Fitted on retail's records against the same frame's 48x48 light map: 21 Rogue Encampment captures
 * (4 entries each) + 13 more with the player's light moving across walls, tree groups and 3/4 wall ends
 * (all 6 entries) -- s257 checks every record.
 *   2 (right wall)            (A - 5 + 2j, B + 2)
 *   1 (left wall)             (A + 2, B + 5 - 2j)
 *   5, 8                      the left wall's line too: 8 fitted on Harrogath's main gate (s238, F120), 5 on the Fortress east gate (s345, F118b: 17,414 of
 *                             17,414 non-hidden pixels vs 4,725 for the right-wall default; the gate's own o8 walls are hidden by the hero and lit uniformly 255 there, so
 *                             that scene is silent on o8)
 *   3, 4, 12, 14 (the rest)   (A + 2, B + 5), (A + 2, B + 3), (A + 2, B + 2), (A + 2, B + 2), (A + 3, B + 2), (A + 5, B + 2)
 *   7 (corner)                the right wall's line for j < 3, then the left wall's: (A + 2, B + 5 - 2j)
 * Orientations not in the captures take the right wall's line (client-parity F42).
 */
export function wallLightCells(o, A, B, j) {
  switch (o) {
    case 1: case 5: case 8: case 16: return [A + 2, B + 5 - 2 * j];
    case 3: case 4: case 12: case 14: case 18: return [[A + 2, B + 5], [A + 2, B + 3], [A + 2, B + 2], [A + 2, B + 2], [A + 3, B + 2], [A + 5, B + 2]][j];
    case 7: return j < 3 ? [A - 5 + 2 * j, B + 2] : [A + 2, B + 5 - 2 * j];
    // o18 (a lower-wall cliff face, isLower below) reads the SAME three-cell corner line as o3/o4/o12/o14,
    // not the right-wall default this switch used to give it. MEASURED two ways, 2026-09-30 (F480):
    //   * s257's own instrument, retail's recorded wall entries: the
    //     o3 rule reproduces all 11 of retail's o18 records (right line 8, left 7, a uniform tile-centre
    //     cell 8 -- the varying records like `t8-mesa-spire 18 5135 5090 -> 179 179 179 179 167 140` need
    //     the corner line, which the right default cannot make);
    //   * s260's single largest world cluster (1,133 of its 1,687 px, every one OURS BRIGHTER) is the
    //     Fortress arrival's o18 Inv_Wall cliff -- where the tile sits in the far field, so the corner line
    //     is six equal cells at the map's ambient and the right default reads the player's brighter light.
    //     o16 keeps the left-wall line (s257: 94/94) and o17 the right line (s257: 42/42). F480.
    default: return [A - 5 + 2 * j, B + 2];
  }
}

export const SESSION_TILE_BYTES = 84;
// Where the camera puts things, in retail's numbers:
// the player -- and so every unit, projected relative to him -- is drawn 8 px ABOVE the canvas
// centre (retail's town frame: the Barbarian at 400,292 on 800x600), and a tile's DT1 blocks land at
// (engine screen x + FLOOR_DX + bx, engine screen y + FLOOR_DY + by) relative to that same centre.
// Measured with retail's static objects (torches, banner, crates: map presets, so the same map seed
// puts them where retail has them): with FLOOR_DX 0 / FLOOR_DY -128 every one of them was drawn
// 80 px left of and 116 px below its place on the terrain.
export const UNIT_CENTRE_Y = -8;
export const FLOOR_DX = -80;
export const FLOOR_DY = -12;
// What a shadow tile leaves of the floor under it -- a MEASUREMENT, not retail's rule: median 0.27 over 3,832 shadow pixels of retail's
// Blood Moor frame (s191 "Shadow tiles").
export const SHADOW_KEEP = 0.27;
const silhouettes = new WeakMap();
function silhouette(bitmap) {
  let s = silhouettes.get(bitmap);
  if (!s) {
    s = (bitmap.ownerDocument || globalThis.document).createElement('canvas');
    s.width = bitmap.width; s.height = bitmap.height;
    const x = s.getContext('2d'); x.drawImage(bitmap, 0, 0); x.globalCompositeOperation = 'source-in';
    x.fillStyle = '#000'; x.fillRect(0, 0, s.width, s.height);
    silhouettes.set(bitmap, s);
  }
  return s;
}
// Retail's floor cull at the bottom: across eight captured frames the lowest floor it draws is anchored
// at y = 552 and the highest it skips at 568 (den-shadow; 584 in town), so the bound lies in (552, 568];
// taken as height - 40 (560 on 600). The top: a floor is drawn while its 80 rows reach y >= 0.
export const FLOOR_CULL_BOTTOM = 40;

/** Parse the engine's tile draw list. One record per placed tile; see D2SessionTile. */
export function parseSessionTiles(heap, pointer, count) {
  const view = new DataView(heap.buffer, heap.byteOffset || 0, heap.byteLength);
  const tiles = new Array(count);
  for (let i = 0; i < count; i++) {
    const b = pointer + i * SESSION_TILE_BYTES;
    tiles[i] = {
      room: view.getUint32(b, true), layer: view.getUint32(b + 4, true), ordinal: view.getUint32(b + 8, true),
      orientation: view.getInt32(b + 12, true), mainIndex: view.getInt32(b + 16, true),
      subIndex: view.getInt32(b + 20, true), frameIndex: view.getInt32(b + 24, true),
      worldX: view.getInt32(b + 40, true), worldY: view.getInt32(b + 44, true),
      screenX: view.getInt32(b + 48, true), screenY: view.getInt32(b + 52, true),
      width: view.getInt32(b + 56, true), height: view.getInt32(b + 60, true), flags: view.getUint32(b + 64, true),
      librarySlot: view.getUint32(b + 68, true), libraryIndex: view.getUint32(b + 72, true),
      // F66: retail's per-record D2DrlgTileDataStrc+0x10, the field that picks its transparent (roof,
      // slot 40) entry over the wall entry (39) on a wall-list record -- s338. The general rule is not
      // derived yet; this is the raw value, exported for that work.
      entryHint: view.getInt32(b + 76, true),
      pop: view.getUint32(b + 80, true),
    };
  }
  return tiles;
}

/**
 * Read the level's DT1 paths out of the engine, as the DENSE list `librarySlot` indexes.
 *
 * LvlTypes.txt gives a level type 32 File slots and writes the literal string "0" in the unused
 * ones. Retail loads only the populated entries, so the level's in-memory DT1 array is COMPACTED
 * and a tile's librarySlot indexes that, not the raw table. Keeping the empties in (so slot N lines
 * up with File N+1) looks tidier and is wrong: Act 1 Town leaves slot 6 blank, which shifted every
 * library from there on by one and left 31 tiles per frame with no bitmap -- black diamonds punched
 * through the town floor.
 */
export function readLevelTileFiles(Module, cap = 4096, exportName = 'd2_session_level_tile_files') {
  const p = Module._malloc(cap);
  try {
    const slots = Module.ccall(exportName, 'number', ['bigint', 'number'], [BigInt(p), cap]);
    const files = [];
    let o = Number(p);
    for (let i = 0; i < slots; i++) {
      let s = '';
      while (Module.HEAPU8[o]) { s += String.fromCharCode(Module.HEAPU8[o]); o++; }
      o++;
      // Unused slots come back as the literal "0" from the table, not as a blank. They are dropped
      // rather than kept as holes -- see above, the dense list is what librarySlot indexes.
      if (s && s !== '0') files.push(s);
    }
    return files;
  } finally { Module._free(p); }
}

/**
 * The engine's light map (d2_session_lightmap: D2Client's 48x48 subtile map around the player) as the
 * floor-grid source litFloor wants: a floor at world subtile (a, b) reads the 8x8 padded grid of cells
 * (a - 1 + i, b - 1 + j), clamped into the map, exactly as D2Client+0xA38C0 / +0xA8230 fill it.
 *
 * `occludeUnits` defaults ON, which is the CAPTURE's own setting and not a preference: D2Client
 * occludes a unit's light (a player's or a monster's -- objects always) only above the Light Quality
 * option, and retail's captures were taken with it on. The two scenes that decided the flag the other
 * way cannot see it at all -- turning it on changes NO cell there -- while the
 * Flayer Jungle's waypoint arrival (s697) needs it: with the player's light unoccluded 18 of
 * the 96 floor tiles retail's own captured map carries are wrong (worst corner 61/255, 66 cells of the
 * 48x48 grid, every one of them OURS HIGHER), and with it occluded all 96 tiles and all 2304 cells are
 * retail's exactly. The flag grid is identical to retail's either way (2304 of 2304): the mask is not
 * the variable, the occlusion is. Setting it is a quality OPTION we do not model, so the measured arm
 * is the default and a caller that needs the other one passes it.
 */
export function readSessionLightmap(Module, ambientOverride = -1, occludeUnits = 1) {
  if (!Module._d2_session_lightmap) return null;
  const p = Number(Module.ccall('d2_session_lightmap', 'number', ['number', 'number'], [ambientOverride, occludeUnits]));
  const v = new DataView(Module.HEAPU8.buffer, p, 16 + 48 * 48 * 2);
  const ox = v.getInt32(0, true), oy = v.getInt32(4, true);
  const cells = Module.HEAPU8.slice(p + 16, p + 16 + 48 * 48), blocked = Module.HEAPU8.slice(p + 16 + 48 * 48, p + 16 + 48 * 48 * 2);
  const at = (x, y) => cells[Math.max(0, Math.min(47, y - oy)) * 48 + Math.max(0, Math.min(47, x - ox))];
  const sv = new DataView(Module.HEAPU8.buffer, p + 16 + 48 * 48 * 2, 64 * 20), list = [];
  for (let i = 0; i < Math.min(64, v.getInt32(12, true)); i++) list.push([0, 4, 8, 12, 16].map((o) => sv.getInt32(i * 20 + o, true)));
  return { origin: [ox, oy], ambient: v.getInt32(8, true), sources: v.getInt32(12, true), list, cells, blocked, at,
    grid: (a, b) => { const g = new Array(64); for (let j = 0; j < 8; j++) for (let i = 0; i < 8; i++) g[j * 8 + i] = at(a - 1 + i, b - 1 + j); return g; } };
}

/**
 * Everything the terrain view draws: the current level's tiles, then the tiles of the OTHER levels'
 * active rooms (d2_session_seam_tiles) -- across an open seam retail draws the neighbouring level's
 * ground (client-parity F24; s187 measured it on the Cold Plains' first frame). The seam tiles index
 * their own library list, appended here after the level's, so one `files` array serves both.
 */
export function readSessionTerrain(Module) {
  // One world rebuild and one tile build for the ten reads below, not one of each per read. Every
  // world rebuild marks the tile lists stale (monspawn_check.cpp updateSessionWorld), so ten loose
  // reads build the level's tile list ten times -- 10.8 ms of the engine's 11.7 ms a frame, measured
  // by s2311, which also measures a single call at 0.652 ms loose against 0.000 held. The engine's
  // own hold is the mechanism, the same one the shell's snapshot read uses; nothing mutates between
  // these reads, so every value returned here is the one a loose read returned (F236).
  const hold = Boolean(Module._d2_session_hold_world);
  if (hold) Module.ccall('d2_session_hold_world', null, ['number'], [1]);
  try { return readSessionTerrainHeld(Module); }
  finally { if (hold) Module.ccall('d2_session_hold_world', null, ['number'], [0]); }
}

function readSessionTerrainHeld(Module) {
  const count = Module.ccall('d2_session_tile_count', 'number', [], []);
  const ptr = Number(Module.ccall('d2_session_tiles', 'number', [], []));
  const tiles = count > 0 && ptr > 0 ? parseSessionTiles(Module.HEAPU8, ptr, count) : [];
  // The engine's own pick: each tile's entry index in its DT1 (parallel array, D2WEB_TileEntryFileIndex).
  const entries = (ptr, n, into) => { if (!ptr || !Module._d2_session_tile_entries) return; const u = new Uint32Array(Module.HEAPU8.buffer, ptr, n); into.forEach((t, i) => { t.entry = u[i]; }); };
  entries(count > 0 ? Number(Module.ccall('d2_session_tile_entries', 'number', [], [])) : 0, count, tiles);
  // F66: each wall tile's coord list (parallel array) and the player's, so the client can apply retail's own
  // hide test (roof-hide.mjs) and record the entry the tile goes through. Absent on an older engine.
  const coords = (ptr, n, into) => {
    if (!ptr || !Module._d2_session_player_coordlist) return;
    const v = new DataView(Module.HEAPU8.buffer, ptr, n * 20);
    const px = Module.ccall('d2_session_player_x', 'number', [], []), py = Module.ccall('d2_session_player_y', 'number', [], []);
    const player = { tileX: Math.trunc(px / 5), tileY: Math.trunc(py / 5), list: Module.ccall('d2_session_player_coordlist', 'number', [], []) };
    into.forEach((t, i) => {
      const nIndex = v.getInt32(i * 20, true);
      t.coord = nIndex < 0 ? null : { nIndex, x0: v.getInt32(i * 20 + 4, true), y0: v.getInt32(i * 20 + 8, true), x1: v.getInt32(i * 20 + 12, true), y1: v.getInt32(i * 20 + 16, true) };
      t.hidesPlayer = t.layer === 1 && wallHidesPlayer(t, t.coord, player);
    });
  };
  coords(count > 0 && Module._d2_session_tile_coordlists ? Number(Module.ccall('d2_session_tile_coordlists', 'number', [], [])) : 0, count, tiles);
  const files = readLevelTileFiles(Module);
  if (Module._d2_session_seam_tiles) {
    const seamCount = Module.ccall('d2_session_seam_tile_count', 'number', [], []);
    const seamPtr = Number(Module.ccall('d2_session_seam_tiles', 'number', [], []));
    if (seamCount > 0 && seamPtr > 0) {
      const base = files.length;
      files.push(...readLevelTileFiles(Module, 4096, 'd2_session_seam_tile_files'));
      const seam = parseSessionTiles(Module.HEAPU8, seamPtr, seamCount);
      entries(Number(Module.ccall('d2_session_seam_tile_entries', 'number', [], [])), seamCount, seam);
      coords(Module._d2_session_seam_tile_coordlists ? Number(Module.ccall('d2_session_seam_tile_coordlists', 'number', [], [])) : 0, seamCount, seam);
      for (const t of seam) tiles.push({ ...t, librarySlot: t.librarySlot + base, seam: true });
    }
  }
  return { tiles, files };
}

/**
 * Draw-list record for a drawn tile (play/assets/drawlist.mjs), in RETAIL's anchor convention, derived
 * from where this renderer actually put the pixels. Measured on retail's own town frame against its
 * draw list: a wall's DT1 block (bx, by) lands at anchor + (bx, by); a tile
 * drawn through the floor entry (orientation 0 and 15) at anchor + (bx - 80, by).
 *
 * The op is the RENDERER ENTRY retail submits the tile through, and the layer says which of the room's
 * three tile lists the engine put it in (0 floors, 1 walls, 2 roofs). Retail takes pRoofTiles through
 * its roof (transparent) entry -- the pass after the units -- so a layer-2 tile recorded as `wall`
 * would be compared against an entry the parser does not even read (the Outer Cloister's and the
 * Catacombs Level 2's "five/16 extra walls", F65). Orientation 13 still wins over the layer: retail
 * draws those through the shadow-tile entry whichever list holds them.
 */
function recordTile(entry, files) {
  const list = globalThis.__d2DrawList;
  if (!Array.isArray(list)) return;
  const t = entry.tile, px = entry.dt1Tile.pixels, floor = t.orientation === 0 || t.orientation === 15;
  const file = String(files[entry.slot] ?? '?').toLowerCase().replace(/\//g, '\\');
  const op = terrainDrawOp(t);
  list.push({ op, path: `${file} ${t.orientation} ${t.mainIndex} ${t.subIndex}`, frame: entry.dt1Tile.rarity,   // the DRAWN art's rarity
    x: entry.x - px.originX + (floor ? 80 : 0), y: entry.y - px.originY, mode: 0, hash: 0, a: t.worldX * 5, b: t.worldY * 5,
    rect: [entry.x, entry.y, entry.bitmap.width, entry.bitmap.height] });   // the painted rectangle: a cull check reads it
}

/**
 * Terrain for one level: decodes the level's DT1 libraries lazily and draws the engine's tile list.
 * A library that fails to decode is recorded and skipped rather than aborting the level -- a
 * missing wall set should cost you those tiles, not the whole map.
 */
export function createSessionTerrain({ read, decodeDt1, decodeDatPalette, palettePath, documentObject = globalThis.document }) {
  // Keyed by FILE, not slot: the engine's library list is rebuilt with the world and grows as the
  // activation wave brings in a neighbouring level's rooms, so a slot number names different files
  // over a level's life (walking the Fortress into the Outer Steppes drew Mesa walls from Fortress
  // libraries -- flat coloured quads -- while nothing reported missing).
  const libraries = new Map();   // path -> { byKey: Map, error }
  const bitmaps = new Map();     // `${path}:${index}` -> canvas
  let palette = null;
  const errors = [];

  const paletteBytes = () => (palette ??= decodeDatPalette(read(palettePath)));
  // The act's PL2 (next to its pal.dat): AlphaBlend[0] is the table retail's shadow tiles go through.
  let pl2 = undefined, pl2Index = null;
  const blendPl2 = () => {
    if (pl2 === undefined) {
      try { pl2 = read(palettePath.replace(/pal\.dat$/i, 'pal.pl2')); } catch { pl2 = null; }
      if (pl2) { pl2Index = new Map(); for (let j = 255; j >= 0; j--) pl2Index.set((pl2[j * 4] << 16) | (pl2[j * 4 + 1] << 8) | pl2[j * 4 + 2], j); }
    }
    return pl2;
  };
  const indexOf = (r, g, b) => {
    const key = (r << 16) | (g << 8) | b; let j = pl2Index.get(key);
    if (j === undefined) { let bd = Infinity; for (let q = 0; q < 256; q++) { const dr = pl2[q * 4] - r, dg = pl2[q * 4 + 1] - g, db = pl2[q * 4 + 2] - b, dd = dr * dr + dg * dg + db * db; if (dd < bd) { bd = dd; j = q; } } pl2Index.set(key, j); }
    return j;
  };
  const shadowSources = new WeakMap();   // bitmap -> { idx: Uint8Array, alpha: Uint8Array }
  const shadowSource = (bitmap) => {
    let v = shadowSources.get(bitmap);
    if (!v) {
      const d = bitmap.getContext('2d').getImageData(0, 0, bitmap.width, bitmap.height).data;
      const idx = new Uint8Array(bitmap.width * bitmap.height), alpha = new Uint8Array(idx.length);
      for (let q = 0; q < idx.length; q++) { alpha[q] = d[q * 4 + 3]; if (alpha[q]) idx[q] = indexOf(d[q * 4], d[q * 4 + 1], d[q * 4 + 2]); }
      v = { idx, alpha }; shadowSources.set(bitmap, v);
    }
    return v;
  };
  // Retail's shadow-tile draw, from the code: D2DDraw's
  // slot 41 hands each DT1 block to D2gfx's shadow-block drawer (D2gfx+0x99C0), which writes
  // dst = AlphaBlend[0][dst * 256 + src] per pixel -- the act PL2's first 64 KiB blend table (dumped
  // from the running game and compared byte for byte, s191).
  const drawShadow = (ctx, o) => {
    const t = blendPl2(); if (!t) return false;
    const { idx, alpha } = shadowSource(o.bitmap);
    const w = o.bitmap.width, h = o.bitmap.height, x0 = Math.max(0, o.x), y0 = Math.max(0, o.y);
    const x1 = Math.min(ctx.canvas.width, o.x + w), y1 = Math.min(ctx.canvas.height, o.y + h);
    if (x1 <= x0 || y1 <= y0) return true;
    const img = ctx.getImageData(x0, y0, x1 - x0, y1 - y0), d = img.data, A = 1024 + 49 * 256;
    for (let y = y0; y < y1; y++) for (let x = x0; x < x1; x++) {
      const q = (y - o.y) * w + (x - o.x); if (!alpha[q]) continue;
      const i = ((y - y0) * (x1 - x0) + (x - x0)) * 4;
      const o2 = t[A + indexOf(d[i], d[i + 1], d[i + 2]) * 256 + idx[q]];
      d[i] = t[o2 * 4]; d[i + 1] = t[o2 * 4 + 1]; d[i + 2] = t[o2 * 4 + 2];
    }
    ctx.putImageData(img, x0, y0); return true;
  };

  function library(slot, files) {
    const path = files[slot];
    if (libraries.has(path ?? '')) return libraries.get(path ?? '');
    let entry = { byKey: new Map(), byIndex: [], error: path ? '' : 'empty slot' };
    if (path) {
      try {
        const dt1 = decodeDt1(read(path));
        entry.byIndex = dt1.tiles;
        // Group by identity, keeping FILE ORDER within each group: a tile's `libraryIndex` counts
        // variants that share an identity, so the n-th tile with this (orientation, main, sub) is
        // what the DRLG asked for.
        for (const t of dt1.tiles) {
          const key = `${t.orientation}:${t.mainIndex}:${t.subIndex}`;
          const bucket = entry.byKey.get(key);
          if (bucket) bucket.push(t); else entry.byKey.set(key, [t]);
        }
      } catch (error) { entry.error = String(error); errors.push(`${path}: ${error}`); }
    }
    libraries.set(path ?? '', entry);
    return entry;
  }

  // Floor lighting as retail draws it: D2gfx's floor draw (+0xA960)
  // lights each 32x15 iso block from four corner intensities of the tile's light grid (6x6 corners,
  // gridX + 8*gridY in retail's layout): nearly equal corners (|B-A|+|C-A|+|D-B| < 10) draw flat at
  // level A>>3, others gouraud -- per row r the edge levels ((E0<<4) + r*(E3-E0)) >> 7 and
  // ((E1<<4) + r*(E2-E1)) >> 7, per column x level L + floor((R-L)*x/32) (the 32x32x32 table D2gfx
  // builds at +0x15450, which is exactly that). The pixel is PL2 LightLevelVariations[level][index].
  const litCache = new Map();
  const ISO_SKIP = [14, 12, 10, 8, 6, 4, 2, 0, 2, 4, 6, 8, 10, 12, 14], ISO_RUN = [4, 8, 12, 16, 20, 24, 28, 32, 28, 24, 20, 16, 12, 8, 4];
  // A gouraud floor block the frame clips on the LEFT is shaded from the LEVEL TABLE one entry per DRAWN
  // pixel from the row's own start, not by the pixel's column in the block: retail's two clipped gouraud
  // drawers (D2gfx+0x9E90 RLE, +0xA5D0 iso) advance the table pointer once per pixel written while the
  // source and destination pointers start at (skip + leftclip), and D2MOO's `sub_6FA72090` -- the 1.10f
  // decompile of that same pair -- does `v24[i] = table[v19[i]][...]` with `v24 = buffer + skip + leftclip`
  // and `v19 = table + skip`. So the index is `blockColumn - leftclip[r]`, leftclip[r] being how far the
  // clip cuts into the row: clamp(canvasLeft - (blockScreenX + skip[r]), 0, run[r]). Unclipped blocks
  // (leftclip 0) are unchanged: `blockColumn`. Read 2026-09-30; measured on F96's band -- 348 of 348
  // misses, and 0 of them our old index, before the change (probe-f249-clip). F249.
  const CLIP_LEFT = 0;
  function litFloor(path, dt1Tile, index, grid, tileX, tileY) {
    const px = dt1Tile.pixels;
    const keyBase = `${path}:${index}:${grid.join(',')}`;
    // Retail clips the left lookup and starts iso edge interpolation at the first visible row.
    // Tiles crossing either boundary therefore need a position-dependent cache key.
    const key = (px && px.width && tileX !== undefined && tileX < CLIP_LEFT
      && dt1Tile.blocks.some((b) => tileX + b.x - px.originX < CLIP_LEFT)) ? `${keyBase}@${tileX}` : keyBase;
    const positionKey = px && tileY !== undefined && dt1Tile.blocks.some((b) => b.format === 1 && tileY + b.y - px.originY < 0) ? `${key}:${tileY}` : key;
    if (litCache.has(positionKey)) return litCache.get(positionKey);
    const t = blendPl2();
    if (!t || !px || !px.width) { litCache.set(positionKey, null); return null; }
    const idx = new Uint8Array(px.indices), opq = px.opaque;
    // The light buffer is an 8x8 padded grid of 12-byte entries (intensity first); a block at
    // (gridX, gridY) has its corners at (gridX+1..gridX+2, gridY+1..gridY+2). Read from D2gfx+0xA960.
    const P = (i, j) => grid[Math.min(7, j) * 8 + Math.min(7, i)];
    for (const b of dt1Tile.blocks) {
      const x0 = b.gridX, y0 = b.gridY;
      const A = P(x0 + 1, y0 + 1), B = P(x0 + 2, y0 + 1), C = P(x0 + 1, y0 + 2), D = P(x0 + 2, y0 + 2);
      const c = { A, B, C, D };
      const flat = Math.abs(B - A) + Math.abs(C - A) + Math.abs(D - B) < 10;
      // Gouraud edges: each corner is the average of four entries around it, exactly as the code sums them.
      const E = [(P(x0, y0 + 2) + C + A + P(x0, y0 + 1)) >> 2, (P(x0 + 2, y0) + B + P(x0 + 1, y0) + A) >> 2,
        (P(x0 + 3, y0 + 2) + C + P(x0 + 3, y0 + 1) + B) >> 2, (P(x0 + 1, y0 + 3) + P(x0 + 2, y0 + 3) + D + C) >> 2];
      // RLE blocks (flag 4) go through D2gfx+0x8E30 (flat: the same light table) or D2gfx+0x8A40
      // (gouraud: 15 rows, edges stepped once per row, the level table indexed exactly as the iso path)
      // -- read from the code. Both index the table by the pixel's column in the block, less
      // the row's leftclip (see CLIP_LEFT above, F249).
      const iso = b.format === 1, rows = 15;
      for (let r = 0; r < rows; r++) {
        // D2gfx 1.13c +0xA5D0 advances iso edges only for visible rows; RLE advances skipped rows too.
        const topclip = iso && tileY !== undefined ? Math.max(0, -(tileY + b.y - px.originY)) : 0;
        if (iso && r < topclip) continue;
        const rr = r - topclip;
        const L = ((E[0] << 4) + rr * (E[3] - E[0])) >> 7, R = ((E[1] << 4) + rr * (E[2] - E[1])) >> 7;
        const skip = iso ? ISO_SKIP[r] : 0, run = iso ? ISO_RUN[r] : 32;
        const leftclip = tileX === undefined ? 0 : Math.max(0, Math.min(run, CLIP_LEFT - (tileX + b.x - px.originX + skip)));
        for (let col = 0; col < run; col++) {
          const x = skip + col, q = (b.y + r - px.originY) * px.width + (b.x + x - px.originX);
          // An RLE skip leaves earlier blocks intact: use this block's write mask and source.
          if (b.y + r - px.originY >= px.height || !b.pixels.written[r * 32 + x]) continue;
          const lvl = flat ? c.A >> 3 : L + Math.floor((R - L) * (x - leftclip) / 32);
          idx[q] = t[1024 + lvl * 256 + b.pixels.indices[r * 32 + x]];
        }
      }
    }
    const rgba = new Uint8ClampedArray(idx.length * 4);
    for (let q = 0; q < idx.length; q++) { if (!opq[q]) continue; rgba[q * 4] = t[idx[q] * 4]; rgba[q * 4 + 1] = t[idx[q] * 4 + 1]; rgba[q * 4 + 2] = t[idx[q] * 4 + 2]; rgba[q * 4 + 3] = 255; }
    const canvas = documentObject.createElement('canvas'); canvas.width = px.width; canvas.height = px.height;
    canvas.getContext('2d').putImageData(new ImageData(rgba, px.width, px.height), 0, 0);
    if (litCache.size > 1500) litCache.clear();   // the light moves with the player: keep the cache bounded
    litCache.set(positionKey, canvas); return canvas;
  }
  // Wall lighting: a wall reaches D2DDraw slot 39 with four light
  // entries, and D2DDraw+0x7320 lights each 32-px block from entry (blockX >> 5) to the next (corners
  // left, right, right, left). The entries are light map cells, wallLightCells (fitted per orientation).
  const wallCells = wallLightCells;
  // roof: the block drawer of D2DDraw's transparent entry (slot 40 -> D2gfx 6fa89660) has no flat shortcut: every block reads the level table T[E[k]>>3][E[k+1]>>3][x]
  // (the wall drawer 6fa8abf0 draws a block flat at E[k] when |dE| is small); s996 fits every pixel of retail's two roof blits to one (a, b) per block.
  function litWall(path, dt1Tile, index, E, roof = false) {
    const key = `w${roof ? 'r' : ''}${path}:${index}:${E.join(',')}`;
    if (litCache.has(key)) return litCache.get(key);
    const t = blendPl2(), px = dt1Tile.pixels;
    if (!t || !px || !px.width) { litCache.set(key, null); return null; }
    const idx = new Uint8Array(px.indices), opq = px.opaque;
    for (const b of dt1Tile.blocks) {
      const k = Math.min(4, Math.max(0, b.x >> 5)), Lv = E[k], Rv = E[k + 1];
      const flat = !roof && 3 * Math.abs(Rv - Lv) < 10, L = Lv >> 3, R = Rv >> 3;
      const iso = b.format === 1, rows = iso ? 15 : 32;
      for (let r = 0; r < rows; r++) {
        const skip = iso ? ISO_SKIP[r] : 0, run = iso ? ISO_RUN[r] : 32;
        for (let col = 0; col < run; col++) {
          const x = skip + col, yy = b.y + r - px.originY, xx = b.x + x - px.originX;
          if (yy < 0 || yy >= px.height || xx < 0 || xx >= px.width) continue;
          const q = yy * px.width + xx;
          if (!opq[q]) continue;
          const lvl = flat ? L : L + Math.floor((R - L) * x / 32);
          idx[q] = t[1024 + lvl * 256 + px.indices[q]];
        }
      }
    }
    const rgba = new Uint8ClampedArray(idx.length * 4);
    for (let q = 0; q < idx.length; q++) { if (!opq[q]) continue; rgba[q * 4] = t[idx[q] * 4]; rgba[q * 4 + 1] = t[idx[q] * 4 + 1]; rgba[q * 4 + 2] = t[idx[q] * 4 + 2]; rgba[q * 4 + 3] = 255; }
    const canvas = documentObject.createElement('canvas'); canvas.width = px.width; canvas.height = px.height;
    canvas.getContext('2d').putImageData(new ImageData(rgba, px.width, px.height), 0, 0);
    canvas.paletteIdx = idx; canvas.paletteOpaque = opq;   // the lit wall's own palette indices: a hidden wall is BLENDED through a PL2 table, which is indexed by them (drawFaded)
    if (litCache.size > 1500) litCache.clear();
    litCache.set(key, canvas); return canvas;
  }
  // A wall the client hides is drawn through D2DDraw's transparent entry (40) at the fade's alpha (F66b). Its block drawer writes per
  // pixel dst = AlphaBlend[k][src * 256 + dst] of the act PL2 -- k by alpha exactly as the shadow drawer picks it (>= 0xC0 table 0,
  // >= 0x80 table 1, >= 0x40 table 2), src being the wall's LIT palette index. Measured on the Fortress east gate against retail's own
  // frame buffer (s345): table 1, src-major, reproduces 17,323 of 19,404 hidden-gate pixels the canvas-alpha stand-in got 10,625 of
  // (dst-major: 10,625; tables 0/2: 4,809/2,574 -- and the orientation-1 walls 1,243 of 1,252).
  const drawFaded = (ctx, o, lit, alpha) => {
    const t = blendPl2(); if (!t || !lit || !lit.paletteIdx) return false;
    const idx = lit.paletteIdx, opq = lit.paletteOpaque, w = lit.width, h = lit.height;
    const x0 = Math.max(0, o.x), y0 = Math.max(0, o.y), x1 = Math.min(ctx.canvas.width, o.x + w), y1 = Math.min(ctx.canvas.height, o.y + h);
    if (x1 <= x0 || y1 <= y0) return true;
    const img = ctx.getImageData(x0, y0, x1 - x0, y1 - y0), d = img.data, A = 1024 + 49 * 256 + (alpha >= 0xC0 ? 0 : alpha >= 0x80 ? 1 : 2) * 65536;
    for (let y = y0; y < y1; y++) for (let x = x0; x < x1; x++) {
      const q = (y - o.y) * w + (x - o.x); if (!opq[q]) continue;
      const i = ((y - y0) * (x1 - x0) + (x - x0)) * 4;
      const r = t[A + idx[q] * 256 + indexOf(d[i], d[i + 1], d[i + 2])];
      d[i] = t[r * 4]; d[i + 1] = t[r * 4 + 1]; d[i + 2] = t[r * 4 + 2];
    }
    ctx.putImageData(img, x0, y0); return true;
  };
  const roofFade = createRoofFades();   // F66b: per-tile alpha of hidden walls
  let lightAmbient = null;   // the light map's ambient: roofs are lit flat at it
  let lightCell = null;   // (subtileX, subtileY) -> the light map cell, for walls
  let lightGrid = null;   // (worldSubtileX, worldSubtileY) -> 36 corner intensities, or null (unlit)

  function bitmapFor(path, tile, index) {
    const key = `${path}:${index}`;
    const cached = bitmaps.get(key);
    if (cached !== undefined) return cached;
    let canvas = null;
    const px = tile?.pixels;
    if (px && px.width > 0 && px.height > 0) {
      const pal = paletteBytes();
      const rgba = new Uint8ClampedArray(px.indices.length * 4);
      for (let i = 0; i < px.indices.length; i++) {
        if (!px.opaque[i]) continue;
        const s = px.indices[i] * 4;
        rgba.set(pal.subarray(s, s + 4), i * 4);
      }
      canvas = documentObject.createElement('canvas');
      canvas.width = px.width; canvas.height = px.height;
      canvas.getContext('2d').putImageData(new ImageData(rgba, px.width, px.height), 0, 0);
    }
    bitmaps.set(key, canvas);
    return canvas;
  }

  // PERF (F198): the identity search below is a pure function of the tile's own fields and the level's file list (libraries are decoded once
  // per path and never change), and a level holds thousands of tiles that resolve the same way every frame. Resolved once per distinct tile
  // shape, with the counters the pass reports carried on the record so the evidence is exactly what a per-frame resolve gave.
  let resolveFiles = '';
  const resolved_ = new Map();
  function resolveTile(tile, files) {
    const cacheable = tile.librarySlot >= 0 && tile.librarySlot < 64 && tile.orientation >= 0 && tile.orientation < 64 && tile.mainIndex >= 0 && tile.mainIndex < 4096
      && tile.subIndex >= 0 && tile.subIndex < 4096 && tile.libraryIndex >= 0 && tile.libraryIndex < 65536 && tile.frameIndex >= 0 && tile.frameIndex < 65536
      && (tile.entry === undefined || (tile.entry >= 0 && tile.entry < 65535));
    let k1, k2, inner;
    // F96 throwaway probe hook: globalThis.__d2VariantOverride forces the DRAWN DT1 variant of one tile
    // (keyed `${a},${b},${layer},${identity}`, a,b = world subtiles) so a frame test can force each
    // same-rarity sibling and count the misses. It disables the resolve cache so a redraw re-picks.
    const vov = globalThis.__d2VariantOverride;
    if (cacheable && !vov) {
      k1 = ((tile.librarySlot * 64 + tile.orientation) * 4096 + tile.mainIndex) * 4096 + tile.subIndex;
      k2 = (tile.libraryIndex * 65536 + tile.frameIndex) * 65536 + (tile.entry === undefined ? 0 : tile.entry + 1);
      inner = resolved_.get(k1);
      const hit = inner && inner.get(k2);
      if (hit) return hit;
    }
    let searched = 0, ambiguous = 0, entryTiles = 0, entryExact = 0, entryDiffers = 0, rarityAmbiguous = 0, resolved = 0, wrongIdentity = 0;
    // An empty/failed slot is not fatal any more: the identity search below can still find
    // the tile in another library, which is the normal case now that librarySlot is per-room.
    const lib = library(tile.librarySlot, files);
    // Resolve by IDENTITY across the level's libraries, then by variant.
    //
    // `librarySlot` cannot be used to pick the file: it indexes the ROOM's own pTiles[] array,
    // which holds only the libraries that room loaded, in its own order -- not the level's
    // LvlTypes list. In Andariel's Lair slot 2 pointed at a stairway library while the tiles
    // there wanted a floor identity, leaving 715 floors undrawn and the dungeon floorless.
    //
    // So the identity is the key and the libraries are searched. That is only safe if an
    // identity lives in ONE library; `ambiguous` counts violations and s126 asserts it is zero,
    // so if a level ever breaks that assumption it is reported rather than silently mispicked.
    //
    // `libraryIndex` is NOT an offset into the DT1's tile array. The engine derives it by
    // walking the room's tile hash for this (type, style, sequence) -- orientation, mainIndex,
    // subIndex -- and counting position within that bucket's ref list. Indexing the file array
    // with it returns an unrelated tile, and because that lookup always succeeds the old
    // `?? byKey` fallback never fired: every floor tile on screen was the wrong art, while the
    // "0 unresolved tiles" check passed happily because something was always found.
    const key = `${tile.orientation}:${tile.mainIndex}:${tile.subIndex}`;
    let variants = lib.byKey.get(key), foundSlot = tile.librarySlot, matches = variants ? 1 : 0;
    if (!variants) {
      for (let slot = 0; slot < files.length; slot++) {
        if (slot === tile.librarySlot) continue;
        const other = library(slot, files);
        const candidate = other.byKey.get(key);
        if (!candidate) continue;
        matches++;
        if (!variants) { variants = candidate; foundSlot = slot; }
      }
      if (variants) searched++;
    }
    if (matches > 1) ambiguous++;
    // The variant is the one whose own rarity is the tile's (nRarity_Frame of the entry the engine
    // picked), not the libraryIndex-th: libraryIndex is the engine's hash-bucket position, which
    // need not be the DT1's order -- s192 found grass floors drawn with a sibling variant's art
    // while the draw list (which recorded the ASKED rarity) matched retail's.
    const sameRarity = variants ? variants.filter((v) => v.rarity === tile.frameIndex) : [];
    // libraryIndex counts the engine's hash-bucket ref list, which D2CMP builds by PREPENDING each
    // DT1 entry: variant i of the bucket is the DT1's (n - 1 - i)-th entry of that identity (s192:
    // same-rarity grass variants drew a sibling's art; reversing took town floors 118,824 -> 17,986
    // differing pixels).
    const vi = variants ? variants.length - 1 - tile.libraryIndex : 0;
    // The engine's own entry (file index of the pTile the room picked) beats every heuristic above when
    // it names a tile of this identity in this room's own library: the exact variant, not a guess.
    const exact = variants && foundSlot === tile.librarySlot && tile.entry !== undefined ? lib.byIndex[tile.entry] : undefined;
    const heur = variants ? (sameRarity.length === 1 ? sameRarity[0] : (variants[vi] ?? variants[0])) : undefined;
    const exactOk = exact && `${exact.orientation}:${exact.mainIndex}:${exact.subIndex}` === key;
    if (variants) { entryTiles++; if (exactOk) { entryExact++; if (exact !== heur) entryDiffers++; } }
    let dt1Tile = variants ? (exactOk ? exact : heur) : undefined;
    if (vov && variants) { const f = vov[`${tile.worldX * 5},${tile.worldY * 5},${tile.layer},${key}`]; if (f !== undefined && variants[f]) dt1Tile = variants[f]; }
    if (variants && sameRarity.length !== 1) rarityAmbiguous++;
    if (dt1Tile) {
      resolved++;
      // Structurally 0, and recorded here so no row quotes it as evidence (T10): `dt1Tile`
      // comes out of `variants`, the byKey bucket the wanted identity was READ OUT OF, so the drawn
      // identity cannot differ from the asked one -- the entry IS the key. It catches nothing, by
      // construction. What catches a wrong lookup is the blob compared against RETAIL, not itself:
      // `missing` (no entry at all) and the draw-list rows (s182/s183/s243/s330) that compare the
      // variant drawn with the one retail drew. Kept because the shape is what a future non-search
      // resolver would be checked with, and s126/s364/s370 still read it.
      if (`${tile.orientation}:${tile.mainIndex}:${tile.subIndex}` !== `${dt1Tile.orientation}:${dt1Tile.mainIndex}:${dt1Tile.subIndex}`) wrongIdentity++;
    }
    // Keyed by the DRAWN variant (its position among the identity's variants), never the asked index.
    const variantNo = variants ? variants.indexOf(dt1Tile) : -1;
    const bitmap = dt1Tile && bitmapFor(files[foundSlot], dt1Tile,
      `${tile.orientation}:${tile.mainIndex}:${tile.subIndex}:v${variantNo}`);
    const rec = { lib, variants, foundSlot, dt1Tile, variantNo, bitmap, searched, ambiguous, entryTiles, entryExact, entryDiffers, rarityAmbiguous, resolved, wrongIdentity,
      want: `${tile.orientation}:${tile.mainIndex}:${tile.subIndex}`, got: dt1Tile ? `${dt1Tile.orientation}:${dt1Tile.mainIndex}:${dt1Tile.subIndex}` : '' };
    if (cacheable) { if (!inner) resolved_.set(k1, inner = new Map()); inner.set(k2, rec); }
    return rec;
  }

  return {
    errors,
    /** Light floors from a grid source: fn(a, b) -> 36 intensities or null. */
    setLightGrid(fn, cell = null, ambient = null) { lightGrid = fn; lightCell = fn ? cell : null; lightAmbient = fn ? ambient : null; },
    /**
     * Draw the level. `tiles` is parseSessionTiles output, `files` readLevelTileFiles output,
     * `player` the snapshot player (subtile coords). Floors (layer 0) draw before walls (layer 1)
     * so walls overlap the ground they stand on.
     *
     * `sprites` interleaves world-anchored sprites (monsters, objects, items, the player) INTO the
     * wall pass instead of painting them all on top of it. Without this a monster standing behind a
     * wall draws over it, which is the single most obvious way an isometric scene reads as wrong.
     * Each entry is `{ depth, draw(ctx) }`; `depth` is the sprite's projected screen Y, the same
     * key the wall tiles sort on.
     *
     * Ordering caveat, stated plainly: this sorts the whole wall+sprite set by screen Y (ties broken
     * by the DRLG's own ordinal, so tile order stays stable). Retail instead walks rooms and then
     * cells within a room, which can differ at room seams and for tall multi-cell walls. This is an
     * approximation of that walk, not a port of it -- the room/cell draw order is not recovered yet.
     *
     * `afterFloors` runs between the floor pass and the wall pass, which is where retail draws its
     * weather: in the retail captures the frame's 96 floor draws are followed by the whole
     * rain block and only then by the shadows and the walls, so a wall or a roof covers the rain
     * behind it (client-parity F11b).
     */
    draw(ctx, tiles, files, player, width, height, sprites = [], afterFloors = null, centreX = width / 2) {
      const origin = worldToScreen(player.x, player.y);
      const offsetX = centreX - origin.x;
      const offsetY = height / 2 + UNIT_CENTRE_Y - origin.y;
      // Count resolution before viewport culling: an off-screen wrong lookup is still wrong art,
      // and must not disappear from level-2 requested-versus-drawn identity evidence.
      let drawn = 0, missing = 0, empty = 0, requested = 0, resolved = 0, wrongIdentity = 0, noEntry = 0;
      // `resolved` counts every tile whose identity was FOUND (including the art-less entries `empty`
      // counts), so the exact accounting of the requests is `requested === resolved + noEntry`: a tile is
      // either found in a library or it is not. Exported so a row can assert no tile was dropped without
      // being counted (T10).
      // Record what failed to resolve, not just how many: a hole in the floor is only diagnosable
      // if you know which (slot, orientation, main, sub, index) the lookup missed.
      const missingSample = [];
      let searched = 0, ambiguous = 0;
      // Resolve every tile to a bitmap and an on-screen box once, then split by layer.
      const filesSig = files.join('|');
      if (filesSig !== resolveFiles) { resolveFiles = filesSig; resolved_.clear(); }
      const floors = [], walls = [];
      let hidden = 0, popHidden = 0, rarityAmbiguous = 0, entryTiles = 0, entryExact = 0, entryDiffers = 0;
      // Attribution (one sample per frame): the per-tile resolve/lookup walk, separate from the blits.
      const pResolve = perfT0();
      for (const tile of tiles) {
        // MAPTILE_HIDDEN (0x8, D2DrlgDrlg.h) is a DRAW flag: DrlgDrlgAnim hides every frame of an
        // animated floor but the current one by toggling it, and hidden walls (Inv_Wall.dt1 and the
        // like) carry it from their DS1/DRLG placement. Drawing them put every animation frame and
        // the editor-coloured invisible walls on screen.
        // Not a shadow tile, though: retail draws Harrogath's main-gate shadows (maingate.dt1 13:1:2/7/8)
        // with the bit set (s231 against retail's frame inside the gate, F34).
        if ((tile.flags & 0x8) && tile.orientation !== 13) { hidden++; continue; }
        // A tile the level's own roof POP hides while the player stands in the pad's region. The engine
        // decides it (`DRLGPRESET_UpdatePops`: a preset's pop names a DT1 STYLE and, while the player is
        // inside its rect, every tile of that style in the rect is the hidden roof) and publishes it on
        // the tile; retail's client draws none of them -- retail's own act4-tyrael frame has no roof over
        // the hero while this client painted the whole Exterior.dt1 15:2:4..15 plane (client-parity F44).
        if (tile.pop === 1) { popHidden++; continue; }
        requested++;
        const rec = resolveTile(tile, files);
        const { lib, variants, foundSlot, dt1Tile, variantNo, bitmap } = rec;
        searched += rec.searched; ambiguous += rec.ambiguous; entryTiles += rec.entryTiles; entryExact += rec.entryExact; entryDiffers += rec.entryDiffers;
        rarityAmbiguous += rec.rarityAmbiguous; resolved += rec.resolved; wrongIdentity += rec.wrongIdentity;
        // A tile the DT1 holds with no blocks at all (some shadow entries: Trees.dt1 13/1/n) is
        // empty in retail too -- drawn as nothing, not missing.
        // Retail still SUBMITS it (s271: Trees.dt1 13/1/5 and 13/1/6 in the Black Marsh's draw list,
        // anchored on screen): the draw list records it, nothing is painted.
        if (!bitmap && dt1Tile && dt1Tile.blocks && dt1Tile.blocks.length === 0) {
          empty++;
          const list = globalThis.__d2DrawList;
          if (Array.isArray(list) && tile.orientation !== 0 && tile.orientation !== 15) {
            const ax = tile.screenX + offsetX + FLOOR_DX, ay = tile.screenY + offsetY + FLOOR_DY + 80;
            if (ax >= 0 && ax < width && ay >= 0 && ay < height)
              list.push({ op: tile.orientation === 13 ? 'shadowtile' : 'wall', path: `${String(files[foundSlot] ?? '?').toLowerCase().replace(/\//g, '\\')} ${tile.orientation} ${tile.mainIndex} ${tile.subIndex}`,
                frame: dt1Tile.rarity, x: ax, y: ay, mode: 0, hash: 0, a: tile.worldX * 5, b: tile.worldY * 5, rect: [ax, ay, 0, 0] });
          }
          continue;
        }
        if (!bitmap) {
          missing++;
          if (!variants) noEntry++;
          if (missingSample.length < 8) missingSample.push({ slot: tile.librarySlot, orientation: tile.orientation,
            mainIndex: tile.mainIndex, subIndex: tile.subIndex, libraryIndex: tile.libraryIndex,
            inLibrary: lib.byIndex.length, variants: variants ? variants.length : 0,
            reason: variants ? 'no variant' : 'identity not in this library' });
          continue;
        }
        const x = tile.screenX + offsetX;
        const y = tile.screenY + offsetY;
        // Where the pixels go, from retail's own draw list and frame: every
        // DT1 block lands at its own (x, y) offset from the tile's anchor -- so the bitmap, which starts
        // at the tiles's top-left block (originX, originY), is drawn at anchor + origin, never at the
        // anchor itself -- and a WALL's anchor sits 80 px lower than a floor's on the same spot, a tile
        // drawn through the floor entry with orientation 15 80 px higher. FLOOR_DY is this client's
        // camera constants (FLOOR_DX/FLOOR_DY, UNIT_CENTRE_Y) place the floors against the units.
        const px = dt1Tile.pixels;
        // A roof (orientation 15) rises by its own DT1 roofHeight, not a fixed 80: every roof in retail's
        // frames sits exactly roofHeight above the floor anchor (Act I Objects 15:0 and Harrogath's
        // blacksmith 15:13 at 80; Buildingses 15:14 subs 10-30 at 160 by the main gate: s231, F34).
        const kindDy = tile.orientation === 0 ? 0 : tile.orientation === 15 ? -(dt1Tile.roofHeight || 80) : 80;
        const drawX = x + FLOOR_DX + px.originX;
        const drawY = y + FLOOR_DY + px.originY + kindDy;
        // Culling, as retail's frames do it. A tile drawn
        // through the floor entry is culled by its 160x80 footprint at its anchor: drawn while any of it
        // is in the 800 columns and its 80 rows reach y >= 0, and while its anchor is above
        // height - FLOOR_CULL_BOTTOM (the bottom rows sit under the control panel). Walls by the rectangle actually painted, never the
        // anchor (the art hangs far above it); retail submits more walls than that (a known
        // non-difference: their art is wholly off the frame).
        if (tile.orientation === 0 || tile.orientation === 15) {
          const ax = x + FLOOR_DX + 80, ay = y + FLOOR_DY + kindDy;
          if (ax - 80 >= width || ax + 80 < 0 || ay >= height - FLOOR_CULL_BOTTOM || ay + 80 < 0) continue;
        } else if (drawX + bitmap.width <= 0 || drawX >= width || drawY + bitmap.height <= 0 || drawY >= height) continue;
        // Carry the identity asked for AND the identity drawn, so a scenario can prove they match.
        // Every floor tile once drew unrelated art while "0 unresolved" passed; only comparing these
        // two would have caught it.
        const entry = { variantNo, bitmap, x: Math.round(drawX), y: Math.round(drawY), depth: y, ordinal: tile.ordinal, tile, dt1Tile, slot: foundSlot,
          want: rec.want,
          got: rec.got, file: files[foundSlot] ?? "", variant: tile.libraryIndex, wall: tile.layer !== 0,
          seam: Boolean(tile.seam) };
        (tile.layer === 0 ? floors : walls).push(entry);
      }
      perfT('terrain-resolve', pResolve);

      floors.sort((a, b) => a.ordinal - b.ordinal);
      // A few real blits, so "the floor has holes" can be checked against pixels instead of guessed.
      const drawnSample = floors.slice(0, 6).map((f) => ({ x: f.x, y: f.y, w: f.bitmap.width, h: f.bitmap.height,
        want: f.want, got: f.got }));
      // LOWER walls (orientation 16-19: cliff faces that hang DOWN from their anchor) go under the
      // floor, before it, as OpenDiablo2's renderer does (renderTilePass1: lower walls, then floors,
      // then shadows). Their 800-px art runs beneath the neighbouring ground, which covers it; drawn
      // in the wall pass instead, their pale foot painted a vertical band over the Bloody Foothills'
      // ground (client-parity F31).
      const isLower = (e) => { const o = Number(e.want.split(':')[0]); return o >= 16 && o <= 19; };
      // A wall's six light entries, from the light map cells wallLightCells picks (F42/F118b/F120/F187).
      const wallEntries = (t) => { const A = t.worldX * 5, B = t.worldY * 5;
        return lightCell ? [0, 1, 2, 3, 4, 5].map((j) => { const [a, b] = wallCells(t.orientation, A, B, j); return lightCell(a, b); }) : null; };
      // Lower walls + floors (litFloor runs here on a cache miss; a cached frame skips the palette work).
      const pFloor = perfT0();
      // A lower wall (orientation 16-19) is terrain and retail draws it as a `wall` record -- the same
      // lit wall entry the wall pass uses, not an unlit blit (the retail captures lines
      // 3-12 are the Pandemonium Fortress's own o16/o17/o18 Inv_Wall.dt1 draws, all `wall`). Drawing
      // them from the raw bitmap lit them at full palette, which is ~1.4x retail's on the Fortress's
      // rubble where the map reads ambient (F38: s260's offending cluster is these very tiles). They
      // stay on THIS pass -- their 800-px art runs beneath the neighbouring ground and drawing them in
      // the wall pass painted a vertical band over the Bloody Foothills' ground (F31) -- so only their
      // pixels change, never their order.
      for (const l of walls.filter(isLower).sort((a, b) => (a.depth - b.depth) || (a.ordinal - b.ordinal))) {
        const E = wallEntries(l.tile);
        // Off unless globalThis.__d2TraceLower is an array: a lower wall (o16-19) is a lit `wall`
        // record like the wall pass's, so a pixel attributed to one needs the same six light entries
        // the wall pass traces (wallLightCells' own cells + E). Its own global so it never changes what
        // __d2TraceBlits (= floors) holds for s448/s629.
        if (globalThis.__d2TraceLower && E) globalThis.__d2TraceLower.push({ x: l.x, y: l.y, path: files[l.slot], want: l.want, variantNo: l.variantNo,
          dt1Tile: l.dt1Tile, o: l.tile.orientation, E, a: l.tile.worldX * 5, b: l.tile.worldY * 5, lower: true });
        const lit = E && litWall(files[l.slot], l.dt1Tile, l.want + ':v' + l.variantNo, E);
        ctx.drawImage(lit || l.bitmap, l.x, l.y); drawn++; recordTile(l, files);
      }
      for (const f of floors) {
        const grid = lightGrid && lightGrid(f.tile.worldX * 5, f.tile.worldY * 5);
        const lit = grid && litFloor(files[f.slot], f.dt1Tile, f.want + ':v' + f.variantNo, grid, f.x, f.y);
        // Off unless globalThis.__d2TraceBlits is an array: then each drawn floor carries the dt1 art
        // and the 8x8 light grid it was lit from, so a row can recompute the LEVEL litFloor gave a
        // pixel (F96: our floor level is 1-2 above retail's on the frame's left edge). s448 reads it.
        if (globalThis.__d2TraceBlits) globalThis.__d2TraceBlits.push({ x: f.x, y: f.y, path: files[f.slot], want: f.want, variantNo: f.variantNo, dt1Tile: f.dt1Tile, grid, a: f.tile.worldX * 5, b: f.tile.worldY * 5, seam: Boolean(f.tile.seam) });
        ctx.drawImage(lit || f.bitmap, f.x, f.y); drawn++; recordTile(f, files);
      }
      if (afterFloors) afterFloors(ctx, floors, { width, height, offsetX, offsetY });
      perfT('terrain-floor', pFloor);

      // Walls and sprites in retail's order: tile by tile, rows (world Y) then columns (world X); a unit belongs to the
      // tile its subtile is in and is drawn BEFORE that tile's walls. A sprite with no world position
      // falls back to its screen Y.
      // One sample per frame for the wall/overlay/roof pass, with the unit draws and the hidden-wall
      // FADED blend accumulated out of it so each is attributable on its own.
      const pWall = perfT0();
      let unitsMs = 0, fadeMs = 0;
      const key = (o) => o.sprite
        ? (o.sprite.wx === undefined ? [1e9, 0, 0, o.depth] : [Math.floor(o.sprite.wy / 5), Math.floor(o.sprite.wx / 5), 0, o.depth])
        : [o.tile.worldY, o.tile.worldX, 1, o.ordinal];
      // Roofs (orientation 15) are retail's LAST terrain pass: every roof draw in arrival, the Den and the
      // Cold Plains captures follows every wall and unit (arrival.txt: the six tent roofs at the end,
      // rows then columns). Drawn in the overlay they sat under the tent's own walls.
      const isRoof = (e) => e.tile.orientation === 15;
      const isShadow = (e) => e.tile.orientation === 13;
      // Shadow tiles (orientation 13) are their OWN pass, entirely before walls and units, not
      // interleaved by tile position: retail's own capture (a3-flayer-wp.txt, world 4440,3800) draws
      // every one of its 13 shadowtile records (lines 115-139) before any of its wall records (140+),
      // whole-list-first -- a per-tile ordinal tie-break put ours the other way round for this tile,
      // so the tree's own opaque wall (drawn after us) never covered the shadow's blend and it darkened
      // the tree canopy by 27% (SHADOW_KEEP) instead of nothing (F117, s699: 469 -> 0 of 2,503 traced
      // pixels of this one wall with only the draw ORDER corrected, litWall's level unchanged).
      const shadows = walls.filter((e) => !isLower(e) && isShadow(e));
      const overlay = walls.filter((e) => !isLower(e) && !isRoof(e) && !isShadow(e)).concat(sprites.map((s) => ({ sprite: s, depth: s.depth, ordinal: -1 })));
      for (const o of shadows) o.k = [o.tile.worldY, o.tile.worldX, 1, o.ordinal];
      shadows.sort((a, b) => (a.k[0] - b.k[0]) || (a.k[1] - b.k[1]) || (a.k[3] - b.k[3]));
      // A roof plane is one contiguous building; retail hides the WHOLE plane when the player stands
      // under its footprint (client-parity F44: five retail captures of the Fortress roof
      // gave 6/6/24/36/0 orientation-15 draws, and the one 0 is the only capture whose hero position
      // falls inside the roof's own world bounding box -- 5015-5030 x 5020-5050, hero at
      // 5020.5,5039.5 -- while the other four hero positions sit outside it). Cluster contiguous roof
      // tiles (8-neighbour on the DRLG grid) and cull a whole cluster when the player's subtile
      // position falls inside its tile-grid bounding box; per-tile culling does not reproduce the
      // measured all-or-nothing counts.
      const roofsAll = walls.filter((e) => !isLower(e) && isRoof(e));
      const parent = roofsAll.map((_, i) => i);
      const find = (i) => { while (parent[i] !== i) { parent[i] = parent[parent[i]]; i = parent[i]; } return i; };
      const union = (a, b) => { const ra = find(a), rb = find(b); if (ra !== rb) parent[ra] = rb; };
      for (let i = 0; i < roofsAll.length; i++) {
        for (let j = i + 1; j < roofsAll.length; j++) {
          if (Math.abs(roofsAll[i].tile.worldX - roofsAll[j].tile.worldX) <= 1 &&
              Math.abs(roofsAll[i].tile.worldY - roofsAll[j].tile.worldY) <= 1) union(i, j);
        }
      }
      const clusters = new Map();
      roofsAll.forEach((e, i) => { const r = find(i); if (!clusters.has(r)) clusters.set(r, []); clusters.get(r).push(e); });
      const culledRoof = new Set();
      for (const members of clusters.values()) {
        const minX = Math.min(...members.map((e) => e.tile.worldX)), maxX = Math.max(...members.map((e) => e.tile.worldX));
        const minY = Math.min(...members.map((e) => e.tile.worldY)), maxY = Math.max(...members.map((e) => e.tile.worldY));
        const inside = player.x >= minX * 5 && player.x < (maxX + 1) * 5 && player.y >= minY * 5 && player.y < (maxY + 1) * 5;
        if (inside) for (const e of members) culledRoof.add(e);
      }
      const roofs = roofsAll.filter((e) => !culledRoof.has(e));
      for (const o of roofs) o.k = [o.tile.worldY, o.tile.worldX, 1, o.ordinal];
      roofs.sort((a, b) => (a.k[0] - b.k[0]) || (a.k[1] - b.k[1]) || (a.k[3] - b.k[3]));
      for (const o of overlay) o.k = key(o);
      overlay.sort((a, b) => (a.k[0] - b.k[0]) || (a.k[1] - b.k[1]) || (a.k[2] - b.k[2]) || (a.k[3] - b.k[3]));
      for (const o of shadows.concat(overlay).concat(roofs)) {
        if (o.sprite) { const u = perfT0(); o.sprite.draw(ctx); unitsMs += perfT0() - u; continue; }
        if (o.tile.orientation === 13) {
          // A shadow tile repaints whatever is under it, so a row attributing a pixel to the terrain
          // blit that painted it has to see this pass too (same guarded trace, __d2TraceWalls).
          if (globalThis.__d2TraceWalls) globalThis.__d2TraceWalls.push({ x: o.x, y: o.y, path: files[o.slot], want: o.want, o: 13, dt1Tile: o.dt1Tile, shadow: true });
          // A shadow tile blends into what is under it through PL2 AlphaBlend[0] (drawShadow; T8).
          if (globalThis.__d2ShadowOpaque) ctx.drawImage(o.bitmap, o.x, o.y);   // a row reading the tiles' own pixels (s191)
          else if (!drawShadow(ctx, o)) {   // no PL2: the measured stand-in
            const prev = ctx.globalAlpha; ctx.globalAlpha = 1 - SHADOW_KEEP;
            ctx.drawImage(silhouette(o.bitmap), o.x, o.y); ctx.globalAlpha = prev;
          }
        } else {
          const at = lightCell, A = o.tile.worldX * 5, B = o.tile.worldY * 5;
          // A roof (orientation 15) goes through retail's FLOOR entry (op 31) with a floor light grid
          // (768 bytes = 8x8 x 12) that is UNIFORM at the light map's ambient: every roof in arrival3 and
          // lit0..1 (133, 144, 146 -- each capture's map minimum), whatever light is under it.
          const roofGrid = o.tile.orientation === 15 && lightAmbient != null && new Array(64).fill(lightAmbient);
          // The SIX light entries this wall is drawn with, and the light map cell each came from -- the
          // same override the floor path below takes, for the wall path (F117: which entry a wall's
          // 32-px block reads is the question the gate frame asks). Off unless __d2TraceWalls is an
          // array; never populated otherwise, and it changes no drawing.
          const t = o.tile, fade = t.layer === 1 && t.hidesPlayer !== undefined
            ? roofFade(`${t.room}:${t.ordinal}:${t.worldX}:${t.worldY}:${t.orientation}:${t.mainIndex}:${t.subIndex}`, t.hidesPlayer, (globalThis.__d2Now || Date.now)()) : 0xff;
          const wallE = at && !roofGrid ? [0, 1, 2, 3, 4, 5].map((j) => { const [a, b] = wallCells(o.tile.orientation, A, B, j); return at(a, b); }) : null;
          if (globalThis.__d2TraceWalls) globalThis.__d2TraceWalls.push({ x: o.x, y: o.y, path: files[o.slot], want: o.want, o: o.tile.orientation,
            dt1Tile: o.dt1Tile, A, B, cells: wallE && [0, 1, 2, 3, 4, 5].map((j) => wallCells(o.tile.orientation, A, B, j)), E: wallE, roof: Boolean(roofGrid), fade });
          // Off unless globalThis.__d2TraceBlits is an array: the wall counterpart of the floor trace
          // above (F117's own routing -- "s448's __d2TraceBlits is the hook") -- each drawn wall carries
          // its six fitted light entries (wallCells' own E array) and the dt1 art, so a row can recompute
          // litWall's per-block level and orientation the same way s448 reads litFloor's.
          if (globalThis.__d2TraceBlits && wallE) globalThis.__d2TraceBlits.push({ x: o.x, y: o.y, path: files[o.slot], want: o.want, variantNo: o.variantNo, dt1Tile: o.dt1Tile, orientation: o.tile.orientation, E: wallE, a: A, b: B, wall: true });
          const lit = roofGrid ? litFloor(files[o.slot], o.dt1Tile, o.want + ':v' + o.variantNo, roofGrid, o.x, o.y) : wallE && litWall(files[o.slot], o.dt1Tile, o.want + ':v' + o.variantNo, wallE, fade < 255);
          // F66b: a wall the client hides fades to retail's transparent entry's alpha (roof-fade.mjs); alpha 0 draws nothing.
          if (fade > 0 && !(globalThis.__d2SkipFaded && fade < 255)) {   // __d2SkipFaded: a probe drawing the frame WITHOUT the hidden walls (their blend needs the pixel under them)
            const pf = perfT0();
            if (fade < 255 && !globalThis.__d2CanvasAlphaFade && drawFaded(ctx, o, lit, fade)) { /* per-pixel PL2 blend, like retail's entry 40 */ }
            else { const prev = ctx.globalAlpha; ctx.globalAlpha = prev * fade / 255; ctx.drawImage(lit || o.bitmap, o.x, o.y); ctx.globalAlpha = prev; }   // no PL2 / unlit: the canvas-alpha stand-in
            fadeMs += perfT0() - pf;
          }
        }
        drawn++; recordTile(o, files);
      }
      perfT('terrain-wall', pWall);
      perfSample('units', unitsMs);
      perfSample('terrain-fade', fadeMs);
      // Identity of every blit (level 2 evidence): which file, which (orientation:main:sub) was asked
      // for and drawn, and the screen box it covers -- so a wrong patch on screen names its tiles.
      const blits = floors.concat(walls).map((e) => ({ file: e.file, want: e.want, got: e.got, variant: e.variant,
        x: e.x, y: e.y, w: e.bitmap.width, h: e.bitmap.height, wall: e.wall, seam: e.seam, flag8000: !!(e.tile.flags & 0x8000), tflags: e.tile.flags, o: e.tile.orientation, room: e.tile.room, layer: e.tile.layer }));
      return { entryTiles, entryExact, entryDiffers, drawn, missing, empty, hidden, popHidden, requested, resolved, noEntry, rarityAmbiguous, blits, seam: floors.concat(walls).filter((e) => e.seam).length, missingSample, drawnSample, wrongIdentity, searched, ambiguous,
        floors: floors.length, walls: walls.length, sprites: sprites.length };
    },
  };
}
