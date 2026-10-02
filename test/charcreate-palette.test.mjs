import { test } from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { frameCanvas, CREATE_ASSETS } from '../play/assets/charcreate.mjs';

test('character creation name box is rasterized with its screen palette', () => {
  const palette = new Uint8ClampedArray(256 * 4);
  palette.set([12, 34, 56, 255], 2 * 4);
  palette.set([78, 90, 123, 255], 4 * 4);
  const pixels = [];
  const originalImageData = globalThis.ImageData;
  globalThis.ImageData = class ImageDataFixture {
    constructor(data, width, height) { Object.assign(this, { data, width, height }); }
  };
  const documentObject = {
    createElement() {
      return {
        getContext() {
          return { putImageData(image) { pixels.push([...image.data]); } };
        },
      };
    },
  };

  try {
    frameCanvas({ width: 3, height: 1, indices: Uint8Array.of(2, 3, 4), opaque: Uint8Array.of(1, 0, 1) },
      palette, documentObject);
  } finally {
    if (originalImageData === undefined) delete globalThis.ImageData;
    else globalThis.ImageData = originalImageData;
  }

  assert.deepEqual(pixels, [[
    12, 34, 56, 255,
    0, 0, 0, 0,
    78, 90, 123, 255,
  ]]);

  // Keep the production call tied to the character-create palette, rather than the separate font palette.
  assert.match(CREATE_ASSETS.palette, /palette\\fechar\\pal\.dat$/i);
  const source = readFileSync(new URL('../play/assets/charcreate.mjs', import.meta.url), 'utf8');
  assert.match(source, /frameCanvas\(textBoxDc6\.frames\[0\], palette, documentObject\)/);
});
