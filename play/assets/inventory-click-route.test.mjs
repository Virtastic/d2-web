// Repair mode on the body slots: the engine's route (src/core/d2client/inv_click_route.cpp, compiled natively
// here) must answer "trade action", never "pick up", for every equipped slot, and the shell turns it into 'repair'.
import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';
import { inventoryClickAction, SHOP_MODE } from './inventory-click-route.mjs';

const src = join(dirname(fileURLToPath(import.meta.url)), '../../src/core/d2client/inv_click_route.cpp');
const dir = mkdtempSync(join(tmpdir(), 'route-'));
writeFileSync(join(dir, 'main.cpp'), `#include "${src}"
#include <cstdio>
#include <cstdlib>
int main(int, char** v){printf("%d",d2_client_inv_click_route(atoi(v[1]),atoi(v[2]),atoi(v[3]),atoi(v[4]),atoi(v[5]),atoi(v[6])));}`);
execFileSync('g++', ['-o', join(dir, 'm'), join(dir, 'main.cpp')]);
const route = (...a) => Number(execFileSync(join(dir, 'm'), a.map(String)));
for (let bodyLoc = 1; bodyLoc <= 10; bodyLoc++) {
  assert.equal(inventoryClickAction(route, { itemId: 7, bodyLoc, mode: SHOP_MODE.repair, npcOpen: true }), 'repair', `slot ${bodyLoc}`);
  assert.equal(inventoryClickAction(route, { itemId: 7, bodyLoc, mode: SHOP_MODE.sell, npcOpen: true }), 'sell', `slot ${bodyLoc}`);
  assert.equal(inventoryClickAction(route, { itemId: 7, bodyLoc, mode: SHOP_MODE.neutral }), 'pickup', `slot ${bodyLoc}`);
}
assert.equal(inventoryClickAction(route, { itemId: 7, bodyLoc: 0, mode: SHOP_MODE.repair, npcOpen: true }), 'repair');
console.log('inventory-click-route: ok');
