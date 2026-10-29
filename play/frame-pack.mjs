// Typed-array pack for the render-worker snapshot.
// Pure: no engine calls, no DOM. Terrain tiles are the objects readSessionTerrain returns
// (session-terrain.mjs parseSessionTiles + entry/coord/hidesPlayer/seam); packed to a fixed
// stride of Int32 words, transferable, unpacked into deep-equal objects.
export const TERRAIN_STRIDE = 26;   // Int32 words per tile
export const TERRAIN_BYTES = TERRAIN_STRIDE * 4;
const F = ['room', 'layer', 'ordinal', 'orientation', 'mainIndex', 'subIndex', 'frameIndex', 'worldX', 'worldY',
  'screenX', 'screenY', 'width', 'height', 'flags', 'librarySlot', 'libraryIndex', 'entryHint', 'pop'];   // words 0-17
const W_ENTRY = 18, W_COORD = 19 /* nIndex,x0,y0,x1,y1 */, W_META = 24;
// meta bits: 1 seam, 2 entry present, 4 coord object, 8 coord null, 16 hidesPlayer false, 32 hidesPlayer true
// Words 24 = meta; word 25 reserved (zero). Unsigned fields (room, flags, ...) round-trip via >>> 0.
const UNSIGNED = new Set(['room', 'layer', 'ordinal', 'flags', 'librarySlot', 'libraryIndex', 'pop']);

export function packTerrain(records, files = []) {
  const count = records.length, buf = new ArrayBuffer(count * TERRAIN_BYTES), w = new Int32Array(buf);
  for (let i = 0; i < count; i++) {
    const t = records[i], o = i * TERRAIN_STRIDE;
    for (let k = 0; k < F.length; k++) w[o + k] = t[F[k]];
    let m = 0;
    if (t.seam) m |= 1;
    if (t.entry !== undefined) { m |= 2; w[o + W_ENTRY] = t.entry; }
    const c = t.coord;
    if (c) { m |= 4; w[o + W_COORD] = c.nIndex; w[o + W_COORD + 1] = c.x0; w[o + W_COORD + 2] = c.y0; w[o + W_COORD + 3] = c.x1; w[o + W_COORD + 4] = c.y1; }
    else if (c === null) m |= 8;
    if (t.hidesPlayer === false) m |= 16; else if (t.hidesPlayer) m |= 32;
    w[o + W_META] = m;
  }
  return { buf, count, stride: TERRAIN_STRIDE, files: files.slice() };
}

export function unpackTerrain(packed) {
  const { buf, count } = packed, w = new Int32Array(buf), out = new Array(count);
  for (let i = 0; i < count; i++) {
    const o = i * TERRAIN_STRIDE, t = {};
    for (let k = 0; k < F.length; k++) t[F[k]] = UNSIGNED.has(F[k]) ? w[o + k] >>> 0 : w[o + k];
    const m = w[o + W_META];
    if (m & 2) t.entry = w[o + W_ENTRY] >>> 0;
    if (m & 4) t.coord = { nIndex: w[o + W_COORD], x0: w[o + W_COORD + 1], y0: w[o + W_COORD + 2], x1: w[o + W_COORD + 3], y1: w[o + W_COORD + 4] };
    else if (m & 8) t.coord = null;
    if (m & 16) t.hidesPlayer = false; else if (m & 32) t.hidesPlayer = true;
    if (m & 1) t.seam = true;
    out[i] = t;
  }
  return out;
}

export const transferList = (packed) => [packed.buf];

// ---- Units: parseSessionUnits records (+ optional preciseX/preciseY from the coords join) ----
// Int32 stride 12: id,type,classId,mode,x,y,flags,flagsEx,hp,hpMax,roomOrdinal, meta (bit 1 = precise present).
// preciseX/Y ride in a Float64Array, stride 2. kind/hostile are derived from type, as parseSessionUnits does.
export const UNIT_STRIDE = 12;
const U_UNSIGNED = [0, 1, 3, 6, 7, 10];
const U_F = ['id', 'type', 'classId', 'mode', 'x', 'y', 'flags', 'flagsEx', 'hp', 'hpMax', 'roomOrdinal'];
const KINDS = ['player', 'monster', 'object', 'missile', 'item'];

export function packUnits(records) {
  const count = records.length, buf = new ArrayBuffer(count * UNIT_STRIDE * 4), w = new Int32Array(buf);
  const fbuf = new ArrayBuffer(count * 16), f = new Float64Array(fbuf);
  for (let i = 0; i < count; i++) {
    const u = records[i], o = i * UNIT_STRIDE;
    for (let k = 0; k < U_F.length; k++) w[o + k] = u[U_F[k]];
    if (u.preciseX !== undefined) { w[o + 11] = 1; f[2 * i] = u.preciseX; f[2 * i + 1] = u.preciseY; }
  }
  return { buf, fbuf, count, stride: UNIT_STRIDE };
}

export function unpackUnits({ buf, fbuf, count }) {
  const w = new Int32Array(buf), f = new Float64Array(fbuf), out = new Array(count);
  for (let i = 0; i < count; i++) {
    const o = i * UNIT_STRIDE, u = {};
    for (let k = 0; k < U_F.length; k++) u[U_F[k]] = U_UNSIGNED.includes(k) ? w[o + k] >>> 0 : w[o + k];
    u.kind = KINDS[u.type] ?? 'tile';
    u.hostile = u.type === 1;
    if (w[o + 11] & 1) { u.preciseX = f[2 * i]; u.preciseY = f[2 * i + 1]; }
    out[i] = u;
  }
  return out;
}

