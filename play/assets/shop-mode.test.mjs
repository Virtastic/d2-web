import test from 'node:test';
import assert from 'node:assert/strict';
import { shopModeOf, SHOP_MODE } from './inventory-click-route.mjs';

test('shopModeOf gives the same mode with or without an engine session', () => {
  for (const session of [null, {}]) {
    assert.equal(shopModeOf({ shopOpen: false, sell: true, repair: true, session }), SHOP_MODE.neutral);
    assert.equal(shopModeOf({ shopOpen: true, sell: true, repair: false, session }), SHOP_MODE.sell);
    assert.equal(shopModeOf({ shopOpen: true, sell: true, repair: true, session }), SHOP_MODE.repair);
    assert.equal(shopModeOf({ shopOpen: true, sell: false, repair: false, session }), SHOP_MODE.neutral);
  }
});
