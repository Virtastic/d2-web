// The data tables a joined Battle.net game reads for its character, skill, quest and waypoint panels:
// Experience.txt, Levels.txt (the waypoint column), Skills.txt and SkillDesc.txt. Each reader takes
// `read(path)` -> bytes, the MPQ asset reader the shell already holds, and parses the file on first use.

function rows(bytes) {
  const lines = new TextDecoder('latin1').decode(bytes).replace(/\r/g, '').split('\n').filter((l) => l.length);
  const header = lines.shift().split('\t').map((name) => name.trim().toLowerCase());
  return lines.map((line) => {
    const values = line.split('\t'), row = {};
    for (let i = 0; i < header.length; i++) row[header[i]] = (values[i] ?? '').trim();
    return row;
  });
}
const num = (v, fallback = 0) => { const n = Number(v); return Number.isFinite(n) && v !== '' ? n : fallback; };

const CLASS_COLUMN = ['amazon', 'sorceress', 'necromancer', 'paladin', 'barbarian', 'druid', 'assassin'];

/** Experience.txt: row N is the total experience that completes level N. */
export function createExperienceTable(read) {
  let byLevel = null;
  function load() {
    if (byLevel) return;
    byLevel = new Map();
    for (const row of rows(read('data\\global\\excel\\experience.txt'))) {
      if (row.level === '' || Number.isNaN(Number(row.level))) continue;   // the MaxLvl row
      byLevel.set(Number(row.level), row);
    }
  }
  const at = (level, playerClass) => { load(); const row = byLevel.get(level); return row ? num(row[CLASS_COLUMN[playerClass] ?? 'amazon'], null) : null; };
  return {
    /** Where `level` begins: the previous level's total (0 for level 1), or null for a level the table lacks. */
    start(level, playerClass) { return level <= 1 ? 0 : at(level - 1, playerClass); },
    /** What `level` must reach to be completed, or null at the cap. */
    next(level, playerClass) { return at(level, playerClass); },
  };
}

/** Levels.txt waypoints: the waypoint number (the bit the server's 0x63 flags) -> level, act and name key. */
export function createWaypointTable(read) {
  let list = null;
  function load() {
    if (list) return;
    list = [];
    for (const row of rows(read('data\\global\\excel\\levels.txt'))) {
      const waypoint = num(row.waypoint, 255);
      if (row.id === '' || waypoint === 255 || waypoint < 0 || waypoint > 39) continue;
      list.push({ waypointNo: waypoint, levelId: num(row.id, -1), act: num(row.act, 0), nameKey: row.levelname ?? '' });
    }
    list.sort((a, b) => a.waypointNo - b.waypointNo);
  }
  return { all() { load(); return list; } };
}

const SKILL_CLASS_CODE = ['ama', 'sor', 'nec', 'pal', 'bar', 'dru', 'ass'];

/**
 * The class skill tree: every Skills.txt row of the class that SkillDesc.txt puts on a tree page (1..3), with
 * its page/row/column, required level, icon cel and the .tbl index of its name, description and mana
 * strings. `langIndex(key)` is the string table's key to .tbl index (d2_lang_index).
 */
export function createSkillTable(read, langIndex) {
  let descs = null, skills = null, ids = null;
  function load() {
    if (skills) return;
    descs = new Map();
    for (const row of rows(read('data\\global\\excel\\skilldesc.txt'))) if (row.skilldesc) descs.set(row.skilldesc, row);
    const all = rows(read('data\\global\\excel\\skills.txt')).filter((row) => row.id !== '');
    ids = new Map(all.map((row) => [row.skill.toLowerCase(), num(row.id)]));
    skills = all.filter((row) => row.skilldesc && descs.has(row.skilldesc));
  }
  return {
    classSkills(playerClass) {
      load();
      const code = SKILL_CLASS_CODE[playerClass], out = [];
      for (const row of skills) {
        if (row.charclass !== code) continue;
        const d = descs.get(row.skilldesc), page = num(d.skillpage, 0);
        if (page < 1 || page > 3) continue;
        out.push({ id: num(row.id), page, row: num(d.skillrow), column: num(d.skillcolumn), requiredLevel: num(row.reqlevel, 1),
          iconCel: num(d.iconcel), nameStr: langIndex(d['str name']) & 0xffff, descStr: langIndex(d['str long']) & 0xffff,
          manaStr: langIndex(d['str mana']) & 0xffff,
          // What the server's 0x3B handler tests before it spends a point (Skills.txt): the skill is in game, the
          // prerequisite skills (by id), the attributes, the level cap (0 means 20) and the points one level costs.
          inGame: num(row.ingame) === 1, reqSkills: [row.reqskill1, row.reqskill2, row.reqskill3].filter((n) => n && ids.has(n.toLowerCase())).map((n) => ids.get(n.toLowerCase())),
          reqStr: num(row.reqstr), reqDex: num(row.reqdex), reqInt: num(row.reqint), reqVit: num(row.reqvit),
          maxLevel: num(row.maxlvl) > 0 ? num(row.maxlvl) : 20, costFormula: row.skpoints !== '' });
      }
      return out;
    },
  };
}
