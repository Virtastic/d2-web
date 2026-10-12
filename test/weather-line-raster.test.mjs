import { test } from 'node:test';
import assert from 'node:assert/strict';
import { plotRainLine } from '../play/assets/weather-line.mjs';

function reference(x0, y0, x1, y1) {
  const pixels = [];
  const steps = Math.max(Math.abs(x1 - x0), Math.abs(y1 - y0));
  for (let i = 0; i <= steps; i++) {
    const fraction = steps ? i / steps : 0;
    pixels.push([Math.round(x0 + (x1 - x0) * fraction) || 0, Math.round(y0 + (y1 - y0) * fraction) || 0, 1, 1]);
  }
  return pixels;
}

test('rain line lookup preserves the original DDA pixels across its weather bounds and fallback', () => {
  for (const x of [-1001, -7, 0, 17, 799]) for (const y of [-600, -2, 0, 301, 599]) {
    for (let dy = 0; dy <= 10; dy++) for (let dx = -4; dx <= 4; dx++) {
      const pixels = [];
      plotRainLine({ fillRect: (...rect) => pixels.push(rect.map(value => value || 0)) }, x, y, x + dx, y + dy);
      assert.deepEqual(pixels, reference(x, y, x + dx, y + dy), `origin ${x},${y}; delta ${dx},${dy}`);
    }
  }
  for (const line of [[0, 0, 12, 2], [3, -5, 0, -8], [1, 2, 1, 2]]) {
    const pixels = [];
    plotRainLine({ fillRect: (...rect) => pixels.push(rect.map(value => value || 0)) }, ...line);
    assert.deepEqual(pixels, reference(...line), `fallback line ${line}`);
  }
});
