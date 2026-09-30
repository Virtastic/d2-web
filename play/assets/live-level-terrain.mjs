// Live terrain has no act atlas: the engine supplies the current level's DT1 list and tile
// identities. The act only selects the retail palette that colours those decoded indexed pixels.
import { decodeDatPalette, decodeDt1 } from './d2-formats.mjs';
import { createMpqAssetReader } from './mpq-assets.mjs';
import { createSessionTerrain, readLevelTileFiles } from './session-terrain.mjs';

const ACT_PALETTES = Object.freeze([
  'data\\global\\palette\\ACT1\\pal.dat',
  'data\\global\\palette\\ACT2\\pal.dat',
  'data\\global\\palette\\ACT3\\pal.dat',
  'data\\global\\palette\\ACT4\\pal.dat',
  'data\\global\\palette\\ACT5\\pal.dat',
]);

/** The palette an act's in-game screen is drawn in -- the world's and its panels'. `act` is 1-based. */
export function actPalettePath(act) { return ACT_PALETTES[Math.min(5, Math.max(1, act | 0)) - 1]; }

export function loadLiveLevelTerrain(Module, act0) {
  try {
    const files = readLevelTileFiles(Module);
    if (!files.some(Boolean)) return { terrain: null, files: null, error: 'engine reported no tile libraries for this level' };
    const palettePath = ACT_PALETTES[act0];
    if (!palettePath) throw new RangeError(`unsupported act ${act0 + 1}`);
    return { terrain: createSessionTerrain({
      read: createMpqAssetReader(Module), decodeDt1, decodeDatPalette, palettePath,
    }), files, error: '' };
  } catch (error) {
    return { terrain: null, files: null, error: String(error) };
  }
}
