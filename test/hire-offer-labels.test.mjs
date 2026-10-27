import test from 'node:test';
import assert from 'node:assert/strict';
import { hireOfferLine } from '../play/assets/npc-menu.mjs';

test('hire offer labels come from string.tbl ids 3368/3366/3367/3369', () => {
  const tbl = { 3368: 'Niv', 3366: 'Vie', 3367: 'Def', 3369: 'Prix' };
  const o = { name: 'Kashya', level: 5, life: 90, def: 7, cost: 100 };
  assert.equal(hireOfferLine(o, (id) => tbl[id]), 'Kashya - Niv:  5  Vie: 90  Def:  7  Prix: 100');
  assert.equal(hireOfferLine(o, () => ''), 'Kashya - Lvl:  5  Life: 90  Def:  7  Cost: 100');
});
