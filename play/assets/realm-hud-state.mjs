// What a joined Battle.net game knows for its character, skill, quest and waypoint panels, from the
// packets the D2GS server sends (the same pure-JS, packet-fed pattern as assets/realm-world.mjs).
// Every answer is null until the server has sent the data behind it, so a panel with nothing to show
// stays closed instead of drawing a value nobody sent.
//
//   0x19                 gold added to the player's carried gold (byte)
//   0x1a / 0x1b          experience added (byte / word); 0x1c sets experience (dword)
//   0x1d / 0x1e / 0x1f   a stat of the player (byte / word / dword value)
//   0x20                 a stat of a unit (taken for the player's own guid)
//   0x29                 the player's quest record (96 bytes = 48 words)
//   0x63                 the waypoint menu opens, with the waypoint flags of the current difficulty
//   0x03                 a new act: an open waypoint menu is closed
// Skills (the list, the levels, the selected hands) are the wasm client state's (d2gsSkillChoices,
// d2gsSelectedSkills); this module shapes them with Skills.txt into the tree's records.

export const STAT = Object.freeze({ strength: 0, energy: 1, dexterity: 2, vitality: 3, statPoints: 4, skillPoints: 5,
  level: 12, experience: 13, gold: 14, stashGold: 15, attackRating: 19, minDamage: 21, maxDamage: 22, defense: 31,
  fireRes: 39, lightRes: 41, coldRes: 43, poisonRes: 45 });

const u16 = (p, at) => p[at] | (p[at + 1] << 8);
const u32 = (p, at) => (p[at] | (p[at + 1] << 8) | (p[at + 2] << 16) | (p[at + 3] << 24)) >>> 0;
const WAYPOINT_COUNT = 39;

/** The 0x49 the client sends to travel through the open waypoint (level 0 closes the menu). */
export function waypointTravelPacket(objectId, levelId) {
  const out = new Uint8Array(9);
  out[0] = 0x49;
  new DataView(out.buffer).setUint32(1, objectId >>> 0, true);
  new DataView(out.buffer).setUint32(5, levelId >>> 0, true);
  return out;
}

/**
 * `tables` is { experience, waypoints, skills } (assets/realm-hud-tables.mjs); `levelName(levelKey)` turns a Levels.txt
 * LevelName key into the player-facing text.
 */
