// Client mode-change sounds, 1.13c D2Client 0x23630 (hit/death) and 0x242B0 (attack start), which retail
// runs when a unit enters a mode. Rows come from the same tables retail reads: MonStats.bin(MonSound) ->
// MonSounds.bin (record = profile id, 37 dwords) for monsters; the class pointer table at D2Client+0xF7694
// (word0 hit group, word1 death group) and the weapon table at D2Client+0xF5538 (index = Weapons.txt
// `hit class`, via D2Common #10862) for players. Limits (unverified): no queue/pump timing, a group's variant uses the client's LCG+history (pickVariant),
// and MonSounds mode substitutions are not applied. The delay (15 ticks, prob<100) and busy gates are ported. Profile choice follows FUN_6FAD2A70
// (monster_sound_profile.h): SuperUniques.bin MonSound (u16 +0x18 of 52-byte rows, entity.superUniqueId) when typeFlags&2,
// then MonStats UMonSound (u16 +0x16) when typeFlags&0x18, else MonSound (+0x14); zero overrides fall through.
const PLAYER_HIT_DEATH = [[2878, 2883], [2910, 2915], [2894, 2899], [2886, 2891], [2902, 2907], [2926, 2931], [2918, 2923]];
const PLAYER_WEAPON = [0, 251, 262, 268, 274, 280, 286, 292, 268, 298, 0, 0, 0, 0]; // by hit class index
const HIT_CLASS = ['', 'hth', '1hss', '1hsl', '2hss', '2hsl', '1ht', '2ht', 'club', 'staf'];
const MON_HIT = new Set([3, 13]), MON_DEATH = new Set([0, 12]), MON_ATTACK = { 4: [1, 3, 7], 5: [4, 6, 10] }, MON_SKILL = { 8: 17, 9: 18, 10: 19, 11: 20 }; // MonSounds dwords: [sound, prob, weapon]; S1-S4 words (D2MOO D2MonSoundsTxt)
const PLAYER_CAST = new Set([10, 13, 14, 15, 16]); // SC, S1-S4: the skill's Skills.txt `stsound` fires on entry
const PLAYER_HIT = new Set([4, 0x13]), PLAYER_DEATH = new Set([0, 17]), PLAYER_ATTACK = new Set([7, 8]);

// D2Client 0x227E0: struck-unit impact (low nibble of the unit's last hit class) and body (high nibble) rows;
// 0x23630 submits both before the per-type hit/death sound. Row ids are Sounds.txt indices used verbatim.
const IMPACT = [0, 0x141, 0x155, 0x155, 0x155, 0x155, 0x14F, 0x14F, 0x149, 0x149, 0x15B, 0x15B, 0x145];
// Block entry (player mode 9 BL, monster mode 6 BL): Sounds.txt block_weapon_1..3 (index 398, group of 3).
const BLOCK_BASE = 398;
export function blockSound(type, mode) { return (type === 0 && mode === 9) || (type === 1 && mode === 6) ? BLOCK_BASE : 0; }

// D2Client 0x5EE70: group of `size` rows from `base`; LCG state s' = lo32(s)*0x6AC690C5 + hi32(s) (64-bit), draw = pow2 ? (size-1)&lo : lo%size;
// redraw while the pick equals one of the last min(size-1,2) picks of this base (history at +100/+0x68). The size<4 gate on 0x59640 is assumed passing.
export function pickVariant(base, size, hist, rng) {
  if (size < 2) return base;
  const h = hist.get(base) ?? [-1, -1], avoid = Math.min(size - 1, 2);
  let r;
  for (;;) {
    rng.s = ((rng.s & 0xFFFFFFFFn) * 0x6AC690C5n + (rng.s >> 32n)) & 0xFFFFFFFFFFFFFFFFn;
    const lo = Number(rng.s & 0xFFFFFFFFn);
    r = base + ((size & (size - 1)) === 0 ? (size - 1) & lo : lo % size);
    if (!h.slice(0, avoid).includes(r)) break;
  }
  hist.set(base, [r, h[0]]);
  return r;
}

export function hitSounds(hc, type, mode, critter = false) {
  if (type === 1 && critter) return [];
  const a = IMPACT[hc & 15] ?? 0, h = hc & 0xF0, melee = (type === 0 || type === 1) && mode === 0;
  let b = 0;
  if (h === 0x10) b = a ? 0x17B : 0; else if (h === 0x20) b = a ? 0x164 : 0x161; else if (h === 0x30) { if (!a) b = 0x167; else if (!melee) b = 0x16A; }
  else if (h === 0x40) b = a ? 0x174 : 0x170; else if (h === 0x50) { if (!a || !melee) b = 0x178; } else if (h === 0x60) b = 0x17E;
  else if (h === 0x70) b = 0x18B; else if (h === 0x80) b = 0x956; else if (h === 0xB0) b = 0x187;
  return [a, b].filter(Boolean);
}

