// Client mode-change sounds, 1.13c D2Client 0x23630 (hit/death) and 0x242B0 (attack start), which retail
// runs when a unit enters a mode. Rows come from the same tables retail reads: MonStats.bin(MonSound) ->
// MonSounds.bin (record = profile id, 37 dwords) for monsters; the class pointer table at D2Client+0xF7694
// (word0 hit group, word1 death group) and the weapon table at D2Client+0xF5538 (index = Weapons.txt
// `hit class`, via D2Common #10862) for players. Limits (unverified): no queue/pump timing, delay words
// and the busy/overlap gates are ignored, a group's variant is round-robin instead of the client's
// LCG+history, SuperUniques/UMonSound profiles and MonSounds mode substitutions are not applied.
const PLAYER_HIT_DEATH = [[2878, 2883], [2910, 2915], [2894, 2899], [2886, 2891], [2902, 2907], [2926, 2931], [2918, 2923]];
const PLAYER_WEAPON = [0, 251, 262, 268, 274, 280, 286, 292, 268, 298, 0, 0, 0, 0]; // by hit class index
const HIT_CLASS = ['', 'hth', '1hss', '1hsl', '2hss', '2hsl', '1ht', '2ht', 'club', 'staf'];
const MON_HIT = new Set([3, 13]), MON_DEATH = new Set([0, 12]), MON_ATTACK = { 4: [1, 3, 7], 5: [4, 6, 10] }, MON_SKILL = { 8: 17, 9: 18, 10: 19, 11: 20 }; // MonSounds dwords: [sound, prob, weapon]; S1-S4 words (D2MOO D2MonSoundsTxt)
const PLAYER_CAST = new Set([10, 13, 14, 15, 16]); // SC, S1-S4: the skill's Skills.txt `stsound` fires on entry
const PLAYER_HIT = new Set([4, 0x13]), PLAYER_DEATH = new Set([0, 17]), PLAYER_ATTACK = new Set([7, 8]);

export function createCombatSoundTrigger(read, play) {
  const text = (p) => new TextDecoder('latin1').decode(read(p));
  const monStats = read('data\\global\\excel\\monstats.bin'), monSounds = read('data\\global\\excel\\MonSounds.bin');
  const sv = new DataView(monStats.buffer, monStats.byteOffset, monStats.byteLength), mv = new DataView(monSounds.buffer, monSounds.byteOffset, monSounds.byteLength);
  const sounds = text('data\\global\\excel\\Sounds.txt').split(/\r?\n/).slice(1).map((l) => Number(l.split('\t')[4]) || 1);
  const weapons = new Map(text('data\\global\\excel\\weapons.txt').split(/\r?\n/).map((l) => l.split('\t')).filter((c) => c.length > 40).map((c) => [c[3], c[40]]));
  const soundIndex = new Map(), skillSound = new Map();
  for (const l of text('data\\global\\excel\\Sounds.txt').split(/\r?\n/).slice(1)) { const c = l.split('\t'); if (c[0]) soundIndex.set(c[0], Number(c[1])); }
  { const rows = text('data\\global\\excel\\skills.txt').split(/\r?\n/).map((l) => l.split('\t')), h = rows[0], id = h.indexOf('Id'), st = h.indexOf('stsound');
    for (const c of rows.slice(1)) if (c[st] && soundIndex.has(c[st])) skillSound.set(Number(c[id]), soundIndex.get(c[st])); }
  const prev = new Map(), turn = new Map();
  let tick = 0, lastRoll = -99, seed = 1;
  const profile = (classId) => {
    if (classId < 0 || 4 + (classId + 1) * 424 > sv.byteLength) return null;
    const id = sv.getUint16(4 + classId * 424 + 0x14, true);
    return id < mv.getUint32(0, true) ? Array.from({ length: 37 }, (_, i) => mv.getUint32(4 + id * 148 + i * 4, true)) : null;
  };
  const row = (base) => { const n = turn.get(base) ?? 0; turn.set(base, n + 1); return base + n % (sounds[base] || 1); };
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
        const old = prev.get(key); prev.set(key, e.mode);
        if (old === undefined || old === e.mode) continue; // a mode CHANGE only; first sight is not an entry
        if (e.type === 1) {
          const p = profile(e.classId); if (!p) continue;
          if (MON_ATTACK[e.mode]) {
            const [s, b, w] = MON_ATTACK[e.mode];
            seed = (seed * 0x6ac690c5 + 1) >>> 0;
            if (p[s] && tick - lastRoll >= 15 && seed % 100 < p[b]) { lastRoll = tick; fire(p[s], e, 'attack'); }
            fire(p[w], e, 'weapon');
          } else if (MON_SKILL[e.mode]) fire(p[MON_SKILL[e.mode]], e, 'skill'); else if (MON_HIT.has(e.mode)) fire(p[13], e, 'hit');
          else if (MON_DEATH.has(e.mode)) fire(p[15], e, 'death');
        } else if (e.classId >= 0 && e.classId < 7) {
          const t = PLAYER_HIT_DEATH[e.classId];
          if (PLAYER_CAST.has(e.mode)) { if (e.id === snapshot?.playerId) fire(skillSound.get(snapshot.player?.castSkillId) ?? 0, e, 'cast'); }
          else if (PLAYER_ATTACK.has(e.mode)) fire(PLAYER_WEAPON[weaponHitClass(e.id === snapshot?.playerId ? snapshot.player : null, snapshot?.items)] ?? 0, e, 'swing');
          else if (PLAYER_HIT.has(e.mode)) fire(t[0], e, 'hit');
          else if (PLAYER_DEATH.has(e.mode)) fire(t[1], e, 'death');
        }
      }
      for (const k of prev.keys()) if (!seen.has(k)) prev.delete(k);
    },
  };
}
