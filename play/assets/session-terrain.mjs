// The real map, drawn from the engine's own tile list.
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

export const SESSION_TILE_BYTES = 76;
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
 */
export function readSessionLightmap(Module, ambientOverride = -1, occludeUnits = 0) {
  if (!Module._d2_session_lightmap) return null;
  const p = Number(Module.ccall('d2_session_lightmap', 'number', ['number', 'number'], [ambientOverride, occludeUnits]));
  const v = new DataView(Module.HEAPU8.buffer, p, 16 + 48 * 48 * 2);
  const ox = v.getInt32(0, true), oy = v.getInt32(4, true);
  const cells = Module.HEAPU8.slice(p + 16, p + 16 + 48 * 48), blocked = Module.HEAPU8.slice(p + 16 + 48 * 48, p + 16 + 48 * 48 * 2);
  const at = (x, y) => cells[Math.max(0, Math.min(47, y - oy)) * 48 + Math.max(0, Math.min(47, x - ox))];
  const sv = new DataView(Module.HEAPU8.buffer, p + 16 + 48 * 48 * 2, 64 * 20), list = [];
  for (let i = 0; i < Math.min(64, v.getInt32(12, true)); i++) list.push([0, 4, 8, 12, 16].map((o) => sv.getInt32(i * 20 + o, true)));
  return { origin: [ox, oy], ambient: v.getInt32(8, true), sources: v.getInt32(12, true), list, cells, blocked,
    grid: (a, b) => { const g = new Array(64); for (let j = 0; j < 8; j++) for (let i = 0; i < 8; i++) g[j * 8 + i] = at(a - 1 + i, b - 1 + j); return g; } };
}

/**
 * Everything the terrain view draws: the current level's tiles, then the tiles of the OTHER levels'
 * active rooms (d2_session_seam_tiles) -- across an open seam retail draws the neighbouring level's
 * ground (client-parity F24; s187 measured it on the Cold Plains' first frame). The seam tiles index
 * their own library list, appended here after the level's, so one `files` array serves both.
 */
export function readSessionTerrain(Module) {
  const count = Module.ccall('d2_session_tile_count', 'number', [], []);
  const ptr = Number(Module.ccall('d2_session_tiles', 'number', [], []));
  const tiles = count > 0 && ptr > 0 ? parseSessionTiles(Module.HEAPU8, ptr, count) : [];
  const files = readLevelTileFiles(Module);
  if (Module._d2_session_seam_tiles) {
    const seamCount = Module.ccall('d2_session_seam_tile_count', 'number', [], []);
    const seamPtr = Number(Module.ccall('d2_session_seam_tiles', 'number', [], []));
    if (seamCount > 0 && seamPtr > 0) {
      const base = files.length;
      files.push(...readLevelTileFiles(Module, 4096, 'd2_session_seam_tile_files'));
      for (const t of parseSessionTiles(Module.HEAPU8, seamPtr, seamCount)) tiles.push({ ...t, librarySlot: t.librarySlot + base, seam: true });
    }
  }
  return { tiles, files };
}

/**
 * Draw-list record for a drawn tile (play/assets/drawlist.mjs), in RETAIL's anchor convention, derived
 * from where this renderer actually put the pixels. Measured on retail's own town frame against its
 * draw list: a wall's DT1 block (bx, by) lands at anchor + (bx, by); a tile
 * drawn through the floor entry (orientation 0 and 15) at anchor + (bx - 80, by).
 */
