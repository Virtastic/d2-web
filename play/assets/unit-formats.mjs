import { Bits } from './dcc-bits.mjs';

// COF and DCC layout is re-derived from the documented formats and cross-checked
// against OpenDiablo2/{cof,dcc} (Apache-2.0). Retail files remain the oracle.

function bytesOf(input, kind) {
  if (input instanceof Uint8Array) return input;
  if (input instanceof ArrayBuffer) return new Uint8Array(input);
  if (ArrayBuffer.isView(input)) return new Uint8Array(input.buffer, input.byteOffset, input.byteLength);
  throw new TypeError(`${kind} input must be bytes`);
}

export function decodeCof(input) {
  const bytes = bytesOf(input, 'COF');
  if (bytes.length < 28) throw new RangeError('truncated COF header');
  const layers = bytes[0], framesPerDirection = bytes[1], directions = bytes[2], speed = bytes[24];
  if (!layers || !framesPerDirection || !directions) {
    throw new RangeError(`invalid COF dimensions ${layers}x${framesPerDirection}x${directions}`);
  }
  const expected = 28 + layers * 9 + framesPerDirection + layers * framesPerDirection * directions;
  if (bytes.length !== expected) throw new RangeError(`COF size ${bytes.length} does not match layout size ${expected}`);
  let at = 28;
  const layerRecords = [];
  for (let i = 0; i < layers; i++, at += 9) {
    const nul = bytes.subarray(at + 5, at + 9).indexOf(0);
    const end = nul < 0 ? at + 9 : at + 5 + nul;
    const weaponClass = String.fromCharCode(...bytes.subarray(at + 5, end));
    if (!/^[\x20-\x7e]{0,4}$/.test(weaponClass)) throw new RangeError(`invalid COF layer ${i} weapon class`);
    layerRecords.push({
      component: bytes[at], shadow: bytes[at + 1], selectable: bytes[at + 2] !== 0,
      transparent: bytes[at + 3] !== 0, drawEffect: bytes[at + 4], weaponClass,
    });
  }
  const events = bytes.slice(at, at += framesPerDirection);
  const priority = [];
  for (let direction = 0; direction < directions; direction++) {
    const frames = [];
    for (let frame = 0; frame < framesPerDirection; frame++) {
      const order = Array.from(bytes.subarray(at, at += layers));
      for (const component of order) if (!layerRecords.some((v) => v.component === component)) {
        throw new RangeError(`COF priority references absent component ${component}`);
      }
      frames.push(order);
    }
    priority.push(frames);
  }
  return { layers, framesPerDirection, directions, speed, layerRecords, events, priority };
}

const DCC_WIDTHS = [0, 1, 2, 4, 6, 8, 10, 12, 14, 16, 20, 24, 26, 28, 30, 32];


function frameCells(frame, box) {
  const left = frame.xOffset, top = frame.bottomUp ? frame.yOffset : frame.yOffset - frame.height + 1;
  const relX = left - box.minX, relY = top - box.minY;
  const partition = (size, relative) => {
    const first = 4 - (relative & 3);
    if (size - first <= 1) return [size];
    const remainder = size - first, tail = remainder & 3;
    if (tail === 0) return [first, ...Array(remainder >>> 2).fill(4)];
    if (tail === 1) return [first, ...Array((remainder >>> 2) - 1).fill(4), 5];
    return [first, ...Array(remainder >>> 2).fill(4), tail];
  };
  const widths = partition(frame.width, relX), heights = partition(frame.height, relY);
  const across = widths.length, down = heights.length;
  const cells = [];
  let y = relY;
  for (const height of heights) {
    let x = relX;
    for (const width of widths) { cells.push({ x, y, width, height }); x += width; }
    y += height;
  }
  return { across, down, cells };
}

