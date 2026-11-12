// Client mode-change sounds. Which rows a unit entering a mode submits is the engine's: 1.13c D2Client+0x24490 and its
// callees (0x23630 hit/death with the 0x227E0 impact/body pair, 0x242B0 attack/weapon/swing, 0x22CA0 monster skill, the
// MonSounds profile 0x22A70 and its CvtMo substitutions), run by src/core/d2client/unit_mode_sound_play_24490.cpp. This
// file decides only WHEN a unit enters a mode (the snapshot's mode changes, re-entries and damage ticks) and the sounds
// 0x24490 does not cover: a player skill's stsound/dosound (0xA1CF0/0xA1B10/0x14F40, not ported), a monster's skill
// stsound/dosound, the monster block cue and the shatter cue.
const PLAYER_CAST = new Set([10, 13, 14, 15, 16, 18]); // SC, S1-S4, SEQUENCE (Lightning, Chain Lightning): the skill's Skills.txt `stsound` fires on entry
const MON_HIT = new Set([3, 13]), PLAYER_HIT = new Set([4, 0x13]), PLAYER_ATTACK = new Set([7, 8]), MON_ACT = new Set([4, 5, 8, 9, 10, 11]);
const MON_STAMP = new Set([0, 3, 12, 13]), PLAYER_STAMP = new Set([0, 4, 17, 0x13]); // struck and death modes
// 0x24490 runs 0x23630 (impact pair, then the hit or death cue) on these modes
const STRUCK = { 0: new Set([0, 4, 0x13]), 1: new Set([0, 3, 0xD]) };
// #10862 for the hero's weapon: Weapons.txt `hit class` -> the index 0x242B0 reads the swing table (D2Client+0xF5538) with
const HIT_CLASS = ['', 'hth', '1hss', '1hsl', '2hss', '2hsl', '1ht', '2ht', 'club', 'staf'];
// Block entry (monster mode 6 BL): Sounds.txt block_weapon_1..3 (index 398). Not a 0x24490 case (its monster switch has no 6).
// D10: retail (a retail recording, 16 player blocks) plays NO sound on a player block, so player mode 9 (0x24490 -> 0x22D60) is not run.
const BLOCK_BASE = 398;
export function blockSound(type, mode) { return (type === 1 && mode === 6) ? BLOCK_BASE : 0; }
const KIND = [null, 'impact', 'hit', 'death', 'attack', 'weapon', 'skill', 'swing', 'block', 'kick'];

// The engine's 0x24490 for the trigger: { modeSounds(type, id, class, typeFlags, superUnique, mode, hitClass, cvt, flags, roll),
// impactSounds(type, class, hitClass, mode) } -> [{ row, kind }] in 0x88A70's submit order. flags: 1 hero, 2 busy, 4 state 1, 8 state 146.
export function combatSoundEngine(M) {
  if (!M?._d2_client_unit_mode_sounds) return null;
  const rows = (n) => Array.from({ length: Number(n) >>> 0 }, (_, i) => { const v = Number(M._d2_client_unit_mode_sound_at(i)) >>> 0; return { row: v & 0xFFFF, kind: KIND[v >>> 16] ?? 'mode' }; });
  return {
    modeSounds: (...a) => rows(M._d2_client_unit_mode_sounds(...a)),
    impactSounds: (...a) => rows(M._d2_client_unit_impact_sounds(...a)),
  };
}

