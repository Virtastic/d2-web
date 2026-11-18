import test from 'node:test';
import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { inventoryClickAction, shopModeOf, SHOP_MODE } from '../play/assets/inventory-click-route.mjs';

// The engine's rule (src/core/d2client/inv_click_route.cpp) compiled natively and tabulated once, so
// the shell module is checked against the real export, not a stand-in.
const d = mkdtempSync(join(tmpdir(), 'invroute-'));
writeFileSync(join(d, 't.cpp'), `#include <cstdio>
#include <cstdint>
extern "C" int32_t d2_client_inv_click_route(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
int main(){ for(int c=1;c<=6;c+=5) for(int m=0;m<0x16;m++) for(int b=0;b<=10;b++) for(int h=0;h<2;h++) for(int k=0;k<4;k++)
  printf("%d %d %d %d %d %d\\n",c,m,b,h,k,d2_client_inv_click_route(c,m,b,h,k&1,k>>1)); }`);
execFileSync('g++', ['-std=c++17', '-o', join(d, 't'), join(d, 't.cpp'), 'src/core/d2client/inv_click_route.cpp'], { stdio: 'pipe' });
const table = new Map(execFileSync(join(d, 't')).toString().trim().split('\n').map((l) => {
  const [c, m, b, h, k, r] = l.split(' ').map(Number); return [`${c},${m},${b},${h},${k}`, r];
}));
const calls = [];
const route = (c, m, b, h, ctrl = 0, npc = 0) => { calls.push([c, m, b, h]); return table.get(`${c},${m},${b},${h},${(ctrl ? 1 : 0) | (npc ? 2 : 0)}`); };

const SLOTS = [['grid', 0], ['head', 1], ['amulet', 2], ['body', 3], ['right hand', 4], ['left hand', 5],
  ['right ring', 6], ['left ring', 7], ['belt', 8], ['boots', 9], ['gloves', 10]];
// mode -> [action on an item, action on an empty slot]; 'pickup' is the only one that puts an item on the cursor.
const EXPECT = { neutral: 'pickup', sell: 'sell', repair: 'repair' };

for (const [name, bodyLoc] of SLOTS) {
  for (const [mode, want] of Object.entries(EXPECT)) {
    test(`${name}: ${mode} mode click on an item -> ${want}`, () => {
      const act = inventoryClickAction(route, { itemId: 77, bodyLoc, mode: SHOP_MODE[mode] });
      assert.equal(act, want);
      if (mode !== 'neutral') assert.notEqual(act, 'pickup', 'nothing may end up on the cursor');
    });
    test(`${name}: ${mode} mode click on an empty ${bodyLoc ? 'slot' : 'cell'} -> none`, () => {
      assert.equal(inventoryClickAction(route, { itemId: undefined, bodyLoc, mode: SHOP_MODE[mode] }), 'none');
    });
    test(`${name}: identify armed in ${mode} mode -> identify (0x27), before the shop mode`, () => {
      assert.equal(inventoryClickAction(route, { itemId: 77, bodyLoc, targeting: true, mode: SHOP_MODE[mode] }), 'identify');
    });
  }
}

test('retail pick-up mode sets: 0xf lifts from a body slot but trades from the grid (+0x8BE10 vs +0x8BE60)', () => {
  for (const m of [0, 1, 2, 0xb, 0xc, 0xe, 0x13]) { assert.equal(table.get(`1,${m},0,1,0`), 1); assert.equal(table.get(`1,${m},3,1,0`), 1); }
  assert.equal(table.get('1,15,3,1,0'), 1); assert.equal(table.get('1,15,0,1,0'), 2);
  for (const m of [3, 4, 5, 6, 7, 8, 9, 0xa, 0xd, 0x10, 0x11, 0x12]) { assert.equal(table.get(`1,${m},0,1,0`), 2); assert.equal(table.get(`1,${m},4,1,0`), 2); }
  for (const m of [0x14, 0x15]) assert.equal(table.get(`1,${m},5,1,0`), 0);
});

test('shop mode: Repair outranks Sell, a closed shop is neutral', () => {
  assert.equal(shopModeOf({ shopOpen: false, sell: true, repair: true }), SHOP_MODE.neutral);
  assert.equal(shopModeOf({ shopOpen: true, sell: true, repair: true }), SHOP_MODE.repair);
  assert.equal(shopModeOf({ shopOpen: true, sell: true, repair: false }), SHOP_MODE.sell);
  assert.equal(shopModeOf({ shopOpen: true, sell: false, repair: false }), SHOP_MODE.neutral);
});

test('the module passes retail cursor states and the slot through to the engine', () => {
  calls.length = 0;
  inventoryClickAction(route, { itemId: 5, bodyLoc: 4, targeting: true, mode: SHOP_MODE.repair });
  inventoryClickAction(route, { itemId: 5, bodyLoc: 9, mode: SHOP_MODE.sell });
  assert.deepEqual(calls, [[6, 4, 4, 1], [1, 3, 9, 1]]);
});

test('Ctrl+click in the grid with a vendor open sells at once; with none it does nothing; slots and other modes are untouched (+0x96AA0)', () => {
  for (const m of [0, 1, 2, 0xb, 0xc, 0xe, 0x13]) {
    assert.equal(table.get(`1,${m},0,1,3`), 4); assert.equal(table.get(`1,${m},0,1,1`), 0);
    assert.equal(table.get(`1,${m},0,0,3`), 0); assert.equal(table.get(`1,${m},3,1,3`), 1);
  }
  assert.equal(table.get('1,3,0,1,3'), 2); assert.equal(table.get('6,1,0,1,3'), 3);
  assert.equal(inventoryClickAction(route, { itemId: 5, bodyLoc: 0, ctrl: true, npcOpen: true }), 'sell-direct');
  assert.equal(inventoryClickAction(route, { itemId: 5, bodyLoc: 0, ctrl: true }), 'none');
});
