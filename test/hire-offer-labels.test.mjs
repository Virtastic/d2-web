import test from 'node:test';
import assert from 'node:assert/strict';
import { hireOfferLine, hireSkillText, hireWindowBox } from '../play/assets/npc-menu.mjs';

test('hire offer labels come from string.tbl ids 3368/3366/3367/3369', () => {
  const tbl = { 3368: 'Niv', 3366: 'Vie', 3367: 'Def', 3369: 'Prix' };
  const o = { name: 'Kashya', level: 5, life: 90, def: 7, cost: 100 };
  assert.equal(hireOfferLine(o, (id) => tbl[id]), 'Kashya - Niv:  5  Vie: 90  Def:  7  Prix: 100');
  assert.equal(hireOfferLine(o, () => ''), 'Kashya - Lvl:  5  Life: 90  Def:  7  Cost: 100');
});

test('hire offer skill line comes from the engine description table by record field', () => {
  const str = (id) => `s${id}`;
  assert.deepEqual([0, 1, 2, 3, 4, 5, 6, 7, 8].map((i) => hireSkillText(i, str)), ['', 's3371', 's3372', 's11090', 's11088', 's11089', 's3377', 's3376', 's3375']);
  assert.equal(hireSkillText(9, str), '');
  assert.equal(hireSkillText(-1, str), '');
});

test('hire window box sits at half the screen height less the 40 px panel, minus 195', () => {
  assert.deepEqual(hireWindowBox(800, 600), { x1: 155, y1: 85, x2: 645, y2: 435, listY: 120 });
  assert.equal(hireWindowBox(640, 480).y1, 25);
});
