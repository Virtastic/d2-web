import { test } from 'node:test';
import assert from 'node:assert/strict';
import { decodeDatPalette, decodeDc6, dc6FrameToRgba, decodeDt1, tilePixelsToRgba } from '../play/assets/d2-formats.mjs';
import { decodeDs1, decodeDs1Cell } from '../play/assets/ds1.mjs';
import { ACT_PRESET_ASSETS, composeDs1Map, normalizeDt1Dependency, TILE_HEIGHT, TILE_WIDTH } from '../play/assets/ds1-compositor.mjs';

function dc6Fixture(frameData, { width = 4, height = 2, pointer = 32 } = {}) {
  const bytes = new Uint8Array(pointer + 32 + frameData.length + 3);
  const v = new DataView(bytes.buffer);
  v.setInt32(0, 6, true); v.setUint32(16, 1, true); v.setUint32(20, 1, true);
  v.setUint32(24, pointer, true);
  v.setUint32(pointer, 0, true); v.setUint32(pointer + 4, width, true);
  v.setUint32(pointer + 8, height, true); v.setInt32(pointer + 12, -2, true);
  v.setInt32(pointer + 16, 7, true); v.setUint32(pointer + 28, frameData.length, true);
  bytes.set(frameData, pointer + 32);
  bytes.fill(0xee, bytes.length - 3);
  return bytes;
}

function dt1Fixture() {
  const tileOffset = 276, blocksOffset = tileOffset + 96;
  const isoOffset = 40, rleOffset = isoOffset + 256;
  const rle = Uint8Array.of(1, 3, 7, 0, 8, 0, 0, 0, 2, 9, 10);
  const bytes = new Uint8Array(blocksOffset + rleOffset + rle.length);
  const v = new DataView(bytes.buffer);
  v.setInt32(0, 7, true); v.setInt32(4, 6, true);
  v.setUint32(268, 1, true); v.setUint32(272, tileOffset, true);
  v.setInt32(tileOffset, 3, true); v.setInt16(tileOffset + 4, -12, true);
  bytes[tileOffset + 6] = 4; bytes[tileOffset + 7] = 1;
  v.setInt32(tileOffset + 8, -80, true); v.setInt32(tileOffset + 12, 64, true);
  v.setInt32(tileOffset + 20, 15, true); v.setInt32(tileOffset + 24, 2, true);
  v.setInt32(tileOffset + 28, 9, true); v.setInt32(tileOffset + 32, 7, true);
  bytes.set([1, 2, 4, 8, 32], tileOffset + 40);
  v.setUint32(tileOffset + 72, blocksOffset, true);
  v.setUint32(tileOffset + 76, 256 + rle.length, true);
  v.setUint32(tileOffset + 80, 2, true);
  v.setInt16(tileOffset + 90, 23, true);

  v.setInt16(blocksOffset, 0, true); v.setInt16(blocksOffset + 2, 0, true);
  bytes[blocksOffset + 6] = 2; bytes[blocksOffset + 7] = 3;
  v.setUint16(blocksOffset + 8, 1, true); v.setUint32(blocksOffset + 10, 256, true);
  v.setUint32(blocksOffset + 16, isoOffset, true);
  const rleHeader = blocksOffset + 20;
  v.setInt16(rleHeader, 32, true); v.setInt16(rleHeader + 2, 0, true);
  v.setUint16(rleHeader + 8, 0x1001, true); v.setUint32(rleHeader + 10, rle.length, true);
  v.setUint32(rleHeader + 16, rleOffset, true);
  for (let i = 0; i < 256; i++) bytes[blocksOffset + isoOffset + i] = i & 0xff;
  bytes.set(rle, blocksOffset + rleOffset);
  return bytes;
}

