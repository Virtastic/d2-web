// Client mode-change sounds, 1.13c D2Client 0x23630 (hit/death) and 0x242B0 (attack start), which retail
// runs when a unit enters a mode. Rows come from the same tables retail reads: MonStats.bin(MonSound) ->
// MonSounds.bin (record = profile id, 37 dwords) for monsters; the class pointer table at D2Client+0xF7694
// (word0 hit group, word1 death group) and the weapon table at D2Client+0xF5538 (index = Weapons.txt
// `hit class`, via D2Common #10862) for players. Limits (unverified): no queue/pump timing, group variants are not drawn (see the note at 0x5EE70 below),
// and MonSounds mode substitutions are not applied. The delay (15 ticks, prob<100) and busy gates are ported. Profile choice follows FUN_6FAD2A70
// (monster_sound_profile.h): SuperUniques.bin MonSound (u16 +0x18 of 52-byte rows, entity.superUniqueId) when typeFlags&2,
// then MonStats UMonSound (u16 +0x16) when typeFlags&0x18, else MonSound (+0x14); zero overrides fall through.
const PLAYER_HIT_DEATH = [[2878, 2883], [2910, 2915], [2894, 2899], [2886, 2891], [2902, 2907], [2926, 2931], [2918, 2923]];
const PLAYER_WEAPON = [0, 251, 262, 268, 274, 280, 286, 292, 268, 298, 0, 0, 0, 0]; // by hit class index
const HIT_CLASS = ['', 'hth', '1hss', '1hsl', '2hss', '2hsl', '1ht', '2ht', 'club', 'staf'];
const MON_HIT = new Set([3, 13]), MON_DEATH = new Set([0, 12]), MON_ATTACK = { 4: [1, 3, 4], 5: [7, 9, 10] }, MON_SKILL = { 8: 17, 9: 18, 10: 19, 11: 20 }; // MonSounds dwords: [sound, prob, weapon]; S1-S4 words (D2MOO D2MonSoundsTxt)
const PLAYER_CAST = new Set([10, 13, 14, 15, 16, 18]); // SC, S1-S4, SEQUENCE (Lightning, Chain Lightning): the skill's Skills.txt `stsound` fires on entry
const PLAYER_HIT = new Set([4, 0x13]), PLAYER_DEATH = new Set([0, 17]), PLAYER_ATTACK = new Set([7, 8]);

// D2Client 0x227E0: struck-unit impact (low nibble of the unit's last hit class) and body (high nibble) rows;
// 0x23630 submits both before the per-type hit/death sound. Row ids are Sounds.txt indices used verbatim.
const IMPACT = [0, 0x141, 0x155, 0x155, 0x155, 0x155, 0x14F, 0x14F, 0x149, 0x149, 0x15B, 0x15B, 0x145];
// Block entry (player mode 9 BL, monster mode 6 BL): Sounds.txt block_weapon_1..3 (index 398, group of 3).
const BLOCK_BASE = 398;
// D10: retail (a retail recording, 16 player blocks) plays NO sound on a player block, so only the monster branch stays.
export function blockSound(type, mode) { return (type === 1 && mode === 6) ? BLOCK_BASE : 0; }

