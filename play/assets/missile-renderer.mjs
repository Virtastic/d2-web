import { decodeDatPalette } from './d2-formats.mjs';
import { decodeDcc } from './unit-formats.mjs';
import { createBlender } from './frontend.mjs';
import { spriteMetrics } from './sprite-metrics.mjs';
import { dccDirection, indexedToRgba } from './unit-renderer.mjs';
import { createRenderCache } from './render-cache.mjs';
import { highlightMap } from './unit-highlight.mjs';

const MISSILE_CEL = Object.freeze({ 1: 'Javelin' });
const PALETTE = Object.freeze(['ACT1', 'ACT2', 'ACT3', 'ACT4', 'ACT5']);
const MISSILE_DECODE_CACHE_ENTRIES = 128;
const MISSILE_DECODE_CACHE_BYTES = 64 * 1024 * 1024;
const MISSILE_RASTER_CACHE_ENTRIES = 256;
const MISSILE_RASTER_CACHE_BYTES = 16 * 1024 * 1024;

function decodedMissileBytes(dcc, sourceBytes) {
  let bytes = sourceBytes + dcc.framesPerDirection * dcc.directions * 128;
  for (const direction of dcc.directionRecords) {
    if (!direction) continue;
    for (const frame of direction.frames) bytes += frame.indexed.byteLength + frame.opaque.byteLength + 96;
  }
  return bytes;
}

// A missile's own record, as `d2_session_missile_animations` writes it (24 bytes): the engine's frame
// clock and facing for the unit, plus Missiles.txt's nTrans.  Read where the missile is drawn, so the
// CelFile is drawn in the state the ENGINE has it in rather than one guessed from the shell's tick.
export function parseMissileAnimations(bytes, count) {
  const view = bytes instanceof DataView ? bytes : new DataView(bytes.buffer, bytes.byteOffset || 0, bytes.byteLength);
  if (!Number.isInteger(count) || count < 0 || view.byteLength < count * 24) throw new RangeError('truncated missile animation records');
  return Array.from({ length: count }, (_, row) => {
    const at = row * 24;
    return { id: view.getUint32(at, true), classId: view.getUint32(at + 4, true), mode: view.getUint32(at + 8, true),
      framePrecise: view.getUint32(at + 12, true), frameCountPrecise: view.getUint32(at + 16, true),
      animSpeed: view.getUint16(at + 20, true), direction: view.getUint8(at + 22), trans: view.getUint8(at + 23) };
  });
}

// Native D2Client 1.13c FUN_6FB1CC00 base draw-mode selection for a non-hovered missile:
// nTrans 1 -> 3, 2 -> 4, every other value -> default normal mode 5. Native mode 7 can
// override the base mode for the hovered unit; this renderer does not yet receive that identity.
// Keep the base request observable even when missing PL2 tables force an opaque pixel fallback.
// FUN_6FB1CC00 sets the override when FUN_6FB01A80's hovered pointer is this unit: same type (3) AND guid, so a monster's id never matches a missile's.
export const isHoveredMissile = (hovered, id) => hovered?.kind === 'missile' && hovered.id === id;

export function missileTransDrawMode(trans, hovered = false) {
  if (hovered) return 7; // FUN_6FB1CC00: the hovered unit (FUN_6FB01A80) takes mode 7 whatever its nTrans
  const value = Number(trans);
  return value === 1 ? 3 : value === 2 ? 4 : 5;
}