function decodeDccPixels(bytes, direction, directionEnd, index) {
  const start = direction.bitstreamStart;
  let cursor = start;
  const stream = (count) => { const out = new Bits(bytes, cursor, cursor + count); cursor += count; return out; };
  const equal = stream(direction.equalCellsBits), masks = stream(direction.pixelMaskBits);
  const encodings = stream(direction.encodingTypeBits), raw = stream(direction.rawPixelCodesBits);
  const codes = new Bits(bytes, cursor, directionEnd * 8);
  const width = direction.box.maxX - direction.box.minX, height = direction.box.maxY - direction.box.minY;
  const cellsAcross = 1 + Math.trunc((width - 1) / 4), cellsDown = 1 + Math.trunc((height - 1) / 4);
  const state = Array(cellsAcross * cellsDown).fill(null);
  const entries = [];
  const bitCounts = [0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4];
  direction.frames.forEach((frame, frameIndex) => {
    const layout = frameCells(frame, direction.box); frame.cellsAcross = layout.across; frame.cellsDown = layout.down;
    const originX = Math.trunc((frame.xOffset - direction.box.minX) / 4);
    const top = frame.bottomUp ? frame.yOffset : frame.yOffset - frame.height + 1;
    const originY = Math.trunc((top - direction.box.minY) / 4);
    layout.cells.forEach((cell, frameCellIndex) => {
      const cx = frameCellIndex % layout.across, cy = Math.trunc(frameCellIndex / layout.across);
      const stateIndex = originX + cx + (originY + cy) * cellsAcross;
      if (stateIndex < 0 || stateIndex >= state.length) throw new RangeError(`DCC direction ${index} cell outside bounds`);
      const old = state[stateIndex];
      if (old && direction.equalCellsBits && equal.read(1, `direction ${index} equal-cell stream`)) return;
      const mask = old ? masks.read(4, `direction ${index} pixel-mask stream`) : 15;
      const changed = bitCounts[mask];
      const rawEncoding = changed && direction.encodingTypeBits ? encodings.read(1, `direction ${index} encoding stream`) : 0;
      const stack = []; let last = 0;
      for (let n = 0; n < changed; n++) {
        let value;
        if (rawEncoding) value = raw.read(8, `direction ${index} raw-pixel stream frame ${frameIndex} cell ${frameCellIndex} value ${n}`);
        else {
          value = last;
          let displacement;
          do { displacement = codes.read(4, `direction ${index} pixel-code stream`); value += displacement; }
          while (displacement === 15);
        }
        if (value === last) break;
        stack.push(value); last = value;
      }
      // A kept cell carries its palette-map INDICES, mapped once below (OpenDiablo2 dcc_direction.go
      // 395/407-409); carrying the mapped colours mapped them twice and scrambled attack frames (F15).
      const values = old ? old.raw.slice() : [0, 0, 0, 0];
      let stackAt = stack.length - 1;
      for (let n = 0; n < 4; n++) if (mask & (1 << n)) values[n] = stackAt >= 0 ? stack[stackAt--] : 0;
      const entry = { raw: values, values: values.map((v) => direction.paletteEntries[v] ?? 0), frameIndex, frameCellIndex };
      entries.push(entry); state[stateIndex] = entry;
    });
  });
  for (const [reader, expected, name] of [[equal, direction.equalCellsBits, 'equal-cell'], [masks, direction.pixelMaskBits, 'pixel-mask'],
    [encodings, direction.encodingTypeBits, 'encoding'], [raw, direction.rawPixelCodesBits, 'raw-pixel']]) {
    if (reader.bit !== reader.limit) throw new RangeError(`DCC direction ${index} ${name} stream used ${reader.bit - (reader.limit - expected)}/${expected} bits`);
  }

  const canvas = new Uint8Array(width * height), frameOutputs = [];
  const lastCells = Array(cellsAcross * cellsDown).fill(null); let entryAt = 0;
  direction.frames.forEach((frame, frameIndex) => {
    const layout = frameCells(frame, direction.box);
    for (let frameCellIndex = 0; frameCellIndex < layout.cells.length; frameCellIndex++) {
      const cell = layout.cells[frameCellIndex];
      let entry = entries[entryAt];
      if (entry?.frameIndex === frameIndex && entry.frameCellIndex === frameCellIndex) entryAt++;
      else entry = null;
      const sx = Math.trunc(cell.x / 4), sy = Math.trunc(cell.y / 4), stateIndex = sx + sy * cellsAcross;
      const previous = lastCells[stateIndex];
      if (!entry) {
        if (!previous || previous.width !== cell.width || previous.height !== cell.height) {
          for (let y = 0; y < cell.height; y++) canvas.fill(0, (cell.y + y) * width + cell.x, (cell.y + y) * width + cell.x + cell.width);
        } else {
          const copy = [];
          for (let y = 0; y < cell.height; y++) copy.push(canvas.slice((previous.y + y) * width + previous.x, (previous.y + y) * width + previous.x + cell.width));
          copy.forEach((row, y) => canvas.set(row, (cell.y + y) * width + cell.x));
        }
      } else if (entry.values[0] === entry.values[1]) {
        for (let y = 0; y < cell.height; y++) canvas.fill(entry.values[0], (cell.y + y) * width + cell.x, (cell.y + y) * width + cell.x + cell.width);
      } else {
        const bits = entry.values[1] === entry.values[2] ? 1 : 2;
        for (let y = 0; y < cell.height; y++) for (let x = 0; x < cell.width; x++) {
          canvas[(cell.y + y) * width + cell.x + x] = entry.values[codes.read(bits, `direction ${index} frame pixels`)];
        }
      }
      lastCells[stateIndex] = { ...cell };
    }
    const indexed = new Uint8Array(frame.width * frame.height), opaque = new Uint8Array(indexed.length);
    const left = frame.xOffset - direction.box.minX;
    const top = (frame.bottomUp ? frame.yOffset : frame.yOffset - frame.height + 1) - direction.box.minY;
    for (let y = 0; y < frame.height; y++) for (let x = 0; x < frame.width; x++) {
      const value = canvas[(top + y) * width + left + x], at = y * frame.width + x;
      indexed[at] = value; opaque[at] = value === 0 ? 0 : 1;
    }
    frame.indexed = indexed; frame.opaque = opaque; frameOutputs.push(frame);
  });
  direction.pixelCodeBits = codes.bit - cursor;
  return frameOutputs;
}

