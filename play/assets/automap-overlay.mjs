import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { worldToScreen, TILE_SCREEN_W } from './session-terrain.mjs';

// DATATBLS_LoadAutomapTxt turns these LevelName strings into its automap-type
// enum.  This is the same 1.13c table order as D2MOO's gszAutomapLevelNames;
// the cells themselves are *only* selected from AutoMap.txt below.
const LEVEL_NAMES = Object.freeze([
  'None', '1 Town', '1 Wilderness', '1 Cave', '1 Crypt', '1 Monestary', '1 Courtyard',
  '1 Barracks', '1 Jail', '1 Cathedral', '1 Catacombs', '1 Tristram', '2 Town', '2 Sewer',
  '2 Harem', '2 Basement', '2 Desert', '2 Tomb', '2 Lair', '2 Arcane', '3 Town', '3 Jungle',
  '3 Kurast', '3 Spider', '3 Dungeon', '3 Sewer', '4 Town', '4 Mesa', '4 Lava', '5 Town',
  '5 Siege', '5 Barricade', '5 Temple', '5 Ice', '5 Baal', '5 Lava',
]);
const TILE_NAMES = Object.freeze(['fl', 'wl', 'wr', 'wtlr', 'wtll', 'wtr', 'wbl', 'wbr', 'wld', 'wrd', 'wle', 'wre', 'co', 'sh', 'tr', 'rf', 'ld', 'rd', 'fd', 'fi']);
// centreY 292 is retail's own screen centre for the player, the one the terrain pass lays the world
// out around (session-terrain.mjs: "retail's town frame: the Barbarian at 400,292 on 800x600"). The
// automap is centred on the player, so it uses the same point; drawing it at height/2 instead put
// the whole map 8 px below the world it describes.
export const AUTOMAP_LAYOUT = Object.freeze({ width: 800, height: 600, centreX: 400, centreY: 292, background: 'transparent' });
export const AUTOMAP_PATH = 'data\\global\\ui\\automap\\maximap.dc6';

function decodeText(bytes) { return new TextDecoder('latin1').decode(bytes).replace(/^﻿/, ''); }
function number(value) { const n = Number.parseInt(String(value ?? '').trim(), 10); return Number.isFinite(n) ? n : -1; }
function rowsFromTxt(bytes) {
  const lines = decodeText(bytes).replace(/\r/g, '').split('\n').filter((line) => line.trim());
  const headings = lines.shift()?.split('\t').map((s) => s.trim()) || [];
  const index = Object.fromEntries(headings.map((name, i) => [name.toLowerCase(), i]));
  for (const field of ['levelname', 'tilename', 'style', 'startsequence', 'endsequence', 'cel1']) if (!(field in index)) throw new Error(`AutoMap.txt lacks ${field}`);
  return lines.map((line) => {
    const fields = line.split('\t'), get = (field) => fields[index[field.toLowerCase()]] ?? '';
    return { levelName: get('LevelName').trim(), tileName: get('TileName').trim(), style: number(get('Style')),
      startSequence: number(get('StartSequence')), endSequence: number(get('EndSequence')),
      frames: ['Cel1', 'Cel2', 'Cel3', 'Cel4'].map((field) => number(get(field))).filter((frame) => frame >= 0) };
  }).filter((row) => row.frames.length);
}
function makeFrameCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas'); canvas.width = frame.width; canvas.height = frame.height;
  canvas.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0);
  return canvas;
}

// The only selection rule here is DATATBLS_GetAutomapCellId's table predicate:
// level type + DT1 tile orientation + main/sub indices -> a row and one of its Cels.
function selectRecord(rows, levelType, tile) {
  const levelName = LEVEL_NAMES[levelType] ?? 'None', tileName = TILE_NAMES[tile.orientation] ?? null;
  if (!tileName) return null;
  const record = rows.find((row) => row.levelName === levelName && row.tileName === tileName &&
    (row.style === -1 || row.style === tile.mainIndex) &&
    (row.startSequence === -1 || (row.startSequence <= tile.subIndex && tile.subIndex <= row.endSequence)));
  return record ? { record, levelName, tileName } : null;
}
function levelRect(Module, levelId) {
  const pointer = Number(Module.ccall('d2_session_level_rect', 'number', ['number'], [levelId]));
  return pointer ? Array.from(new Int32Array(Module.HEAPU8.buffer, pointer, 4)) : [0, 0, 0, 0];
}
function seamEdges(Module, destinationLevel) {
  const count = Module.ccall('d2_session_seam_edges', 'number', ['number'], [destinationLevel]);
  const pointer = Number(Module.ccall('d2_session_seam_edge_data', 'number', [], []));
  return pointer ? Array.from(new Int32Array(Module.HEAPU8.buffer, pointer, count * 4), (v) => v) : [];
}
// One automap CELL, keyed the way retail's layer holds one: by where on the map it sits, not by the
// room that happens to hold it. A map cell carries three tile lists (floors, walls, roofs) and can
// hold one of each, so the layer is part of the key.
const cellKey = (tile) => `${tile.worldX},${tile.worldY},${tile.layer}`;

