const DC6_HEADER_BYTES = 24;
const DC6_FRAME_HEADER_BYTES = 32;
const DC6_FRAME_TRAILER_BYTES = 3;
const MAX_FRAMES = 16384;
const MAX_DIMENSION = 16384;
const MAX_PIXELS = 268435456;
const DT1_HEADER_BYTES = 276;
const DT1_TILE_HEADER_BYTES = 96;
const DT1_BLOCK_HEADER_BYTES = 20;
const DT1_MAX_TILES = 1048576;
const DT1_MAX_BLOCKS = 1048576;
const DT1_ISO_SKIP = [14, 12, 10, 8, 6, 4, 2, 0, 2, 4, 6, 8, 10, 12, 14];
const DT1_ISO_RUN = [4, 8, 12, 16, 20, 24, 28, 32, 28, 24, 20, 16, 12, 8, 4];

function bytesOf(input, label) {
  if (input instanceof Uint8Array) return input;
  if (input instanceof ArrayBuffer) return new Uint8Array(input);
  if (ArrayBuffer.isView(input)) {
    return new Uint8Array(input.buffer, input.byteOffset, input.byteLength);
  }
  throw new TypeError(`${label} must be an ArrayBuffer or byte view`);
}

function requireRange(bytes, offset, length, what) {
  if (!Number.isSafeInteger(offset) || !Number.isSafeInteger(length) ||
      offset < 0 || length < 0 || offset > bytes.length - length) {
    throw new RangeError(`truncated ${what}`);
  }
}

/** Decode a Diablo II palette.dat (256 BGR triples) to packed RGBA bytes. */
export function decodeDatPalette(input) {
  const bytes = bytesOf(input, 'DAT palette');
  if (bytes.length !== 256 * 3) {
    throw new RangeError(`DAT palette must be exactly 768 bytes (got ${bytes.length})`);
  }
  const rgba = new Uint8Array(256 * 4);
  for (let i = 0; i < 256; i++) {
    rgba[i * 4] = bytes[i * 3 + 2];
    rgba[i * 4 + 1] = bytes[i * 3 + 1];
    rgba[i * 4 + 2] = bytes[i * 3];
    rgba[i * 4 + 3] = 255;
  }
  return rgba;
}

function decodeDc6Frame(bytes, view, offset, frameIndex) {
  requireRange(bytes, offset, DC6_FRAME_HEADER_BYTES, `DC6 frame ${frameIndex} header`);
  const u32 = n => view.getUint32(offset + n, true);
  const i32 = n => view.getInt32(offset + n, true);
  const width = u32(4), height = u32(8), length = u32(28);
  if (!width || !height || width > MAX_DIMENSION || height > MAX_DIMENSION ||
      width > Math.floor(MAX_PIXELS / height)) {
    throw new RangeError(`invalid DC6 frame ${frameIndex} dimensions ${width}x${height}`);
  }
  const dataOffset = offset + DC6_FRAME_HEADER_BYTES;
  requireRange(bytes, dataOffset, length + DC6_FRAME_TRAILER_BYTES,
    `DC6 frame ${frameIndex} data`);

  const indices = new Uint8Array(width * height);
  const opaque = new Uint8Array(width * height);
  let cursor = dataOffset, x = 0, y = height - 1;
  const end = dataOffset + length;
  let rows = 0;
  while (cursor < end) {
    const command = bytes[cursor++];
    if (command === 0x80) {
      if (x > width) throw new RangeError(`DC6 frame ${frameIndex} scanline overrun`);
      rows++;
      if (rows === height) break;
      x = 0;
      y--;
      continue;
    }
    const run = command & 0x7f;
    if (run === 0) throw new RangeError(`DC6 frame ${frameIndex} has a zero-length run`);
    if (x > width - run) throw new RangeError(`DC6 frame ${frameIndex} scanline overrun`);
    if (command & 0x80) {
      x += run;
      continue;
    }
    if (cursor > end - run) throw new RangeError(`truncated DC6 frame ${frameIndex} opaque run`);
    const output = y * width + x;
    indices.set(bytes.subarray(cursor, cursor + run), output);
    opaque.fill(255, output, output + run);
    cursor += run;
    x += run;
  }
  if (rows !== height) throw new RangeError(`DC6 frame ${frameIndex} ended after ${rows}/${height} rows`);
  if (cursor !== end) throw new RangeError(`DC6 frame ${frameIndex} has data after its final row`);

  return {
    flipped: u32(0), width, height, offsetX: i32(12), offsetY: i32(16),
    unknown: u32(20), nextBlock: u32(24), indices, opaque,
    // The encoded bytes as stored: what retail keeps in memory for this frame, so a draw list can
    // name the exact frame drawn (play/assets/drawlist.mjs hashes them).
    encoded: bytes.subarray(dataOffset, end),
  };
}

