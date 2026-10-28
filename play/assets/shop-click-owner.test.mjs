import assert from 'node:assert/strict';
import { inventoryOwnsClick } from './shop-click-owner.mjs';
assert.equal(inventoryOwnsClick(true, true), true);   // shop open, click on inventory panel (equipped slot / empty area)
assert.equal(inventoryOwnsClick(true, false), false); // shop open, outside the panel: not the inventory's
assert.equal(inventoryOwnsClick(false, false), true); // no shop: unchanged
console.log('shop-click-owner ok');
