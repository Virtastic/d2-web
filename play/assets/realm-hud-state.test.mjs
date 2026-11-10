import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmHud, waypointTravelPacket, waypointActCompletion, STAT } from './realm-hud-state.mjs';
import { createExperienceTable, createWaypointTable, createSkillTable } from './realm-hud-tables.mjs';
import { realmFrameReads } from './realm-client-view.mjs';

const enc = (text) => new TextEncoder().encode(text);
const files = {
  'data\\global\\excel\\experience.txt': 'Level\tAmazon\tSorceress\tNecromancer\tPaladin\tBarbarian\tDruid\tAssassin\tExpRatio\nMaxLvl\t99\t99\t99\t99\t99\t99\t99\t10\n0\t0\t0\t0\t0\t0\t0\t0\t1024\n1\t500\t500\t500\t500\t500\t500\t500\t1024\n2\t1500\t1500\t1500\t1500\t1500\t1500\t1500\t1024\n3\t3750\t3750\t3750\t3750\t3750\t3750\t3750\t1024\n',
  'data\\global\\excel\\levels.txt': 'Name\tId\tAct\tWaypoint\tLevelName\nNull\t0\t0\t255\t\nRogue\t1\t0\t0\tRogue Encampment\nCold\t3\t0\t1\tCold Plains\nStony\t4\t0\t2\tStony Field\nLut\t40\t1\t9\tLut Gholein\nNone\t2\t0\t255\tBlood Moor\n',
  'data\\global\\excel\\skilldesc.txt': 'skilldesc\tSkillPage\tSkillRow\tSkillColumn\tIconCel\tstr name\tstr long\tstr mana\nmagic arrow\t1\t1\t2\t0\tskillname6\tskillld6\tStrSkill3\nattack\t0\t0\t0\t0\tskillname0\tskillld0\tStrSkill3\nfire bolt\t1\t1\t1\t2\tskillname36\tskillld36\tStrSkill3\n',
  'data\\global\\excel\\skills.txt': 'skill\tId\tcharclass\tskilldesc\treqlevel\nAttack\t0\t\tattack\t1\nMagic Arrow\t6\tama\tmagic arrow\t1\nFire Bolt\t36\tsor\tfire bolt\t1\n',
};
const read = (path) => { const f = files[path]; if (f === undefined) throw new Error(path); return enc(f); };
const hud = () => createRealmHud({ experience: createExperienceTable(read), waypoints: createWaypointTable(read),
  skills: createSkillTable(read, (key) => key.length) }, { levelName: (key) => `T:${key}` });
const SIZES = { 0x03: 3, 0x1d: 3, 0x1e: 4, 0x1f: 6, 0x20: 10, 0x29: 97, 0x63: 21 };
const sizeOf = (b) => SIZES[b[0]] ?? 0;
const le32 = (v) => [v & 255, (v >>> 8) & 255, (v >>> 16) & 255, (v >>> 24) & 255];
const stat1 = (id, v) => [0x1d, id, v], stat2 = (id, v) => [0x1e, id, v & 255, v >> 8], stat4 = (id, v) => [0x1f, id, ...le32(v)];
const pack = (...packets) => Uint8Array.from(packets.flat());

test('every answer is null before the server has sent its data', () => {
  const h = hud();
  assert.equal(h.advancement(0), null);
  assert.equal(h.attributeTotals(), null);
  assert.equal(h.characterSlots(0), null);
  assert.equal(h.questWords, null);
  assert.equal(h.classSkills(0, new Map()), null);
  assert.equal(h.waypointTable(), null);
  assert.equal(h.takeMenu(), null);
});

test('stat packets of the three widths fill the advancement record, with the experience bounds from Experience.txt', () => {
  const h = hud();
  h.feed(pack(stat1(STAT.level, 2), [0x1e, STAT.strength, 30, 0], stat1(STAT.energy, 15), stat1(STAT.dexterity, 25), stat1(STAT.vitality, 20),
    stat1(STAT.statPoints, 5), stat1(STAT.skillPoints, 1), stat4(STAT.experience, 700)), sizeOf);
  assert.deepEqual(h.advancement(0), { level: 2, experience: 700, nextExperience: 1500, prevExperience: 500, statPoints: 5, skillPoints: 1,
    strength: 30, energy: 15, dexterity: 25, vitality: 20 });
  assert.deepEqual(h.attributeTotals(), { strength: 30, energy: 15, dexterity: 25, vitality: 20 });
});

test('a 0x20 stat is the player\'s only when it names the player\'s guid', () => {
  const h = hud();
  h.setOwner(0x11223344);
  h.feed(pack(stat1(STAT.level, 1), [0x20, ...le32(0x99), STAT.defense, ...le32(77)], [0x20, ...le32(0x11223344), STAT.defense, ...le32(40)]), sizeOf);
  assert.equal(h.stat(STAT.defense), 40);
});

test('the character screen slots carry only the stats the server sent; a stat never sent is 0', () => {
  const h = hud();
  h.feed(pack(stat1(STAT.level, 1), stat2(STAT.defense, 120), stat2(STAT.fireRes, 30)), sizeOf);
  const slots = h.characterSlots(0);
  assert.equal(slots[0], 120);
  assert.equal(slots[6], 30);
  assert.equal(slots[7], 0);
  assert.equal(slots[10], 500);
});