/** Decode DC6 metadata and bottom-up RLE frames. Transparent pixels have opaque[i] === 0. */
export function decodeDc6(input) {
  const bytes = bytesOf(input, 'DC6');
  requireRange(bytes, 0, DC6_HEADER_BYTES, 'DC6 header');
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  const directions = view.getUint32(16, true);
  const framesPerDirection = view.getUint32(20, true);
  if (!directions || !framesPerDirection ||
      directions > MAX_FRAMES || framesPerDirection > MAX_FRAMES ||
      directions > Math.floor(MAX_FRAMES / framesPerDirection)) {
    throw new RangeError(`invalid DC6 frame grid ${directions}x${framesPerDirection}`);
  }
  const count = directions * framesPerDirection;
  requireRange(bytes, DC6_HEADER_BYTES, count * 4, 'DC6 frame pointer table');
  const framePointers = new Uint32Array(count);
  const frames = new Array(count);
  for (let i = 0; i < count; i++) {
    const pointer = view.getUint32(DC6_HEADER_BYTES + i * 4, true);
    framePointers[i] = pointer;
    frames[i] = decodeDc6Frame(bytes, view, pointer, i);
  }
  return {
    version: view.getInt32(0, true), flags: view.getUint32(4, true),
    encoding: view.getUint32(8, true), termination: bytes.slice(12, 16),
    directions, framesPerDirection, framePointers, frames,
  };
}

/** Expand an indexed DC6 frame with a decoded DAT palette into RGBA pixels. */
export function dc6FrameToRgba(frame, palette) {
  if (!(palette instanceof Uint8Array) || palette.length !== 1024) {
    throw new RangeError('decoded palette must contain 256 RGBA colors');
  }
  const rgba = new Uint8ClampedArray(frame.indices.length * 4);
  for (let i = 0; i < frame.indices.length; i++) {
    if (!frame.opaque[i]) continue;
    const source = frame.indices[i] * 4;
    rgba.set(palette.subarray(source, source + 4), i * 4);
  }
  return rgba;
}

function decodeDt1TilePixels(bytes, tile, tileIndex) {
  if (tile.blocks.length === 0) {
    return { originX: 0, originY: 0, width: 0, height: 0,
      indices: new Uint8Array(), opaque: new Uint8Array() };
  }

  let x0 = Infinity, y0 = Infinity, x1 = -Infinity, y1 = -Infinity;
  for (const block of tile.blocks) {
    const blockHeight = block.format === 1 ? 15 : 32;
    x0 = Math.min(x0, block.x); y0 = Math.min(y0, block.y);
    x1 = Math.max(x1, block.x + 32); y1 = Math.max(y1, block.y + blockHeight);
  }
  const width = x1 - x0, height = y1 - y0;
  if (!Number.isSafeInteger(width) || !Number.isSafeInteger(height) ||
      width <= 0 || height <= 0 || width > MAX_DIMENSION || height > MAX_DIMENSION ||
      width > Math.floor(MAX_PIXELS / height)) {
    throw new RangeError(`invalid DT1 tile ${tileIndex} pixel bounds ${width}x${height}`);
  }
  const indices = new Uint8Array(width * height);
  const opaque = new Uint8Array(width * height);
  const put = (blockIndex, x, y, index) => {
    const px = x - x0, py = y - y0;
    if (px < 0 || py < 0 || px >= width || py >= height) {
      throw new RangeError(`DT1 tile ${tileIndex} block ${blockIndex} pixel outside bounds`);
    }
    const block = tile.blocks[blockIndex];
    const local = (y - block.y) * 32 + x - block.x;
    block.pixels.indices[local] = index;
    block.pixels.written[local] = 255;
    const destination = py * width + px;
    indices[destination] = index;
    opaque[destination] = index === 0 ? 0 : 255;
  };

  for (let blockIndex = 0; blockIndex < tile.blocks.length; blockIndex++) {
    const block = tile.blocks[blockIndex];
    const blockHeight = block.format === 1 ? 15 : 32;
    block.pixels = { indices: new Uint8Array(32 * blockHeight), written: new Uint8Array(32 * blockHeight) };
    const start = tile.blockHeadersOffset + block.dataOffset;
    requireRange(bytes, start, block.dataLength,
      `DT1 tile ${tileIndex} block ${blockIndex} pixel data`);
    const end = start + block.dataLength;
    let cursor = start;
    if (block.format === 1) {
      if (block.dataLength !== 256) {
        throw new RangeError(`DT1 tile ${tileIndex} block ${blockIndex} isometric data must be 256 bytes`);
      }
      for (let row = 0; row < DT1_ISO_RUN.length; row++) {
        const count = DT1_ISO_RUN[row];
        for (let column = 0; column < count; column++) {
          put(blockIndex, block.x + DT1_ISO_SKIP[row] + column, block.y + row, bytes[cursor++]);
        }
      }
      continue;
    }
    if (block.format !== 0 && block.format !== 0x1001 && block.format !== 0x2005) {
      throw new RangeError(`unsupported DT1 tile ${tileIndex} block ${blockIndex} format 0x${block.format.toString(16)}`);
    }
    let x = 0, y = 0;
    while (cursor < end) {
      if (cursor > end - 2) {
        throw new RangeError(`truncated DT1 tile ${tileIndex} block ${blockIndex} RLE command`);
      }
      const skip = bytes[cursor++], run = bytes[cursor++];
      if (skip === 0 && run === 0) {
        x = 0; y++;
        if (y > 32) throw new RangeError(`DT1 tile ${tileIndex} block ${blockIndex} RLE row overrun`);
        continue;
      }
      if (run === 0) {
        throw new RangeError(`DT1 tile ${tileIndex} block ${blockIndex} has a zero-length RLE run`);
      }
      x += skip;
      if (y >= 32 || x > 32 - run) {
        throw new RangeError(`DT1 tile ${tileIndex} block ${blockIndex} RLE pixel overrun`);
      }
      if (cursor > end - run) {
        throw new RangeError(`truncated DT1 tile ${tileIndex} block ${blockIndex} RLE run`);
      }
      for (let column = 0; column < run; column++) {
        put(blockIndex, block.x + x++, block.y + y, bytes[cursor++]);
      }
    }
  }
  return { originX: x0, originY: y0, width, height, indices, opaque };
}

