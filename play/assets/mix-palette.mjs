// D2CMP_MixPalette, read from the player's own MPQs: a component colour byte is
// `1 + nColor + 32*nTrans` (nTrans in {1,2,5,6,7,8}, nColor in [0,20]) and its 256-entry
// palette-index remap is row nColor of data\global\items\palette\<name>.dat (21 rows of 256).
const NAMES = ['grey', 'grey2', 'gold', 'brown', 'greybrown', 'invgrey', 'invgrey2', 'invgreybrown'];
const VALID = new Set([1, 2, 5, 6, 7, 8]);
const files = new WeakMap();

/** The remap a colour byte selects, or null (0xFF, out of range, or the file is missing). */
export function mixTable(read, colourByte) {
  if (typeof read !== 'function' || !Number.isInteger(colourByte) || colourByte < 1 || colourByte === 0xFF) return null;
  const nTrans = (colourByte - 1) >> 5, nColor = (colourByte - 1) & 31;
  if (!VALID.has(nTrans) || nColor > 20) return null;
  let cache = files.get(read); if (!cache) files.set(read, cache = new Map());
  const name = NAMES[nTrans - 1];
  if (!cache.has(name)) {
    let bytes = null;
    try { bytes = read(`data\\global\\items\\palette\\${name}.dat`); } catch { bytes = null; }
    cache.set(name, bytes && bytes.length >= 21 * 256 ? bytes : null);
  }
  const bytes = cache.get(name);
  return bytes ? bytes.subarray(nColor * 256, nColor * 256 + 256) : null;
}