function ds1Fixture({ version = 18, trailing = 0 } = {}) {
  const bytes = [];
  const u32 = n => bytes.push(n & 255, n >>> 8 & 255, n >>> 16 & 255, n >>> 24 & 255);
  const text = s => { for (const c of new TextEncoder().encode(s)) bytes.push(c); bytes.push(0); };
  const cells = (a, b) => { u32(a); u32(b); };
  u32(version); u32(1); u32(0); // stored dimensions: 2x1 cells
  if (version >= 8) u32(2);
  if (version >= 10) u32(1);
  if (version >= 3) { u32(1); text('data\\global\\tiles\\x.dt1'); }
  if (version >= 9 && version <= 13) { u32(0); u32(0); }
  if (version >= 4) { u32(1); if (version >= 16) u32(2); }
  if (version < 4) {
    cells(0x01234567, 0); // wall
    cells(0x11111111, 0x22222222); // floor
    cells(3, 24); // legacy orientations => 1,20
    cells(7, 8); // tags
    cells(9, 10); // shadow
  } else {
    cells(0x01234567, 0); // wall
    cells(12, 20); // orientation
    cells(0x11111111, 0x22222222); // floor 0
    if (version >= 16) cells(0x33333333, 0x44444444); // floor 1
    cells(9, 10); // shadow
    if (version >= 10) cells(7, 8); // tag for substitution method 1
  }
  if (version >= 2) {
    u32(1); u32(2); u32(77); u32(25); u32(35); if (version >= 6) u32(0xa5);
  }
  if (version >= 12) {
    if (version >= 18) u32(0xfeed);
    u32(1); u32(1); u32(2); u32(3); u32(4); if (version >= 13) u32(5);
  }
  if (version >= 14) {
    u32(1); u32(2); u32(25); u32(35);
    u32(26); u32(36); if (version >= 15) u32(6);
    u32(27); u32(37); if (version >= 15) u32(7);
  }
  for (let i = 0; i < trailing; i++) bytes.push(0);
  return Uint8Array.from(bytes);
}

test('DAT palette converts BGR entries to opaque RGBA', () => {
  const dat = new Uint8Array(768);
  dat.set([3, 2, 1], 0); dat.set([30, 20, 10], 3);
  const rgba = decodeDatPalette(dat);
  assert.deepEqual([...rgba.slice(0, 8)], [1, 2, 3, 255, 10, 20, 30, 255]);
  assert.throws(() => decodeDatPalette(dat.subarray(1)), /exactly 768/);
});

test('DT1 palette conversion preserves transparent and opaque pixels', () => {
  const palette = new Uint8Array(1024);
  palette.set([11, 22, 33, 255], 4);
  const pixels = { width: 2, height: 1, indices: Uint8Array.of(1, 1), opaque: Uint8Array.of(255, 0) };
  assert.deepEqual(Array.from(tilePixelsToRgba(pixels, palette)), [11, 22, 33, 255, 0, 0, 0, 0]);
  assert.throws(() => tilePixelsToRgba({ indices: Uint8Array.of(1), opaque: new Uint8Array() }, palette), /equal index and opacity/);
  assert.throws(() => tilePixelsToRgba(pixels, new Uint8Array(768)), /256 RGBA colors/);
});

test('obsolete per-act mock terrain atlas module is absent', async () => {
  await assert.rejects(import('../play/assets/retail-tile-view.mjs'), { code: 'ERR_MODULE_NOT_FOUND' });
});

test('DC6 honors frame pointers and decodes bottom-up opaque and transparent runs', () => {
  // File order: bottom row [1,2,transparent,3], then top row [transparent,4,5,6].
  const dc6 = decodeDc6(dc6Fixture(Uint8Array.of(
    2, 1, 2, 0x81, 1, 3, 0x80,
    0x81, 3, 4, 5, 6, 0x80,
  )));
  assert.equal(dc6.version, 6); assert.equal(dc6.frames.length, 1);
  const frame = dc6.frames[0];
  assert.deepEqual([frame.width, frame.height, frame.offsetX, frame.offsetY], [4, 2, -2, 7]);
  assert.deepEqual([...frame.indices], [0, 4, 5, 6, 1, 2, 0, 3]);
  assert.deepEqual([...frame.opaque], [0, 255, 255, 255, 255, 255, 0, 255]);
});

