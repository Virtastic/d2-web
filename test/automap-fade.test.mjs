import test from 'node:test';
import assert from 'node:assert/strict';
import { createBlender, PL2_ADDITIVE, PL2_ALPHA_25, PL2_ALPHA, PL2_ALPHA_75 } from '../play/assets/frontend.mjs';

test('automap Fade DrawModes 0, 1 and 2 use their indexed PL2 AlphaBlend banks', () => {
  const pl2 = new Uint8Array(PL2_ADDITIVE + 65536);
  const banks = [[0, PL2_ALPHA_25, 31], [1, PL2_ALPHA, 41], [2, PL2_ALPHA_75, 51]];
  for (const [, offset, result] of banks) pl2[offset + 2 * 256 + 7] = result;
  const palette = new Uint8Array(256 * 4);
  for (let i = 0; i < 256; i++) { palette[i * 4] = i; palette[i * 4 + 3] = 255; }
  const frame = { width: 1, height: 1, indices: new Uint8Array([7]), opaque: new Uint8Array([1]) };
  for (const [mode, , expected] of banks) {
    const context = {
      output: null,
      getImageData() { return { data: new Uint8ClampedArray([2, 0, 0, 255]) }; },
      putImageData(image) { this.output = [...image.data]; },
    };
    createBlender(pl2, palette)(context, frame, 0, 0, mode);
    assert.deepEqual(context.output, [expected, 0, 0, 255], `DrawMode ${mode} should use PL2 AlphaBlend[${mode}][under=2][src=7]`);
  }
});
