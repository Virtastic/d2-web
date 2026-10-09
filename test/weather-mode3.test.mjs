import { test } from 'node:test';
import assert from 'node:assert/strict';
import { createBlender, PL2_ADDITIVE, PL2_MULTIPLICATIVE } from '../play/assets/frontend.mjs';
import { createWeatherRenderer } from '../play/assets/weather-renderer.mjs';

function contextOf(index, palette) {
  let pixel = Uint8ClampedArray.from(palette.subarray(index * 4, index * 4 + 4));
  return {
    getImageData() { return { data: Uint8ClampedArray.from(pixel) }; },
    putImageData(image) { pixel = image.data; },
    get pixel() { return pixel; },
  };
}

test('rain ripple DrawMode 3 applies act PL2 AdditiveBlend[under][source]', () => {
  const palette = new Uint8Array(256 * 4);
  for (let i = 0; i < 256; i++) {
    palette[i * 4] = palette[i * 4 + 1] = palette[i * 4 + 2] = i;
    palette[i * 4 + 3] = 255;
  }
  const pl2 = new Uint8Array(PL2_MULTIPLICATIVE + 65536);
  const under = 40, source = 172, additiveResult = 88, multiplyResult = 99;
  pl2[PL2_ADDITIVE + under * 256 + source] = additiveResult;
  pl2[PL2_MULTIPLICATIVE + under * 256 + source] = multiplyResult;
  const frame = { width: 1, height: 1, indices: Uint8Array.of(source), opaque: Uint8Array.of(255) };
  const ctx = contextOf(under, palette), blend = createBlender(pl2, palette);

  blend(ctx, frame, 0, 0, 3);
  assert.deepEqual([...ctx.pixel], [additiveResult, additiveResult, additiveResult, 255]);

  const control = contextOf(under, palette);
  blend(control, frame, 0, 0, 4);
  assert.deepEqual([...control.pixel], [multiplyResult, multiplyResult, multiplyResult, 255],
    'the negative control proves mode 3 selects the additive table, not another PL2 bank');
});

function onePixelTenFrameDc6(source) {
  const headerBytes = 24 + 10 * 4;
  const frameBytes = 32 + 3 + 3;
  const bytes = new Uint8Array(headerBytes + 10 * frameBytes);
  const view = new DataView(bytes.buffer);
  view.setInt32(0, 6, true);
  view.setUint32(16, 1, true);
  view.setUint32(20, 10, true);
  for (let i = 0; i < 10; i++) {
    const offset = headerBytes + i * frameBytes;
    view.setUint32(24 + i * 4, offset, true);
    view.setUint32(offset + 4, 1, true);
    view.setUint32(offset + 8, 1, true);
    view.setUint32(offset + 28, 3, true);
    bytes.set([1, source, 0x80], offset + 32);
  }
  return bytes;
}

function indexedPaletteDat() {
  const dat = new Uint8Array(256 * 3);
  for (let i = 0; i < 256; i++) dat.set([i, i, i], i * 3);
  return dat;
}

function onePixelContext(index) {
  let pixel = Uint8ClampedArray.of(index, index, index, 255);
  return {
    getImageData(x, y, width, height) {
      assert.deepEqual([x, y, width, height], [1, 0, 1, 1]);
      return { data: Uint8ClampedArray.from(pixel) };
    },
    putImageData(image, x, y) {
      assert.deepEqual([x, y], [1, 0]);
      pixel = Uint8ClampedArray.from(image.data);
    },
    drawImage(_bitmap, x, y) {
      // Model the old source-over prepaint path so the expected PL2 result rejects it.
      assert.deepEqual([x, y], [1, 0]);
      pixel = Uint8ClampedArray.of(172, 172, 172, 255);
    },
    get pixel() { return pixel; },
  };
}