test('DC6 keeps opaque palette index zero distinct from transparency', () => {
  const frame = decodeDc6(dc6Fixture(Uint8Array.of(1, 0, 0x83, 0x80, 0x84, 0x80))).frames[0];
  const dat = new Uint8Array(768); dat.set([7, 8, 9]);
  const rgba = dc6FrameToRgba(frame, decodeDatPalette(dat));
  assert.deepEqual([...rgba.slice(0, 4)], [0, 0, 0, 0]);
  assert.deepEqual([...rgba.slice(16, 20)], [9, 8, 7, 255]);
});

test('DC6 rejects malformed dimensions, runs, truncation, and incomplete rows', () => {
  assert.throws(() => decodeDc6(new Uint8Array(23)), /truncated DC6 header/);
  assert.throws(() => decodeDc6(dc6Fixture(Uint8Array.of(5, 1, 2, 3, 4, 5, 0x80))), /scanline overrun/);
  assert.throws(() => decodeDc6(dc6Fixture(Uint8Array.of(4, 1, 2))), /truncated.*opaque run/);
  assert.throws(() => decodeDc6(dc6Fixture(Uint8Array.of(0x84, 0x80))), /ended after 1\/2 rows/);
  assert.throws(() => decodeDc6(dc6Fixture(Uint8Array.of(0, 0x80, 0x84, 0x80))), /zero-length run/);
});

test('DT1 decodes v7.6 tile metadata, isometric pixels, and RLE pixels', () => {
  const dt1 = decodeDt1(dt1Fixture());
  assert.deepEqual([dt1.versionMajor, dt1.versionMinor, dt1.tileCount], [7, 6, 1]);
  const tile = dt1.tiles[0];
  assert.deepEqual(
    [tile.direction, tile.roofHeight, tile.materialFlags, tile.soundIndex, tile.animated, tile.height, tile.width,
      tile.orientation, tile.mainIndex, tile.subIndex, tile.rarity, tile.cacheIndex],
    [3, -12, 0x0104, 4, 1, -80, 64, 15, 2, 9, 7, 23]);
  assert.deepEqual([...tile.subtileFlags.slice(0, 5)], [1, 2, 4, 8, 32]);
  assert.deepEqual([tile.blocks[0].gridX, tile.blocks[0].gridY, tile.blocks[0].format], [2, 3, 1]);
  assert.deepEqual(
    [tile.pixels.originX, tile.pixels.originY, tile.pixels.width, tile.pixels.height],
    [0, 0, 64, 32]);
  // First iso scanline begins at x=14; copied palette index 0 remains transparent.
  assert.deepEqual([...tile.pixels.indices.slice(14, 18)], [0, 1, 2, 3]);
  assert.deepEqual([...tile.pixels.opaque.slice(14, 18)], [0, 255, 255, 255]);
  // RLE block starts at x=32: skip one, copy [7, transparent zero, 8].
  assert.deepEqual([...tile.pixels.indices.slice(33, 36)], [7, 0, 8]);
  assert.deepEqual([...tile.pixels.opaque.slice(33, 36)], [255, 0, 255]);
  assert.deepEqual([...tile.pixels.indices.slice(64 + 32, 64 + 34)], [9, 10]);
});

test('DT1 retains each overlapping block source and distinguishes skipped from written zero', () => {
  const bytes = dt1Fixture();
  // Place the RLE block over the iso row instead of beside it.
  new DataView(bytes.buffer).setInt16(276 + 96 + 20, 14, true);
  const tile = decodeDt1(bytes).tiles[0];
  const [iso, rle] = tile.blocks;
  assert.deepEqual([...iso.pixels.indices.slice(14, 18)], [0, 1, 2, 3]);
  assert.deepEqual([...rle.pixels.indices.slice(0, 4)], [0, 7, 0, 8]);
  assert.deepEqual([...rle.pixels.written.slice(0, 4)], [0, 255, 255, 255]);
  assert.deepEqual([...tile.pixels.indices.slice(14, 18)], [0, 7, 0, 8]);
  assert.equal(iso.pixels.indices[15], 1, 'later block must not replace earlier source');
});

