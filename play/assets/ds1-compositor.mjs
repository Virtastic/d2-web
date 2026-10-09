import { decodeDatPalette, decodeDt1, tilePixelsToRgba } from './d2-formats.mjs';
import { decodeDs1, decodeDs1Cell } from './ds1.mjs';

export const ACT_PRESET_ASSETS = Object.freeze([
  Object.freeze({ ds1: 'data\\global\\tiles\\ACT1\\TOWN\\townE1.ds1', palette: 'data\\global\\palette\\ACT1\\pal.dat' }),
  Object.freeze({ ds1: 'data\\global\\tiles\\ACT2\\Town\\lut2.ds1', palette: 'data\\global\\palette\\ACT2\\pal.dat' }),
  Object.freeze({ ds1: 'data\\global\\tiles\\ACT3\\Docktown\\docktown3.ds1', palette: 'data\\global\\palette\\ACT3\\pal.dat' }),
  Object.freeze({ ds1: 'data\\global\\tiles\\ACT4\\Fort\\fortress.ds1', palette: 'data\\global\\palette\\ACT4\\pal.dat' }),
  Object.freeze({ ds1: 'data\\global\\tiles\\expansion\\Town\\townWest.ds1', palette: 'data\\global\\palette\\ACT5\\pal.dat' }),
]);
export const TOWN_E1_DS1 = ACT_PRESET_ASSETS[0].ds1;
export const ACT1_PALETTE = ACT_PRESET_ASSETS[0].palette;
export const TILE_WIDTH = 160;
export const TILE_HEIGHT = 80;

export function normalizeDt1Dependency(path) {
  if (typeof path !== 'string' || !path) throw new TypeError('DT1 dependency path must be non-empty');
  let normalized = path.replaceAll('/', '\\').replace(/^\\+/, '');
  normalized = normalized.replace(/^[a-z]:\\d2\\/i, '');
  if (/^d2\\/i.test(normalized)) normalized = normalized.slice(3);
  normalized = normalized.replace(/\.tg1$/i, '.dt1');
  return normalized;
}

function indexOptions(lookup, mainIndex, subIndex, orientation, value) {
  let bySubIndex = lookup.get(mainIndex);
  if (!bySubIndex) lookup.set(mainIndex, bySubIndex = new Map());
  let byOrientation = bySubIndex.get(subIndex);
  if (!byOrientation) bySubIndex.set(subIndex, byOrientation = new Map());
  let options = byOrientation.get(orientation);
  if (!options) {
    byOrientation.set(orientation, options = []);
    options.push(value);
    return true;
  }
  options.push(value);
  return false;
}

function findOptions(lookup, mainIndex, subIndex, orientation) {
  return lookup.get(mainIndex)?.get(subIndex)?.get(orientation);
}

function isDrawable(cell) {
  return cell.prop1 !== 0 && (cell.raw & 0x80000000) === 0;
}

// Stable weighted choice. Retail's selected tile ultimately comes from the game seed; until the
// session exports its preset-room seed, coordinates keep this presentation deterministic while
// preserving each DT1 entry's rarity weighting.
function selectVariant(options, x, y, layer) {
  if (options.length === 1) return options[0];
  let total = 0;
  for (const option of options) total += Math.max(0, option.tile.rarity);
  if (!total) return options[0];
  let h = (Math.imul(x + 1, 0x1f123bb5) ^ Math.imul(y + 1, 0x05491333) ^
    Math.imul(layer + 1, 0x2c1b3c6d)) >>> 0;
  h ^= h << 13; h ^= h >>> 17; h ^= h << 5;
  let value = (h >>> 0) % total;
  for (const option of options) {
    value -= Math.max(0, option.tile.rarity);
    if (value < 0) return option;
  }
  return options[0];
}

function addCommand(commands, lookup, raw, orientation, x, y, layer, kind) {
  const cell = decodeDs1Cell(raw, orientation);
  if (!isDrawable(cell)) return false;
  const options = findOptions(lookup, cell.mainIndex, cell.subIndex, cell.orientation);
  if (!options?.length) return false;
  const chosen = selectVariant(options, x, y, layer);
  const anchorX = (x - y) * (TILE_WIDTH / 2);
  const anchorY = (x + y) * (TILE_HEIGHT / 2);
  commands.push({ ...chosen, kind, layer, x, y, anchorX, anchorY, cell });
  return true;
}

