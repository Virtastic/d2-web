const MIN_VERSION = 1;
const MAX_VERSION = 18;
const MAX_DIMENSION = 16384;
const MAX_CELLS = 16 * 1024 * 1024;
const MAX_DEPENDENCIES = 4096;
const MAX_LAYERS = 4;
const MAX_RECORDS = 1 << 20;
const MAX_STRING_BYTES = 4096;

// DRLGANIM_GetOrientationFromIndex for DS1 versions before 7.
const LEGACY_ORIENTATION = Uint8Array.of(
  0, 1, 2, 1, 2, 3, 3, 5, 5, 6, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14,
  15, 16, 17, 18, 20,
);

function bytesOf(input) {
  if (input instanceof Uint8Array) return input;
  if (input instanceof ArrayBuffer) return new Uint8Array(input);
  if (ArrayBuffer.isView(input)) return new Uint8Array(input.buffer, input.byteOffset, input.byteLength);
  throw new TypeError('DS1 must be an ArrayBuffer or byte view');
}

class Reader {
  constructor(input) {
    this.bytes = bytesOf(input);
    this.view = new DataView(this.bytes.buffer, this.bytes.byteOffset, this.bytes.byteLength);
    this.offset = 0;
  }

  require(length, what) {
    if (!Number.isSafeInteger(length) || length < 0 || this.offset > this.bytes.length - length) {
      throw new RangeError(`truncated DS1 ${what}`);
    }
  }

  u32(what) { this.require(4, what); const n = this.view.getUint32(this.offset, true); this.offset += 4; return n; }
  i32(what) { this.require(4, what); const n = this.view.getInt32(this.offset, true); this.offset += 4; return n; }
  skip(length, what) { this.require(length, what); this.offset += length; }

  cstring(what) {
    const start = this.offset;
    const limit = Math.min(this.bytes.length, start + MAX_STRING_BYTES + 1);
    while (this.offset < limit && this.bytes[this.offset] !== 0) this.offset++;
    if (this.offset >= limit || this.offset >= this.bytes.length) throw new RangeError(`unterminated DS1 ${what}`);
    const value = new TextDecoder('utf-8', { fatal: true }).decode(this.bytes.subarray(start, this.offset));
    this.offset++;
    return value;
  }

  cells(count, what) {
    this.require(count * 4, what);
    const out = new Uint32Array(count);
    for (let i = 0; i < count; i++, this.offset += 4) out[i] = this.view.getUint32(this.offset, true);
    return out;
  }
}

function checkedCount(value, max, what) {
  if (value > max) throw new RangeError(`invalid DS1 ${what} ${value}`);
  return value;
}

/** Break a packed DS1 cell into the fields consumed by DT1 tile selection. */
export function decodeDs1Cell(raw, orientation = 0) {
  raw >>>= 0;
  return {
    raw,
    prop1: raw & 0xff,
    prop2: (raw >>> 8) & 0xff,
    prop3: (raw >>> 16) & 0xff,
    prop4: raw >>> 24,
    mainIndex: (raw >>> 20) & 0x3f,
    subIndex: (raw >>> 8) & 0xff,
    orientation: orientation >>> 0,
  };
}