test('the character screen reads the server\'s slots under the wasm client\'s stamina, and stays closed without a level', () => {
  const h = hud();
  assert.equal(realmFrameReads({ stamina: 100 << 8, staminaMax: 120 << 8 }, null, null, 0, h).advancement, null);
  h.feed(pack(stat1(STAT.level, 1), stat1(STAT.strength, 10)), sizeOf);
  const reads = realmFrameReads({ stamina: 100 << 8, staminaMax: 120 << 8, hp: 50 << 8, hpMax: 60 << 8, mana: 20 << 8, manaMax: 30 << 8 }, null, null, 0, h);
  assert.equal(reads.advancement.level, 1);
  assert.equal(reads.attributeTotals.strength, 10);
  assert.equal(reads.characterScreen[4], 100);
  assert.equal(reads.characterScreen[5], 120);
  assert.equal(reads.characterScreen[11], 50);
  assert.equal(reads.characterScreen[14], 30);
});

test('0x29 gives the 48 quest words of the quest log', () => {
  const h = hud();
  const body = new Array(96).fill(0); body[2] = 0x81; body[3] = 0x10;   // word 1 = 0x1081
  h.feed(pack([0x29], body), sizeOf);
  const words = h.questWords;
  assert.equal(words.length, 48);
  assert.equal(words[1], 0x1081);
  assert.equal(words[0], 0);
});

test('the skill tree lists the class\'s Skills.txt rows with the levels the server sent, none before it sent any', () => {
  const h = hud();
  assert.equal(h.classSkills(0, new Map()), null);
  const tree = h.classSkills(0, new Map([[6, 3], [0, 1]]));
  assert.equal(tree.length, 1);   // Attack is general (no tree page) and Fire Bolt is the sorceress's
  assert.deepEqual({ id: tree[0].id, page: tree[0].page, row: tree[0].row, column: tree[0].column, baseLevel: tree[0].baseLevel, canLearn: tree[0].canLearn },
    { id: 6, page: 1, row: 1, column: 2, baseLevel: 3, canLearn: false });
  assert.equal(tree[0].nameStr, 'skillname6'.length);
  assert.equal(h.classSkills(1, new Map([[36, 1]]))[0].baseLevel, 1);
  assert.equal(h.classSkills(0, new Map([[99, 1]]))[0].baseLevel, 0);
});

test('0x63 opens the waypoint menu with the flags the server sent, active rows only where a bit is set', () => {
  const h = hud();
  const flags = new Array(14).fill(0); flags[0] = 0b101; flags[1] = 0b10;   // waypoints 0, 2 and 9
  h.feed(pack([0x63], le32(0x4242), [0x02, 0x01], flags), sizeOf);
  assert.deepEqual(h.takeMenu(), { id: 0x4242 });
  assert.equal(h.takeMenu(), null);
  const table = h.waypointTable();
  assert.deepEqual(table.rows(1).map((r) => [r.waypointNo, r.levelId, r.active, r.name]),
    [[0, 1, true, 'T:Rogue Encampment'], [1, 3, false, 'T:Cold Plains'], [2, 4, true, 'T:Stony Field']]);
  assert.deepEqual(table.rows(2).map((r) => [r.waypointNo, r.active]), [[9, true]]);
  assert.deepEqual(table.tabs, [1, 2]);
  assert.deepEqual(table.rows(3), []);
});

test('a new act closes an open waypoint menu, and travel and close are the 0x49 the client sends', () => {
  const h = hud();
  h.feed(pack([0x63], le32(7), [0x02, 0x01], new Array(14).fill(1)), sizeOf);
  h.feed(pack([0x03, 0, 0]), sizeOf);
  assert.equal(h.takeMenu(), null);
  assert.deepEqual([...waypointTravelPacket(0x4242, 40)], [0x49, 0x42, 0x42, 0, 0, 40, 0, 0, 0]);
  assert.deepEqual([...waypointTravelPacket(0x4242, 0)].slice(5), [0, 0, 0, 0]);
});

test('reset forgets everything', () => {
  const h = hud();
  h.feed(pack(stat1(STAT.level, 5)), sizeOf);
  h.reset();
  assert.equal(h.advancement(0), null);
});

test('the waypoint act tabs follow the server quest record: a tab click reaches the highest completed act and its row travels there', async () => {
  const { createWaypointMenuInput } = await import('./waypoint-menu-input.mjs');
  const menu = createWaypointMenuInput(), h = hud();
  const tab = (act, done) => menu.hitTest(80 + 64 * (act - 1) + 4, 70, 800, 600, [], () => null, done, 1);   // inside tab zone `act`
  // No quest record yet: the record is zero, so every tab resolves to Act I (never a dead click).
  assert.deepEqual(waypointActCompletion(h.questWords), [false, false, false, false]);
  assert.deepEqual(tab(3, waypointActCompletion(h.questWords)), { action: 'tab', act: 1 });
  // Acts I and II completed (words 7 and 15, bit 0); Act IV's word alone does not skip Act III.
  const body = new Array(96).fill(0); body[14] = 1; body[30] = 1; body[56] = 1;
  h.feed(pack([0x29], body), sizeOf);
  const done = waypointActCompletion(h.questWords);
  assert.deepEqual(done, [true, true, false, true]);
  assert.deepEqual(tab(2, done), { action: 'tab', act: 2 });
  assert.deepEqual(tab(3, done), { action: 'tab', act: 3 });
  assert.deepEqual(tab(5, done), { action: 'tab', act: 3 });
  assert.deepEqual(Array.from(waypointTravelPacket(0x21, 75)), [0x49, 0x21, 0, 0, 0, 75, 0, 0, 0]);
});
