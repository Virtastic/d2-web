import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';

const src = readFileSync(new URL('../src/core/d2moo/src/Items/Items.cpp', import.meta.url), 'utf8');
const at = src.indexOf('// 1.13c D2Common+0x23B70');
const body = src.slice(at, src.indexOf('}', at));

test('ITEMS_SetItemStartSeed: three retail guards, then the +0x10 store', () => {
  const order = ['D2_ASSERT(pItem);', 'pItem->dwUnitType == UNIT_ITEM', 'D2_ASSERT(pItem->pItemData);', 'pItemData->dwInitSeed = nSeed'];
  let last = -1;
  for (const s of order) { const i = body.indexOf(s); assert.ok(i > last, s); last = i; }
  assert.match(src, /static_assert\(offsetof\(D2ItemDataStrc, dwInitSeed\) == 0x10\)/);
});