/** Parse a retail DS1 preset map. Versions 1 through 18 are supported. */
export function decodeDs1(input) {
  const r = new Reader(input);
  const version = r.i32('version');
  if (version < MIN_VERSION || version > MAX_VERSION) throw new RangeError(`unsupported DS1 version ${version}`);

  const storedWidth = r.u32('width'), storedHeight = r.u32('height');
  const width = storedWidth + 1, height = storedHeight + 1;
  if (width > MAX_DIMENSION || height > MAX_DIMENSION || width > Math.floor(MAX_CELLS / height)) {
    throw new RangeError(`invalid DS1 dimensions ${width}x${height}`);
  }
  const cellCount = width * height;
  const act = version < 8 ? 0 : Math.min(r.u32('act'), 4);
  const substitutionMethod = version < 10 ? 0 : r.i32('substitution method');

  const dependencyCount = version < 3 ? 0 : checkedCount(r.u32('dependency count'), MAX_DEPENDENCIES, 'dependency count');
  const dependencies = new Array(dependencyCount);
  for (let i = 0; i < dependencyCount; i++) dependencies[i] = r.cstring(`dependency ${i}`);
  if (version >= 9 && version <= 13) r.skip(8, 'legacy header extension');

  let wallCount = 1, floorCount = 1;
  if (version >= 4) {
    wallCount = checkedCount(r.u32('wall layer count'), MAX_LAYERS, 'wall layer count');
    floorCount = version < 16 ? 1 : checkedCount(r.u32('floor layer count'), 2, 'floor layer count');
  }
  if (wallCount === 0 || floorCount === 0) throw new RangeError('DS1 must contain wall and floor layers');

  const walls = new Array(wallCount);
  const orientations = new Array(wallCount);
  const floors = new Array(floorCount);
  let shadow, tags = null;

  if (version < 4) {
    // Blizzard's legacy stream order is wall, floor, orientation, tag, shadow.
    walls[0] = r.cells(cellCount, 'wall layer 0');
    floors[0] = r.cells(cellCount, 'floor layer 0');
    orientations[0] = r.cells(cellCount, 'orientation layer 0');
    tags = r.cells(cellCount, 'tag layer');
    shadow = r.cells(cellCount, 'shadow layer');
  } else {
    for (let i = 0; i < wallCount; i++) {
      walls[i] = r.cells(cellCount, `wall layer ${i}`);
      orientations[i] = r.cells(cellCount, `orientation layer ${i}`);
    }
    for (let i = 0; i < floorCount; i++) floors[i] = r.cells(cellCount, `floor layer ${i}`);
    shadow = r.cells(cellCount, 'shadow layer');
    if (version >= 10 && (substitutionMethod === 1 || substitutionMethod === 2)) {
      tags = r.cells(cellCount, 'tag layer');
    }
  }

  if (version < 7) {
    for (const layer of orientations) {
      for (let i = 0; i < layer.length; i++) {
        const old = layer[i];
        if (old >= LEGACY_ORIENTATION.length) throw new RangeError(`invalid DS1 legacy orientation ${old}`);
        layer[i] = LEGACY_ORIENTATION[old];
      }
    }
  }

  const objectCount = version < 2 ? 0 : checkedCount(r.u32('object count'), MAX_RECORDS, 'object count');
  const objects = new Array(objectCount);
  for (let i = 0; i < objectCount; i++) {
    objects[i] = {
      type: r.u32(`object ${i} type`), id: r.u32(`object ${i} id`),
      x: r.u32(`object ${i} x`), y: r.u32(`object ${i} y`),
      flags: version < 6 ? 0 : r.u32(`object ${i} flags`),
    };
  }

  const groups = [];
  if (version >= 12 && (substitutionMethod === 1 || substitutionMethod === 2)) {
    if (version >= 18) r.skip(4, 'group header');
    const count = checkedCount(r.u32('group count'), MAX_RECORDS, 'group count');
    for (let i = 0; i < count; i++) groups.push({
      x: r.u32(`group ${i} x`), y: r.u32(`group ${i} y`),
      width: r.u32(`group ${i} width`), height: r.u32(`group ${i} height`),
      unknown: version < 13 ? 0 : r.u32(`group ${i} unknown`),
    });
  }

  const paths = [];
  if (version >= 14) {
    const count = checkedCount(r.u32('path count'), MAX_RECORDS, 'path count');
    for (let i = 0; i < count; i++) {
      const nodeCount = checkedCount(r.u32(`path ${i} node count`), MAX_RECORDS, `path ${i} node count`);
      const path = { x: r.u32(`path ${i} x`), y: r.u32(`path ${i} y`), objectIndex: null, nodes: new Array(nodeCount) };
      for (let n = 0; n < nodeCount; n++) path.nodes[n] = {
        x: r.u32(`path ${i} node ${n} x`), y: r.u32(`path ${i} node ${n} y`),
        action: version < 15 ? 1 : r.u32(`path ${i} node ${n} action`),
      };
      for (let j = objects.length - 1; j >= 0; j--) {
        if (objects[j].x === path.x && objects[j].y === path.y) { path.objectIndex = j; break; }
      }
      paths.push(path);
    }
  }

  const trailing = r.bytes.subarray(r.offset);
  if (trailing.length > 4 || trailing.some(byte => byte !== 0)) {
    throw new RangeError(`unexpected DS1 trailing data (${trailing.length} bytes)`);
  }
  return {
    version, storedWidth, storedHeight, width, height, act, substitutionMethod,
    dependencies, wallCount, floorCount, walls, orientations, floors, shadow, tags,
    objects, groups, paths, bytesConsumed: r.offset, trailingBytes: trailing.length,
  };
}
