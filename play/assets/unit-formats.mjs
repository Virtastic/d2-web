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
  const cof = { layers, framesPerDirection, directions, speed, layerRecords, events, priority };
  const layerByComponent = new Map();
  for (const record of layerRecords) if (!layerByComponent.has(record.component)) layerByComponent.set(record.component, record);
  // Keep this renderer lookup cache out of the public decoded-data shape.
  Object.defineProperty(cof, 'layerByComponent', { value: layerByComponent });
  return cof;
}

/** Retrieve the first COF layer record for a component. */
export function cofLayerRecord(cof, component) {
  return cof?.layerByComponent?.get(component) ?? cof?.layerRecords?.find((record) => record.component === component);
}

const DCC_WIDTHS = [0, 1, 2, 4, 6, 8, 10, 12, 14, 16, 20, 24, 26, 28, 30, 32];

// Cell sizes along one axis: a first cell up to the next 4-pixel boundary, then 4s, with a 1-pixel tail
// folded into the last cell (5) and any other tail kept as its own cell.
function partition(size, relative) {
  const first = 4 - (relative & 3);
  if (size - first <= 1) return [size];
  const remainder = size - first, tail = remainder & 3, out = [first];
  const fours = tail === 1 ? (remainder >>> 2) - 1 : remainder >>> 2;
  for (let i = 0; i < fours; i++) out.push(4);
  if (tail === 1) out.push(5); else if (tail) out.push(tail);
  return out;
}

const BIT_COUNTS = Uint8Array.of(0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4);
const truncated = (label, bit, count, limit) => new RangeError(`truncated DCC ${label} at bit ${bit} reading ${count}, limit ${limit}`);

