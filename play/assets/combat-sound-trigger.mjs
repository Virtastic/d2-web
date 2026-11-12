// Client mode-change sounds. Which rows a unit entering a mode submits is the engine's: 1.13c D2Client+0x24490 and its
// callees (0x23630 hit/death with the 0x227E0 impact/body pair, 0x242B0 attack/weapon/swing, 0x22CA0 monster skill, the
// MonSounds profile 0x22A70 and its CvtMo substitutions), run by src/core/d2client/unit_mode_sound_play_24490.cpp. This
// file decides only WHEN a unit enters a mode (the snapshot's mode changes, re-entries and damage ticks) and the sounds
// 0x24490 does not cover: a player skill's stsound/dosound (0xA1CF0/0xA1B10/0x14F40, not ported), a monster's skill
// stsound/dosound, the monster block cue and the shatter cue. When a cue sounds is the engine's too: every cue goes to the
// client sound list (src/core/d2client/play_sound_queue.cpp: 0x88A70 submit with its start offset, the 0x88DD0 pump with
// its Defer/Stop Inst, busy and overlap rules, 0x5D7A0's group variant with its pick history on the client hero seed).
const PLAYER_CAST = new Set([10, 13, 14, 15, 16, 18]); // SC, S1-S4, SEQUENCE (Lightning, Chain Lightning): the skill's Skills.txt `stsound` fires on entry
const MON_HIT = new Set([3, 13]), PLAYER_HIT = new Set([4, 0x13]), PLAYER_ATTACK = new Set([7, 8]), MON_ACT = new Set([4, 5, 8, 9, 10, 11]);
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
// impactSounds(type, class, hitClass, mode) } -> [{ row, kind, delay }] in 0x88A70's submit order, delay = the submit's
// start offset in ticks. flags: 1 hero, 2 busy, 4 state 1, 8 state 146. roll: -2 = the play path's 0x242B0 gate decides
// (D2Client+0x11C4C4 and the queue's busy state) and takes the session's draw when it opens.
// queue: the client sound list (src/core/d2client/play_sound_queue.cpp, 0x88A70 submit / 0x88DD0 pump / 0x5D7A0 start).
export function combatSoundEngine(M) {
  if (!M?._d2_client_unit_mode_sounds) return null;
  const rows = (n) => Array.from({ length: Number(n) >>> 0 }, (_, i) => { const v = Number(M._d2_client_unit_mode_sound_at(i)) >>> 0; return { row: v & 0xFFFF, kind: KIND[v >>> 16] ?? 'mode', delay: M._d2_client_unit_mode_sound_delay_at ? Number(M._d2_client_unit_mode_sound_delay_at(i)) >>> 0 : 0 }; });
  const at = (i, f) => Number(M._d2_client_play_sound_started(i, f));
  const queue = M._d2_client_play_sound_pump ? {
    submit: (row, unit, delay = 0, flags = 0) => Number(M._d2_client_play_sound_submit(row, unit >>> 0, delay >>> 0, flags, 0, 0)) | 0,
    // one 0x88DD0 pass at sound tick `now`: the entries it started, { handle, asked, row (the variant), start }
    pump: (now) => Array.from({ length: Number(M._d2_client_play_sound_pump(now >>> 0)) | 0 }, (_, i) => ({ handle: at(i, 0) | 0, asked: at(i, 1) | 0, row: at(i, 2) | 0, start: at(i, 3) >>> 0 })),
    ended: (handle) => M._d2_client_play_sound_ended(handle),
    state: (handle) => Number(M._d2_client_play_sound_state(handle)) | 0,
    hero: (unit) => M._d2_client_play_sound_hero(unit >>> 0),
    stamp: () => M._d2_client_unit_mode_sound_stamp(),
    busy: () => Number(M._d2_client_play_sound_busy()) !== 0,
    reset: (lo = 0, hi = 0, pinned = 0) => M._d2_client_play_sound_reset(lo >>> 0, hi >>> 0, pinned),
  } : null;
  return {
    modeSounds: (...a) => rows(M._d2_client_unit_mode_sounds(...a)),
    impactSounds: (...a) => rows(M._d2_client_unit_impact_sounds(...a)),
    queue,
  };
}
// The unit a queue entry is attached to: any nonzero key unique per (type, id); the hero's is handed to queue.hero.
export const soundUnitKey = (type, id) => ((((id >>> 0) * 2) + (type & 1) + 2) % 0x100000000) >>> 0;