function recordTile(entry, files) {
  const list = globalThis.__d2DrawList;
  if (!Array.isArray(list)) return;
  const t = entry.tile, px = entry.dt1Tile.pixels, floor = t.orientation === 0 || t.orientation === 15;
  const file = String(files[entry.slot] ?? '?').toLowerCase().replace(/\//g, '\\');
  list.push({ op: floor ? 'floor' : t.orientation === 13 ? 'shadowtile' : 'wall', path: `${file} ${t.orientation} ${t.mainIndex} ${t.subIndex}`, frame: entry.dt1Tile.rarity,   // the DRAWN art's rarity
    x: entry.x - px.originX + (floor ? 80 : 0), y: entry.y - px.originY, mode: 0, hash: 0, a: t.worldX * 5, b: t.worldY * 5 });
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
  function litFloor(path, dt1Tile, index, grid) {
    const key = `${path}:${index}:${grid.join(',')}`;
    if (litCache.has(key)) return litCache.get(key);
    const t = blendPl2(), px = dt1Tile.pixels;
    if (!t || !px || !px.width) { litCache.set(key, null); return null; }
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
      // (gouraud: 15 rows, edges stepped once per row, the level table indexed by the pixel's column in
      // the block, exactly as the iso path) -- read from the code.
      const iso = b.format === 1, rows = 15;
      for (let r = 0; r < rows; r++) {
        const rr = r;
        const L = ((E[0] << 4) + rr * (E[3] - E[0])) >> 7, R = ((E[1] << 4) + rr * (E[2] - E[1])) >> 7;
        const skip = iso ? ISO_SKIP[r] : 0, run = iso ? ISO_RUN[r] : 32;
        for (let col = 0; col < run; col++) {
          const x = skip + col, q = (b.y + r - px.originY) * px.width + (b.x + x - px.originX);
          if (!iso && (b.y + r - px.originY >= px.height || !opq[q])) continue;
          const lvl = flat ? c.A >> 3 : L + Math.floor((R - L) * x / 32);
          idx[q] = t[1024 + lvl * 256 + px.indices[q]];
        }
      }
    }
    const rgba = new Uint8ClampedArray(idx.length * 4);
    for (let q = 0; q < idx.length; q++) { if (!opq[q]) continue; rgba[q * 4] = t[idx[q] * 4]; rgba[q * 4 + 1] = t[idx[q] * 4 + 1]; rgba[q * 4 + 2] = t[idx[q] * 4 + 2]; rgba[q * 4 + 3] = 255; }
    const canvas = documentObject.createElement('canvas'); canvas.width = px.width; canvas.height = px.height;
    canvas.getContext('2d').putImageData(new ImageData(rgba, px.width, px.height), 0, 0);
    if (litCache.size > 1500) litCache.clear();   // the light moves with the player: keep the cache bounded
    litCache.set(key, canvas); return canvas;
  }
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

  return {
    errors,
    /** Light floors from a grid source: fn(a, b) -> 36 intensities or null. */
    setLightGrid(fn) { lightGrid = fn; },
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
     */
    draw(ctx, tiles, files, player, width, height, sprites = []) {
      const origin = worldToScreen(player.x, player.y);
      const offsetX = width / 2 - origin.x;
      const offsetY = height / 2 + UNIT_CENTRE_Y - origin.y;
      let drawn = 0, missing = 0, empty = 0;
      // Record what failed to resolve, not just how many: a hole in the floor is only diagnosable
      // if you know which (slot, orientation, main, sub, index) the lookup missed.
      const missingSample = [];
      let searched = 0, ambiguous = 0;
      // Resolve every tile to a bitmap and an on-screen box once, then split by layer.
      const floors = [], walls = [];
      let hidden = 0, rarityAmbiguous = 0;
      for (const tile of tiles) {
        // MAPTILE_HIDDEN (0x8, D2DrlgDrlg.h) is a DRAW flag: DrlgDrlgAnim hides every frame of an
        // animated floor but the current one by toggling it, and hidden walls (Inv_Wall.dt1 and the
        // like) carry it from their DS1/DRLG placement. Drawing them put every animation frame and
        // the editor-coloured invisible walls on screen.
        if (tile.flags & 0x8) { hidden++; continue; }
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
        const dt1Tile = variants ? (sameRarity.length === 1 ? sameRarity[0] : (variants[vi] ?? variants[0])) : undefined;
        if (variants && sameRarity.length !== 1) rarityAmbiguous++;
        // Keyed by the DRAWN variant (its position among the identity's variants), never the asked index.
        const variantNo = variants ? variants.indexOf(dt1Tile) : -1;
        const bitmap = dt1Tile && bitmapFor(files[foundSlot], dt1Tile,
          `${tile.orientation}:${tile.mainIndex}:${tile.subIndex}:v${variantNo}`);
        // A tile the DT1 holds with no blocks at all (some shadow entries: Trees.dt1 13/1/n) is
        // empty in retail too -- drawn as nothing, not missing.
        if (!bitmap && dt1Tile && dt1Tile.blocks && dt1Tile.blocks.length === 0) { empty++; continue; }
        if (!bitmap) {
          missing++;
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
        const kindDy = tile.orientation === 0 ? 0 : tile.orientation === 15 ? -80 : 80;
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
          want: `${tile.orientation}:${tile.mainIndex}:${tile.subIndex}`,
          got: `${dt1Tile.orientation}:${dt1Tile.mainIndex}:${dt1Tile.subIndex}` };
        (tile.layer === 0 ? floors : walls).push(entry);
      }

      floors.sort((a, b) => a.ordinal - b.ordinal);
      // A few real blits, so "the floor has holes" can be checked against pixels instead of guessed.
      const drawnSample = floors.slice(0, 6).map((f) => ({ x: f.x, y: f.y, w: f.bitmap.width, h: f.bitmap.height,
        want: f.want, got: f.got }));
      for (const f of floors) {
        const grid = lightGrid && lightGrid(f.tile.worldX * 5, f.tile.worldY * 5);
        const lit = grid && litFloor(files[f.slot], f.dt1Tile, f.want + ':v' + f.variantNo, grid);
        ctx.drawImage(lit || f.bitmap, f.x, f.y); drawn++; recordTile(f, files);
      }

      // Walls and sprites in retail's order: tile by tile, rows (world Y) then columns (world X); a unit belongs to the
      // tile its subtile is in and is drawn BEFORE that tile's walls. A sprite with no world position
      // falls back to its screen Y.
      const key = (o) => o.sprite
        ? (o.sprite.wx === undefined ? [1e9, 0, 0, o.depth] : [Math.floor(o.sprite.wy / 5), Math.floor(o.sprite.wx / 5), 0, o.depth])
        : [o.tile.worldY, o.tile.worldX, 1, o.ordinal];
      const overlay = walls.concat(sprites.map((s) => ({ sprite: s, depth: s.depth, ordinal: -1 })));
      for (const o of overlay) o.k = key(o);
      overlay.sort((a, b) => (a.k[0] - b.k[0]) || (a.k[1] - b.k[1]) || (a.k[2] - b.k[2]) || (a.k[3] - b.k[3]));
      for (const o of overlay) {
        if (o.sprite) { o.sprite.draw(ctx); continue; }
        if (o.tile.orientation === 13) {
          // A shadow tile blends into what is under it through PL2 AlphaBlend[0] (drawShadow; T8).
          if (globalThis.__d2ShadowOpaque) ctx.drawImage(o.bitmap, o.x, o.y);   // a row reading the tiles' own pixels (s191)
          else if (!drawShadow(ctx, o)) {   // no PL2: the measured stand-in
            const prev = ctx.globalAlpha; ctx.globalAlpha = 1 - SHADOW_KEEP;
            ctx.drawImage(silhouette(o.bitmap), o.x, o.y); ctx.globalAlpha = prev;
          }
        } else ctx.drawImage(o.bitmap, o.x, o.y);
        drawn++; recordTile(o, files);
      }
      const wrongIdentity = floors.concat(walls).filter((e) => e.want !== e.got).length;
      return { drawn, missing, empty, hidden, rarityAmbiguous, missingSample, drawnSample, wrongIdentity, searched, ambiguous,
        floors: floors.length, walls: walls.length, sprites: sprites.length };
    },
  };
}