// PERF: the pixel decode is the client's hottest path with many players on screen. Each substream is read
// through an inline bit cursor (every read here is at most 8 bits, so a 16-bit window always holds it), the
// per-cell state lives in typed arrays instead of an object per cell, and an error label is only built when a
// stream overruns. Output is byte-identical to the object-per-cell decoder this replaced
// (tools/online/dcc-decode-bench.mjs --check-hashes, test/dcc-decode-identity.test.mjs).
function decodeDccPixels(bytes, direction, directionEnd, index) {
  const start = direction.bitstreamStart;
  const equalCellsBits = direction.equalCellsBits, encodingTypeBits = direction.encodingTypeBits;
  const equalLimit = start + equalCellsBits, maskLimit = equalLimit + direction.pixelMaskBits;
  const encodingLimit = maskLimit + encodingTypeBits, rawLimit = encodingLimit + direction.rawPixelCodesBits;
  const codesLimit = directionEnd * 8;
  let equalBit = start, maskBit = equalLimit, encodingBit = maskLimit, rawBit = encodingLimit, codeBit = rawLimit;
  const box = direction.box, frames = direction.frames, frameCount = frames.length;
  const palette = direction.paletteEntries, paletteSize = palette.length;
  const width = box.maxX - box.minX, height = box.maxY - box.minY;
  const cellsAcross = 1 + Math.trunc((width - 1) / 4), cellsDown = 1 + Math.trunc((height - 1) / 4);
  const stateCount = cellsAcross * cellsDown;

  const relXs = new Int32Array(frameCount), relYs = new Int32Array(frameCount), widthsOf = new Array(frameCount), heightsOf = new Array(frameCount);
  let totalCells = 0;
  for (let f = 0; f < frameCount; f++) {
    const frame = frames[f];
    const top = frame.bottomUp ? frame.yOffset : frame.yOffset - frame.height + 1;
    relXs[f] = frame.xOffset - box.minX; relYs[f] = top - box.minY;
    widthsOf[f] = partition(frame.width, relXs[f]); heightsOf[f] = partition(frame.height, relYs[f]);
    totalCells += widthsOf[f].length * heightsOf[f].length;
  }

  // Pass 1: which cells change and their 4-entry palette stacks. cellEntry is indexed in visit order
  // (frame, then row-major cell); entryRaw keeps palette-map INDICES, entryValue the mapped colours.
  // A kept cell carries its palette-map INDICES, mapped once (OpenDiablo2 dcc_direction.go 395/407-409);
  // carrying the mapped colours mapped them twice and scrambled attack frames (F15).
  const stateEntry = new Int32Array(stateCount).fill(-1), cellEntry = new Int32Array(totalCells);
  const entryRaw = new Int32Array(totalCells * 4), entryValue = new Uint8Array(totalCells * 4), stack = new Int32Array(4);
  let entries = 0, visit = 0;
  for (let f = 0; f < frameCount; f++) {
    const frame = frames[f], widths = widthsOf[f], heights = heightsOf[f], across = widths.length, down = heights.length;
    frame.cellsAcross = across; frame.cellsDown = down;
    const originX = Math.trunc(relXs[f] / 4), originY = Math.trunc(relYs[f] / 4);
    for (let cy = 0; cy < down; cy++) for (let cx = 0; cx < across; cx++) {
      const stateIndex = originX + cx + (originY + cy) * cellsAcross;
      if (stateIndex < 0 || stateIndex >= stateCount) throw new RangeError(`DCC direction ${index} cell outside bounds`);
      const old = stateEntry[stateIndex];
      let p;
      if (old >= 0 && equalCellsBits) {
        if (equalBit + 1 > equalLimit) throw truncated(`direction ${index} equal-cell stream`, equalBit, 1, equalLimit);
        p = equalBit >>> 3; const same = (bytes[p] >>> (equalBit & 7)) & 1; equalBit++;
        if (same) { cellEntry[visit++] = -1; continue; }
      }
      let mask = 15;
      if (old >= 0) {
        if (maskBit + 4 > maskLimit) throw truncated(`direction ${index} pixel-mask stream`, maskBit, 4, maskLimit);
        p = maskBit >>> 3; mask = ((bytes[p] | (bytes[p + 1] << 8)) >>> (maskBit & 7)) & 15; maskBit += 4;
      }
      const changed = BIT_COUNTS[mask];
      let rawEncoding = 0;
      if (changed && encodingTypeBits) {
        if (encodingBit + 1 > encodingLimit) throw truncated(`direction ${index} encoding stream`, encodingBit, 1, encodingLimit);
        p = encodingBit >>> 3; rawEncoding = (bytes[p] >>> (encodingBit & 7)) & 1; encodingBit++;
      }
      let depth = 0, last = 0;
      for (let n = 0; n < changed; n++) {
        let value;
        if (rawEncoding) {
          if (rawBit + 8 > rawLimit) {
            throw truncated(`direction ${index} raw-pixel stream frame ${f} cell ${cy * across + cx} value ${n}`, rawBit, 8, rawLimit);
          }
          p = rawBit >>> 3; value = ((bytes[p] | (bytes[p + 1] << 8)) >>> (rawBit & 7)) & 255; rawBit += 8;
        } else {
          value = last;
          let displacement;
          do {
            if (codeBit + 4 > codesLimit) throw truncated(`direction ${index} pixel-code stream`, codeBit, 4, codesLimit);
            p = codeBit >>> 3; displacement = ((bytes[p] | (bytes[p + 1] << 8)) >>> (codeBit & 7)) & 15; codeBit += 4;
            value += displacement;
          } while (displacement === 15);
        }
        if (value === last) break;
        stack[depth++] = value; last = value;
      }
      const at = entries * 4, from = old * 4;
      let stackAt = depth - 1;
      for (let n = 0; n < 4; n++) {
        let v;
        if (mask & (1 << n)) v = stackAt >= 0 ? stack[stackAt--] : 0;
        else v = old >= 0 ? entryRaw[from + n] : 0;
        entryRaw[at + n] = v; entryValue[at + n] = v < paletteSize ? palette[v] : 0;
      }
      cellEntry[visit++] = entries; stateEntry[stateIndex] = entries++;
    }
  }
  for (const [bit, limit, expected, name] of [[equalBit, equalLimit, equalCellsBits, 'equal-cell'], [maskBit, maskLimit, direction.pixelMaskBits, 'pixel-mask'],
    [encodingBit, encodingLimit, encodingTypeBits, 'encoding'], [rawBit, rawLimit, direction.rawPixelCodesBits, 'raw-pixel']]) {
    if (bit !== limit) throw new RangeError(`DCC direction ${index} ${name} stream used ${bit - (limit - expected)}/${expected} bits`);
  }

  // Pass 2: paint each frame's cells onto the direction canvas (a kept cell copies its last placement, a
  // one-colour cell fills, otherwise 1- or 2-bit indices into the cell's stack), then cut the frame out.
  const canvas = new Uint8Array(width * height), frameOutputs = [];
  const lastX = new Int32Array(stateCount), lastY = new Int32Array(stateCount), lastW = new Int32Array(stateCount), lastH = new Int32Array(stateCount);
  visit = 0;
  for (let f = 0; f < frameCount; f++) {
    const frame = frames[f], widths = widthsOf[f], heights = heightsOf[f], across = widths.length, down = heights.length;
    let y = relYs[f];
    for (let cy = 0; cy < down; cy++) {
      const ch = heights[cy];
      let x = relXs[f];
      for (let cx = 0; cx < across; cx++) {
        const cw = widths[cx], entry = cellEntry[visit++];
        const stateIndex = Math.trunc(x / 4) + Math.trunc(y / 4) * cellsAcross;
        if (entry < 0) {
          const pw = lastW[stateIndex];
          if (pw === 0 || pw !== cw || lastH[stateIndex] !== ch) {
            for (let row = 0; row < ch; row++) for (let to = (y + row) * width + x, end = to + cw; to < end; to++) canvas[to] = 0;
          } else {
            const px = lastX[stateIndex], py = lastY[stateIndex];
            for (let row = 0; row < ch; row++) {
              // a row copy with copyWithin's (memmove) semantics when the old and new placements overlap
              const source = (py + row) * width + px, to = (y + row) * width + x;
              if (to > source) for (let i = cw - 1; i >= 0; i--) canvas[to + i] = canvas[source + i];
              else for (let i = 0; i < cw; i++) canvas[to + i] = canvas[source + i];
            }
          }
        } else {
          const e = entry * 4, v0 = entryValue[e], v1 = entryValue[e + 1];
          if (v0 === v1) {
            for (let row = 0; row < ch; row++) for (let to = (y + row) * width + x, end = to + cw; to < end; to++) canvas[to] = v0;
          } else {
            const bits = v1 === entryValue[e + 2] ? 1 : 2, pick = (1 << bits) - 1;
            if (codeBit + bits * cw * ch <= codesLimit) {
              // the whole cell is inside the stream: no per-pixel bound check
              for (let row = 0; row < ch; row++) {
                let to = (y + row) * width + x;
                const end = to + cw;
                while (to < end) {
                  const p = codeBit >>> 3;
                  canvas[to++] = entryValue[e + (((bytes[p] | (bytes[p + 1] << 8)) >>> (codeBit & 7)) & pick)];
                  codeBit += bits;
                }
              }
            } else for (let row = 0; row < ch; row++) {
              let to = (y + row) * width + x;
              for (let col = 0; col < cw; col++) {
                if (codeBit + bits > codesLimit) throw truncated(`direction ${index} frame pixels`, codeBit, bits, codesLimit);
                const p = codeBit >>> 3;
                canvas[to++] = entryValue[e + (((bytes[p] | (bytes[p + 1] << 8)) >>> (codeBit & 7)) & pick)];
                codeBit += bits;
              }
            }
          }
        }
        lastX[stateIndex] = x; lastY[stateIndex] = y; lastW[stateIndex] = cw; lastH[stateIndex] = ch;
        x += cw;
      }
      y += ch;
    }
    const fw = frame.width, fh = frame.height, indexed = new Uint8Array(fw * fh), opaque = new Uint8Array(indexed.length);
    const left = relXs[f], top = relYs[f];
    for (let row = 0, at = 0; row < fh; row++) {
      for (let source = (top + row) * width + left, end = source + fw; source < end; source++, at++) {
        const value = canvas[source];
        indexed[at] = value; opaque[at] = value === 0 ? 0 : 1;
      }
    }
    frame.indexed = indexed; frame.opaque = opaque; frameOutputs.push(frame);
  }
  direction.pixelCodeBits = codeBit - rawLimit;
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
export function composeCofFrame(cof, direction, frameIndex, components, dccDirection = direction, precomputedDirectionKey = null) {
  if (cof && cof.priority?.[direction]?.[frameIndex]) {
    const get = components instanceof Map ? (id) => components.get(id) : (id) => components?.[id];
    const directionKey = dccDirection instanceof Map
      ? (precomputedDirectionKey ?? [...dccDirection].sort(([a], [b]) => a - b).map(([component, value]) => `${component}.${value}`).join(','))
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
