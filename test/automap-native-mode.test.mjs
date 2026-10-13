import test from 'node:test';
import assert from 'node:assert/strict';
import { createAutomapOverlay, AUTOMAP_PATHS, automapViewport, automapScrollOffset } from '../play/assets/automap-overlay.mjs';
import { nativeAutomapMode } from '../play/assets/automap-mode.mjs';

// One opaque pixel per scanline in each distinct-sized art fixture.
function dc6(width, height) {
  const bytes = new Uint8Array(28 + 32 + height * 3 + 3), view = new DataView(bytes.buffer);
  for (const [offset, value] of [[0, 6], [16, 1], [20, 1], [24, 28], [32, width], [36, height], [56, height * 3]]) view.setUint32(offset, value, true);
  for (let row = 0; row < height; row++) bytes.set([1, 7, 128], 60 + row * 3);
  return bytes;
}

test('native modes 0/1/2 retain drawn family, scale and distinct Fade behavior', () => {
  const oldImageData = globalThis.ImageData;
  globalThis.ImageData = class { constructor(data, width, height) { Object.assign(this, { data, width, height }); } };
  try {
    const palette = new Uint8Array(768);
    for (let i = 0; i < 256; i++) palette.fill(i, i * 3, i * 3 + 3);
    const text = (s) => new TextEncoder().encode(s);
    const assets = new Map([
      [AUTOMAP_PATHS.full, dc6(16, 32)], [AUTOMAP_PATHS.mini, dc6(8, 16)],
      ['data\\global\\excel\\automap.txt', text('LevelName\tTileName\tStyle\tStartSequence\tEndSequence\tCel1\n1 Town\tfl\t0\t0\t0\t0')],
      ['data\\global\\excel\\levels.txt', text('Id\tLayer\n1\t0')],
      ['data\\global\\palette\\act1\\pal.dat', palette],
      ['data\\global\\palette\\act1\\pal.pl2', new Uint8Array(500000)],
    ]);
    const overlay = createAutomapOverlay({
      read(path) { assert.ok(assets.has(path), path); return assets.get(path); },
      Module: { ccall(name) { if (name === 'd2_session_level_type') return 1; if (name === 'd2_session_level_rect') return 0; throw new Error(name); } },
      documentObject: { createElement: () => ({ getContext: () => ({ putImageData() {} }) }) },
    });
    overlay.setSaved(new Map([[0, [{ frame: 0, x: 0, y: 27, wall: false }]]]));
    overlay.toggle();
    const context = {
      images: 0, blends: 0, save() {}, restore() {}, beginPath() {}, rect() {}, clip() {},
      drawImage() { this.images++; },
      getImageData(x, y, width, height) { return { data: new Uint8ClampedArray(width * height * 4) }; },
      putImageData() { this.blends++; },
    };
    const draw = (mode, fade = 'on') => {
      context.images = context.blends = 0;
      overlay.setSettings({ mode, fade, left: 'no' });
      return overlay.draw(context, 800, 600, { level: 1, player: { x: 0, y: 0 }, warps: [] });
    };
    const full = draw(0);
    assert.equal(full.nativeMode, 0);
    assert.equal(full.drawn[0].drawMode, 0);
    assert.equal(context.blends, 1);
    const alternate = draw(2);
    assert.equal(alternate.nativeMode, 2);
    assert.equal(alternate.layout.nativeMode, 2);
    assert.equal(alternate.source, AUTOMAP_PATHS.full);
    assert.equal(alternate.divisor, 10);
    assert.deepEqual(alternate.drawn[0].at, full.drawn[0].at);
    assert.deepEqual(alternate.drawn[0].size, [16, 32]);
    assert.equal(alternate.drawn[0].drawMode, 5);
    assert.equal(context.images, 1);
    assert.equal(context.blends, 0, 'mode 2 never uses mode 0’s Fade ramp');
    const mini = draw(1, 'off');
    assert.equal(mini.nativeMode, 1);
    assert.equal(mini.source, AUTOMAP_PATHS.mini);
    assert.equal(mini.divisor, 20);
    assert.deepEqual(mini.drawn[0].size, [8, 16]);
    assert.equal(mini.drawn[0].drawMode, 1);
    assert.equal(context.blends, 1);
    assert.equal(draw('mini').drawn[0].drawMode, 5);
    assert.equal(draw('full').drawn[0].drawMode, 0);
  } finally {
    if (oldImageData === undefined) delete globalThis.ImageData;
    else globalThis.ImageData = oldImageData;
  }
});

test('native mode 1 shares mini viewport and center offset; mode 2 shares full geometry', () => {
  assert.deepEqual(automapViewport(800, 600, { mode: 1 }), automapViewport(800, 600, { mode: 'mini' }));
  assert.deepEqual(automapViewport(800, 600, { mode: 2 }), automapViewport(800, 600, { mode: 'full' }));
  assert.deepEqual(automapScrollOffset(800, 600, { mode: 1, anchorX: 96, anchorY: 0 }), { x: 154, y: 184 });
  assert.deepEqual(automapScrollOffset(800, 600, { mode: 2, anchorX: 96, anchorY: 0 }), { x: 0, y: 0 });
  for (const invalid of [3, -1, 1.5, undefined]) assert.equal(nativeAutomapMode(invalid), 0);
});
