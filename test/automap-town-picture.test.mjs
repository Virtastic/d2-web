import test from 'node:test';
import assert from 'node:assert/strict';
import { createAutomapOverlay, AUTOMAP_PATHS, TOWN_PICTURE_PATHS, townPictureCells, automapObjectCell, automapNameAt, automapTileOnScreen }
  from '../play/assets/automap-overlay.mjs';

// D2Client 0x62760: level centre (tiles) -> AX=(X-Y)*8, AY=(X+Y)*4, then the family's grid.
test('0x62760 lays the Act 4 picture 2x2, row-major, 136/90 strides from (AX-133, AY-40)', () => {
  const pic = townPictureCells(103, [25, 10, 20, 20]);   // nPos (5,2), nSize (4,4) -> centre (7,4)
  assert.equal(pic.family, 'act4');
  assert.deepEqual(pic.cells, [{ frame: 0, x: 24 - 133, y: 44 - 40 }, { frame: 1, x: 24 + 3, y: 4 },
    { frame: 2, x: -109, y: 94 }, { frame: 3, x: 27, y: 94 }]);
});

test('0x62760 centre uses integer tile halves of nSize', () => {
  const pic = townPictureCells(103, [0, 0, 15, 15]);   // nSize 3 -> half 1
  assert.deepEqual(pic.cells[0], { frame: 0, x: -133, y: 8 - 40 });
});

test('0x62760 Harrogath is 3x2 of 180x170 from (AX-250, AY-15)', () => {
  const pic = townPictureCells(109, [0, 0, 10, 10]);   // centre (1,1): AX 0, AY 8
  assert.equal(pic.family, 'extn');
  assert.deepEqual(pic.cells.map((c) => [c.frame, c.x, c.y]),
    [[0, -250, -7], [1, -70, -7], [2, 110, -7], [3, -250, 163], [4, -70, 163], [5, 110, 163]]);
});

test('0x62760 Lut Gholein skips the listed frames and starts at 20 unless the picked file is 1', () => {
  const one = townPictureCells(40, [0, 0, 10, 10], 1).cells.map((c) => c.frame);
  assert.deepEqual(one, [1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 17, 18]);
  const other = townPictureCells(40, [0, 0, 10, 10], 0).cells;
  assert.deepEqual(other.map((c) => c.frame), [21, 22, 23, 24, 26, 27, 28, 29, 31, 32, 33, 34, 37, 38]);
  assert.deepEqual([other[0].x, other[0].y], [-453 + 160, 8 - 119]);   // frame 20 (column 0) skipped, frame 21 is column 1
  assert.equal(townPictureCells(40, [0, 0, 10, 10]), null, 'no picked file: no cells rather than a guess');
  assert.equal(townPictureCells(1, [0, 0, 10, 10]), null);
});

test('0x61D00 object cell: objects.txt frame at (trunc(px/10)+1, trunc(py/10)-3), bank only in acts 3/4', () => {
  const bank = { classId: 0x10B, x: 5043, y: 5031, mode: 0 };
  assert.deepEqual(automapObjectCell(bank, 319, 3, 103), { frame: 319, x: Math.trunc(12 * 16 / 10) + 1, y: Math.trunc(10074 * 8 / 10) - 3 });
  assert.equal(automapObjectCell(bank, 319, 0, 1), null);
  assert.equal(automapObjectCell({ classId: 0x192, x: 0, y: 0 }, 5, 1, 73), null);
  assert.ok(automapObjectCell({ classId: 0x192, x: 0, y: 0 }, 5, 1, 74));
  assert.equal(automapObjectCell({ classId: 0x16E, x: 0, y: 0, mode: 1 }, 5, 0, 1), null);
  assert.equal(automapObjectCell({ classId: 2, x: 0, y: 0 }, 0, 0, 1), null);
});

test('0x6fb119e0 centres a name on the cross, bottom row 10 above it (cov-a4-automap Tyrael 400,244 -> 383,234)', () => {
  assert.deepEqual(automapNameAt(400, 244, 34), [383, 234]);
  assert.deepEqual(automapNameAt(400, 244, 35), [383, 234]);
});

test('per-frame reveal takes only tiles whose floor diamond meets the screen', () => {
  const hero = { x: 50, y: 50 };
  assert.ok(automapTileOnScreen({ worldX: 10, worldY: 10 }, hero, 400, 292, 800, 600));
  assert.ok(!automapTileOnScreen({ worldX: 30, worldY: 30 }, hero, 400, 292, 800, 600));
  assert.ok(!automapTileOnScreen({ worldX: 20, worldY: 0 }, hero, 400, 292, 800, 600));
});

