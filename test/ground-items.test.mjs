import assert from 'node:assert/strict';
import { goldPileFrame, itemStringKey } from '../play/assets/item-renderer.mjs';

// Gold's Items.txt name key is "gld" (string 2215 "Gold"); its code is " dlg".
assert.equal(itemStringKey('dlg'), 'gld');
assert.equal(itemStringKey('cap'), 'cap');

// D2Client 0x6CC00 buckets STAT_GOLD (<100, <500, <5000, else) into the DC6 DIRECTION of flpgld (4 x 17 frames);
// the frame inside is framePrecise >> 8 (16 when landed).
assert.deepEqual([0, 99, 100, 499, 500, 4999, 5000, 2000000000].map((n) => goldPileFrame(n, 68, 4096, 4)), [16, 16, 33, 33, 50, 50, 67, 67]);
assert.equal(goldPileFrame(5000, 1, 4096, 1), 0);   // a 1-frame flippy never indexes past its frames
console.log('ok');
