import test from 'node:test';
import assert from 'node:assert/strict';
import { createSkillNameTable } from './realm-tables.mjs';

const enc = (text) => new TextEncoder().encode(text);
const skillsTxt = ['skill\t*id\tskilldesc\tmana\tlvlmana\tminmana\tmanashift',
  'Fire Bolt\t36\tfb\t2\t1\t1\t8', 'Guided Arrow\t22\tga\t5\t0\t1\t6', 'Free\t0\tat\t0\t0\t0\t0'].join('\n');
const read = (path) => enc(path.endsWith('skills.txt') ? skillsTxt : 'skilldesc\tstr name\nfb\tx\n');

test('the hover mana cost is mana plus the level step, shifted, with the minimum floor', () => {
  const t = createSkillNameTable(read);
  assert.equal(t.manaCost(36, 1), 2 * 256);
  assert.equal(t.manaCost(36, 5), 6 * 256);
  assert.equal(t.manaCost(22, 3), 320);          // 5 << 6, above the 256 floor
  assert.equal(createSkillNameTable((p) => enc(p.endsWith("skills.txt") ? skillsTxt.replace("5\t0\t1\t6", "1\t0\t1\t6") : "skilldesc\tstr name\n")).manaCost(22, 3), 256);   // 1 << 6 rises to the floor
  assert.equal(t.manaCost(0, 1), 0);
  assert.equal(t.manaCost(999, 1), 0);
  assert.equal(t.manaCost(36, 0), 0);
});