// engine: combatSoundEngine(Module). With its queue every cue is submitted to the client sound list with its start
// offset (`note(row, event)` records the submit); each update ends by running the list's pump on the update's tick and
// plays what it started (`play(row, event)`, event.variantRow = the variant started, event.queueHandle = the entry to
// report ended). Without it a cue plays at once.
export function createCombatSoundTrigger(read, play, engine = null, note = null) {
  const queue = engine?.queue ?? null, pending = new Map();
  queue?.reset();   // the list and its clock start with this trigger's tick count
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
  let refusedSeq, pendingRefused = 0, tick = 0;
  // 0x88A70: a cue is queued on its unit with its start offset (a Compound row may join a live entry: its first event stands)
  const fire = (row, e, kind, delay = 0) => {
    if (!(row > 0)) return;
    const event = { unitType: e.type, unitId: e.id, classId: e.classId, kind, mode: e.mode, baseRow: row };
    if (!queue) { play(row, event); return; }
    note?.(row, event);
    const h = queue.submit(row, soundUnitKey(e.type, e.id), delay);
    if (h && !pending.has(h)) pending.set(h, event);
  };
  // 0x88DD0 at this tick: play every entry of ours it started, its variant picked by 0x5D7A0 (another caller's entries
  // are that caller's to play and end)
  const pump = () => {
    for (const s of queue.pump(tick)) {
      const event = pending.get(s.handle); pending.delete(s.handle);
      if (event) play(s.asked, { ...event, baseRow: s.asked, variantRow: s.row, queueHandle: s.handle });
    }
    for (const h of pending.keys()) if (queue.state(h) !== 0) pending.delete(h);   // refused at start (0x88DD0 drops it)
  };
  const weaponHitClass = (player, items) => {
    const w = (items || []).find((i) => i.mode === 1 && (Number(i.id) >>> 0) === (Number(player?.primaryWeaponId) >>> 0));
    if (!w) return 0; // unarmed: retail's CVT (#10862) selector is 0 -> table[0] = 0, the 0x88A70 submit is rejected (a retail recording Q 0 at each swing); no fist swing sound
    const code = typeof w.code === 'string' ? w.code : String.fromCharCode(...[0, 8, 16, 24].map((s) => (Number(w.code) >>> s) & 255));
    return Math.max(1, HIT_CLASS.indexOf(weapons.get(code.replace(/\0/g, '').trim()) ?? ''));
  };
  // one 0x24490 entry: the engine's rows, submitted in order with their start offsets. The busy test (0x87BD0), the 0x242B0
  // gate on D2Client+0x11C4C4 and its stamps are the engine's (roll -2).
  const enter = (e, snapshot) => {
    if (!engine || (e.type === 0 && e.mode === 9)) return;
    const hero = e.type === 0 && e.id === snapshot?.playerId;
    const flags = (hero ? 1 : 0) | (e.states?.has(1) ? 4 : 0) | (e.states?.has(146) ? 8 : 0);
    const cvt = hero ? weaponHitClass(snapshot.player, snapshot?.items) : 0;
    const r = e.type === 1 && (e.mode === 4 || e.mode === 5) ? -2 : -1;
    for (const s of engine.modeSounds(e.type, e.id >>> 0, e.classId | 0, e.typeFlags | 0, e.superUniqueId ?? -1, e.mode, (e.lastHitClass ?? 0) >>> 0, cvt, flags, r)) fire(s.row, e, s.kind, s.delay);
  };
  return {
    /** Call once per engine tick with the snapshot's entities (+player/items for the hero's weapon). */
    update(entities, snapshot) {
      tick++;
      if (queue && snapshot?.playerId !== undefined) queue.hero(soundUnitKey(0, snapshot.playerId));
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
        if (e.type === 1 && e.mode === 0 && (e.states?.has(1) || e.states?.has(107) || e.states?.has(184))) { fire(385, e, 'shatter'); queue?.stamp(); continue; }
        // a killing hit on a unit whose mode is no 0x23630 mode: the impact pair alone
        if (dmg && engine && !STRUCK[e.type].has(e.mode)) for (const s of engine.impactSounds(e.type, e.classId | 0, (e.lastHitClass ?? 0) >>> 0, e.mode)) fire(s.row, e, s.kind, s.delay);
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
      // the frame's 0x5F020 runs 0x88DD0 after the game update (0x44B50: 0x44A80, then 0x5F020): this tick's cues start this tick
      if (queue) pump();
    },
    /** Run the queue's pump on the following ticks (no entities) until every queued cue has started or been dropped. */
    flush(limit = 512) { while (queue && pending.size && limit-- > 0) { tick++; pump(); } },
    get tick() { return tick; },
  };
}