// Retail Missiles.txt supplies CelFile: `celFile(classId)` is the engine's table (d2_missile_celfile);
// without it only the Throw slice's Javelin is known.  `frame` is the engine's own sequence counter
// (framePrecise >> 8) when the caller has one, and falls back to `tick` for a caller that does not.
// `direction` is the unit's 64-slot facing and is resolved through the DCC's own direction count, the
// same step a unit's DCC goes through (`dccDirection`); passing it straight in as an index was wrong
// for any DCC with fewer than 64 directions.
// F20/F185: non-hovered Missiles.txt nTrans -> base draw mode, recovered from D2Client 1.13c FUN_6FB1CC00 (image base
// 0x6FAB0000, RVA 0x6CC00;: 0 -> mode 5 (normal), 1 -> mode 3
// (PL2 additive), 2 -> mode 4 (PL2 multiplicative, UnholyBolt).  Retail only ever ships those three
// Trans values; anything else uses native default mode 5.
export function createMissileRenderer(read, documentObject = globalThis.document, celFile = null, folder = 'missiles') {
  const decoded = createRenderCache(MISSILE_DECODE_CACHE_ENTRIES, MISSILE_DECODE_CACHE_BYTES, (entry) => entry.retainedBytes);
  const rasters = createRenderCache(MISSILE_RASTER_CACHE_ENTRIES, MISSILE_RASTER_CACHE_BYTES,
    (canvas) => canvas.width * canvas.height * 4);
  const palettes = new Map(), blenders = new Map();
  const palette = (act) => {
    const act0 = Math.min(4, Math.max(0, Math.trunc(Number(act) || 1) - 1));
    let entry = palettes.get(act0);
    if (!entry) {
      entry = [act0, decodeDatPalette(read(`data\\global\\palette\\${PALETTE[act0]}\\pal.dat`))];
      palettes.set(act0, entry);
    }
    return entry;
  };
  const blender = (act0, pal) => {
    if (!blenders.has(act0)) {
      let value = null;
      try { value = createBlender(read(`data\\global\\palette\\${PALETTE[act0]}\\pal.pl2`), pal); } catch { /* opaque draw if the palette tables are unavailable */ }
      blenders.set(act0, value);
    }
    return blenders.get(act0);
  };
  return { draw(ctx, x, y, missile, direction = 0, tick = 0, frame = null, trans = null, act = 1, hovered = false) {
    // A supplied table resolver is authoritative, including a failed resolution. Native
    // FUN_6FB1D920 leaves the cel context unset for an invalid row; it never substitutes Javelin.
    const cel = celFile ? celFile(missile?.classId) : MISSILE_CEL[missile?.classId];
    if (!cel) return null;
    const dkey = folder === 'missiles' ? cel : `${folder}:${cel}`;
    let cached = decoded.get(dkey);
    if (!cached) {
      try {
        const bytes = read(`data\\global\\${folder}\\${cel}.dcc`), dcc = decodeDcc(bytes);
        cached = { dcc, retainedBytes: decodedMissileBytes(dcc, bytes.byteLength) };
        decoded.set(dkey, cached);
      } catch { cached = null; }
    }
    const dcc = cached?.dcc;
    if (!dcc) return null;
    const celFile_ = cel;
    const dir = dccDirection(direction, dcc.directions);
    const record = dcc.directionRecords[dir], count = record.frames.length;
    const frameIndex = frame == null ? Math.max(0, Math.trunc(tick)) % count
      : ((Math.trunc(frame) % count) + count) % count;
    const drawnFrame = record.frames[frameIndex];
    const [act0, pal] = palette(act), blend = blender(act0, pal);
    const mode = missileTransDrawMode(trans, hovered);
    let drawPal = pal;
    if (mode === 7) { // same index remap the unit/item renderers use for DrawMode 7
      const hmap = highlightMap(read('data\\global\\palette\\units\\pal.dat'));
      drawPal = new Uint8Array(1024); for (let k = 0; k < 256; k++) drawPal.set(pal.subarray(hmap[k] * 4, hmap[k] * 4 + 4), k * 4);
    }
    const key = `${folder === 'missiles' ? '' : folder + ':'}${celFile_}:${dir}:${frameIndex}:${act0}:${mode === 7 ? 7 : 0}`; let canvas = rasters.get(key);
    if (!canvas) { canvas = documentObject.createElement('canvas'); canvas.width = drawnFrame.width; canvas.height = drawnFrame.height;
      canvas.getContext('2d').putImageData(new ImageData(indexedToRgba(drawnFrame.indexed, drawnFrame.opaque, drawPal),drawnFrame.width,drawnFrame.height),0,0);rasters.set(key,canvas); }
    const left = Math.round(x + drawnFrame.xOffset), top = Math.round(y + (drawnFrame.bottomUp ? drawnFrame.yOffset : drawnFrame.yOffset - drawnFrame.height + 1));
    if (blend && mode !== 5 && mode !== 7) blend(ctx, { width: drawnFrame.width, height: drawnFrame.height, indices: drawnFrame.indexed, opaque: drawnFrame.opaque }, left, top, mode);
    else ctx.drawImage(canvas, left, top);
    const { pixelHash, opaque } = spriteMetrics(drawnFrame, drawnFrame.indexed, drawnFrame.opaque);
    return {classId:missile.classId,celFile:celFile_,palette:PALETTE[act0],direction:direction,directionIndex:dir,directions:dcc.directions,
      drawMode:mode,frame:frameIndex,frames:count,framePrecise:frame == null ? null : Math.trunc(frame),trans,width:drawnFrame.width,
      height:drawnFrame.height,opaque,pixelHash};
  }, get decoded() { return decoded.size; }, get rasters() { return rasters.size; }};
}
