import { decodeDatPalette } from './d2-formats.mjs';
import { decodeDcc } from './unit-formats.mjs';
import { createBlender } from './frontend.mjs';
import { dccDirection, indexedToRgba } from './unit-renderer.mjs';

const MISSILE_CEL = Object.freeze({ 1: 'Javelin' });

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

// Retail Missiles.txt supplies CelFile: `celFile(classId)` is the engine's table (d2_missile_celfile);
// without it only the Throw slice's Javelin is known.  `frame` is the engine's own sequence counter
// (framePrecise >> 8) when the caller has one, and falls back to `tick` for a caller that does not.
// `direction` is the unit's 64-slot facing and is resolved through the DCC's own direction count, the
// same step a unit's DCC goes through (`dccDirection`); passing it straight in as an index was wrong
// for any DCC with fewer than 64 directions.
// F20: nTrans 1 draws in mode 3 (retail: units-shaman-bolt-anim.txt, ShamanFireball nTrans 1, 368 of 368
// records mode=3). nTrans 0 and 2 in flight are uncaptured and stay opaque.
export function createMissileRenderer(read, documentObject = globalThis.document, celFile = null) {
  const palette = decodeDatPalette(read('data\\global\\palette\\units\\pal.dat'));
  const decoded = new Map(), rasters = new Map();
  let blend = null;
  for (const dir of ['units', 'ACT1']) { try { blend = createBlender(read(`data\\global\\palette\\${dir}\\pal.pl2`), palette); break; } catch { /* next */ } }
  return { draw(ctx, x, y, missile, direction = 0, tick = 0, frame = null, trans = null) {
    const cel = (celFile && celFile(missile?.classId)) || MISSILE_CEL[missile?.classId];
    if (!cel) return null;
    let dcc = decoded.get(cel);
    if (dcc === undefined) { try { dcc = decodeDcc(read(`data\\global\\missiles\\${cel}.dcc`)); } catch { dcc = null; } decoded.set(cel, dcc); }
    if (!dcc) return null;
    const celFile_ = cel;
    const dir = dccDirection(direction, dcc.directions);
    const record = dcc.directionRecords[dir], count = record.frames.length;
    const frameIndex = frame == null ? Math.max(0, Math.trunc(tick)) % count
      : ((Math.trunc(frame) % count) + count) % count;
    const drawnFrame = record.frames[frameIndex];
    const key = `${celFile_}:${dir}:${frameIndex}`; let canvas = rasters.get(key);
    if (!canvas) { canvas = documentObject.createElement('canvas'); canvas.width = drawnFrame.width; canvas.height = drawnFrame.height;
      canvas.getContext('2d').putImageData(new ImageData(indexedToRgba(drawnFrame.indexed, drawnFrame.opaque, palette),drawnFrame.width,drawnFrame.height),0,0);rasters.set(key,canvas); }
    const left = Math.round(x + drawnFrame.xOffset), top = Math.round(y + (drawnFrame.bottomUp ? drawnFrame.yOffset : drawnFrame.yOffset - drawnFrame.height + 1));
    const mode = trans === 1 && blend ? 3 : 0;
    if (mode) blend(ctx, { width: drawnFrame.width, height: drawnFrame.height, indices: drawnFrame.indexed, opaque: drawnFrame.opaque }, left, top, mode);
    else ctx.drawImage(canvas, left, top);
    let pixelHash=0x811c9dc5,opaque=0;for(let i=0;i<drawnFrame.indexed.length;i++){pixelHash^=drawnFrame.indexed[i];pixelHash=Math.imul(pixelHash,0x01000193);opaque+=drawnFrame.opaque[i];}
    return {classId:missile.classId,celFile:celFile_,direction:direction,directionIndex:dir,directions:dcc.directions,
      drawMode:mode,frame:frameIndex,frames:count,framePrecise:frame == null ? null : Math.trunc(frame),trans,width:drawnFrame.width,
      height:drawnFrame.height,opaque,pixelHash:pixelHash>>>0};
  }};
}
