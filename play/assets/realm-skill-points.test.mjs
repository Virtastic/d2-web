import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmHud, STAT } from './realm-hud-state.mjs';
import { createExperienceTable, createWaypointTable, createSkillTable } from './realm-hud-tables.mjs';
import { createRealmInput } from './realm-input.mjs';

const files = {
  'data\\global\\excel\\experience.txt': 'Level\tAmazon\n1\t500\n',
  'data\\global\\excel\\levels.txt': 'Name\tId\tAct\tWaypoint\tLevelName\n',
  'data\\global\\excel\\skilldesc.txt': 'skilldesc\tSkillPage\tSkillRow\tSkillColumn\tIconCel\tstr name\tstr long\tstr mana\na\t1\t1\t1\t0\tn\tl\tm\nb\t1\t2\t1\t0\tn\tl\tm\nc\t1\t3\t1\t0\tn\tl\tm\nd\t1\t3\t2\t0\tn\tl\tm\ne\t1\t3\t3\t0\tn\tl\tm\n',
  'data\\global\\excel\\skills.txt': ['skill\tId\tcharclass\tskilldesc\tskpoints\treqlevel\tmaxlvl\treqstr\treqdex\treqint\treqvit\treqskill1\treqskill2\treqskill3\tingame',
    'Alpha\t10\tama\ta\t\t1\t0\t0\t0\t0\t0\t\t\t\t1',
    'Beta\t11\tama\tb\t\t6\t3\t0\t0\t0\t0\tAlpha\t\t\t1',
    'Gamma\t12\tama\tc\t\t12\t0\t25\t0\t0\t0\talpha\tBETA\t\t1',
    'Delta\t13\tama\td\t\t1\t0\t0\t0\t0\t0\t\t\t\t0',
    'Epsilon\t14\tama\te\t5\t1\t0\t0\t0\t0\t0\t\t\t\t1'].join('\n') + '\n',
};
const read = (p) => new TextEncoder().encode(files[p]);
const make = () => createRealmHud({ experience: createExperienceTable(read), waypoints: createWaypointTable(read), skills: createSkillTable(read, () => 0) });
const feed = (h, ...stats) => h.feed(Uint8Array.from(stats.flatMap(([id, v]) => [0x1d, id, v])), () => 3);
const learn = (h, levels) => Object.fromEntries(h.classSkills(0, new Map(Object.entries(levels).map(([k, v]) => [Number(k), v]))).map((r) => [r.id, r.canLearn]));

test('a + is offered only where the server\'s skill handler would accept the point', () => {
  const h = make();
  feed(h, [STAT.level, 6], [STAT.skillPoints, 3], [STAT.strength, 30]);
  assert.deepEqual(learn(h, { 10: 0 }), { 10: true, 11: false, 12: false, 13: false, 14: false });   // Beta needs Alpha learned; Delta is not in game; Epsilon's cost is a formula
  assert.deepEqual(learn(h, { 10: 1 }), { 10: true, 11: true, 12: false, 13: false, 14: false });
  assert.equal(learn(h, { 10: 1, 11: 3 })[11], false);                       // Beta's cap is 3
  assert.equal(learn(h, { 10: 6 })[10], false);                              // reqlevel 1 + level 6 > character level 6
  feed(h, [STAT.level, 18]);
  assert.equal(learn(h, { 10: 1, 11: 1 })[12], true);                        // level, both prerequisites, strength
  feed(h, [STAT.strength, 24]);
  assert.equal(learn(h, { 10: 1, 11: 1 })[12], false);                       // strength short
  feed(h, [STAT.strength, 30], [STAT.skillPoints, 0]);
  assert.equal(learn(h, { 10: 1, 11: 1 })[12], false);                       // no points
});

test('the point packets are the retail layouts and refuse out-of-range arguments', () => {
  const sent = [];
  const input = createRealmInput({ transmit: (b) => { sent.push(Array.from(b)); return true; }, scene: () => ({ ready: false }) });
  assert.equal(input.addStat(2, 1), true);
  assert.equal(input.addStat(3, 5), true);
  assert.equal(input.addSkill(0x136), true);
  assert.equal(input.respawn(), true);
  assert.equal(input.addStat(16, 1), false);
  assert.equal(input.addStat(0, 101), false);
  assert.deepEqual(sent, [[0x3a, 2, 0], [0x3a, 3, 4], [0x3b, 0x36, 1], [0x41]]);
});
