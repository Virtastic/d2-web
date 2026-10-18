import { test } from 'node:test';
import assert from 'node:assert/strict';
import { plotRainLine } from '../play/assets/weather-line.mjs';

function reference(x0, y0, x1, y1) {
  const pixels = [];
  const steps = Math.max(Math.abs(x1 - x0), Math.abs(y1 - y0));
  for (let i = 0; i <= steps; i++) {
    const fraction = steps ? i / steps : 0;
    pixels.push([Math.round(x0 + (x1 - x0) * fraction) || 0, Math.round(y0 + (y1 - y0) * fraction) || 0]);
  }
  return pixels.sort((a, b) => a[1] - b[1] || a[0] - b[0]);
}

// The rasterizer writes merged rectangles now, so the proof is on the PIXEL SET each call covers,
// which is the whole story for one call: every pixel of a line is written once, in one colour. A
// rectangle's pixels are expanded in row-major order, then canonicalised, so a wrong pixel, a
// duplicate or a gap all fail here regardless of how the call was split into rectangles.
function drawn(x0, y0, x1, y1) {
  const pixels = [];
  plotRainLine({ fillRect: (x, y, width, height) => {
    for (let j = 0; j < height; j++) for (let i = 0; i < width; i++) pixels.push([x + i || 0, y + j || 0]);
  } }, x0, y0, x1, y1);
  return pixels.sort((a, b) => a[1] - b[1] || a[0] - b[0]);
}

test('rain line lookup preserves the original DDA pixels across its weather bounds and fallback', () => {
  for (const x of [-1001, -7, 0, 17, 799]) for (const y of [-600, -2, 0, 301, 599]) {
    for (let dy = 0; dy <= 10; dy++) for (let dx = -4; dx <= 4; dx++) {
      assert.deepEqual(drawn(x, y, x + dx, y + dy), reference(x, y, x + dx, y + dy), `origin ${x},${y}; delta ${dx},${dy}`);
    }
  }
  for (const line of [[0, 0, 12, 2], [3, -5, 0, -8], [1, 2, 1, 2]]) {
    assert.deepEqual(drawn(...line), reference(...line), `fallback line ${line}`);
  }
});