// busy: optional () => bool, retail 0x87BD0: a monster-range sound (rows 0xB76..0x1230) is still playing.
// roll: (monsterId) => the 0x242B0 draw the session made at the attack order's arrival, -1 when none.
export function createCombatSoundTrigger(read, play, busy = () => false, roll = null, engine = null) {
  const text = (p) => new TextDecoder('latin1').decode(read(p));
  const weapons = new Map(text('data\\global\\excel\\weapons.txt').split(/\r?\n/).map((l) => l.split('\t')).filter((c) => c.length > 40).map((c) => [c[3], c[40]]));
  const soundIndex = new Map(), skillSound = new Map();
  for (const l of text('data\\global\\excel\\Sounds.txt').split(/\r?\n/).slice(1)) { const c = l.split('\t'); if (c[0]) soundIndex.set(c[0], Number(c[1])); }
  // D2Client 0x14F40 runs as cltdofunc 16 (table entry 16 at 0xDEA88); for a player unit (+0 type 0) it submits Skills.txt `dosound a` (+0x102)
  const monStSound = new Map(); // skill id -> stsound row alone (0xA1CF0 +0xFC), for a monster's skill start
  const doSound = new Map(); // skill id -> dosound A row, only for cltdofunc 16 (Jab)
  const monDoSound = new Map(); // skill id -> Skills.txt `dosound` (+0x100) for a skill with a client do-function: 0xA1B10 submits it on the unit each time the do-function returns nonzero
  // a retail recording: retail never submits an Assassin skill's stsound (assassin_* rows, 0 of the 51 a retail recording traces; the cast tick plays 2720 or 251),
  // so Skills.txt charclass 'ass' is left out. ponytail: class gate, the cause in the 0xA1CF0 chain is not traced; drop it when that is.
  { const rows = text('data\\global\\excel\\skills.txt').split(/\r?\n/).map((l) => l.split('\t')), h = rows[0], id = h.indexOf('Id'), st = h.indexOf('stsound'), pg = h.indexOf('prgsound'), fn = h.indexOf('cltdofunc'), da = h.indexOf('dosound a'), cc = h.indexOf('charclass');
    for (const c of rows.slice(1)) {
      // prgsound is the cast sound when stsound is empty (Arctic Blast: druid_windcast, Sounds row 2352)
      const cast = soundIndex.has(c[st]) ? c[st] : c[pg];
      if (cast && soundIndex.has(cast) && c[cc] !== 'ass') skillSound.set(Number(c[id]), soundIndex.get(cast));
      if (soundIndex.has(c[st])) monStSound.set(Number(c[id]), soundIndex.get(c[st]));
      if (Number(c[fn]) === 16 && soundIndex.has(c[da])) doSound.set(Number(c[id]), soundIndex.get(c[da]));
      if (Number(c[fn]) > 0 && soundIndex.has(c[h.indexOf('dosound')])) monDoSound.set(Number(c[id]), soundIndex.get(c[h.indexOf('dosound')])); } }
  const dmgPrev = new Set(), prev = new Map(), hpPrev = new Map(), frames = new Map();
  let refusedSeq, pendingRefused = 0, tick = 0, lastRoll = -99;
  const fire = (row, e, kind) => { if (row > 0) play(row, { unitType: e.type, unitId: e.id, classId: e.classId, kind, mode: e.mode, baseRow: row }); };
  const weaponHitClass = (player, items) => {
    const w = (items || []).find((i) => i.mode === 1 && (Number(i.id) >>> 0) === (Number(player?.primaryWeaponId) >>> 0));
    if (!w) return 0; // unarmed: retail's CVT (#10862) selector is 0 -> table[0] = 0, the 0x88A70 submit is rejected (a retail recording Q 0 at each swing); no fist swing sound
    const code = typeof w.code === 'string' ? w.code : String.fromCharCode(...[0, 8, 16, 24].map((s) => (Number(w.code) >>> s) & 255));
    return Math.max(1, HIT_CLASS.indexOf(weapons.get(code.replace(/\0/g, '').trim()) ?? ''));
  };
  // one 0x24490 entry: the engine's rows, played in submit order
  const enter = (e, snapshot) => {
    if (!engine || (e.type === 0 && e.mode === 9)) return;
    const hero = e.type === 0 && e.id === snapshot?.playerId;
    const flags = (hero ? 1 : 0) | (busy() ? 2 : 0) | (e.states?.has(1) ? 4 : 0) | (e.states?.has(146) ? 8 : 0);
    const cvt = hero ? weaponHitClass(snapshot.player, snapshot?.items) : 0;
    // 0x242B0's delay gate (D2Client+0x11C4C4, 15 ticks since the last struck/death/attack sound) and busy gate, on the play path's
    // own entry tick: the session's draw is taken only when this entry would have drawn
    const r = e.type === 1 && (e.mode === 4 || e.mode === 5) && roll && tick - lastRoll >= 15 && !busy() ? Number(roll(e.id)) | 0 : -1;
    for (const s of engine.modeSounds(e.type, e.id >>> 0, e.classId | 0, e.typeFlags | 0, e.superUniqueId ?? -1, e.mode, (e.lastHitClass ?? 0) >>> 0, cvt, flags, r)) { fire(s.row, e, s.kind); if (s.kind === 'attack') lastRoll = tick; }
    if ((e.type === 1 ? MON_STAMP : PLAYER_STAMP).has(e.mode)) lastRoll = tick; // 0x23630 stamps the clock on a struck or death entry
  };
  return {
    /** Call once per engine tick with the snapshot's entities (+player/items for the hero's weapon). */
    update(entities, snapshot) {
      tick++;
      { const q = snapshot?.player?.castRefusedSeq; if (q !== undefined) { if (pendingRefused) { const h = entities.find((x) => x.id === snapshot.playerId && x.type === 0); if (h) fire(skillSound.get(pendingRefused) ?? 0, h, 'cast'); pendingRefused = 0; } if (refusedSeq !== undefined && q !== refusedSeq) pendingRefused = snapshot.player.castRefusedSkill; refusedSeq = q; } } // a refused cast still plays the skill's stsound at entry (retail); one update late, as the mode-change cues drain
      const seen = new Set();
      for (const e00 of entities) {
        if (e00.type > 1 || (e00.type === 1 && e00.animation?.inClientRoom === false)) continue; // 0x24490 runs only for units the client holds (room with a client), as 0x23C00
        // D2Common #10175 UNITS_GetAnimOrSeqMode (via D2Client 0x223D0): a monster in SQ (14) with an animation sequence reports the sequence's mode
        const e0 = e00.type === 1 && e00.mode === 14 && e00.animation?.hasAnimSequence ? { ...e00, mode: e00.animation.seqMode } : e00;
        const key = `${e0.type}:${e0.id}`; seen.add(key);
        // a retail recording Q records: retail submits the hit sounds when the unit TAKES DAMAGE, whatever its mode (a hero swinging, a zombie
        // mid-attack keep their mode); a hit that does not change the mode is a virtual GH entry, a killing hit submits the impact only.
        const ph = hpPrev.get(key); hpPrev.set(key, e0.hp);
        // Sacrifice (96) charges the caster's own HP in its do frame, mid-swing: no SUCCESSFULHIT, lastHitClass stays stale, retail plays no impact (a retail recording)
        const sac = e0.id === snapshot?.playerId && snapshot.player?.castSkillId === 96 && PLAYER_ATTACK.has(e0.mode) && prev.get(key) === e0.mode;
        const castHit = e0.type === 0 && PLAYER_CAST.has(e0.mode) && e0.mode !== 18; // a retail recording: a zombie claw landing on a player in a cast mode (Shiver Armor) plays no impact or hit cue, and the client got no life update for it; unit-level cause not traced
        const dmgRaw = !snapshot?.directHpLoss && !sac && !castHit && ph !== undefined && e0.hp < ph && e0.lastHitClass !== undefined && !e0.critter, dot = dmgRaw && dmgPrev.has(key) && e0.lastHitClass === 93 && !(e0.type === 1 ? MON_HIT : PLAYER_HIT).has(e0.mode); // poison ticks (hit class 93) after the first: no new hit, retail plays it once
        dmgRaw ? dmgPrev.add(key) : dmgPrev.delete(key);
        // the engine's 0x0C hit packet (message 19) on a monster runs 0x23630 even when its life byte did not move (a retail recording: Frozen Orb, 0c..130d7f, cue 2209)
        const dmg = dmgRaw ? !dot : (e0.type === 1 && !!e0.hitPacket && e0.lastHitClass !== undefined && !e0.critter);
        const e = dmg && e0.hp > 0 && !(e0.type === 1 ? MON_HIT : PLAYER_HIT).has(e0.mode) ? { ...e0, mode: e0.type === 1 ? 3 : 4 } : e0;
        const fp = e.animation?.framePrecise ?? (e.id === snapshot?.playerId && e.type === 0 ? snapshot.player?.framePrecise : undefined), hitMode = e.type === 1 ? MON_HIT.has(e.mode) : e.type === 0 && PLAYER_HIT.has(e.mode), restart = (((e.id === snapshot?.playerId && (PLAYER_CAST.has(e.mode) || PLAYER_ATTACK.has(e.mode)))) && fp < frames.get(key)) || (hitMode && ((e0.hp === undefined && fp <= frames.get(key)) || (dmg && !dmgRaw))); // D23: a unit struck again while still in GH re-runs SetMode(GH) (retail 0x23630 each time): a damage tick is the re-entry (a retail recording Q, not the frame clock) // same attack mode re-entered back to back: SetMode ran again, the frame clock rewound
        if (fp !== undefined) frames.set(key, fp);
        const old = prev.get(key); prev.set(key, e0.mode);
        // 0xA1B10, at the client animation's event frame: a monster's skill with a client do-function submits its dosound each time that returns nonzero
        if (e0.type === 1 && e0.animEvent && e0.usedSkill >= 0 && MON_ACT.has(e0.mode)) fire(monDoSound.get(e0.usedSkill) ?? 0, e0, 'dosound');
        if (old === undefined || (old === e.mode && !restart)) continue; // a mode CHANGE only (or an attack mode re-entered); first sight is not an entry
        if ((e.type === 1 ? e.mode === 12 : e.mode === 17) && old === 0) continue; // DT -> DD (corpse settles): the death cue already fired on entering DT
        // A monster dying while frozen/shattered (States.txt shatter column: 1, 107, 184) shatters: impact_shatter replaces the hit impact and the death cue.
        if (e.type === 1 && e.mode === 0 && (e.states?.has(1) || e.states?.has(107) || e.states?.has(184))) { play(385, { unitType: 1, unitId: e.id, classId: e.classId, kind: 'shatter', mode: e.mode, baseRow: 385 }); lastRoll = tick; continue; }
        // a killing hit on a unit whose mode is no 0x23630 mode: the impact pair alone
        if (dmg && engine && !STRUCK[e.type].has(e.mode)) for (const s of engine.impactSounds(e.type, e.classId | 0, (e.lastHitClass ?? 0) >>> 0, e.mode)) fire(s.row, e, s.kind);
        if (blockSound(e.type, e.mode)) fire(blockSound(e.type, e.mode), e, 'block');
        if (e.type === 1) {
          // a hit landing as the unit enters its attack: the attack entry's cue still plays (retail ran 0x24490 at the mode change; the damage cue is separate)
          if (e !== e0 && old !== e0.mode && MON_ACT.has(e0.mode)) enter(e0, snapshot);
          enter(e, snapshot);
          // 0xA1CF0 (the client's skill start on a monster's 0x4C/0x4D): after its cltstfunc it submits Skills.txt stsound on the unit, no unit-type gate (Grizzly's BearSmite: its weapon cue, then druidbear_attack_1)
          if (old !== e0.mode && e0.usedSkill >= 0 && MON_ACT.has(e0.mode)) fire(monStSound.get(e0.usedSkill) ?? 0, e0, 'cast');
        } else if (e.classId >= 0 && e.classId < 7) {
          if (PLAYER_CAST.has(e.mode)) { if (e.id === snapshot?.playerId) fire(skillSound.get(snapshot.player?.castSkillId) ?? 0, e, 'cast'); }
          else if (PLAYER_ATTACK.has(e.mode) && e.id === snapshot?.playerId) fire(doSound.get(snapshot.player?.castSkillId) ?? 0, e, 'dosound'); /* melee dispatch (Sacrifice, Smite): retail never enters the cast path (no 1A060/A1CF0 at the cast tick, no stsound: a retail recording pinned runs), so no stsound here */
          enter(e, snapshot);
        }
      }
      for (const k of prev.keys()) if (!seen.has(k)) { hpPrev.delete(k); prev.delete(k); frames.delete(k); }
    },
  };
}