test('DT1 rejects bad versions, pointer spans, block formats, and malformed RLE', () => {
  assert.throws(() => decodeDt1(new Uint8Array(275)), /truncated DT1 header/);
  const version = dt1Fixture(); new DataView(version.buffer).setInt32(4, 5, true);
  assert.throws(() => decodeDt1(version), /unsupported DT1 version 7\.5/);
  const headers = dt1Fixture(); new DataView(headers.buffer).setUint32(272, headers.length - 1, true);
  assert.throws(() => decodeDt1(headers), /truncated DT1 tile headers/);
  const format = dt1Fixture(); new DataView(format.buffer).setUint16(276 + 96 + 20 + 8, 0x7777, true);
  assert.throws(() => decodeDt1(format), /unsupported DT1.*format 0x7777/);
  const run = dt1Fixture();
  const rleStart = 276 + 96 + 40 + 256;
  run[rleStart] = 31; run[rleStart + 1] = 3;
  assert.throws(() => decodeDt1(run), /RLE pixel overrun/);
  const truncated = dt1Fixture().subarray(0, dt1Fixture().length - 1);
  assert.throws(() => decodeDt1(truncated), /truncated DT1.*pixel data/);
});

test('DS1 decodes dependencies, modern layers, objects, groups, and NPC paths', () => {
  const ds1 = decodeDs1(ds1Fixture({ trailing: 4 }));
  assert.deepEqual(
    [ds1.version, ds1.width, ds1.height, ds1.act, ds1.substitutionMethod,
      ds1.wallCount, ds1.floorCount, ds1.trailingBytes],
    [18, 2, 1, 2, 1, 1, 2, 4]);
  assert.deepEqual(ds1.dependencies, ['data\\global\\tiles\\x.dt1']);
  assert.deepEqual([...ds1.walls[0]], [0x01234567, 0]);
  assert.deepEqual([...ds1.orientations[0]], [12, 20]);
  assert.deepEqual([...ds1.floors[1]], [0x33333333, 0x44444444]);
  assert.deepEqual([...ds1.shadow], [9, 10]);
  assert.deepEqual([...ds1.tags], [7, 8]);
  assert.deepEqual(ds1.objects, [{ type: 2, id: 77, x: 25, y: 35, flags: 0xa5 }]);
  assert.deepEqual(ds1.groups, [{ x: 1, y: 2, width: 3, height: 4, unknown: 5 }]);
  assert.deepEqual(ds1.paths, [{ x: 25, y: 35, objectIndex: 0, nodes: [
    { x: 26, y: 36, action: 6 }, { x: 27, y: 37, action: 7 },
  ] }]);
  assert.deepEqual(decodeDs1Cell(0x81234567, 12), {
    raw: 0x81234567, prop1: 0x67, prop2: 0x45, prop3: 0x23, prop4: 0x81,
    mainIndex: 0x12, subIndex: 0x45, orientation: 12,
  });
});

test('DS1 preserves pre-v4 layer order and remaps legacy orientations', () => {
  const ds1 = decodeDs1(ds1Fixture({ version: 3 }));
  assert.deepEqual([...ds1.floors[0]], [0x11111111, 0x22222222]);
  assert.deepEqual([...ds1.orientations[0]], [1, 20]);
  assert.deepEqual([...ds1.tags], [7, 8]);
  assert.deepEqual([...ds1.shadow], [9, 10]);
  assert.deepEqual(ds1.objects, [{ type: 2, id: 77, x: 25, y: 35, flags: 0 }]);
});

test('DS1 rejects unsupported versions, truncation, hostile counts, and trailing data', () => {
  const unsupported = ds1Fixture(); new DataView(unsupported.buffer).setUint32(0, 19, true);
  assert.throws(() => decodeDs1(unsupported), /unsupported DS1 version 19/);
  assert.throws(() => decodeDs1(ds1Fixture().subarray(0, 30)), /(?:truncated|unterminated) DS1/);
  const layers = ds1Fixture();
  // Header + dependency string ends at byte 55; version 18 wall layer count follows.
  const marker = new TextEncoder().encode('data\\global\\tiles\\x.dt1\0').length;
  new DataView(layers.buffer).setUint32(24 + marker, 5, true);
  assert.throws(() => decodeDs1(layers), /invalid DS1 wall layer count 5/);
  const trailing = ds1Fixture({ trailing: 5 });
  assert.throws(() => decodeDs1(trailing), /unexpected DS1 trailing data/);
});