export function selectProfileId(isSuper, isUniqueOrMinion, superId, uniqueId, classId) {
  if (isSuper && superId > 0) return superId;
  if (isUniqueOrMinion && uniqueId > 0) return uniqueId;
  return classId >= 0 ? classId : -1;
}

// busy: optional () => bool, retail FUN_6fb37bd0(0): a monster-range sound (rows 0xB76..0x1230) is still playing.
export function createCombatSoundTrigger(read, play, busy = () => false) {
  const text = (p) => new TextDecoder('latin1').decode(read(p));
  let superUniques = null; try { superUniques = read('data\\global\\excel\\superuniques.bin'); } catch { /* optional */ }
  const monStats = read('data\\global\\excel\\monstats.bin'), monSounds = read('data\\global\\excel\\MonSounds.bin');
  const sv = new DataView(monStats.buffer, monStats.byteOffset, monStats.byteLength), mv = new DataView(monSounds.buffer, monSounds.byteOffset, monSounds.byteLength);
  const sounds = text('data\\global\\excel\\Sounds.txt').split(/\r?\n/).slice(1).map((l) => Number(l.split('\t')[4]) || 1);
  const weapons = new Map(text('data\\global\\excel\\weapons.txt').split(/\r?\n/).map((l) => l.split('\t')).filter((c) => c.length > 40).map((c) => [c[3], c[40]]));
  const soundIndex = new Map(), skillSound = new Map();
  for (const l of text('data\\global\\excel\\Sounds.txt').split(/\r?\n/).slice(1)) { const c = l.split('\t'); if (c[0]) soundIndex.set(c[0], Number(c[1])); }
  { const rows = text('data\\global\\excel\\skills.txt').split(/\r?\n/).map((l) => l.split('\t')), h = rows[0], id = h.indexOf('Id'), st = h.indexOf('stsound');
    for (const c of rows.slice(1)) if (c[st] && soundIndex.has(c[st])) skillSound.set(Number(c[id]), soundIndex.get(c[st])); }
  const prev = new Map(), frames = new Map(), hist = new Map(), rng = { s: 1n };
  let tick = 0, lastRoll = -99, seed = 1;
  const profile = (classId, typeFlags = 0, superUniqueId = -1) => {
    if (classId < 0 || 4 + (classId + 1) * 424 > sv.byteLength) return null;
    const flags = typeFlags | 0, base = 4 + classId * 424;
    let superId = 0;
    if ((flags & 2) && superUniques && superUniqueId >= 0 && 4 + (superUniqueId + 1) * 52 <= superUniques.byteLength)
      superId = new DataView(superUniques.buffer, superUniques.byteOffset, superUniques.byteLength).getUint16(4 + superUniqueId * 52 + 0x18, true);
    const id = selectProfileId((flags & 2) !== 0, (flags & 0x18) !== 0, superId, sv.getUint16(base + 0x16, true), sv.getUint16(base + 0x14, true));
    return id >= 0 && id < mv.getUint32(0, true) ? Array.from({ length: 37 }, (_, i) => mv.getUint32(4 + id * 148 + i * 4, true)) : null;
  };
  const row = (base) => pickVariant(base, sounds[base] || 1, hist, rng);
  const fire = (base, e, kind) => { if (base > 0) play(row(base), { unitType: e.type, unitId: e.id, classId: e.classId, kind, mode: e.mode, baseRow: base }); };
  const weaponHitClass = (player, items) => {
    const w = (items || []).find((i) => i.mode === 1 && (Number(i.id) >>> 0) === (Number(player?.primaryWeaponId) >>> 0));
    if (!w) return 1;
    const code = typeof w.code === 'string' ? w.code : String.fromCharCode(...[0, 8, 16, 24].map((s) => (Number(w.code) >>> s) & 255));
    return Math.max(1, HIT_CLASS.indexOf(weapons.get(code.replace(/\0/g, '').trim()) ?? ''));
  };
  return {
    /** Call once per engine tick with the snapshot's entities (+player/items for the hero's weapon). */
    update(entities, snapshot) {
      tick++;
      const seen = new Set();
      for (const e of entities) {
        if (e.type > 1) continue;
        const key = `${e.type}:${e.id}`; seen.add(key);
        const fp = e.animation?.framePrecise, hitMode = e.type === 1 ? MON_HIT.has(e.mode) : e.type === 0 && PLAYER_HIT.has(e.mode), restart = (((e.type === 1 && MON_ATTACK[e.mode]) || (e.id === snapshot?.playerId && (PLAYER_CAST.has(e.mode) || PLAYER_ATTACK.has(e.mode)))) && fp < frames.get(key)) || (hitMode && fp <= frames.get(key)); // D23: a unit struck again while still in GH re-runs SetMode(GH) (retail 0x23630 each time); GH frames only advance, so a non-advancing clock is a re-entry // same attack mode re-entered back to back: SetMode ran again, the frame clock rewound
        if (fp !== undefined) frames.set(key, fp);
        const old = prev.get(key); prev.set(key, e.mode);
        if (old === undefined || (old === e.mode && !restart)) continue; // a mode CHANGE only (or an attack mode re-entered); first sight is not an entry
        if ((e.type === 1 ? e.mode === 12 : e.mode === 17) && old === 0) continue; // DT -> DD (corpse settles): the death cue already fired on entering DT
        if ((e.type === 1 ? MON_HIT.has(e.mode) : e.type === 0 && PLAYER_HIT.has(e.mode)) && e.lastHitClass !== undefined)
          for (const r of hitSounds(e.lastHitClass, e.type, e.mode, e.critter)) play(r, { unitType: e.type, unitId: e.id, classId: e.classId, kind: 'impact', mode: e.mode, baseRow: r });
        if ((e.lastHitClass & 0xF0) === 0xA0 && (e.type === 1 ? MON_HIT.has(e.mode) || MON_DEATH.has(e.mode) : PLAYER_HIT.has(e.mode) || PLAYER_DEATH.has(e.mode))) continue; // 0x23630: body class 0xA0 returns before the profile hit/death cue
        if (blockSound(e.type, e.mode)) fire(blockSound(e.type, e.mode), e, 'block');
        if (e.type === 1) {
          const p = profile(e.classId, e.typeFlags, e.superUniqueId); if (!p) continue;
          if (MON_ATTACK[e.mode]) {
            const [s, b, w] = MON_ATTACK[e.mode];
            // 0x242B0: prob < 100 -> the 15-tick delay gate and the busy gate apply; prob >= 100 plays unconditionally.
            // The LCG advances only when the gate passes.
            const gated = p[b] < 100, pass = !!p[s] && (!gated || (tick - lastRoll >= 15 && !busy()));
            if (pass) { seed = (seed * 0x6ac690c5 + 1) >>> 0; if (seed % 100 < p[b]) { lastRoll = tick; fire(p[s], e, 'attack'); } }
            fire(p[w], e, 'weapon');
          } else if (MON_SKILL[e.mode]) fire(p[MON_SKILL[e.mode]], e, 'skill'); else if (MON_HIT.has(e.mode)) { if (!busy()) { fire(p[13], e, 'hit'); if (p[13] === 0x2e9) { play(0x6a, { unitType: 1, unitId: e.id, classId: e.classId, kind: 'hit', mode: e.mode, baseRow: 0x6a }); play(0xc1, { unitType: 1, unitId: e.id, classId: e.classId, kind: 'hit', mode: e.mode, baseRow: 0xc1 }); } } lastRoll = tick; } // 0x23630 stamps the delay clock
          else if (MON_DEATH.has(e.mode)) { fire(p[15], e, 'death'); lastRoll = tick; }
        } else if (e.classId >= 0 && e.classId < 7) {
          const t = PLAYER_HIT_DEATH[e.classId];
          if (PLAYER_CAST.has(e.mode)) { if (e.id === snapshot?.playerId) fire(skillSound.get(snapshot.player?.castSkillId) ?? 0, e, 'cast'); }
          else if (PLAYER_ATTACK.has(e.mode)) { if (e.id === snapshot?.playerId) fire(skillSound.get(snapshot.player?.castSkillId) ?? 0, e, 'cast'); /* A1 skills (Sacrifice, Zeal): stsound fires on skill start */ fire(PLAYER_WEAPON[weaponHitClass(e.id === snapshot?.playerId ? snapshot.player : null, snapshot?.items)] ?? 0, e, 'swing'); }
          else if (PLAYER_HIT.has(e.mode)) { if (!busy()) fire(t[0], e, 'hit'); lastRoll = tick; }
          else if (PLAYER_DEATH.has(e.mode)) { fire(t[1], e, 'death'); lastRoll = tick; }
        }
      }
      for (const k of prev.keys()) if (!seen.has(k)) { prev.delete(k); frames.delete(k); }
    },
  };
}
