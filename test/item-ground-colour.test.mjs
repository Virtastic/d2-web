import assert from 'node:assert/strict';
import RETAIL_MIX_HEX from '../play/assets/retail-mixpalette.mjs';
import { itemColourPalette } from '../play/assets/item-renderer.mjs';
const pal = new Uint8Array(1024); for (let k = 0; k < 256; k++) pal.set([k, 255 - k, k ^ 90, 255], k * 4);
assert.equal(itemColourPalette(pal, 0xFF), pal);            // no transform: the palette itself
const byte = Number(Object.keys(RETAIL_MIX_HEX)[0]), hex = RETAIL_MIX_HEX[byte];
const out = itemColourPalette(pal, byte);
for (let k = 0; k < 256; k++) { const t = parseInt(hex.substr(k * 2, 2), 16); assert.deepEqual(out.subarray(k * 4, k * 4 + 4), pal.subarray(t * 4, t * 4 + 4)); }
assert.notDeepEqual(out, pal);
console.log('ok');