// Group variants (D2Client 0x5EE70, LCG + recent-pick history) are NOT applied: every retail submit in the captured
// sessions plays the group base row (0 non-base rows among the 141 Q rows of the session traces, 398 plays of 325
// impact_claw alone), and a retail recording Paladin audit matched 470 of its 488 sound diffs to our variant picks. Upgrade path:
// trace the gate that keeps retail on the base (the record's variant count at +0x3D / the 0x5D7A0 entry flag +0x38 bit 0).
// Thin wrapper kept for the callers and tests that import it: the group base row is the only pick (size/hist/rng are unused).
export const pickVariant = (base) => base;
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
export function createCombatSoundTrigger(read, play, busy = () => false, roll = null, stamp = null) {
  const text = (p) => new TextDecoder('latin1').decode(read(p));
  let superUniques = null; try { superUniques = read('data\\global\\excel\\superuniques.bin'); } catch { /* optional */ }
  const monStats = read('data\\global\\excel\\monstats.bin'), monSounds = read('data\\global\\excel\\MonSounds.bin');
  const sv = new DataView(monStats.buffer, monStats.byteOffset, monStats.byteLength), mv = new DataView(monSounds.buffer, monSounds.byteOffset, monSounds.byteLength);
  const weapons = new Map(text('data\\global\\excel\\weapons.txt').split(/\r?\n/).map((l) => l.split('\t')).filter((c) => c.length > 40).map((c) => [c[3], c[40]]));
  const soundIndex = new Map(), skillSound = new Map();
  for (const l of text('data\\global\\excel\\Sounds.txt').split(/\r?\n/).slice(1)) { const c = l.split('\t'); if (c[0]) soundIndex.set(c[0], Number(c[1])); }
  // D2Client 0x14F40 runs as cltdofunc 16 (table entry 16 at 0xDEA88); for a player unit (+0 type 0) it submits Skills.txt `dosound a` (+0x102)
  const doSound = new Map(); // skill id -> dosound A row, only for cltdofunc 16 (Jab)
  // a retail recording: retail never submits an Assassin skill's stsound (assassin_* rows, 0 of the 51 a retail recording traces; the cast tick plays 2720 or 251),
  // so Skills.txt charclass 'ass' is left out. ponytail: class gate, the cause in the 0xA1CF0 chain is not traced; drop it when that is.
  { const rows = text('data\\global\\excel\\skills.txt').split(/\r?\n/).map((l) => l.split('\t')), h = rows[0], id = h.indexOf('Id'), st = h.indexOf('stsound'), pg = h.indexOf('prgsound'), fn = h.indexOf('cltdofunc'), da = h.indexOf('dosound a'), cc = h.indexOf('charclass');
    for (const c of rows.slice(1)) {
      // prgsound is the cast sound when stsound is empty (Arctic Blast: druid_windcast, Sounds row 2352)
      const cast = soundIndex.has(c[st]) ? c[st] : c[pg];
      if (cast && soundIndex.has(cast) && c[cc] !== 'ass') skillSound.set(Number(c[id]), soundIndex.get(cast));
      if (Number(c[fn]) === 16 && soundIndex.has(c[da])) doSound.set(Number(c[id]), soundIndex.get(c[da])); } }
  const dmgPrev = new Set(), prev = new Map(), hpPrev = new Map(), frames = new Map(), hist = new Map(), rng = { s: 1n };
  let refusedSeq, pendingRefused = 0, tick = 0, lastRoll = -99;
  const profile = (classId, typeFlags = 0, superUniqueId = -1) => {
    if (classId < 0 || 4 + (classId + 1) * 424 > sv.byteLength) return null;
    const flags = typeFlags | 0, base = 4 + classId * 424;
    let superId = 0;
    if ((flags & 2) && superUniques && superUniqueId >= 0 && 4 + (superUniqueId + 1) * 52 <= superUniques.byteLength)
      superId = new DataView(superUniques.buffer, superUniques.byteOffset, superUniques.byteLength).getUint16(4 + superUniqueId * 52 + 0x18, true);
    const id = selectProfileId((flags & 2) !== 0, (flags & 0x18) !== 0, superId, sv.getUint16(base + 0x16, true), sv.getUint16(base + 0x14, true));
    return id >= 0 && id < mv.getUint32(0, true) ? Array.from({ length: 37 }, (_, i) => mv.getUint32(4 + id * 148 + i * 4, true)) : null;
  };
  const row = (base) => pickVariant(base);
  const fire = (base, e, kind) => { if (base > 0) play(row(base), { unitType: e.type, unitId: e.id, classId: e.classId, kind, mode: e.mode, baseRow: base }); };
  // 0x242B0: the attack and weapon rows are submitted raw (SNDPLAY with the table id, no 0x5EE70 pick); the roll advances the player's
  // LCG (the same state the variant picks draw from), so one rng object serves both.
  const lcgStep = () => { rng.s = ((rng.s & 0xFFFFFFFFn) * 0x6AC690C5n + (rng.s >> 32n)) & 0xFFFFFFFFFFFFFFFFn; };
  const playRaw = (base, e, kind) => { if (base > 0) play(base, { unitType: e.type, unitId: e.id, classId: e.classId, kind, mode: e.mode, baseRow: base }); };
  const weaponHitClass = (player, items) => {
    const w = (items || []).find((i) => i.mode === 1 && (Number(i.id) >>> 0) === (Number(player?.primaryWeaponId) >>> 0));
    if (!w) return 0; // unarmed: retail's CVT (#10862) selector is 0 -> table[0] = 0, the 0x88A70 submit is rejected (a retail recording Q 0 at each swing); no fist swing sound
    const code = typeof w.code === 'string' ? w.code : String.fromCharCode(...[0, 8, 16, 24].map((s) => (Number(w.code) >>> s) & 255));
    return Math.max(1, HIT_CLASS.indexOf(weapons.get(code.replace(/\0/g, '').trim()) ?? ''));
  };
  return {
    /** Call once per engine tick with the snapshot's entities (+player/items for the hero's weapon). */
    update(entities, snapshot) {
      tick++;
      { const q = snapshot?.player?.castRefusedSeq; if (q !== undefined) { if (pendingRefused) { const h = entities.find((x) => x.id === snapshot.playerId && x.type === 0); if (h) fire(skillSound.get(pendingRefused) ?? 0, h, 'cast'); pendingRefused = 0; } if (refusedSeq !== undefined && q !== refusedSeq) pendingRefused = snapshot.player.castRefusedSkill; refusedSeq = q; } } // a refused cast still plays the skill's stsound at entry (retail); one update late, as the mode-change cues drain
      const seen = new Set();
      for (const e00 of entities) {
        if (e00.type > 1 || (e00.type === 1 && e00.animation?.inClientRoom === false)) continue; // 0x23630/0x242B0 run only for units the client holds (room with a client), as 0x23C00
        // D2Common #10175 UNITS_GetAnimOrSeqMode (via D2Client 0x223D0): a monster in SQ (14) with an animation sequence reports the sequence's mode
        const e0 = e00.type === 1 && e00.mode === 14 && e00.animation?.hasAnimSequence ? { ...e00, mode: e00.animation.seqMode } : e00;
        const key = `${e0.type}:${e0.id}`; seen.add(key);
        // a retail recording Q records: retail submits the hit sounds when the unit TAKES DAMAGE, whatever its mode (a hero swinging, a zombie
        // mid-attack keep their mode); a hit that does not change the mode is a virtual GH entry, a killing hit submits the impact only.
        const ph = hpPrev.get(key); hpPrev.set(key, e0.hp);
        // Sacrifice (96) charges the caster's own HP in its do frame, mid-swing: no SUCCESSFULHIT, lastHitClass stays stale, retail plays no impact (a retail recording)
        const sac = e0.id === snapshot?.playerId && snapshot.player?.castSkillId === 96 && PLAYER_ATTACK.has(e0.mode) && prev.get(key) === e0.mode;
        const castHit = e0.type === 0 && PLAYER_CAST.has(e0.mode); // a retail recording: a zombie claw landing on a player in a cast mode (Shiver Armor) plays no impact or hit cue, and the client got no life update for it; unit-level cause not traced
        const dmgRaw = !sac && !castHit && ph !== undefined && e0.hp < ph && e0.lastHitClass !== undefined && !e0.critter, dot = dmgRaw && dmgPrev.has(key) && e0.lastHitClass === 93 && !(e0.type === 1 ? MON_HIT : PLAYER_HIT).has(e0.mode); // poison ticks (hit class 93) after the first: no new hit, retail plays it once
        dmgRaw ? dmgPrev.add(key) : dmgPrev.delete(key);
        const dmg = dmgRaw && !dot;
        const e = dmg && e0.hp > 0 && !(e0.type === 1 ? MON_HIT : PLAYER_HIT).has(e0.mode) ? { ...e0, mode: e0.type === 1 ? 3 : 4 } : e0;
        const fp = e.animation?.framePrecise ?? (e.id === snapshot?.playerId && e.type === 0 ? snapshot.player?.framePrecise : undefined), hitMode = e.type === 1 ? MON_HIT.has(e.mode) : e.type === 0 && PLAYER_HIT.has(e.mode), restart = (((e.id === snapshot?.playerId && (PLAYER_CAST.has(e.mode) || PLAYER_ATTACK.has(e.mode)))) && fp < frames.get(key)) || (hitMode && (dmg || (e0.hp === undefined && fp <= frames.get(key)))); // D23: a unit struck again while still in GH re-runs SetMode(GH) (retail 0x23630 each time): a damage tick is the re-entry (a retail recording Q, not the frame clock) // same attack mode re-entered back to back: SetMode ran again, the frame clock rewound
        if (fp !== undefined) frames.set(key, fp);
        const old = prev.get(key); prev.set(key, e0.mode);
        if (old === undefined || (old === e.mode && !restart)) continue; // a mode CHANGE only (or an attack mode re-entered); first sight is not an entry
        if ((e.type === 1 ? e.mode === 12 : e.mode === 17) && old === 0) continue; // DT -> DD (corpse settles): the death cue already fired on entering DT
        // A monster dying while frozen/shattered (States.txt shatter column: 1, 107, 184) shatters: impact_shatter replaces the hit impact and the death cue.
        if (e.type === 1 && e.mode === 0 && (e.states?.has(1) || e.states?.has(107) || e.states?.has(184))) { play(385, { unitType: 1, unitId: e.id, classId: e.classId, kind: 'shatter', mode: e.mode, baseRow: 385 }); lastRoll = tick; stamp?.(e.id); continue; }
        if (((e.type === 1 ? MON_HIT.has(e.mode) : e.type === 0 && PLAYER_HIT.has(e.mode)) || dmg) && e.lastHitClass !== undefined)
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
            if (pass) { lcgStep(); const r = roll?.() ?? Number(rng.s & 0xFFFFFFFFn) % 100; if (r < p[b]) { lastRoll = tick; stamp?.(e.id); playRaw(p[s], e, 'attack'); } }
            playRaw(p[w], e, 'weapon');
          } else if (MON_SKILL[e.mode]) fire(p[MON_SKILL[e.mode]], e, 'skill'); else if (MON_HIT.has(e.mode)) { if (!busy()) { fire(p[13], e, 'hit'); if (p[13] === 0x2e9) { play(0x6a, { unitType: 1, unitId: e.id, classId: e.classId, kind: 'hit', mode: e.mode, baseRow: 0x6a }); play(0xc1, { unitType: 1, unitId: e.id, classId: e.classId, kind: 'hit', mode: e.mode, baseRow: 0xc1 }); } } lastRoll = tick; stamp?.(e.id); } // 0x23630 stamps the delay clock and the unit's +0x7C
          else if (MON_DEATH.has(e.mode)) { fire(p[15], e, 'death'); lastRoll = tick; stamp?.(e.id); }
        } else if (e.classId >= 0 && e.classId < 7) {
          const t = PLAYER_HIT_DEATH[e.classId];
          if (PLAYER_CAST.has(e.mode)) { if (e.id === snapshot?.playerId) fire(skillSound.get(snapshot.player?.castSkillId) ?? 0, e, 'cast'); }
          else if (PLAYER_ATTACK.has(e.mode)) { if (e.id === snapshot?.playerId) { fire(doSound.get(snapshot.player?.castSkillId) ?? 0, e, 'dosound'); } /* melee dispatch (Sacrifice, Smite): retail never enters the cast path (no 1A060/A1CF0 at the cast tick, no stsound: a retail recording pinned runs), so no stsound here */ fire(PLAYER_WEAPON[weaponHitClass(e.id === snapshot?.playerId ? snapshot.player : null, snapshot?.items)] ?? 0, e, 'swing'); }
          else if (PLAYER_HIT.has(e.mode)) { if (!busy()) fire(t[0], e, 'hit'); lastRoll = tick; }
          else if (PLAYER_DEATH.has(e.mode)) { fire(t[1], e, 'death'); lastRoll = tick; }
        }
      }
      for (const k of prev.keys()) if (!seen.has(k)) { hpPrev.delete(k); prev.delete(k); frames.delete(k); }
    },
  };
}
