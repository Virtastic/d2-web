import { test } from 'node:test';
import assert from 'node:assert/strict';
import { createBlender, PL2_ALPHA_25, PL2_MULTIPLICATIVE } from '../play/assets/frontend.mjs';

// The blend before its direct-mapped cache: nearest palette entry per under pixel, PL2 table[under][source].
function reference(pl2, palette, under, frame, offset) {
  const out = Uint8ClampedArray.from(under);
  for (let k = 0; k < frame.indices.length; k++) {
    if (!frame.opaque[k]) continue;
    let best = 0, bd = Infinity;
    for (let j = 0; j < 256; j++) {
      const dr = palette[j * 4] - out[k * 4], dg = palette[j * 4 + 1] - out[k * 4 + 1], db = palette[j * 4 + 2] - out[k * 4 + 2];
      const dd = dr * dr + dg * dg + db * db; if (dd < bd) { bd = dd; best = j; }
    }
    const c = pl2[offset + best * 256 + frame.indices[k]];
    out.set([palette[c * 4], palette[c * 4 + 1], palette[c * 4 + 2], 255], k * 4);
  }
  return out;
}

test('the blender with its index cache writes the same bytes as the uncached blend, off-palette colours included', () => {
  let s = 7; const rnd = () => ((s = Math.imul(s, 1103515245) + 12345 >>> 0) >>> 8) & 0xff;
  const palette = Uint8Array.from({ length: 1024 }, (_, i) => (i & 3) === 3 ? 255 : rnd());
  const pl2 = Uint8Array.from({ length: PL2_MULTIPLICATIVE + 65536 }, rnd);
  const w = 37, h = 23, n = w * h;
  const frame = { width: w, height: h, indices: Uint8Array.from({ length: n }, rnd), opaque: Uint8Array.from({ length: n }, () => rnd() & 1) };
  // Half the under pixels are palette colours, half arbitrary RGB (the nearest-entry path), with repeats.
  const under = new Uint8ClampedArray(n * 4);
  for (let k = 0; k < n; k++) {
    const j = rnd(), off = k & 1;
    under.set(off ? [rnd() & 0xf0, rnd() & 0xf0, rnd() & 0xf0, 255] : [palette[j * 4], palette[j * 4 + 1], palette[j * 4 + 2], 255], k * 4);
  }
  const blend = createBlender(pl2, palette);
  for (let pass = 0; pass < 2; pass++) {   // the second pass is served by the cache
    let got;
    blend({ getImageData: () => ({ data: Uint8ClampedArray.from(under) }), putImageData: (img) => { got = img.data; } }, frame, 0, 0, 0);
    assert.deepEqual([...got], [...reference(pl2, palette, under, frame, PL2_ALPHA_25)]);
  }
});
