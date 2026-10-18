import { dc6FrameToRgba, decodeDatPalette, decodeDc6 } from './d2-formats.mjs';
import { spriteMetrics } from './sprite-metrics.mjs';
import RETAIL_MIX_HEX from './retail-mixpalette.mjs';
import { highlightMap } from './unit-highlight.mjs';

const RECORD_SIZE = 112;

// The string-table key for an item is its Items.txt name, not its code: gold's code is " dlg" but
// its name key is "gld" (string 2215, "Gold"). Every other shipped row has name == code.
export const itemStringKey = (code) => (code === 'dlg' ? 'gld' : code);

// D2Client 0x6CC00 (sub_6CC00) buckets a ground item's STAT_GOLD (D2Common #10973(unit, 0x0E, 0))
// into the draw class it hands the item blit: <100 -> 0, <500 -> 1, <5000 -> 2, else 3. flpgld.DC6
// (gold's flippy) is the only 4-frame item flippy; those four frames are the four pile sizes.
export function goldPileFrame(amount, frameCount) {
  const bucket = amount < 100 ? 0 : amount < 500 ? 1 : amount < 5000 ? 2 : 3;
  return Math.min(frameCount - 1, bucket);
}

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
    colourByte: 0xFF, id: view.getUint32(at, true), classId: view.getUint32(at + 4, true), code: view.getUint32(at + 8, true), mode: view.getUint32(at + 12, true),
    x: view.getInt32(at + 16, true), y: view.getInt32(at + 20, true), quality: view.getUint32(at + 24, true), itemFlags: view.getUint32(at + 28, true),
    framePrecise: view.getUint32(at + 32, true), frameCountPrecise: view.getUint32(at + 36, true),
    flippyFile: cstring(raw, at + 40, 32), invFile: cstring(raw, at + 72, 32),
    gridX: view.getInt16(at + 104, true), gridY: view.getInt16(at + 106, true), invPage: view.getUint8(at + 108), bodyLoc: view.getUint8(at + 109),
    // ItemsTxt nInvWidth/nInvHeight (the engine fills the record's last two bytes): cells covered.
    invWidth: view.getUint8(at + 110) || 1, invHeight: view.getUint8(at + 111) || 1,
  }; });
}

// D2Client draws a ground flippy through the item's own ITEMS_GetColor remap (the engine exports its
// component colour byte per item, 0xFF = none): entry k of the palette becomes entry table[k].
// Retail's 126 captured MixPalette tables are keyed by that byte; an unknown byte stays unmapped.
export function itemColourPalette(palette, colourByte) {
  const hex = RETAIL_MIX_HEX[colourByte]; if (!hex) return palette;
  const out = new Uint8Array(1024);
  for (let k = 0; k < 256; k++) { const t = parseInt(hex.substr(k * 2, 2), 16); out.set(palette.subarray(t * 4, t * 4 + 4), k * 4); }
  return out;
}

export function createItemRenderer(read, documentObject = globalThis.document) {
  const decoded = new Map(), rasters = new Map(), failures = new Set();
  const palette = decodeDatPalette(read('data\\global\\palette\\units\\pal.dat'));
  let highlightPalette = null, lastEvidence = null;
  // Retro DrawMode 7 (HIGHLIGHT): D2Client selects the mode-7 table of the active palette for the
  // hovered unit, the same index remap monster-renderer.mjs applies (unit-highlight.mjs, V5). The
  // item's ITEMS_GetColor remap is applied at cel decode, so it composes FIRST and the highlight
  // table then selects palette entries: index -> colour table -> mode-7 entry -> RGB.
  function highlightedPalette() {
    if (!highlightPalette) {
      const hmap = highlightMap(read('data\\global\\palette\\units\\pal.dat'));
      highlightPalette = new Uint8Array(1024);
      for (let k = 0; k < 256; k++) highlightPalette.set(palette.subarray(hmap[k] * 4, hmap[k] * 4 + 4), k * 4);
    }
    return highlightPalette;
  }
  // The cel the item is DRAWN from. The blit and the hover hit test both resolve the frame here, so
  // the rectangle D2Client's FUN_6FB56730 (0xA6730) builds comes from the same cel as the sprite.
  function frameFor(item) {
    const name = item.flippyFile?.trim(); if (!name || failures.has(name)) return null;
    let dc6 = decoded.get(name);
    if (!dc6) try { dc6 = decodeDc6(read(`data\\global\\items\\${name}.dc6`)); decoded.set(name, dc6); }
      catch { failures.add(name); return null; }
    const index = dc6.frames.length > 1 && Number.isFinite(item.goldAmount) && item.goldAmount >= 0
      ? goldPileFrame(item.goldAmount, dc6.frames.length)
      : Math.min(dc6.frames.length - 1, Math.max(0, (item.framePrecise >>> 8) % dc6.frames.length));
    return { name, dc6, index, frame: dc6.frames[index] };
  }
  return { draw(ctx, x, y, item, highlight = false) {
    const resolved = frameFor(item); if (!resolved) return null;
    const name = resolved.name, frameIndex = resolved.index, frame = resolved.frame;
    const colour = item.colourByte ?? 0xFF, key = `${name}:${frameIndex}:${colour}${highlight ? ':hl' : ''}`; let canvas = rasters.get(key);
    if (!canvas) { canvas = documentObject.createElement('canvas'); canvas.width = frame.width; canvas.height = frame.height;
      if (frame.width && frame.height) canvas.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, itemColourPalette(highlight ? highlightedPalette() : palette, colour)), frame.width, frame.height), 0, 0); rasters.set(key, canvas); }
    if (canvas.width && canvas.height) ctx.drawImage(canvas, Math.round(x + frame.offsetX), Math.round(y + frame.offsetY - frame.height));
    const { pixelHash, opaque } = spriteMetrics(frame, frame.indices, frame.opaque);
    lastEvidence = { id: item.id, classId: item.classId, flippyFile: name, frame: frameIndex, width: frame.width, height: frame.height, opaque, pixelHash, mode: highlight ? 7 : 5 };
    return lastEvidence;
  },
  // The current frame's cel box in the convention D2Client's rect builder reads: the blit origin is
  // (x + offsetX, y + offsetY - height), so `yOffset` here is the box's TOP in screen space (y down)
  // and the sprite occupies [yOffset, yOffset + height). nativeDccHoverContains pads this box.
  measure(item) {
    const resolved = frameFor(item); if (!resolved) return null;
    const frame = resolved.frame;
    return { flippyFile: resolved.name, frame: resolved.index, xOffset: frame.offsetX,
      yOffset: frame.offsetY - frame.height, width: frame.width, height: frame.height };
  },
  get lastEvidence() { return lastEvidence && { ...lastEvidence }; }, get decoded() { return decoded.size; }, get failures() { return failures.size; } };
}