test('production weather draw blends each ripple against its existing canvas underlay', () => {
  const underlay = 37, source = 172, step = source + 7;
  const pl2 = new Uint8Array(PL2_MULTIPLICATIVE + 65536);
  for (let under = 0; under < 256; under++)
    pl2[PL2_ADDITIVE + under * 256 + source] = (under + step) & 0xff;
  pl2.fill(0, PL2_MULTIPLICATIVE, PL2_MULTIPLICATIVE + 65536);

  const dc6 = onePixelTenFrameDc6(source), dat = indexedPaletteDat();
  const read = (path) => {
    const key = String(path).toLowerCase();
    if (key.endsWith('pal.dat')) return dat;
    if (key.endsWith('pal.pl2')) return pl2;
    if (key.endsWith('rain3.dc6') || key.endsWith('rain4.dc6')) return dc6;
    throw new Error(`unexpected weather fixture asset ${path}`);
  };
  const renderer = createWeatherRenderer({ read });
  renderer.setLevel(1, 0);
  const ctx = onePixelContext(underlay);
  const blits = renderer.draw(ctx, 2, 64, 0, 0, 0, [{ x: 0, y: 0, sx: 0, sy: -24 }]);

  // The renderer emits 42 far + 45 near cels at its initial full-rain cycle state. Reapply the
  // exact PL2 table transform for every opaque cel at the single overlapping fixture pixel.
  let expected = underlay;
  for (let i = 0; i < 42 + 45; i++) expected = (expected + step) & 0xff;
  assert.equal(blits.length, 87);
  assert.deepEqual([...ctx.pixel], [expected, expected, expected, 255]);
  assert.notEqual(expected, 172, 'result must retain the prepainted underlay contribution');
});

test('falling rain returns no streaks when the act palette cannot be loaded', () => {
  const renderer = createWeatherRenderer({ read: () => { throw new Error('missing palette'); } });
  renderer.setLevel(1, 0);
  assert.deepEqual(renderer.drawStreaks({}, 800, 600, 0, 0, 0), []);
  assert.match(renderer.errors[0], /palette for act 1: Error: missing palette/);
});


test('ripple submission uses native vertical anchor bounds before cel offsets', () => {
  // Own encoded fixture: all ten frames have a negative vertical offset, so rejected anchors
  // would still paint canvas pixels if visibility were incorrectly tested on the cel rectangle.
  const dc6 = new Uint8Array(64 + 10 * 38), view = new DataView(dc6.buffer);
  view.setUint32(16, 1, true); view.setUint32(20, 10, true);
  for (let i = 0; i < 10; i++) {
    const offset = 64 + i * 38;
    view.setUint32(24 + i * 4, offset, true);
    view.setUint32(offset + 4, 1, true); view.setUint32(offset + 8, 1, true);
    view.setInt32(offset + 16, -11, true); view.setUint32(offset + 28, 3, true);
    dc6.set([1, 172, 0x80], offset + 32);
  }
  const dat = new Uint8Array(768), pl2 = new Uint8Array(PL2_MULTIPLICATIVE + 65536);
  const drawAt = (y, height = 600) => {
    const renderer = createWeatherRenderer({ read(path) {
      return path.endsWith('pal.dat') ? dat : path.endsWith('pal.pl2') ? pl2 : dc6;
    } });
    renderer.setLevel(1, 0);
    let painted = 0;
    const paintedRows = [];
    const ctx = { getImageData() { return { data: Uint8ClampedArray.of(0, 0, 0, 255) }; },
      putImageData(_image, _x, top) { painted++; paintedRows.push(top); } };
    const blits = renderer.draw(ctx, 800, height, 0, 0, 0,
      [{ x: 0, y: 0, sx: 0, sy: y - height / 2 + 8 }]);
    assert.deepEqual(renderer.errors, []);
    assert.ok(paintedRows.every(top => top === y - 11), 'cel offsets follow the anchor gate');
    return { blits, painted, live: renderer.ripples };
  };
  for (const y of [0, 524, 552]) {
    const result = drawAt(y);
    assert.ok(result.blits.length > 0, `native accepts anchor ${y}`);
    assert.ok(result.painted > 0);
    assert.ok(result.blits.every(blit => blit.y === y), 'records retain the submitted anchor identity');
    assert.ok(result.live.every(ripple => ripple.sy === y));
  }
  for (const y of [-1, 553, 599]) {
    assert.deepEqual(drawAt(y), { blits: [], painted: 0, live: [] }, `native rejects anchor ${y}`);
  }
  assert.ok(drawAt(432, 480).blits.length > 0);
  assert.deepEqual(drawAt(433, 480), { blits: [], painted: 0, live: [] },
    'the cutoff follows the screen height rather than a fixed 600px HUD coordinate');
});
