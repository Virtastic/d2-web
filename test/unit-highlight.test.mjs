import { test } from 'node:test';
import assert from 'node:assert/strict';
import { highlightMap, HIGHLIGHT_SCALE } from '../play/assets/unit-highlight.mjs';

test('mode-7 remap uses retail integer scaling, saturation, and lowest-index ties', () => {
  const palette = new Uint8Array(256 * 3);
  for (let i = 0; i < 256; i++) palette.set([i, i, i], i * 3);

  const remap = highlightMap(palette);
  assert.equal(HIGHLIGHT_SCALE, 1.7);
  assert.equal(remap[4], 6);   // floor(4 * 170 / 100), not nearest(4 * 1.7) == 7
  assert.equal(remap[14], 23); // floor(14 * 170 / 100), not nearest(14 * 1.7) == 24
  assert.equal(remap[15], 25);
  assert.equal(remap[150], 255);
  assert.equal(remap[151], 255); // scaled channel is capped at 255

  const tiedPalette = palette.slice();
  tiedPalette.set([25, 25, 24], 24 * 3); // RGB (24,25,25): one unit below the scaled target
  tiedPalette.set([255, 0, 255], 25 * 3); // decoy between the tied entries
  tiedPalette.set([25, 25, 26], 26 * 3); // RGB (26,25,25): one unit above the scaled target
  assert.equal(highlightMap(tiedPalette)[15], 24); // equal squared distance; native strict-less keeps lower index
});
