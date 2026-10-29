import assert from 'node:assert/strict';
import { itemColourPalette } from '../play/assets/item-renderer.mjs';
import { mixTable } from '../play/assets/mix-palette.mjs';
const pal = new Uint8Array(1024); for (let k = 0; k < 256; k++) pal.set([k, 255 - k, k ^ 90, 255], k * 4);
// A synthetic grey2.dat: 21 rows of 256, row n maps k -> (k + n + 1) & 255.
const dat = new Uint8Array(21 * 256); for (let n = 0; n < 21; n++) for (let k = 0; k < 256; k++) dat[n * 256 + k] = (k + n + 1) & 255;
const read = (p) => { if (p === 'data\\global\\items\\palette\\grey2.dat') return dat; throw new Error(`missing ${p}`); };
assert.equal(itemColourPalette(pal, 0xFF, read), pal);       // no transform: the palette itself
assert.equal(mixTable(read, 1 + 3 + 32 * 3), null);          // nTrans 3 is rejected
assert.equal(mixTable(read, 1 + 21 + 32 * 2), null);         // nColor > 20 is rejected
const byte = 1 + 4 + 32 * 2, table = mixTable(read, byte);   // grey2, row 4
assert.deepEqual(table, dat.subarray(4 * 256, 5 * 256));
const out = itemColourPalette(pal, byte, read);
for (let k = 0; k < 256; k++) assert.deepEqual(out.subarray(k * 4, k * 4 + 4), pal.subarray(table[k] * 4, table[k] * 4 + 4));
assert.notDeepEqual(out, pal);
console.log('ok');