/** Decode a Diablo II DT1 v7.6 tile library, including indexed tile pixels. */
export function decodeDt1(input) {
  const bytes = bytesOf(input, 'DT1');
  requireRange(bytes, 0, DT1_HEADER_BYTES, 'DT1 header');
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  const versionMajor = view.getInt32(0, true);
  const versionMinor = view.getInt32(4, true);
  if (versionMajor !== 7 || versionMinor !== 6) {
    throw new RangeError(`unsupported DT1 version ${versionMajor}.${versionMinor}`);
  }
  const tileCount = view.getUint32(268, true);
  const tileHeadersOffset = view.getUint32(272, true);
  if (tileCount > DT1_MAX_TILES) throw new RangeError(`invalid DT1 tile count ${tileCount}`);
  requireRange(bytes, tileHeadersOffset, tileCount * DT1_TILE_HEADER_BYTES, 'DT1 tile headers');

  const tiles = new Array(tileCount);
  for (let tileIndex = 0; tileIndex < tileCount; tileIndex++) {
    const offset = tileHeadersOffset + tileIndex * DT1_TILE_HEADER_BYTES;
    const blockHeadersOffset = view.getUint32(offset + 72, true);
    const blockDataLength = view.getUint32(offset + 76, true);
    const blockCount = view.getUint32(offset + 80, true);
    if (blockCount > DT1_MAX_BLOCKS) {
      throw new RangeError(`invalid DT1 tile ${tileIndex} block count ${blockCount}`);
    }
    requireRange(bytes, blockHeadersOffset, blockCount * DT1_BLOCK_HEADER_BYTES,
      `DT1 tile ${tileIndex} block headers`);
    const blocks = new Array(blockCount);
    for (let blockIndex = 0; blockIndex < blockCount; blockIndex++) {
      const blockOffset = blockHeadersOffset + blockIndex * DT1_BLOCK_HEADER_BYTES;
      blocks[blockIndex] = {
        x: view.getInt16(blockOffset, true), y: view.getInt16(blockOffset + 2, true),
        unknown: view.getUint16(blockOffset + 4, true),
        gridX: bytes[blockOffset + 6], gridY: bytes[blockOffset + 7],
        format: view.getUint16(blockOffset + 8, true),
        dataLength: view.getUint32(blockOffset + 10, true),
        unknown2: view.getUint16(blockOffset + 14, true),
        dataOffset: view.getUint32(blockOffset + 16, true),
      };
    }
    const tile = {
      direction: view.getInt32(offset, true), roofHeight: view.getInt16(offset + 4, true),
      soundIndex: bytes[offset + 6], animated: bytes[offset + 7],
      height: view.getInt32(offset + 8, true), width: view.getInt32(offset + 12, true),
      heightToBottom: view.getInt32(offset + 16, true),
      orientation: view.getInt32(offset + 20, true), mainIndex: view.getInt32(offset + 24, true),
      subIndex: view.getInt32(offset + 28, true), rarity: view.getInt32(offset + 32, true),
      transparentColor: view.getUint32(offset + 36, true),
      subtileFlags: bytes.slice(offset + 40, offset + 65),
      blockHeadersOffset, blockDataLength, blockCount,
      cacheIndex: view.getUint16(offset + 90, true), blocks,
    };
    tile.pixels = decodeDt1TilePixels(bytes, tile, tileIndex);
    tiles[tileIndex] = tile;
  }
  return { versionMajor, versionMinor, tileCount, tileHeadersOffset, tiles };
}
