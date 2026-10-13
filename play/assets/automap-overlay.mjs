import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { createBlender } from './frontend.mjs';
import { UNIT_CENTRE_Y } from './session-terrain.mjs';
import { MA0_K } from './ma0.mjs';
import { createAutomapRecordIndex, selectIndexedAutomapRecord } from './automap-record-index.mjs';
import { perfT0, perfT, perfSample } from './frame-perf.mjs';
import { nativeAutomapMode } from './automap-mode.mjs';
import { fullMapFadeDrawMode } from './automap-full-fade.mjs';

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
// F234: retail's two automap art families, from D2Client 1.13c. The loader `0x6fb0fb50` branches `dec eax; je` ONCE: mode 1
// loads the `*S` small sheet, every other mode the full sheet. The AutoMap.txt Cel index is shared, so
// a cell resolves to the same frame in either sheet and only the DC6 and the world->map divisor differ.
export const AUTOMAP_PATHS = Object.freeze({ full: 'data\\global\\ui\\automap\\maximap.dc6', mini: 'data\\global\\ui\\automap\\maximaps.dc6' });
export const AUTOMAP_PATH = AUTOMAP_PATHS.full;

// F234: D2Client's viewport producer (`0x6fb0f9f0`) uses this rectangle for marker gating, while
// `0x6fb10250` supplies the separate small-map cell clip. The `selector284` input is deliberately
// explicit: the binary's initial value is zero, but its runtime producer/meaning is not yet recovered.
export function automapViewport(width, height, { mode = 'full', left = true, selector284 = 0 } = {}) {
  const mini = nativeAutomapMode(mode) === 1;
  const anchorX = left ? (selector284 === 2 ? 0 : 96) : Math.floor(2 * width / 3);
  const anchorY = left ? 0 : 78;
  const viewport = mini
    ? { left: anchorX - 8, top: anchorY, right: anchorX + Math.floor(width / 3), bottom: anchorY + Math.floor(height / 3) }
    : { left: -16, top: -16, right: width, bottom: height };
  const cellClip = !mini ? null : left
    ? { left: 0, top: selector284 === 2 ? -21 : 75, right: 279, bottom: (selector284 === 2 ? -21 : 75) + 225 }
    : { left: width - 281, top: 57, right: width - 2, bottom: 282 };
  return { anchorX, anchorY, viewport, cellClip };
}
// D2Client's per-unit automap pass uses the recomputed global viewport (0x6fbcc228..0x6fbcc234)
// as a marker gate. This is deliberately separate from the mini cell renderer's clip rectangle.
// The native per-unit routine compares all four edges inclusively. Keep this predicate available
// for native automap-projected points; ordinary world-projection `sx/sy` are not those points.
export function automapMarkerInViewport(x, y, viewport) {
  return x >= viewport.left && x <= viewport.right && y >= viewport.top && y <= viewport.bottom;
}
export function automapScrollOffset(width, height, { mode = 'full', center = 'yes', anchorX, anchorY } = {}) {
  if (nativeAutomapMode(mode) !== 1 || center !== 'yes') return { x: 0, y: 0 };
  return { x: Math.floor(width / 3) - anchorX - 16, y: Math.floor(height / 3) - anchorY - 16 };
}

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
function selectRecord(index, levelType, tile) {
  const levelName = LEVEL_NAMES[levelType] ?? 'None', tileName = TILE_NAMES[tile.orientation] ?? null;
  if (!tileName) return null;
  const record = selectIndexedAutomapRecord(index, levelName, tileName, tile);
  return record ? { record, levelName, tileName } : null;
}
// One retail marker: the 12 segments of its draw-list records as offsets from the glyph's centre.
const CROSS = Object.freeze([[0, -2], [4, -4], [8, -2], [4, 0], [8, 2], [4, 4], [0, 2], [-4, 4], [-8, 2], [-4, 0], [-8, -2], [-4, -4], [0, -2]]);
const AX = 0, AY = -29, AY_CAVE = 1;   // full-map DC6 top relative to the retail hero anchor; 890/890 saved cells match f5b
export function automapCellTopY(centerY, cellWorldY, heroWorldY, scale, cave = false) {
  return Math.round(centerY + (cellWorldY - heroWorldY) * 8 * scale + (cave ? AY_CAVE : AY));
}
const HERO_COLOUR = 151, NPC_COLOUR = 32;
function line(ctx, x0, y0, x1, y1) {   // Bresenham, 1 px, no antialiasing
  const dx = Math.abs(x1 - x0), dy = -Math.abs(y1 - y0), sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
  let err = dx + dy;
  for (;;) { ctx.fillRect(x0, y0, 1, 1); if (x0 === x1 && y0 === y1) break; const e2 = 2 * err; if (e2 >= dy) { err += dy; x0 += sx; } if (e2 <= dx) { err += dx; y0 += sy; } }
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
// Retail's D2AutomapLayerStrc is identified by Levels.txt's `Layer` value, not
// by the current level id. Several levels can therefore contribute to one saved
// map layer (for example, level 1 and level 2 both use layer 0).
export function parseAutomapLevelLayers(bytes) {
  const lines = decodeText(bytes).replace(/\r/g, '').split('\n').filter((line) => line.trim());
  if (!lines.length) throw new Error('Levels.txt is empty');
  const headings = lines[0].split('\t').map((s) => s.trim().toLowerCase());
  const id = headings.indexOf('id'), layer = headings.indexOf('layer');
  if (id < 0 || layer < 0) throw new Error('Levels.txt lacks Id or Layer');
  const out = new Map();
  for (const [index, line] of lines.slice(1).entries()) {
    const fields = line.split('\t'), idText = fields[id]?.trim(), layerText = fields[layer]?.trim();
    const levelId = /^\d+$/.test(idText || '') ? Number(idText) : -1;
    const layerNo = /^\d+$/.test(layerText || '') ? Number(layerText) : -1;
    if (!Number.isSafeInteger(levelId) || levelId < 0 || !Number.isSafeInteger(layerNo) || layerNo < 0 || layerNo >= 100 || out.has(levelId)) {
      throw new Error(`Levels.txt has invalid Id/Layer at data row ${index + 1}`);
    }
    out.set(levelId, layerNo);
  }
  if (!out.size) throw new Error('Levels.txt has no Id/Layer rows');
  return out;
}
export function getAutomapLayer(layers, levelLayers, levelId, seed = () => {}) {
  const layerNo = levelLayers.get(levelId);
  if (layerNo === undefined) throw new Error(`Levels.txt has no Layer for level ${levelId}`);
  if (!layers.has(layerNo)) { const layer = new Map(); seed(layer, layerNo); layers.set(layerNo, layer); }
  return layers.get(layerNo);
}
export function getAutomapRevealState(revealed, levelId) {
  if (!revealed.has(levelId)) revealed.set(levelId, { rooms: new Set(), hero: null });
  return revealed.get(levelId);
}
// Retail's automap layer is drawn as typed lists. The browser keeps one Map, so the visible pass
// sorts entries back into retail layer order. That order is stable until a saved or newly revealed
// cell changes the Map; cache it between those mutations. Stable sort preserves insertion order for
// equal layer types, matching the former per-frame operation.
export function createAutomapCellOrderCache() {
  const ordered = new WeakMap();
  let builds = 0;
  return {
    get(layer) {
      let entries = ordered.get(layer);
      if (!entries) {
        entries = [...layer.entries()].sort((a, b) => a[1].layer - b[1].layer);
        ordered.set(layer, entries);
        builds++;
      }
      return entries;
    },
    invalidate(layer) { ordered.delete(layer); },
    get builds() { return builds; },
  };
}
export function serializeAutomapLayers(layers, saved) {
  const out = [], seen = new Set(), covered = new Set();
  const push = (layer, frame, x, y, wall) => {
    const key = `${layer},${x},${y},${wall}`;
    if (seen.has(key)) return;
    seen.add(key); out.push(layer, frame, x, y, wall);
  };
  for (const [layerNo, layer] of layers) {
    covered.add(layerNo);
    for (const cell of layer.values()) {
      const px = cell.saved || [Math.round((cell.world[0] - cell.world[1]) * 1.6 + MA0_K.x), Math.round((cell.world[0] + cell.world[1]) * 0.8 + MA0_K.y)];
      push(layerNo, cell.frame, px[0], px[1], cell.layer ? 1 : 0);
    }
  }
  for (const [layerNo, cells] of saved) {
    if (covered.has(layerNo)) continue;
    for (const cell of cells) push(layerNo, cell.frame, cell.x, cell.y, cell.wall ? 1 : 0);
  }
  return out;
}
// One automap CELL, keyed the way retail's layer holds one: by where on the map it sits, not by the
// room that happens to hold it. A map cell carries three tile lists (floors, walls, roofs) and can
// hold one of each, so the layer is part of the key.
// RETAIL'S REVEAL RULE: on entering a NEW hero Room1,
// RevealAutomapRoom runs for each room of its pRoomsNear (the hero room + existing 3x3 neighbours) on the hero's
// level; reveals union across room changes; a room adds the cells of its own tiles, unclipped.
function levelRooms(Module, levelId) {
  const n = Module.ccall('d2_session_room_count', 'number', [], []), p = Number(Module.ccall('d2_session_rooms', 'number', [], []));
  const v = new Int32Array(Module.HEAPU8.buffer, p, n * 9), out = [];   // D2SessionRoom: ordinal, level, x, y, w, h (subtiles), ...
  for (let i = 0; i < n; i++) if (v[i * 9 + 1] === levelId) out.push({ x: v[i * 9 + 2], y: v[i * 9 + 3], w: v[i * 9 + 4], h: v[i * 9 + 5] });
  return out;
}
const inRoom = (r, x, y) => x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
const cellKey = (tile) => `${tile.worldX},${tile.worldY},${tile.layer}`;
// A cheap, order-independent signature of everything the absorb reads off a tile: its cell key (world
// position + layer), its `seam` flag, and the inputs AutoMap.txt selection uses (orientation,
// mainIndex, subIndex -- see selectRecord). One allocation-free pass; it is compared, never stored.
const tileReadSignature = (tiles) => {
  let h = tiles.length | 0;
  for (let i = 0; i < tiles.length; i++) {
    const t = tiles[i];
    h = (h + (Math.imul(t.worldX, 73856093) ^ Math.imul(t.worldY, 19349663) ^ Math.imul(t.layer, 83492791) ^
      Math.imul(t.orientation, 2654435761) ^ Math.imul(t.mainIndex, 40503) ^ Math.imul(t.subIndex, 2246822519) ^ (t.seam ? 1 : 0))) | 0;
  }
  return h;
};

export function createAutomapOverlay({ read, Module, documentObject = globalThis.document }) {
  const rows = rowsFromTxt(read('data\\global\\excel\\automap.txt'));
  const recordIndex = createAutomapRecordIndex(rows);
  const palette = decodeDatPalette(read('data\\global\\palette\\act1\\pal.dat'));
  // D2Client's mini-map Fade=No path sends each ordinary cell through DrawMode 1 (PL2
  // AlphaBlend[1]); the town palette is the current measured automap palette.
  const blend = createBlender(read('data\\global\\palette\\act1\\pal.pl2'), palette);
  // F234: the two art families, built on first use. `divisor` is retail's `nAutoMapZoomDivisor`
  // (0x6fba16b0): the loader writes 10 for the full map and 20 when mode == 1 (0x6fb1476b/0x6fb14777),
  // and the world->map scale is 1/divisor. `step` is read off the sheet rather than declared (a cell is
  // one world tile, so the sheet's widest frame box is the tile's footprint on the map); it is kept for
  // evidence and confirmation, the divisor is what the RE'd rule fixes the scale to.
  const buildFamily = (mode) => {
    const key = nativeAutomapMode(mode) === 1 ? 'mini' : 'full', path = AUTOMAP_PATHS[key], dc6 = decodeDc6(read(path));
    return { key, path, divisor: key === 'mini' ? 20 : 10, step: Math.max(...dc6.frames.map((frame) => frame.width)),
      rawFrames: dc6.frames, samples: dc6.frames.map((frame) => frame.opaque.findIndex((value) => value !== 0)),
      frames: dc6.frames.map((frame) => makeFrameCanvas(frame, palette, documentObject)) };
  };
  const families = new Map([['full', buildFamily('full')]]);
  const family = (mode) => { const key = nativeAutomapMode(mode) === 1 ? 'mini' : 'full'; if (!families.has(key)) families.set(key, buildFamily(key)); return families.get(key); };
  // F234's five options, exactly as the Automap Options page holds them (game-menu.mjs AUTOMAP_DEFAULTS).
  // Default `full` is retail's page default and the layout this overlay drew before the options existed.
  let settings = { mode: 'full', fade: 'off', center: 'yes', party: 'yes', names: 'yes', left: 'yes' };
  const makeLayout = (width = AUTOMAP_LAYOUT.width, height = AUTOMAP_LAYOUT.height) => {
    const f = family(settings.mode);
    return { ...AUTOMAP_LAYOUT, step: f.step, scale: 1 / f.divisor, divisor: f.divisor, mode: f.key, nativeMode: nativeAutomapMode(settings.mode), source: f.path,
      viewport: automapViewport(width, height, { mode: f.key, left: settings.left !== 'no' }) };
  };
  let frames = family('full').frames, layout = Object.freeze(makeLayout());
  let visible = false, evidence = { open: false, drawn: [], unresolved: [], layout, error: null };
  // Retail's automap is CLIENT state and it is per level. D2Client keeps a D2AutomapLayerStrc per
  // level (`nLayerNo`, the pFloors/pWalls/pObjects/pExtras cell lists, `pNext` the next layer), and
  // D2Game's own DUNGEON_AllocAct call passes NULL for both automap callbacks -- the server reveals
  // nothing. So a cell that has been drawn stays drawn while the room holding it is freed and rebuilt,
  // and the layer survives leaving and re-entering its level. Cells are stored by retail layer number;
  // reveal bookkeeping below stays per level id.
  const layers = new Map();
  const orderedCells = createAutomapCellOrderCache();
  // The absorb is a PURE FUNCTION of (the frame's tile read, the level's revealed rooms), and both move
  // only when the player does something: rooms reveal as the hero enters them, the tile read changes as
  // rooms activate. Re-running it on a frame where neither changed absorbs nothing -- it re-keys every
  // tile, re-walks AutoMap.txt for every tile whose room is not yet revealed, and re-tests every cell
  // already in the layer -- and THAT was the frame: 46.5 ms, over half of it, in the Rogue Encampment
  // with the map CLOSED (s2310, F238). So it runs when its inputs moved and is
  // skipped when they did not. `live` is the loop's only other product (the evidence's `remembered`
  // flag reads it) and an unchanged key means unchanged keys, so it is kept from the frame that built it.
  let absorbed = { key: null, live: new Set() };
  // The character's saved automap (parseMa0: layer number -> cells), seeded into a level's layer when it is
  // created, at retail's own frame -- Levels.txt `Layer` names the layer a level belongs to (1,2,3 -> 0; Den 8 -> 1).
  let saved = new Map();
  const levelLayer = parseAutomapLevelLayers(read('data\\global\\excel\\levels.txt'));
  const seed = (layer, layerNo) => {
    let changed = false;
    for (const c of saved.get(layerNo) || []) {
      const wsum = (c.y - MA0_K.y) / 0.8, wdif = (c.x - MA0_K.x) / 1.6, wx = (wsum + wdif) / 2, wy = (wsum - wdif) / 2, kind = c.wall ? 1 : 0;
      if (!frames[c.frame]) continue;
      layer.set(`${wx / 5},${wy / 5},${kind}`, { frame: c.frame, requestedFrames: [c.frame], levelName: 'saved', tileName: '.ma0', style: -1, startSequence: -1, endSequence: -1,
        orientation: -1, mainIndex: -1, subIndex: -1, layer: kind, world: [wx, wy], saved: [c.x, c.y] });
      changed = true;
    }
    if (changed) orderedCells.invalidate(layer);
  };
  // Cells the AutoMap.txt route cannot name, per level: the frame's terrain read offers them, no row
  // matches. Counted rather than dropped, because this is the one part of the map that still needs a
  // retail TAB frame to settle.
  const unrevealed = new Map();
  const revealed = new Map();   // level id -> { rooms: revealed room keys 'x,y', hero: the hero Room1's key }
  return {
    // `tiles` is the frame's own terrain read, handed over by the shell so the layer costs no second
    // one. The layer is filled on EVERY frame, not only while TAB is up: retail's map fills as the
    // player explores, and a map that only grows while it is being looked at is not a map of where
    // the player has been.
    draw(ctx, width, height, snapshot, tiles = null, view = null, nextSettings = null) {
      // F234: the settings arrive from the Automap Options page (the shell reads gameMenu.settings). The
      // art family follows the mode, so the active sheet and the layout move with it, BEFORE the layer
      // seeding below reads `frames`.
      if (nextSettings) settings = { ...settings, ...nextSettings };
      frames = family(settings.mode).frames;
      layout = Object.freeze(makeLayout(width, height));
      const fam = family(settings.mode), mode = fam.key, nativeMode = nativeAutomapMode(settings.mode);
      const levelId = snapshot.level;
      const layer = getAutomapLayer(layers, levelLayer, levelId, seed);
      if (!unrevealed.has(levelId)) unrevealed.set(levelId, new Set());
      const unnamed = unrevealed.get(levelId);
      const rv = getAutomapRevealState(revealed, levelId);
      let live = new Set();
      if (tiles && tiles.length) {
        const pRooms = perfT0();
        const levelType = Module.ccall('d2_session_level_type', 'number', [], []);
        const rooms = levelRooms(Module, levelId);
        const hp = view?.camera || snapshot.player, heroRoom = rooms.find((r) => inRoom(r, Math.floor(hp.x), Math.floor(hp.y)));
        perfT('automap-rooms', pRooms);
        if (heroRoom && rv.hero !== `${heroRoom.x},${heroRoom.y}`) {
          // Layer creation (amap5: 35 RevealAutomapRoom calls, ret 6FB12AD4) reveals every room built at game entry; later Room1 changes reveal the near set.
          const entry = rv.hero === null && rv.rooms.size === 0;
          rv.hero = `${heroRoom.x},${heroRoom.y}`;
          if (entry && levelId === 1) for (const r of rooms) rv.rooms.add(`${r.x},${r.y}`);
          else for (const r of rooms) if (Math.abs(r.x - heroRoom.x) <= heroRoom.w && Math.abs(r.y - heroRoom.y) <= heroRoom.h) rv.rooms.add(`${r.x},${r.y}`);
        }
        // WHAT IS REVEALED IS THE ENGINE'S ANSWER, and this frame's own terrain read already carries
        // it: `d2_session_tiles` is built from the rooms that hold an active pRoom, so a tile in this
        // list is a tile of a room the engine has built for the player to be in. It used to be
        // filtered by `d2_session_room_active_at`, which is the PATH code's one-hop resolver from the
        // player's own room -- the engine's own comment on that export says it "is right for the path
        // code, and useless for telling 'a wall' from 'not built' from 'out of the resolver's reach'"
        // -- and the filter's measured cost in the Rogue Encampment was **233 cells drawn of the
        // level's 2,337** (s590). A seam tile belongs to the neighbouring level and goes into THAT
        // level's layer, when that level is the one being played.
        // The revealed set only ever GROWS (every rv.rooms.add above), so its size is its whole state:
        // same size and same tile read means every tile's outcome below is the one already stored.
        const absorbKey = `${levelId}|${levelType}|${rv.rooms.size}|${tileReadSignature(tiles)}`;
        if (absorbKey === absorbed.key) {
          live = absorbed.live;
        } else {
          const pAbsorb = perfT0(); perfSample('automap-tiles', tiles.length);
          live = new Set();
          const sizeBeforeAbsorb = layer.size;
          for (const tile of tiles) {
            if (tile.seam) continue;
            const key = cellKey(tile); live.add(key);
            // A cell is absorbed once, on the frame its tile first exists: re-selecting it every frame
            // would walk AutoMap.txt per tile per frame for a record the engine's own tile cannot change.
            if (layer.has(key)) continue;
            const room = rooms.find((r) => inRoom(r, tile.worldX * 5, tile.worldY * 5));
            if (!room || !rv.rooms.has(`${room.x},${room.y}`)) continue;
            const selected = selectRecord(recordIndex, levelType, tile);
            if (!selected || !frames[selected.record.frames[0]]) { unnamed.add(key); continue; }
            // Retail rolls its private automap seed (gpAutomapSeed, low seed 0) per GetAutomapCellId call, in
            // the order retail adds cells -- an order this client does not have -- so the pick is a hash of the
            // cell's own position: uniform over Cel1..4 like retail's draw list (f5b: frames 0-3 ~110 each),
            // stable across frames and across layers.
            const cels = selected.record.frames;
            const frame = cels[Math.abs((tile.worldX * 73856093) ^ (tile.worldY * 19349663) ^ (tile.layer * 83492791)) % cels.length];
            layer.set(key, { frame, requestedFrames: [...selected.record.frames], levelName: selected.levelName,
              tileName: selected.tileName, style: selected.record.style, startSequence: selected.record.startSequence,
              endSequence: selected.record.endSequence, orientation: tile.orientation, mainIndex: tile.mainIndex,
              subIndex: tile.subIndex, layer: tile.layer, world: [tile.worldX * 5, tile.worldY * 5] });
          }
          if (layer.size !== sizeBeforeAbsorb) orderedCells.invalidate(layer);
          perfT('automap-absorb', pAbsorb);
          absorbed = { key: absorbKey, live };
        }
      }
      if (!visible) return (evidence = { open: false, drawn: [], unresolved: [], layout, error: null });
      const levelType = Module.ccall('d2_session_level_type', 'number', [], []);
      const drawn = [], unresolved = [];
      const scale = layout.scale;   // 1/divisor: 0.1 for the full map (mode != 1), 0.05 for the mini map (mode 1)
      // These read-only DRLG surfaces are map geometry, not a JS reconstruction: the level's
      // placed rectangle gives the panel's extent, while seam segments identify adjacent areas.
      const geometry = { levelRect: levelRect(Module, levelId), seams: (snapshot.warps || []).filter((w) => w.destinationLevel !== levelId)
        .map((warp) => ({ destinationLevel: warp.destinationLevel, rect: levelRect(Module, warp.destinationLevel), edges: seamEdges(Module, warp.destinationLevel) })) };
      // The map is centred on the hero's PRECISE position, at the screen point the world pass puts the hero
      // (w/2, h/2 + UNIT_CENTRE_Y = retail's 400,292 on 800x600, and the middle of a wide viewport).
      // THE MAP IS CENTRED ON THE HERO'S PRECISE POSITION at the screen point the world pass puts the hero
      // (w/2, h/2 + UNIT_CENTRE_Y = retail's 400,292 on 800x600), at a tenth of the world's scale: a tile is a
      // 16x8 diamond, (8,4) per tile step -- every one of the 890 cells of f5b-automap-town.txt and 826 of
      // a3-forest-walkin.txt sits on that lattice, offset by the hero's sub-tile position (0 in the town, where
      // the hero starts on a tile origin; 4,3 in the forest).
      // AutoMapLeft defaults on at native startup. Its initial selector state is zero in `.data`, so
      // 0x6fb0f9f0 places the mini-map anchor at (96,0); the exact marker viewport and cell clip are
      // carried in layout. AutoMapCenter is a SCROLL offset, so projected cells move opposite it.
      const viewport = automapViewport(width, height, { mode, left: settings.left !== 'no' });
      const centre = automapScrollOffset(width, height, { mode, center: settings.center,
        anchorX: viewport.anchorX, anchorY: viewport.anchorY });
      const cx = Math.round(width / 2) - centre.x, cy = Math.round(height / 2 + UNIT_CENTRE_Y) - centre.y;
      const hero = view?.camera || { x: snapshot.player.x + .5, y: snapshot.player.y + .5 };
      if (!layer.size) unresolved.push({ reason: 'this level has no revealed cell yet' });
      ctx.save(); ctx.imageSmoothingEnabled = false;
      if (viewport.cellClip) {
        const { left, top, right, bottom } = viewport.cellClip;
        ctx.beginPath(); ctx.rect(left, top, right - left + 1, bottom - top + 1); ctx.clip();
      }
      // RETAIL'S OWN LIST ORDER, not the order the cells happened to be discovered in: a layer holds
      // pFloors, pWalls, pObjects and pExtras and the client walks them in that order, while this map
      // drew floors and walls interleaved by insertion -- so a floor absorbed after a wall painted
      // over it. The engine's tile layer is the same 0 floors / 1 walls / 2 roofs split.
      const ordered = orderedCells.get(layer);
      for (const [key, cell] of ordered) {
        // D2Common chooses one Cel from this selected row. The browser has no access to that
        // private Automap RNG stream, so it deterministically uses the row's first valid Cel;
        // evidence retains every permitted Cel rather than claiming the retail random choice.
        const bitmap = frames[cell.frame];
        const x = Math.round(cx + ((cell.world[0] - cell.world[1]) - (hero.x - hero.y)) * 16 * scale + AX);
        const y = automapCellTopY(cy, cell.world[0] + cell.world[1], hero.x + hero.y, scale, LEVEL_NAMES[levelType] === '1 Cave');   // '1 Cave' MEASURED +1 (Den, s960/s961: 81/81 and 82/82 cells at one shift); other dungeon types unmeasured
        // Retail lists only what is on screen, and it culls on the cell's ANCHOR row (its DC6 anchor is
        // bottom-left, "the art fills the bottom of the box"), not the box top: f5b's lowest listing is y=592
        // of 600 and the next lattice row (596) is culled. Culling on the box top kept every cell whose
        // bottom rows fell past 592 -- 31 of s953's ours-only cells (the `.ma0` tail) were that, not reveal.
        if (x + bitmap.width <= 0 || x >= width || y + bitmap.height <= 0 || y + bitmap.height - 1 > height - 8) continue;
        // F10 / AutoMapFade: the mini-map uses one PL2 blend mode when Fade is off;
        // full-map mode 0 uses the native distance ramp for default MaxiMap cells.
        const drawMode = mode === 'mini'
          ? (settings.fade === 'off' ? 1 : 5)
          : fullMapFadeDrawMode({ mode: nativeMode, fade: settings.fade, source: fam.path, x, y, width, height,
            targetX: view?.fadeTargetX ?? width / 2 });
        if (drawMode >= 0 && drawMode <= 2) blend(ctx, fam.rawFrames[cell.frame], x, y, drawMode);
        else ctx.drawImage(bitmap, x, y);
        const rawFrame = fam.rawFrames[cell.frame], firstOpaque = fam.samples[cell.frame];
        const sample = firstOpaque < 0 ? null : [x + firstOpaque % rawFrame.width, y + Math.floor(firstOpaque / rawFrame.width)];
        drawn.push({ source: fam.path, frame: cell.frame, requestedFrames: [...cell.requestedFrames], levelName: cell.levelName,
          tileName: cell.tileName, style: cell.style, startSequence: cell.startSequence, endSequence: cell.endSequence,
          tile: { orientation: cell.orientation, style: cell.mainIndex, sequence: cell.subIndex, layer: cell.layer, world: cell.world },
          at: [x, y], size: [bitmap.width, bitmap.height], drawMode, sample, order: drawn.length, remembered: !live.has(key) });
      }
      if (viewport.cellClip) { ctx.restore(); ctx.save(); }
      // Unit markers, as retail draws them over the map (f5b-automap-town.txt, a3-forest-walkin.txt): a
      // 12-segment 16x8 glyph centred 8 right / 7 up of the unit's feet, palette index 151 for the hero and
      // 32 for a town NPC (the draw list's colour is index + 256*k), plus the NPC's name beside it.
      const markers = [], nameplates = [];
      const cross = (x, y, index, kind) => {
        const r = palette[index * 4], g = palette[index * 4 + 1], b = palette[index * 4 + 2]; ctx.fillStyle = `rgb(${r},${g},${b})`;
        for (let i = 0; i < CROSS.length - 1; i++) line(ctx, x + CROSS[i][0], y + CROSS[i][1], x + CROSS[i + 1][0], y + CROSS[i + 1][1]);
        markers.push({ index, at: [x, y], kind });
      };
      if (view) {
        // F234: the native per-unit pass gates map-space markers before its unit-type/options branches.
        // This client only has ordinary world-screen `sx/sy` here, so it cannot safely apply that gate
        // until the native automap projection is represented. Preserve the known NPC marker/name option
        // behavior while leaving viewport culling open (canvas bounds still clip naturally).
        for (const n of view.npcs || []) {
          const markerX = Math.round(n.sx) + 8, markerY = Math.round(n.sy) - 7;
          cross(markerX, markerY, NPC_COLOUR, 'npc');
          if (settings.names === 'yes' && n.label && view.font) { const at = [Math.round(n.sx) - 18 + 8, Math.round(n.sy) - 10 + 2 - 7]; view.font.draw(ctx, n.label, at[0], at[1]); nameplates.push({ text: n.label, at }); }
        }
        cross(Math.round(cx) + 8, Math.round(cy) - 7, HERO_COLOUR, 'hero');
      }
      ctx.restore();
      return (evidence = { open: true, markers, nameplates, levelType, levelName: LEVEL_NAMES[levelType] ?? 'None', cells: layer.size,
        mode, nativeMode, divisor: fam.divisor, source: fam.path, settings: { ...settings }, centre,
        unrevealed: unnamed.size, drawn, unresolved, geometry, layout, error: null });
    },
    get evidence() { return JSON.parse(JSON.stringify(evidence)); },
    /** F234: the Automap Options page's five settings, read by the shell from gameMenu.settings each frame. */
    setSettings(next) { if (next) settings = { ...settings, ...next }; },
    // The overlay's own open state, as a boolean: the frame loop asks this every frame, and
    // `evidence` is a JSON copy of every drawn cell, which is not a per-frame read (F115 gates the
    // area-name block on it).
    get isOpen() { return visible; },
    toggle() { visible = !visible; },
    // The saved layer arrives AFTER the overlay exists (the file is fetched, or the store is read), so a
    // level that already drew keeps its derived cells and gains the saved ones here -- otherwise the saved
    // cells never enter a layer created on a frame between the overlay's construction and this call. A
    // saved cell wins over a derived one at the same tile, as seed's own set does.
    setSaved(map) { saved = map; for (const [layerNo, layer] of layers) seed(layer, layerNo); },
    // The character's automap layer as cells for retail's `<character>.ma0`, flat `{layer, frame, x, y,
    // wall}` quints: every level this session holds, in automap pixels (ma0.mjs's own transform). The
    // shell hands these to the engine's writer (d2_session_save_ma0) on Save and Exit -- the reveal is
    // the CLIENT's, so the engine is given the cells rather than re-deriving them, and a seeded cell
    // keeps its own pixels so the round trip is exact. Levels no session has drawn yet keep the cells
    // the loaded file gave them, which is what a resumed character's other floors are.
    savedCells() {
      return serializeAutomapLayers(layers, saved);
    },
  };
}