export function createRealmHud({ experience, waypoints, skills }, { levelName = (key) => key } = {}) {
  let ownGuid = null;
  const stats = new Map();
  let quests = null;
  let menu = null;          // { id, flags: Uint8Array(5) } while the server's menu is open
  let flags = null;         // the last waypoint flags the server sent

  function reset() { stats.clear(); quests = null; menu = null; flags = null; }
  const own = (guid) => ownGuid !== null && guid === ownGuid;

  function handle(p) {
    switch (p[0]) {
      case 0x03: menu = null; break;
      case 0x19: if (p.length >= 2) stats.set(STAT.gold, ((stats.get(STAT.gold) ?? 0) + p[1]) >>> 0); break;
      case 0x1a: if (p.length >= 2) stats.set(STAT.experience, ((stats.get(STAT.experience) ?? 0) + p[1]) >>> 0); break;
      case 0x1b: if (p.length >= 3) stats.set(STAT.experience, ((stats.get(STAT.experience) ?? 0) + u16(p, 1)) >>> 0); break;
      case 0x1c: if (p.length >= 5) stats.set(STAT.experience, u32(p, 1)); break;
      case 0x1d: stats.set(p[1], p[2]); break;
      case 0x1e: stats.set(p[1], u16(p, 2)); break;
      case 0x1f: stats.set(p[1], u32(p, 2)); break;
      case 0x20: if (p.length >= 10 && own(u32(p, 1))) stats.set(p[5], u32(p, 6)); break;
      case 0x29: if (p.length >= 97) quests = new Uint16Array(48).map((_, i) => u16(p, 1 + i * 2)); break;
      case 0x63: if (p.length >= 21) {
        flags = Uint8Array.from(p.subarray(7, 12));
        menu = { id: u32(p, 1), flags };
      } break;
      default: break;
    }
  }

  return {
    reset,
    setOwner(guid) { ownGuid = guid === null || guid === undefined ? null : guid >>> 0; },
    feed(bytes, sizeOf) {
      for (let at = 0; at < bytes.length;) {
        const n = sizeOf(bytes.subarray(at));
        if (!(n > 0) || at + n > bytes.length) break;
        handle(bytes.subarray(at, at + n));
        at += n;
      }
    },
    stat(id) { return stats.get(id) ?? 0; },
    /** True once the server has sent the player's level: the stats panel has something to show. */
    get hasStats() { return stats.has(STAT.level); },
    /** The advancement record the character and skill panels read; null before the server's level arrives. */
    advancement(playerClass) {
      if (!stats.has(STAT.level)) return null;
      const level = stats.get(STAT.level), s = (id) => stats.get(id) ?? 0;
      return { level, experience: s(STAT.experience) | 0, nextExperience: experience.next(level, playerClass) ?? 0,
        prevExperience: experience.start(level, playerClass) ?? 0, statPoints: s(STAT.statPoints), skillPoints: s(STAT.skillPoints),
        strength: s(STAT.strength), energy: s(STAT.energy), dexterity: s(STAT.dexterity), vitality: s(STAT.vitality) };
    },
    /** Attribute totals as sent (the server's stat is the character's own; item bonuses are not decoded). */
    attributeTotals() {
      if (!stats.has(STAT.level)) return null;
      return { strength: stats.get(0) ?? 0, energy: stats.get(1) ?? 0, dexterity: stats.get(2) ?? 0, vitality: stats.get(3) ?? 0 };
    },
    /**
     * The numeric slots of the character screen the server's stats fill: 0 defense, 1 attack rating, 2-3 damage,
     * 6-9 resistances, 10 experience for the next level. Slots 4 and 5 (stamina) and 11-14 (life, mana) are
     * set by the caller from the resources the wasm client holds. A stat the server did not send is 0.
     */
    characterSlots(playerClass, slots = new Array(15).fill(0)) {
      if (!stats.has(STAT.level)) return null;
      const s = (id) => stats.get(id) ?? 0, level = stats.get(STAT.level);
      slots[0] = s(STAT.defense); slots[1] = s(STAT.attackRating); slots[2] = s(STAT.minDamage); slots[3] = s(STAT.maxDamage);
      slots[6] = s(STAT.fireRes) | 0; slots[7] = s(STAT.coldRes) | 0; slots[8] = s(STAT.lightRes) | 0; slots[9] = s(STAT.poisonRes) | 0;
      slots[10] = experience.next(level, playerClass) ?? 0;
      return slots;
    },
    /** The player's 48 quest words, or null until the server has sent the record. */
    get questWords() { return quests ? Uint16Array.from(quests) : null; },
    /**
     * The class skill tree: Skills.txt rows of the class with the level the server sent (`levels`: a map from skill id
     * to level, from the wasm client's skill list). Null before the server has sent any skill. `canLearn` is the
     * conjunction the server's 0x3B handler tests (in game, level, prerequisites, attributes, the level cap, points) so
     * the + shows only where the server will accept; the server still decides. A point cost the table gives as a
     * formula cannot be worked out here, so such a skill is not offered. No mana line is drawn.
     */
    classSkills(playerClass, levels) {
      if (!levels || levels.size === 0) return null;
      const s = (id) => stats.get(id) ?? 0, level = s(STAT.level);
      return skills.classSkills(playerClass).map((row) => {
        const baseLevel = levels.get(row.id) ?? 0;
        const canLearn = Boolean(row.inGame) && row.requiredLevel + baseLevel <= level
          && (row.reqSkills ?? []).every((id) => (levels.get(id) ?? 0) > 0)
          && (row.reqStr ?? 0) <= s(STAT.strength) && (row.reqDex ?? 0) <= s(STAT.dexterity)
          && (row.reqInt ?? 0) <= s(STAT.energy) && (row.reqVit ?? 0) <= s(STAT.vitality)
          && baseLevel < (row.maxLevel ?? 20) && !row.costFormula && s(STAT.skillPoints) >= 1;
        return { ...row, baseLevel, canLearn, manaLine: false, manaCost: 0 };
      });
    },
    /** The waypoint menu the server opened (consumed once), or null. */
    takeMenu() { const m = menu; menu = null; return m ? { id: m.id } : null; },
    get menuOpen() { return menu !== null; },
    /** The waypoint table the menu reads: rows of an act (1-based) and the acts with a tab; null before any flags. */
    waypointTable() {
      if (!flags) return null;
      const byAct = new Map();
      for (const w of waypoints.all()) {
        const act = w.act + 1;
        if (!byAct.has(act)) byAct.set(act, []);
        byAct.get(act).push({ waypointNo: w.waypointNo, levelId: w.levelId, active: Boolean(flags[w.waypointNo >> 3] & (1 << (w.waypointNo & 7))),
          name: levelName(w.nameKey) });
      }
      const tabs = [...byAct.keys()].filter((act) => byAct.get(act).some((row) => row.active)).sort((a, b) => a - b);
      return { rows: (act) => byAct.get(act) || [], tabs };
    },
  };
}