function decodeDccDirection(bytes, offset, directionEnd, framesPerDirection, index) {
  const r = new Bits(bytes, offset * 8, directionEnd * 8);
  const outSizeCoded = r.read(32, `direction ${index} header`);
  const compressionFlags = r.read(2, `direction ${index} flags`);
  const widths = Array.from({ length: 7 }, (_, n) => DCC_WIDTHS[r.read(4, `direction ${index} width ${n}`)]);
  const [variable0Bits, widthBits, heightBits, xOffsetBits, yOffsetBits, optionalDataBits, codedBytesBits] = widths;
  const frames = [];
  let minX = Infinity, minY = Infinity, maxX = -Infinity, maxY = -Infinity;
  for (let frame = 0; frame < framesPerDirection; frame++) {
    const variable0 = r.read(variable0Bits, `direction ${index} frame ${frame} variable0`);
    const width = r.read(widthBits, `direction ${index} frame ${frame} width`);
    const height = r.read(heightBits, `direction ${index} frame ${frame} height`);
    const xOffset = r.signed(xOffsetBits, `direction ${index} frame ${frame} x offset`);
    const yOffset = r.signed(yOffsetBits, `direction ${index} frame ${frame} y offset`);
    const optionalBytes = r.read(optionalDataBits, `direction ${index} frame ${frame} optional bytes`);
    const codedBytes = r.read(codedBytesBits, `direction ${index} frame ${frame} coded bytes`);
    const bottomUp = r.read(1, `direction ${index} frame ${frame} orientation`) !== 0;
    if (!width || !height) throw new RangeError(`invalid DCC direction ${index} frame ${frame} dimensions ${width}x${height}`);
    const left = xOffset, top = bottomUp ? yOffset : yOffset - height + 1;
    minX = Math.min(minX, left); minY = Math.min(minY, top);
    maxX = Math.max(maxX, left + width); maxY = Math.max(maxY, top + height);
    frames.push({ variable0, width, height, xOffset, yOffset, optionalBytes, codedBytes, bottomUp });
  }
  const optionalBytesTotal = frames.reduce((sum, frame) => sum + frame.optionalBytes, 0);
  if (optionalBytesTotal) {
    r.bit = (r.bit + 7) & ~7;
    for (const [frameIndex, frame] of frames.entries()) {
      if (frame.optionalBytes) {
        const data = new Uint8Array(frame.optionalBytes);
        for (let i = 0; i < data.length; i++) data[i] = r.read(8, `direction ${index} frame ${frameIndex} optional data`);
        frame.optionalData = data;
      }
    }
  }
  let equalCellsBits = 0, encodingTypeBits = 0, rawPixelCodesBits = 0;
  if (compressionFlags & 2) equalCellsBits = r.read(20, `direction ${index} equal-cell stream size`);
  const pixelMaskBits = r.read(20, `direction ${index} pixel-mask stream size`);
  if (compressionFlags & 1) {
    encodingTypeBits = r.read(20, `direction ${index} encoding-type stream size`);
    rawPixelCodesBits = r.read(20, `direction ${index} raw-pixel stream size`);
  }
  const paletteEntries = [];
  for (let palette = 0; palette < 256; palette++) if (r.read(1, `direction ${index} palette bitmap`)) paletteEntries.push(palette);
  const streamEnd = r.bit + equalCellsBits + pixelMaskBits + encodingTypeBits + rawPixelCodesBits;
  if (streamEnd > bytes.length * 8) throw new RangeError(`DCC direction ${index} substreams exceed file`);
  const direction = { offset, outSizeCoded, compressionFlags, widths, frames, box: { minX, minY, maxX, maxY },
    equalCellsBits, pixelMaskBits, encodingTypeBits, rawPixelCodesBits, paletteEntries, bitstreamStart: r.bit };
  decodeDccPixels(bytes, direction, directionEnd, index);
  return direction;
}

