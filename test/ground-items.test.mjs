import assert from 'node:assert/strict';
import { goldPileFrame, itemStringKey } from '../play/assets/item-renderer.mjs';

// Gold's Items.txt name key is "gld" (string 2215 "Gold"); its code is " dlg".
assert.equal(itemStringKey('dlg'), 'gld');
assert.equal(itemStringKey('cap'), 'cap');

// D2Client 0x6CC00 buckets STAT_GOLD (<100, <500, <5000, else) onto flpgld.DC6's 4 frames.
assert.deepEqual([0, 99, 100, 499, 500, 4999, 5000, 2000000000].map((n) => goldPileFrame(n, 4)), [0, 0, 1, 1, 2, 2, 3, 3]);
assert.equal(goldPileFrame(5000, 1), 0);   // a 1-frame flippy (every non-gold item) never indexes past its frames
console.log('ok');
