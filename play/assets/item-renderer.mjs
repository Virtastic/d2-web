import { dc6FrameToRgba, decodeDatPalette, decodeDc6 } from './d2-formats.mjs';
import { spriteMetrics } from './sprite-metrics.mjs';

const RECORD_SIZE = 112;

function cstring(bytes, start, size) {
  let text = '';
  for (let at = start; at < start + size && bytes[at]; at++) text += String.fromCharCode(bytes[at]);
  return text;
}

export function parseItemPresentations(bytes, count) {
  const raw = bytes instanceof Uint8Array ? bytes : new Uint8Array(bytes.buffer, bytes.byteOffset || 0, bytes.byteLength);
  if (!Number.isInteger(count) || count < 0 || raw.byteLength < count * RECORD_SIZE) throw new RangeError('truncated item presentation records');
  const view = new DataView(raw.buffer, raw.byteOffset, raw.byteLength);
  return Array.from({ length: count }, (_, row) => { const at = row * RECORD_SIZE; return {
    id: view.getUint32(at, true), classId: view.getUint32(at + 4, true), code: view.getUint32(at + 8, true), mode: view.getUint32(at + 12, true),
    x: view.getInt32(at + 16, true), y: view.getInt32(at + 20, true), quality: view.getUint32(at + 24, true), itemFlags: view.getUint32(at + 28, true),
    framePrecise: view.getUint32(at + 32, true), frameCountPrecise: view.getUint32(at + 36, true),
    flippyFile: cstring(raw, at + 40, 32), invFile: cstring(raw, at + 72, 32),
    gridX: view.getInt16(at + 104, true), gridY: view.getInt16(at + 106, true), invPage: view.getUint8(at + 108), bodyLoc: view.getUint8(at + 109),
    // ItemsTxt nInvWidth/nInvHeight (the engine fills the record's last two bytes): cells covered.
    invWidth: view.getUint8(at + 110) || 1, invHeight: view.getUint8(at + 111) || 1,
  }; });
}

export function createItemRenderer(read, documentObject = globalThis.document) {
  const decoded = new Map(), rasters = new Map(), failures = new Set();
  const palette = decodeDatPalette(read('data\\global\\palette\\units\\pal.dat'));
  let lastEvidence = null;
  return { draw(ctx, x, y, item) {
    const name = item.flippyFile?.trim(); if (!name || failures.has(name)) return null;
    let dc6 = decoded.get(name);
    if (!dc6) try { dc6 = decodeDc6(read(`data\\global\\items\\${name}.dc6`)); decoded.set(name, dc6); }
      catch { failures.add(name); return null; }
    const frameIndex = Math.min(dc6.frames.length - 1, Math.max(0, (item.framePrecise >>> 8) % dc6.frames.length));
    const key = `${name}:${frameIndex}`; let canvas = rasters.get(key); const frame = dc6.frames[frameIndex];
    if (!canvas) { canvas = documentObject.createElement('canvas'); canvas.width = frame.width; canvas.height = frame.height;
      if (frame.width && frame.height) canvas.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0); rasters.set(key, canvas); }
    if (canvas.width && canvas.height) ctx.drawImage(canvas, Math.round(x + frame.offsetX), Math.round(y + frame.offsetY - frame.height));
    const { pixelHash, opaque } = spriteMetrics(frame, frame.indices, frame.opaque);
    lastEvidence = { id: item.id, classId: item.classId, flippyFile: name, frame: frameIndex, width: frame.width, height: frame.height, opaque, pixelHash };
    return lastEvidence;
  }, get lastEvidence() { return lastEvidence && { ...lastEvidence }; }, get decoded() { return decoded.size; }, get failures() { return failures.size; } };
}
