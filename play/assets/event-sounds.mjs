// Footstep, item pickup/drop, gold and level-up sounds. Retail routes all of them through the D2Client sound
// queue (FUN_6FB38A70). Rows: Sounds.txt by name; footsteps by the class group base at D2Client+0xF7694 word +0x28
//: base + 4*(terrain-1) [+24 run]. Limits: terrain is retail's fallback
// category 1 (tile-flag lookup FUN_6FAD26A0 is not exported), the footfall cadence is two per walk cycle
// (the callback's due predicate is unrecovered) and the 4-row variant uses the client's LCG+history (pickVariant, D2Client 0x5EE70).
import { pickVariant } from './combat-sound-trigger.mjs';
const GAIT_BASE = [2768, 2720, 2720, 2768, 2816, 2720, 2720]; // Ama Sor Nec Pal Bar Dru Asn
import { OBJECT_MODE_SOUNDS, SHARED_ROW_CLASSES } from './object-mode-sounds.mjs';
export function createEventSounds(read, play) {
  const rows = (p) => new TextDecoder('latin1').decode(read(p)).split(/\r?\n/).map((l) => l.split('\t'));
  const idx = new Map(rows('data\\global\\excel\\Sounds.txt').slice(1).filter((c) => c[0]).map((c) => [c[0], Number(c[1])]));
  const drop = new Map();
  for (const f of ['armor', 'weapons', 'misc']) {
    const [h, ...r] = rows(`data\\global\\excel\\${f}.txt`), code = h.indexOf('code'), ds = h.indexOf('dropsound'), df = h.indexOf('dropsfxframe');
    for (const c of r) if (c[code] && idx.has(c[ds])) drop.set(c[code], { row: idx.get(c[ds]), frame: Number(c[df]) || 0 });
  }
  const objMode = new Map(); // object id -> last mode
  const seen = new Map(); // item id -> { ground, code, played }
  // The snapshot's code is dwCode, the 4 ASCII bytes little-endian ('hax ').
  const codeOf = (c) => (typeof c === 'string' ? c : String.fromCharCode(c & 255, (c >> 8) & 255, (c >> 16) & 255, (c >>> 24) & 255)).replace(/[ \0]+$/, '');
  let level = 0, lastFrame = -1, serverLevelUps = 0;
  const hist = new Map(), rng = { s: 1n };
  return {
    /** The engine's own level-up event (soundEvent 2 -> row 7) was played; the level poll below must not repeat it. */
    noteServerLevelUp() { serverLevelUps++; },
    /** Per tick: snapshot.player (+ .motion), snapshot.items, and the hero's level. */
    update(snapshot, heroLevel) {
      const p = snapshot?.player, m = p?.motion?.active ? p.motion : p;
      if (m && (m.mode === 2 || m.mode === 3) && m.frameCountPrecise > 0) {
        const f = Math.floor(((m.framePrecise / m.frameCountPrecise) % 1) * 2); // 0|1 half of the cycle
        if (f !== lastFrame) { lastFrame = f; play(pickVariant((GAIT_BASE[p.classId] ?? 2768) + (m.mode === 3 ? 24 : 0), 4, hist, rng), { kind: 'footstep' }); }
      } else lastFrame = -1;
      const now = new Map();
      for (const it of snapshot?.items ?? []) {
        const ground = it.mode === 3 && it.invPage === 255, code = codeOf(it.code), old = seen.get(it.id), d = drop.get(code);
        // Retail: a dropping item (mode 5) plays Items.txt dropsound when its flip reaches dropsfxframe; any other
        // arrival on the ground (cursor drop) plays it at once. Once per item.
        let played = old?.played ?? false;
        if (d && !played && ((it.mode === 5 && it.invPage === 255 && (it.framePrecise >>> 8) >= d.frame) || (old && !old.ground && ground))) { play(d.row, { kind: 'drop' }); played = true; }
        now.set(it.id, { ground, code, played: (ground || it.mode === 5) && played });
      }
      for (const [id, old] of seen) if (old.ground && !(now.get(id)?.ground)) play(idx.get(old.code === 'gld' ? 'item_gold' : 'item_pickup'), { kind: old.code === 'gld' ? 'gold' : 'pickup' });
      seen.clear(); for (const [k, v] of now) seen.set(k, v);
      // D2Client+0x237E0 object branch: a seen object entering mode m plays row[m] of its class record (table
      // D2Client+0xF6C58, play/assets/object-mode-sounds.mjs). The shared row (0xF6988) plays only if previous <= m.
      // Class 0x3B also plays on first sight. Chests: OP (Mode1=1) or ON (Mode1=0) entry plays object_chest_*.
      const nowObj = new Map();
      for (const o of snapshot?.objects ?? []) {
        nowObj.set(o.id, o.mode);
        const prev = objMode.get(o.id), row = OBJECT_MODE_SOUNDS[o.classId];
        if (!row || prev === o.mode || (prev === undefined && o.classId !== 0x3b)) continue;
        if (row[o.mode] && !(SHARED_ROW_CLASSES.has(o.classId) && (prev ?? 0) > o.mode)) play(row[o.mode], { kind: 'object', id: o.id });
      }
      objMode.clear(); for (const [k, v] of nowObj) objMode.set(k, v);
      if (heroLevel > 0) { if (level && heroLevel > level) { if (serverLevelUps > 0) serverLevelUps--; else play(idx.get('cursor_level_up'), { kind: 'levelup' }); } level = heroLevel; }
    },
  };
}