test('DS1 compositor resolves declared DT1 identities at isometric tile spacing', () => {
  const raw = (main, sub, prop1 = 1) => (prop1 | sub << 8 | main << 20) >>> 0;
  const pixels = { originX: 0, originY: 0, width: 160, height: 80,
    indices: new Uint8Array(160 * 80), opaque: new Uint8Array(160 * 80) };
  const tile = (mainIndex, subIndex, orientation, rarity = 1) =>
    ({ mainIndex, subIndex, orientation, rarity, pixels });
  const map = {
    width: 2, height: 2, dependencies: ['a.dt1', 'b.dt1'],
    floors: [Uint32Array.of(raw(1, 2), raw(1, 2), 0, raw(9, 9))],
    walls: [Uint32Array.of(0, raw(3, 4), raw(3, 4), raw(3, 4, 0))],
    orientations: [Uint32Array.of(0, 2, 3, 2)],
  };
  const plan = composeDs1Map(map, [
    { tiles: [tile(1, 2, 0), tile(3, 4, 2)] },
    { tiles: [tile(3, 4, 3), tile(3, 4, 4)] },
  ]);
  assert.deepEqual(
    [plan.width, plan.height, plan.dependencies, plan.libraries, plan.libraryTileCount,
      plan.identityCount, plan.floorCount, plan.wallCount],
    [2, 2, 2, 2, 4, 4, 2, 3]);
  assert.deepEqual(plan.floors.map(({ anchorX, anchorY }) => [anchorX, anchorY]),
    [[0, 0], [TILE_WIDTH / 2, TILE_HEIGHT / 2]]);
  assert.deepEqual(plan.walls.map(({ anchorX, anchorY, cell }) => [anchorX, anchorY, cell.orientation]),
    [[TILE_WIDTH / 2, TILE_HEIGHT / 2, 2], [-TILE_WIDTH / 2, TILE_HEIGHT / 2, 3], [-TILE_WIDTH / 2, TILE_HEIGHT / 2, 4]]);
});

test('DS1 compositor keeps duplicate variants in declared library order', () => {
  const raw = (main, sub) => (1 | sub << 8 | main << 20) >>> 0;
  const first = { mainIndex: 2, subIndex: 7, orientation: 0, rarity: 1, pixels: {} };
  const second = { mainIndex: 2, subIndex: 7, orientation: 0, rarity: 4, pixels: {} };
  const plan = composeDs1Map({ width: 1, height: 1, dependencies: ['a', 'b'],
    floors: [Uint32Array.of(raw(2, 7))], walls: [], orientations: [] },
  [{ tiles: [first] }, { tiles: [second] }]);
  assert.equal(plan.identityCount, 1);
  assert.ok(plan.floors[0].tile === first || plan.floors[0].tile === second);
  assert.deepEqual([plan.floors[0].libraryIndex, plan.floors[0].tileIndex],
    plan.floors[0].tile === first ? [0, 0] : [1, 0]);
});

test('all acts declare listfile-backed presets and normalize DS1 dependency roots', () => {
  assert.equal(ACT_PRESET_ASSETS.length, 5);
  ACT_PRESET_ASSETS.forEach((asset, act0) => {
    assert.match(asset.ds1, /\.ds1$/i);
    assert.match(asset.palette, new RegExp(`ACT${act0 + 1}`, 'i'));
  });
  assert.equal(normalizeDt1Dependency('\\d2\\data/global/tiles/x.dt1'), 'data\\global\\tiles\\x.dt1');
  assert.equal(normalizeDt1Dependency('C:\\d2\\data\\global\\tiles\\wagon.tg1'), 'data\\global\\tiles\\wagon.dt1');
  assert.equal(normalizeDt1Dependency('data\\global\\tiles\\x.dt1'), 'data\\global\\tiles\\x.dt1');
  assert.throws(() => normalizeDt1Dependency(''), /non-empty/);
});
