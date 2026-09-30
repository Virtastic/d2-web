// D2Client's saved automap, `<character>.ma0`: a 100-dword table
// (layer number -> offset of that layer's first chunk, 0 = none), then chunks chained by a next-offset.
// A chunk = 32-byte header {next, layer, 0, seed, floorBytes, wallBytes, 0, 0} + floor cells + wall cells,
// each cell 6 bytes {u16 cellNo (= MaxiMap frame), i16 x, u16 y} in automap pixels. The loader unions every
// chunk of a layer (measured: Oracle layer 0 = 881 floors + 296 walls = retail's 1177-cell entry layer).
export function parseMa0(bytes) {
  const v = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength), layers = new Map();
  if (bytes.length < 400) return layers;
  for (let layer = 0; layer < 100; layer++) {
    const cells = [];
    for (let o = v.getUint32(layer * 4, true), guard = 0; o && o + 32 <= bytes.length && guard < 1000; guard++) {
      const next = v.getUint32(o, true), f = v.getUint32(o + 16, true) / 6, w = v.getUint32(o + 20, true) / 6;
      if (o + 32 + (f + w) * 6 > bytes.length) break;
      for (let i = 0; i < f + w; i++) { const c = o + 32 + i * 6; cells.push({ frame: v.getUint16(c, true), x: v.getInt16(c + 2, true), y: v.getInt16(c + 4, true), wall: i >= f }); }
      o = next;
    }
    if (cells.length) layers.set(layer, cells);
  }
  return layers;
}
// Automap pixels <-> the overlay's world subtiles: x = (wx-wy)*1.6 - 8, y = (wx+wy)*0.8 - 2028 (fitted to the
// town's own tiles: 584 of 692 coincide, the rest is the fixture's differing reveal; s953).
export const MA0_K = Object.freeze({ x: -8, y: -2028 });