// ---- Facings: {id, direction, hasDynamicPath, lastHitClass, typeFlags, superUniqueId} ----
// Int32 stride 3: id, direction | lastHitClass<<8 | typeFlags<<16 | hasDynamicPath<<24, superUniqueId (int16).
export const FACING_STRIDE = 3;

export function packFacings(records) {
  const count = records.length, buf = new ArrayBuffer(count * FACING_STRIDE * 4), w = new Int32Array(buf);
  for (let i = 0; i < count; i++) {
    const r = records[i], o = i * FACING_STRIDE;
    w[o] = r.id;
    w[o + 1] = (r.direction & 255) | ((r.lastHitClass & 255) << 8) | ((r.typeFlags & 255) << 16) | ((r.hasDynamicPath ? 1 : 0) << 24);
    w[o + 2] = r.superUniqueId;
  }
  return { buf, count, stride: FACING_STRIDE };
}

export function unpackFacings({ buf, count }) {
  const w = new Int32Array(buf), out = new Array(count);
  for (let i = 0; i < count; i++) {
    const o = i * FACING_STRIDE, m = w[o + 1];
    out[i] = { id: w[o] >>> 0, direction: m & 255, hasDynamicPath: ((m >>> 24) & 1) === 1, lastHitClass: (m >>> 8) & 255,
      typeFlags: (m >>> 16) & 255, superUniqueId: w[o + 2] };
  }
  return out;
}

// ---- Lightmap: readSessionLightmap's result (48x48 cells + blocked, <=64 sources) ----
// Int32 header stride 5: ox, oy, ambient, sources, listCount; then list stride 6 (five int32 + guid as uint32).
// Bytes: cells (2304) then blocked (2304). at/grid are closures, rebuilt on unpack. null stays null.
export const LIGHT_HEADER = 5, LIGHT_LIST_STRIDE = 6, LIGHT_CELLS = 48 * 48;

export function packLightmap(lm) {
  if (!lm) return null;
  const n = lm.list.length, buf = new ArrayBuffer((LIGHT_HEADER + n * LIGHT_LIST_STRIDE) * 4), w = new Int32Array(buf);
  w[0] = lm.origin[0]; w[1] = lm.origin[1]; w[2] = lm.ambient; w[3] = lm.sources; w[4] = n;
  for (let i = 0; i < n; i++) for (let k = 0; k < 6; k++) w[LIGHT_HEADER + i * 6 + k] = lm.list[i][k];
  const bytes = new ArrayBuffer(2 * LIGHT_CELLS), b = new Uint8Array(bytes);
  b.set(lm.cells, 0); b.set(lm.blocked, LIGHT_CELLS);
  return { buf, bytes };
}

export function unpackLightmap(p) {
  if (!p) return null;
  const w = new Int32Array(p.buf), n = w[4], ox = w[0], oy = w[1], b = new Uint8Array(p.bytes);
  const cells = b.slice(0, LIGHT_CELLS), blocked = b.slice(LIGHT_CELLS), list = new Array(n);
  for (let i = 0; i < n; i++) {
    const o = LIGHT_HEADER + i * 6;
    list[i] = [w[o], w[o + 1], w[o + 2], w[o + 3], w[o + 4], w[o + 5] >>> 0];
  }
  const at = (x, y) => cells[Math.max(0, Math.min(47, y - oy)) * 48 + Math.max(0, Math.min(47, x - ox))];
  return { origin: [ox, oy], ambient: w[2], sources: w[3], list, cells, blocked, at,
    grid: (a, c) => { const g = new Array(64); for (let j = 0; j < 8; j++) for (let i = 0; i < 8; i++) g[j * 8 + i] = at(a - 1 + i, c - 1 + j); return g; } };
}

// ---- Whole frame ----
// `extra` is a structured-clone side array: monster animation and missile data per unit id, plus the object
// presentations (objects are not in the unit list). Frames without it decode as before.
export function packFrame({ tiles = [], files = [], units = [], facings = [], lightmap = null, objects = [] } = {}) {
  const extra = [];
  // Keyed by position: unit ids are per type (the hero and a monster can both be id 1).
  units.forEach((u, i) => { if (u.animation || u.missile || u.overlay || u.stateOverlays?.length) extra.push({ i, id: u.id, animation: u.animation, missile: u.missile, overlay: u.overlay, stateOverlays: u.stateOverlays }); });
  return { terrain: packTerrain(tiles, files), units: packUnits(units), facings: packFacings(facings), lightmap: packLightmap(lightmap),
    extra, objects: objects.slice() };
}

export function unpackFrame(p) {
  const units = unpackUnits(p.units);
  if (p.extra?.length) {
    for (const x of p.extra) { const u = x.i === undefined ? units.find((q) => q.id === x.id >>> 0) : units[x.i]; if (!u) continue; if (x.animation) u.animation = x.animation; if (x.missile) u.missile = x.missile; if (x.overlay) u.overlay = x.overlay; if (x.stateOverlays) u.stateOverlays = x.stateOverlays; }
  }
  for (const o of p.objects ?? []) units.push({ ...o, kind: 'object' });
  return { tiles: unpackTerrain(p.terrain), files: p.terrain.files.slice(), units,
    facings: unpackFacings(p.facings), lightmap: unpackLightmap(p.lightmap) };
}

export const frameTransferList = (p) => [p.terrain.buf, p.units.buf, p.units.fbuf, p.facings.buf,
  ...(p.lightmap ? [p.lightmap.buf, p.lightmap.bytes] : [])];
