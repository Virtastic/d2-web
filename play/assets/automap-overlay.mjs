import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { TILE_SCREEN_W, UNIT_CENTRE_Y } from './session-terrain.mjs';
import { MA0_K } from './ma0.mjs';
import { perfT0, perfT, perfSample } from './frame-perf.mjs';

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
// One retail marker: the 12 segments of its draw-list records as offsets from the glyph's centre.
const CROSS = Object.freeze([[0, -2], [4, -4], [8, -2], [4, 0], [8, 2], [4, 4], [0, 2], [-4, 4], [-8, 2], [-4, 0], [-8, -2], [-4, -4], [0, -2]]);
const AX = 0, AY = -36, AY_CAVE = 1;   // the hero's tile-origin cell's box relative to the hero's screen point (fitted: s953/s954)
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
  const scale = step / (2 * TILE_SCREEN_W); // TILE_SCREEN_W is a tile diamond HALF-width: the 16 px box is the 160 px tile diamond (retail step 8/4 per tile, f5b-automap-town.txt)
  const layout = Object.freeze({ ...AUTOMAP_LAYOUT, step, scale });
  let visible = false, evidence = { open: false, drawn: [], unresolved: [], layout, error: null };
  // Retail's automap is CLIENT state and it is per level. D2Client keeps a D2AutomapLayerStrc per
  // level (`nLayerNo`, the pFloors/pWalls/pObjects/pExtras cell lists, `pNext` the next layer), and
  // D2Game's own DUNGEON_AllocAct call passes NULL for both automap callbacks -- the server reveals
  // nothing. So a cell that has been drawn stays drawn while the room holding it is freed and rebuilt,
  // and the layer survives leaving and re-entering its level. This is that layer, per level id:
  // level id -> (cell key -> cell).
  const layers = new Map();
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
  const levelLayer = new Map();
  try { const t = decodeText(read('data\\global\\excel\\levels.txt')).replace(/\r/g, '').split('\n'), h = t[0].split('\t'), id = h.indexOf('Id'), ly = h.indexOf('Layer');
    for (const l of t.slice(1)) { const f = l.split('\t'); if (f[id] !== undefined && f[id] !== '') levelLayer.set(+f[id], +f[ly]); } } catch { /* no levels.txt: no saved layer */ }
  const seed = (layer, levelId) => {
    for (const c of saved.get(levelLayer.get(levelId)) || []) {
      const wsum = (c.y - MA0_K.y) / 0.8, wdif = (c.x - MA0_K.x) / 1.6, wx = (wsum + wdif) / 2, wy = (wsum - wdif) / 2, kind = c.wall ? 1 : 0;
      if (!frames[c.frame]) continue;
      layer.set(`${wx / 5},${wy / 5},${kind}`, { frame: c.frame, requestedFrames: [c.frame], levelName: 'saved', tileName: '.ma0', style: -1, startSequence: -1, endSequence: -1,
        orientation: -1, mainIndex: -1, subIndex: -1, layer: kind, world: [wx, wy], saved: [c.x, c.y] });
    }
  };
  // Cells the AutoMap.txt route cannot name, per level: the frame's terrain read offers them, no row
  // matches. Counted rather than dropped, because this is the one part of the map that still needs a
  // retail TAB frame to settle.
  const unrevealed = new Map();
  const revealed = new Map();   // level id -> { rooms: revealed room keys 'x,y', hero: the hero Room1's key }
  globalThis.addEventListener('keydown', (event) => {
    if (isGameplay() && event.code === 'Tab' && !event.repeat) { visible = !visible; event.preventDefault(); event.stopImmediatePropagation(); }
  });
  return {
    // `tiles` is the frame's own terrain read, handed over by the shell so the layer costs no second
    // one. The layer is filled on EVERY frame, not only while TAB is up: retail's map fills as the
    // player explores, and a map that only grows while it is being looked at is not a map of where
    // the player has been.
    draw(ctx, width, height, snapshot, tiles = null, view = null) {
      const levelId = snapshot.level;
            if (!layers.has(levelId)) { const fresh = new Map(); seed(fresh, levelId); layers.set(levelId, fresh); unrevealed.set(levelId, new Set()); revealed.set(levelId, { rooms: new Set(), hero: null }); }
      const layer = layers.get(levelId), unnamed = unrevealed.get(levelId);
      let live = new Set();
      if (tiles && tiles.length) {
        const pRooms = perfT0();
        const levelType = Module.ccall('d2_session_level_type', 'number', [], []);
        const rv = revealed.get(levelId), rooms = levelRooms(Module, levelId);
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
          for (const tile of tiles) {
            if (tile.seam) continue;
            const key = cellKey(tile); live.add(key);
            // A cell is absorbed once, on the frame its tile first exists: re-selecting it every frame
            // would walk AutoMap.txt per tile per frame for a record the engine's own tile cannot change.
            if (layer.has(key)) continue;
            const room = rooms.find((r) => inRoom(r, tile.worldX * 5, tile.worldY * 5));
            if (!room || !rv.rooms.has(`${room.x},${room.y}`)) continue;
            const selected = selectRecord(rows, levelType, tile);
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
          perfT('automap-absorb', pAbsorb);
          absorbed = { key: absorbKey, live };
        }
      }
      if (!visible) return (evidence = { open: false, drawn: [], unresolved: [], layout, error: null });
      const levelType = Module.ccall('d2_session_level_type', 'number', [], []);
      const drawn = [], unresolved = [];
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
      const cx = Math.round(width / 2), cy = Math.round(height / 2 + UNIT_CENTRE_Y);
      const hero = view?.camera || { x: snapshot.player.x + .5, y: snapshot.player.y + .5 };
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
        const x = Math.round(cx + ((cell.world[0] - cell.world[1]) - (hero.x - hero.y)) * 16 * scale + AX);
        const y = Math.round(cy + ((cell.world[0] + cell.world[1]) - (hero.x + hero.y)) * 8 * scale + (LEVEL_NAMES[levelType] === '1 Cave' ? AY_CAVE : AY));   // '1 Cave' MEASURED +1 (Den, s960/s961: 81/81 and 82/82 cells at one shift); other dungeon types unmeasured
        if (x + bitmap.width <= 0 || x >= width || y + bitmap.height <= 0 || y > height - 8) continue;   // retail lists only what is on screen: f5b's lowest cell is y=592 of 600, the next lattice row (596) is culled
        ctx.drawImage(bitmap, x, y);
        drawn.push({ source: AUTOMAP_PATH, frame: cell.frame, requestedFrames: [...cell.requestedFrames], levelName: cell.levelName,
          tileName: cell.tileName, style: cell.style, startSequence: cell.startSequence, endSequence: cell.endSequence,
          tile: { orientation: cell.orientation, style: cell.mainIndex, sequence: cell.subIndex, layer: cell.layer, world: cell.world },
          at: [x, y], size: [bitmap.width, bitmap.height], order: drawn.length, remembered: !live.has(key) });
      }
      // Unit markers, as retail draws them over the map (f5b-automap-town.txt, a3-forest-walkin.txt): a
      // 12-segment 16x8 glyph centred 8 right / 7 up of the unit's feet, palette index 151 for the hero and
      // 32 for a town NPC (the draw list's colour is index + 256*k), plus the NPC's name beside it.
      const markers = [];
      const cross = (x, y, index) => {
        const r = palette[index * 4], g = palette[index * 4 + 1], b = palette[index * 4 + 2]; ctx.fillStyle = `rgb(${r},${g},${b})`;
        for (let i = 0; i < CROSS.length - 1; i++) line(ctx, x + CROSS[i][0], y + CROSS[i][1], x + CROSS[i + 1][0], y + CROSS[i + 1][1]);
        markers.push({ index, at: [x, y] });
      };
      if (view) {
        for (const n of view.npcs || []) { cross(Math.round(n.sx) + 8, Math.round(n.sy) - 7, NPC_COLOUR); if (n.label && view.font) view.font.draw(ctx, n.label, Math.round(n.sx) - 18 + 8, Math.round(n.sy) - 10 + 2 - 7); }
        cross(Math.round(cx) + 8, Math.round(cy) - 7, HERO_COLOUR);
      }
      ctx.restore();
      return (evidence = { open: true, markers, levelType, levelName: LEVEL_NAMES[levelType] ?? 'None', cells: layer.size,
        unrevealed: unnamed.size, drawn, unresolved, geometry, layout, error: null });
    },
    get evidence() { return JSON.parse(JSON.stringify(evidence)); },
    // The overlay's own open state, as a boolean: the frame loop asks this every frame, and
    // `evidence` is a JSON copy of every drawn cell, which is not a per-frame read (F115 gates the
    // area-name block on it).
    get isOpen() { return visible; },
    toggle() { visible = !visible; },
    // The saved layer arrives AFTER the overlay exists (the file is fetched, or the store is read), so a
    // level that already drew keeps its derived cells and gains the saved ones here -- otherwise the saved
    // cells never enter a layer created on a frame between the overlay's construction and this call. A
    // saved cell wins over a derived one at the same tile, as seed's own set does.
    setSaved(map) { saved = map; for (const levelId of layers.keys()) seed(layers.get(levelId), levelId); },
    // The character's automap layer as cells for retail's `<character>.ma0`, flat `{layer, frame, x, y,
    // wall}` quints: every level this session holds, in automap pixels (ma0.mjs's own transform). The
    // shell hands these to the engine's writer (d2_session_save_ma0) on Save and Exit -- the reveal is
    // the CLIENT's, so the engine is given the cells rather than re-deriving them, and a seeded cell
    // keeps its own pixels so the round trip is exact. Levels no session has drawn yet keep the cells
    // the loaded file gave them, which is what a resumed character's other floors are.
    savedCells() {
      const out = [], seen = new Set();
      const push = (layer, frame, x, y, wall) => { const k = `${layer},${x},${y},${wall}`; if (seen.has(k)) return; seen.add(k); out.push(layer, frame, x, y, wall); };
      const covered = new Set();
      for (const [levelId, layer] of layers) {
        const n = levelLayer.get(levelId); if (n === undefined) continue;
        covered.add(n);
        for (const cell of layer.values()) {
          const px = cell.saved || [Math.round((cell.world[0] - cell.world[1]) * 1.6 + MA0_K.x), Math.round((cell.world[0] + cell.world[1]) * 0.8 + MA0_K.y)];
          push(n, cell.frame, px[0], px[1], cell.layer ? 1 : 0);
        }
      }
      for (const [n, cells] of saved) { if (covered.has(n)) continue; for (const c of cells) push(n, c.frame, c.x, c.y, c.wall ? 1 : 0); }
      return out;
    },
  };
}