/** Build a deterministic draw plan from a decoded DS1 and its declared decoded DT1 libraries. */
export function composeDs1Map(map, libraries) {
  if (!map?.floors || !map?.walls || !Array.isArray(libraries)) {
    throw new TypeError('decoded DS1 and DT1 libraries are required');
  }
  const lookup = new Map();
  let libraryTileCount = 0, identityCount = 0;
  libraries.forEach((library, libraryIndex) => {
    if (!library?.tiles) throw new TypeError(`invalid DT1 library ${libraryIndex}`);
    libraryTileCount += library.tiles.length;
    library.tiles.forEach((tile, tileIndex) => {
      if (indexOptions(lookup, tile.mainIndex, tile.subIndex, tile.orientation,
        { tile, libraryIndex, tileIndex })) identityCount++;
    });
  });

  const floors = [], walls = [];
  const cells = map.width * map.height;
  for (let y = 0; y < map.height; y++) for (let x = 0; x < map.width; x++) {
    const index = y * map.width + x;
    for (let layer = 0; layer < map.floors.length; layer++) {
      addCommand(floors, lookup, map.floors[layer][index], 0, x, y, layer, 'floor');
    }
    for (let layer = 0; layer < map.walls.length; layer++) {
      const orientation = map.orientations[layer][index] >>> 0;
      if (orientation === 10 || orientation === 11) continue; // logical exit/warp markers
      if (addCommand(walls, lookup, map.walls[layer][index], orientation, x, y, layer, 'wall') && orientation === 3) {
        addCommand(walls, lookup, map.walls[layer][index], 4, x, y, layer + map.walls.length, 'wall');
      }
    }
  }
  if (cells !== map.width * map.height) throw new Error('DS1 cell count overflow');
  walls.sort((a, b) => (a.x + a.y) - (b.x + b.y) || a.y - b.y || a.layer - b.layer);
  return {
    width: map.width, height: map.height, dependencies: map.dependencies.length,
    libraries: libraries.length, libraryTileCount, identityCount,
    floorCount: floors.length, wallCount: walls.length,
    commands: [...floors, ...walls], floors, walls,
  };
}

/** Stream townE1 and exactly the DT1 libraries declared by that DS1 through the MPQ reader. */
export function loadTownE1Composition(read) {
  return loadActPresetComposition(read, 0);
}

/** Stream one listfile-proven representative preset and all its declared DT1s for an act. */
export function loadActPresetComposition(read, act0) {
  const asset = ACT_PRESET_ASSETS[act0];
  if (!asset) throw new RangeError(`unsupported act ${act0 + 1}`);
  const map = decodeDs1(read(asset.ds1));
  const libraries = map.dependencies.map((path) => decodeDt1(read(normalizeDt1Dependency(path))));
  const palette = decodeDatPalette(read(asset.palette));
  return { act0, asset, map, libraries, palette, plan: composeDs1Map(map, libraries) };
}

function tileCanvas(command, palette, documentObject, cache) {
  const id = `${command.libraryIndex}:${command.tileIndex}`;
  let canvas = cache.get(id);
  if (canvas) return canvas;
  const pixels = command.tile.pixels;
  canvas = documentObject.createElement('canvas');
  canvas.width = pixels.width; canvas.height = pixels.height;
  const context = canvas.getContext('2d');
  context.putImageData(new ImageData(tilePixelsToRgba(pixels, palette), pixels.width, pixels.height), 0, 0);
  cache.set(id, canvas);
  return canvas;
}

/** Rasterize a composition plan to one transparent offscreen canvas at retail tile spacing. */
export function createDs1Canvas(composition, documentObject = globalThis.document) {
  if (!documentObject?.createElement) throw new Error('canvas document is unavailable');
  const { plan, palette } = composition;
  let left = Infinity, top = Infinity, right = -Infinity, bottom = -Infinity;
  for (const command of plan.commands) {
    const p = command.tile.pixels;
    const x = command.anchorX - TILE_WIDTH / 2 + p.originX;
    const y = command.anchorY + p.originY;
    left = Math.min(left, x); top = Math.min(top, y);
    right = Math.max(right, x + p.width); bottom = Math.max(bottom, y + p.height);
  }
  if (!Number.isFinite(left)) throw new Error('DS1 composition contains no drawable tiles');
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.ceil(right - left); canvas.height = Math.ceil(bottom - top);
  const context = canvas.getContext('2d'); context.imageSmoothingEnabled = false;
  const cache = new Map();
  for (const command of plan.commands) {
    const p = command.tile.pixels;
    context.drawImage(tileCanvas(command, palette, documentObject, cache),
      command.anchorX - TILE_WIDTH / 2 + p.originX - left,
      command.anchorY + p.originY - top);
  }
  canvas.d2OriginX = -left; canvas.d2OriginY = -top;
  canvas.d2TileCacheSize = cache.size;
  return canvas;
}