/** Strictly decode DCC container, direction metadata, frame headers, palette map and stream bounds. */
export function decodeDcc(input, options = {}) {
  const bytes = bytesOf(input, 'DCC');
  if (bytes.length < 19) throw new RangeError('truncated DCC header');
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  if (bytes[0] !== 0x74) throw new RangeError(`invalid DCC signature 0x${bytes[0].toString(16)}`);
  const version = bytes[1], directions = bytes[2], framesPerDirection = view.getUint32(3, true);
  const sanity = view.getUint32(7, true), totalSizeCoded = view.getUint32(11, true);
  if (sanity !== 1) throw new RangeError(`invalid DCC sanity value ${sanity}`);
  if (!directions || !framesPerDirection || directions > 64 || framesPerDirection > 256) {
    throw new RangeError(`invalid DCC frame grid ${directions}x${framesPerDirection}`);
  }
  if (bytes.length < 15 + directions * 4) throw new RangeError('truncated DCC direction table');
  const offsets = Array.from({ length: directions }, (_, i) => view.getUint32(15 + i * 4, true));
  const tableEnd = 15 + directions * 4;
  for (let i = 0; i < offsets.length; i++) {
    if (offsets[i] < tableEnd || offsets[i] >= bytes.length) throw new RangeError(`invalid DCC direction ${i} offset ${offsets[i]}`);
    if (i && offsets[i] <= offsets[i - 1]) throw new RangeError('DCC direction offsets are not strictly increasing');
  }
  const requested = options.directions == null ? null : new Set(options.directions);
  if (requested) for (const direction of requested) if (!Number.isInteger(direction) || direction < 0 || direction >= directions) {
    throw new RangeError(`requested DCC direction ${direction} is outside 0..${directions - 1}`);
  }
  return { version, directions, framesPerDirection, totalSizeCoded,
    directionRecords: offsets.map((offset, i) => !requested || requested.has(i)
      ? decodeDccDirection(bytes, offset, offsets[i + 1] ?? bytes.length, framesPerDirection, i) : null) };
}

// PERF (F198): a composition is a pure function of the COF, the direction, the frame and WHICH decoded components fill the layers, and the
// renderers asked for it again on every frame (monster, its shadow, objects). Kept per COF; callers only read the result.
const composeMemo = new WeakMap(), dccIds = new WeakMap();
let nextDccId = 1;
const dccId = (dcc) => { let n = dccIds.get(dcc); if (!n) dccIds.set(dcc, n = nextDccId++); return n; };