export function createAutomapOverlay({ read, Module, documentObject = globalThis.document, isGameplay }) {
  const rows = rowsFromTxt(read('data\\global\\excel\\automap.txt'));
  const palette = decodeDatPalette(read('data\\global\\palette\\act1\\pal.dat'));
  const map = decodeDc6(read(AUTOMAP_PATH));
  const frames = map.frames.map((frame) => makeFrameCanvas(frame, palette, documentObject));
  // THE AUTOMAP'S OWN STEP, read off the sheet rather than declared. A cell is one world tile, and
  // the sheet packs every cell into a frame box exactly as wide as its widest cell art, so that box
  // width is the tile's footprint on the map (the floor diamonds fill it; the wall pieces are taller
  // and hang upward, which is why the box is 16x32 and not 16x8). The overlay used to declare 0.25 --
  // 20 px per tile -- which spaces the cells 4 px apart, and the sparse floor dithers 8 px apart, so
  // TAB drew a lattice of isolated marks instead of a map (measured: s950/s951).
  const step = Math.max(...map.frames.map((frame) => frame.width));
  const scale = step / TILE_SCREEN_W;
  const layout = Object.freeze({ ...AUTOMAP_LAYOUT, step, scale });
  let visible = false, evidence = { open: false, drawn: [], unresolved: [], layout, error: null };
  // Retail's automap is CLIENT state and it is per level. D2Client keeps a D2AutomapLayerStrc per
  // level (`nLayerNo`, the pFloors/pWalls/pObjects/pExtras cell lists, `pNext` the next layer), and
  // D2Game's own DUNGEON_AllocAct call passes NULL for both automap callbacks -- the server reveals
  // nothing. So a cell that has been drawn stays drawn while the room holding it is freed and rebuilt,
  // and the layer survives leaving and re-entering its level. This is that layer, per level id:
  // level id -> (cell key -> cell).
  const layers = new Map();
  // Cells the AutoMap.txt route cannot name, per level: the frame's terrain read offers them, no row
  // matches. Counted rather than dropped, because this is the one part of the map that still needs a
  // retail TAB frame to settle.
  const unrevealed = new Map();
  globalThis.addEventListener('keydown', (event) => {
    if (isGameplay() && event.code === 'Tab' && !event.repeat) { visible = !visible; event.preventDefault(); event.stopImmediatePropagation(); }
  });
  return {
    // `tiles` is the frame's own terrain read, handed over by the shell so the layer costs no second
    // one. The layer is filled on EVERY frame, not only while TAB is up: retail's map fills as the
    // player explores, and a map that only grows while it is being looked at is not a map of where
    // the player has been.
    draw(ctx, width, height, snapshot, tiles = null) {
      const levelId = snapshot.level;
      if (!layers.has(levelId)) { layers.set(levelId, new Map()); unrevealed.set(levelId, new Set()); }
      const layer = layers.get(levelId), unnamed = unrevealed.get(levelId), live = new Set();
      if (tiles && tiles.length) {
        const levelType = Module.ccall('d2_session_level_type', 'number', [], []);
        // WHAT IS REVEALED IS THE ENGINE'S ANSWER, and this frame's own terrain read already carries
        // it: `d2_session_tiles` is built from the rooms that hold an active pRoom, so a tile in this
        // list is a tile of a room the engine has built for the player to be in. It used to be
        // filtered by `d2_session_room_active_at`, which is the PATH code's one-hop resolver from the
        // player's own room -- the engine's own comment on that export says it "is right for the path
        // code, and useless for telling 'a wall' from 'not built' from 'out of the resolver's reach'"
        // -- and the filter's measured cost in the Rogue Encampment was **233 cells drawn of the
        // level's 2,337** (s590). A seam tile belongs to the neighbouring level and goes into THAT
        // level's layer, when that level is the one being played.
        for (const tile of tiles) {
          if (tile.seam) continue;
          const key = cellKey(tile); live.add(key);
          // A cell is absorbed once, on the frame its tile first exists: re-selecting it every frame
          // would walk AutoMap.txt per tile per frame for a record the engine's own tile cannot change.
          if (layer.has(key)) continue;
          const selected = selectRecord(rows, levelType, tile);
          if (!selected || !frames[selected.record.frames[0]]) { unnamed.add(key); continue; }
          const frame = selected.record.frames[0];
          layer.set(key, { frame, requestedFrames: [...selected.record.frames], levelName: selected.levelName,
            tileName: selected.tileName, style: selected.record.style, startSequence: selected.record.startSequence,
            endSequence: selected.record.endSequence, orientation: tile.orientation, mainIndex: tile.mainIndex,
            subIndex: tile.subIndex, layer: tile.layer, world: [tile.worldX * 5, tile.worldY * 5] });
        }
      }
      if (!visible) return (evidence = { open: false, drawn: [], unresolved: [], layout, error: null });
      const levelType = Module.ccall('d2_session_level_type', 'number', [], []);
      const drawn = [], unresolved = [];
      // These read-only DRLG surfaces are map geometry, not a JS reconstruction: the level's
      // placed rectangle gives the panel's extent, while seam segments identify adjacent areas.
      const geometry = { levelRect: levelRect(Module, levelId), seams: (snapshot.warps || []).filter((w) => w.destinationLevel !== levelId)
        .map((warp) => ({ destinationLevel: warp.destinationLevel, rect: levelRect(Module, warp.destinationLevel), edges: seamEdges(Module, warp.destinationLevel) })) };
      const camera = worldToScreen(snapshot.player.x + .5, snapshot.player.y + .5);
      // The panel is centred on the PLAYER, at retail's own screen centre for the player.
      const cx = width * (AUTOMAP_LAYOUT.centreX / AUTOMAP_LAYOUT.width);
      const cy = height * (AUTOMAP_LAYOUT.centreY / AUTOMAP_LAYOUT.height);
      if (!layer.size) unresolved.push({ reason: 'this level has no revealed cell yet' });
      ctx.save(); ctx.imageSmoothingEnabled = false;
      // RETAIL'S OWN LIST ORDER, not the order the cells happened to be discovered in: a layer holds
      // pFloors, pWalls, pObjects and pExtras and the client walks them in that order, while this map
      // drew floors and walls interleaved by insertion -- so a floor absorbed after a wall painted
      // over it. The engine's tile layer is the same 0 floors / 1 walls / 2 roofs split.
      const ordered = [...layer.entries()].sort((a, b) => a[1].layer - b[1].layer);
      for (const [key, cell] of ordered) {
        // D2Common chooses one Cel from this selected row. The browser has no access to that
        // private Automap RNG stream, so it deterministically uses the row's first valid Cel;
        // evidence retains every permitted Cel rather than claiming the retail random choice.
        const bitmap = frames[cell.frame];
        const p = worldToScreen(cell.world[0], cell.world[1]);
        // RETAIL'S DC6 ANCHOR, the one every other renderer here places a frame by (`left = anchorX
        // + offsetX`, `top = anchorY + offsetY - height + 1`, drawlist.mjs): the frame's box sits
        // with its BOTTOM at the tile's south vertex. The sheet's own geometry says the same and
        // says where the box goes -- the cell art fills the bottom of the box and the wall pieces
        // hang upward from it -- so centring the box put every cell half a footprint below its tile.
        const x = Math.round(cx + (p.x - camera.x) * scale - bitmap.width / 2);
        const y = Math.round(cy + (p.y - camera.y) * scale + step / 2 - bitmap.height + 1);
        ctx.drawImage(bitmap, x, y);
        drawn.push({ source: AUTOMAP_PATH, frame: cell.frame, requestedFrames: [...cell.requestedFrames], levelName: cell.levelName,
          tileName: cell.tileName, style: cell.style, startSequence: cell.startSequence, endSequence: cell.endSequence,
          tile: { orientation: cell.orientation, style: cell.mainIndex, sequence: cell.subIndex, layer: cell.layer, world: cell.world },
          at: [x, y], size: [bitmap.width, bitmap.height], order: drawn.length, remembered: !live.has(key) });
      }
      ctx.restore();
      return (evidence = { open: true, levelType, levelName: LEVEL_NAMES[levelType] ?? 'None', cells: layer.size,
        unrevealed: unnamed.size, drawn, unresolved, geometry, layout, error: null });
    },
    get evidence() { return JSON.parse(JSON.stringify(evidence)); },
  };
}