function dc6(frames) {   // frames: [[w,h], ...], one opaque pixel per row
  const size = frames.reduce((n, [, h]) => n + 32 + h * 3 + 3, 0), bytes = new Uint8Array(24 + frames.length * 4 + size), v = new DataView(bytes.buffer);
  v.setUint32(0, 6, true); v.setUint32(16, 1, true); v.setUint32(20, frames.length, true);
  let at = 24 + frames.length * 4;
  frames.forEach(([w, h], i) => {
    v.setUint32(24 + i * 4, at, true); v.setUint32(at + 4, w, true); v.setUint32(at + 8, h, true); v.setUint32(at + 28, h * 3, true);
    for (let r = 0; r < h; r++) bytes.set([1, 7, 128], at + 32 + r * 3);
    at += 32 + h * 3 + 3;
  });
  return bytes;
}

test('the Act 4 picture draws after the MaxiMap cells, anchored at the hero map point (cov-a4-automap 291,292 .. 427,382)', () => {
  const oldImageData = globalThis.ImageData;
  globalThis.ImageData = class { constructor(data, width, height) { Object.assign(this, { data, width, height }); } };
  try {
    const palette = new Uint8Array(768), text = (s) => new TextEncoder().encode(s);
    const assets = new Map([
      [AUTOMAP_PATHS.full, dc6([[16, 32]])], [TOWN_PICTURE_PATHS.act4.full, dc6([[136, 90], [136, 90], [136, 90], [136, 90]])],
      ['data\\global\\excel\\automap.txt', text('LevelName\tTileName\tStyle\tStartSequence\tEndSequence\tCel1\n4 Town\tfl\t0\t0\t0\t0')],
      ['data\\global\\excel\\levels.txt', text('Id\tLayer\n103\t0')],
      ['data\\global\\palette\\act4\\pal.dat', palette], ['data\\global\\palette\\act4\\pal.pl2', new Uint8Array(500000)],
      ['data\\global\\palette\\act1\\pal.dat', palette], ['data\\global\\palette\\act1\\pal.pl2', new Uint8Array(500000)],
    ]);
    const heap = new Uint8Array(64); new Int32Array(heap.buffer, 16, 4).set([25, 10, 20, 20]);   // centre (7,4): AX 24, AY 44
    const overlay = createAutomapOverlay({
      read(path) { if (!assets.has(path)) throw new Error(path); return assets.get(path); },
      Module: { HEAPU8: heap, ccall(name) { if (name === 'd2_session_level_type') return 26; if (name === 'd2_session_level_rect') return 16; throw new Error(name); } },
      documentObject: { createElement: () => ({ getContext: () => ({ putImageData() {} }) }) },
    });
    overlay.setSaved(new Map([[0, [{ frame: 0, x: 0, y: 27, wall: false }]]]));
    overlay.toggle();
    const ctx = { save() {}, restore() {}, beginPath() {}, rect() {}, clip() {}, fillRect() {}, drawImage() {},
      getImageData(x, y, w, h) { return { data: new Uint8ClampedArray(w * h * 4) }; }, putImageData() {} };
    overlay.setSettings({ mode: 2 });
    // Hero map point (0, 5): AX - 0 = 24 and AY - 5 = 39 put frame 0's left at 400-109 and its anchor at 293-1.
    const e = overlay.draw(ctx, 800, 600, { level: 103, act: 4, player: { x: 0, y: 0 }, warps: [] }, null, { camera: { x: 3.125, y: 3.125 }, npcs: [], fadeTargetX: 400 });
    const pics = e.drawn.filter((d) => d.source === TOWN_PICTURE_PATHS.act4.full);
    assert.equal(e.drawn[0].source, AUTOMAP_PATHS.full, 'pExtras come after the MaxiMap lists');
    assert.deepEqual(pics.map((d) => [d.frame, d.at[0], d.at[1] + d.size[1] - 1]), [[0, 291, 292], [1, 427, 292], [2, 291, 382], [3, 427, 382]]);
    assert.ok(!overlay.savedCells().some((v, i) => i % 5 === 1 && v > 0), 'pictures are not written to the .ma0');
  } finally {
    if (oldImageData === undefined) delete globalThis.ImageData; else globalThis.ImageData = oldImageData;
  }
});