/** Compose available DCC component frames in the draw order selected by a COF. */
export function composeCofFrame(cof, direction, frameIndex, components, dccDirection = direction) {
  if (cof && cof.priority?.[direction]?.[frameIndex]) {
    const get = components instanceof Map ? (id) => components.get(id) : (id) => components?.[id];
    const directionKey = dccDirection instanceof Map
      ? [...dccDirection].sort(([a], [b]) => a - b).map(([component, value]) => `${component}.${value}`).join(',')
      : dccDirection;
    let key = `${direction}:${directionKey}:${frameIndex}`, ok = true;
    for (const component of cof.priority[direction][frameIndex]) { const dcc = get(component); if (dcc) { if (typeof dcc !== 'object') { ok = false; break; } key += `,${component}=${dccId(dcc)}`; } }
    if (ok) {
      let memo = composeMemo.get(cof); if (!memo) composeMemo.set(cof, memo = new Map());
      let hit = memo.get(key); if (!hit) memo.set(key, hit = composeCofFrameUncached(cof, direction, frameIndex, components, dccDirection));
      return hit;
    }
  }
  return composeCofFrameUncached(cof, direction, frameIndex, components, dccDirection);
}

function composeCofFrameUncached(cof, direction, frameIndex, components, dccDirection) {
  if (!cof || direction < 0 || direction >= cof.directions || frameIndex < 0 || frameIndex >= cof.framesPerDirection) {
    throw new RangeError('COF composition frame is outside the animation grid');
  }
  const get = components instanceof Map ? (id) => components.get(id) : (id) => components?.[id];
  const selected = [];
  for (const component of cof.priority[direction][frameIndex]) {
    const dcc = get(component); if (!dcc) continue;
    // A component whose grid is short of the COF's holds its last cel for the frames it does not carry.
    // F153: the Frozen Horror (`f0`) death COF f0DTHTH is 1x14 and AnimData.d2's own F0DTHTH record is
    // 14 too (both read off the retail bytes, s716), but the shipped f0TRlitDThth.DCC carries 13 -- so
    // retail's own 14th frame has no cel. Drawing the death's last pose twice shows a corpse; wrapping
    // would snap it back to the standing frame 0. Clamp, for either axis: the same "does not cover" case.
    // Native D2CMP resolves the 64-slot facing against each DCC's own direction
    // count. Player COFs can be 16-direction while a particular layer DCC is 8.
    // Callers may pass a per-component map; the scalar form remains for objects
    // and callers whose layers share one DCC direction index.
    const requestedDirection = dccDirection instanceof Map
      ? dccDirection.get(component) ?? 0 : dccDirection;
    const dir = Math.min(requestedDirection, dcc.directions - 1), frame = Math.min(frameIndex, dcc.framesPerDirection - 1);
    selected.push({ component, frame: dcc.directionRecords[dir].frames[frame] });
  }
  if (!selected.length) return { xOffset: 0, yOffset: 0, width: 0, height: 0, bottomUp: false, indexed: new Uint8Array(), opaque: new Uint8Array() };
  const left = Math.min(...selected.map(({ frame }) => frame.xOffset));
  const top = Math.min(...selected.map(({ frame }) => frame.bottomUp ? frame.yOffset : frame.yOffset - frame.height + 1));
  const right = Math.max(...selected.map(({ frame }) => frame.xOffset + frame.width));
  const bottom = Math.max(...selected.map(({ frame }) => (frame.bottomUp ? frame.yOffset : frame.yOffset - frame.height + 1) + frame.height));
  const width = right - left, height = bottom - top;
  if (width * height > 16_777_216) throw new RangeError('COF composition dimensions are too large');
  const indexed = new Uint8Array(width * height), opaque = new Uint8Array(width * height);
  for (const { frame } of selected) {
    const x0 = frame.xOffset - left, y0 = (frame.bottomUp ? frame.yOffset : frame.yOffset - frame.height + 1) - top;
    for (let y = 0; y < frame.height; y++) for (let x = 0; x < frame.width; x++) {
      const source = y * frame.width + x; if (!frame.opaque[source]) continue;
      const target = (y0 + y) * width + x0 + x; indexed[target] = frame.indexed[source]; opaque[target] = 1;
    }
  }
  return { xOffset: left, yOffset: top, width, height, bottomUp: selected.every(({ frame }) => frame.bottomUp), indexed, opaque };
}
