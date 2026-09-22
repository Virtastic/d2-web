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
      width: view.getInt32(b + 56, true), height: view.getInt32(b + 60, true),
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
export function readLevelTileFiles(Module, cap = 4096) {
  const p = Module._malloc(cap);
  try {
    const slots = Module.ccall('d2_session_level_tile_files', 'number', ['bigint', 'number'], [BigInt(p), cap]);
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
 * Terrain for one level: decodes the level's DT1 libraries lazily and draws the engine's tile list.
 * A library that fails to decode is recorded and skipped rather than aborting the level -- a
 * missing wall set should cost you those tiles, not the whole map.
 */
export function createSessionTerrain({ read, decodeDt1, decodeDatPalette, palettePath, documentObject = globalThis.document }) {
  const libraries = new Map();   // slot -> { byKey: Map, error }
  const bitmaps = new Map();     // `${slot}:${index}` -> canvas
  let palette = null;
  const errors = [];

  const paletteBytes = () => (palette ??= decodeDatPalette(read(palettePath)));

  function library(slot, files) {
    if (libraries.has(slot)) return libraries.get(slot);
    const path = files[slot];
    let entry = { byKey: new Map(), byIndex: [], error: path ? '' : 'empty slot' };
    if (path) {
      try {
        const dt1 = decodeDt1(read(path));
        entry.byIndex = dt1.tiles;
        for (let i = 0; i < dt1.tiles.length; i++) {
          const t = dt1.tiles[i];
          entry.byKey.set(`${t.orientation}:${t.mainIndex}:${t.subIndex}`, t);
        }
      } catch (error) { entry.error = String(error); errors.push(`${path}: ${error}`); }
    }
    libraries.set(slot, entry);
    return entry;
  }

  function bitmapFor(slot, tile, index) {
    const key = `${slot}:${index}`;
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
      const offsetY = height / 2 - origin.y;
      let drawn = 0, missing = 0;
      // Record what failed to resolve, not just how many: a hole in the floor is only diagnosable
      // if you know which (slot, orientation, main, sub, index) the lookup missed.
      const missingSample = [];
      // Resolve every tile to a bitmap and an on-screen box once, then split by layer.
      const floors = [], walls = [];
      for (const tile of tiles) {
        const lib = library(tile.librarySlot, files);
        if (lib.error && !lib.byIndex.length) {
          missing++;
          if (missingSample.length < 8) missingSample.push({ slot: tile.librarySlot, libraryError: lib.error });
          continue;
        }
        const dt1Tile = lib.byIndex[tile.libraryIndex]
          ?? lib.byKey.get(`${tile.orientation}:${tile.mainIndex}:${tile.subIndex}`);
        const bitmap = dt1Tile && bitmapFor(tile.librarySlot, dt1Tile, tile.libraryIndex);
        if (!bitmap) {
          missing++;
          if (missingSample.length < 8) missingSample.push({ slot: tile.librarySlot, orientation: tile.orientation,
            mainIndex: tile.mainIndex, subIndex: tile.subIndex, libraryIndex: tile.libraryIndex,
            inLibrary: lib.byIndex.length, resolved: Boolean(dt1Tile) });
          continue;
        }
        const x = tile.screenX + offsetX;
        const y = tile.screenY + offsetY;
        if (x + bitmap.width < 0 || y + bitmap.height < 0 || x > width || y > height) continue;
        // DT1 floor records carry a negative height; the art hangs UP from the anchor row.
        const entry = { bitmap, x: Math.round(x), y: Math.round(y + Math.min(0, tile.height)), depth: y, ordinal: tile.ordinal };
        (tile.layer === 0 ? floors : walls).push(entry);
      }

      floors.sort((a, b) => a.ordinal - b.ordinal);
      // A few real blits, so "the floor has holes" can be checked against pixels instead of guessed.
      const drawnSample = floors.slice(0, 6).map((f) => ({ x: f.x, y: f.y, w: f.bitmap.width, h: f.bitmap.height }));
      for (const f of floors) { ctx.drawImage(f.bitmap, f.x, f.y); drawn++; }

      // Walls and sprites share one depth-sorted pass: a sprite at a smaller screen Y is further
      // "back" and must be painted before the wall in front of it.
      const overlay = walls.concat(sprites.map((s) => ({ sprite: s, depth: s.depth, ordinal: -1 })));
      overlay.sort((a, b) => (a.depth - b.depth) || (a.ordinal - b.ordinal));
      for (const o of overlay) {
        if (o.sprite) { o.sprite.draw(ctx); continue; }
        ctx.drawImage(o.bitmap, o.x, o.y); drawn++;
      }
      return { drawn, missing, missingSample, drawnSample, floors: floors.length, walls: walls.length, sprites: sprites.length };
    },
  };
}
