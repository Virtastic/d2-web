// D18: client missile sounds from Missiles.txt (D2Client creates the unit -> TravelSound; removal -> HitSound, else the
// ExplosionMissile's TravelSound). A client-created missile (pCltDoFunc 1) hits through the native terminal event, which plays
// its impact only for the fireball class (client_missile_sound.h), so the removal sound is not played here. Sounds.txt names -> row index. Limits (unverified): removal is seen at snapshot granularity.
const table = (read, p) => { const r = new TextDecoder('latin1').decode(read(p)).split(/\r?\n/).map((l) => l.split('\t')), h = r[0];
  return r.slice(1).filter((c) => c[0]).map((c) => Object.fromEntries(h.map((k, i) => [k, c[i]]))); };
// onClient(id): whether the client holds its own missile for engine missile `id` (d2_session_missile_on_client); one it lacks plays no TravelSound.
// cltRoll(id): a client unit created in engine missile `id`'s room rolls that client room's seed; the roll is the new unit's init seed (d2_session_client_unit_roll).
// boltSeed(id): engine missile `id`'s seed [lo, hi] when it took a charged-bolt path (d2_session_missile_bolt_seed), else null.
export function createMissileSounds(read, play, onClient = () => true, cltRoll = () => 0, boltSeed = () => null) {
  const idx = new Map(table(read, 'data\\global\\excel\\Sounds.txt').map((s) => [s.Sound, Number(s.Index)]));
  const rows = table(read, 'data\\global\\excel\\Missiles.txt'), byId = new Map(rows.map((m) => [Number(m.Id), m])), byName = new Map(rows.map((m) => [m.Missile, m]));
  // A class skill's own client missile (Skills.txt cltmissile of a charclass skill, e.g. Fire Bolt's firebolt) is a pCltDoFunc 1
  // missile whose impact the session plays at removal; the other pCltDoFunc 1 missiles (Hydra's 247 is HydraMissile 337's, no charclass) stay silent.
  const skills = table(read, 'data\\global\\excel\\Skills.txt'), castMissiles = new Set(skills.filter((k) => k.charclass && k.cltmissile).map((k) => k.cltmissile)),
    // Arctic Blast and Inferno (cltdofunc 24): the client spawns one blast every 2 ticks from the cast's first server missile, for as long as the server keeps making them (a last one lands a period after the final), and sounds each as it spawns, not with the server's per-tick missiles. The period is 2 for both: Param2 is 2 for Arctic Blast but a level range (3) for Inferno, and retail's Inferno sounds every 2 ticks.
    blasts = new Map(skills.filter((k) => k.cltdofunc === '24' && k.cltmissilea).map((k) => [k.cltmissilea, 2])), runs = new Map();
  // Molten Boulder: the client emerge's hit (pCltHitFunc 53) creates the client boulder, whose travel sound plays on that tick (not a tick
  // after the server boulder appears); the server's trail missiles (SubMissile1 of a pCltDoFunc 47 missile) are the client's own, not
  // sounded from the server. The boulder ends two ticks short of its Range, with its hit submissiles (explode x1, rocks x10, firepath x18).
  const cltBoulder = new Set(rows.filter((m) => m.pCltHitFunc === '53').map((m) => m.CltHitSubMissile1)), trailOwned = new Set([...rows.filter((m) => m.pCltDoFunc === '47').map((m) => m.SubMissile1), ...rows.filter((m) => m.pCltDoFunc === '13' || /^blizzard[1-4]$/.test(m.Missile)).map((m) => m.Missile)]);   // blizzard: the engine flies the client center and shards (blizzard_bbc10.cpp)
  // pCltDoFunc 6 makers (D2Client+0xB92B0): on each path step the client maker rolls rand(3) on its own seed, 0 -> CltSubMissile2,
  // 1 -> CltSubMissile3, 2 -> CltSubMissile1, then rand(CltParam1); its seed is {init, 666}, init the roll of its client room's seed
  // at its creation (D2Client+0xA7400), and the sub-missile it makes rolls that room again. The cast's first maker (the packet's own missile)
  // takes the room's second roll and steps a tick ahead of the engine's, so its first sub-missile is the first one made where it stands
  // (a retail recording, skill 225); a maker's first pick goes before a later one's. A cast's makers after its first (D2Client+0xC9A50's
  // loop) are reseeded by the charged-bolt init (D2Client+0x73B60) the engine's makers run on the same inputs: theirs is the engine maker's seed.
  const cltMaker = new Map(rows.filter((m) => m.pCltDoFunc === '6' && m.CltSubMissile2).map((m) => [Number(m.Id), m])), cltPick = new Set([...cltMaker.values()].map((m) => m.CltSubMissile1)), makers = new Map();
  const rand = (s, n) => { if (n < 1) return 0; const v = BigInt(s[0]) * 0x6ac690c5n + BigInt(s[1]); s[0] = Number(v & 0xffffffffn); s[1] = Number(v >> 32n); return (n & (n - 1)) ? s[0] % n : s[0] & (n - 1); };
  // The client boulder hops (D2Client+0xB5A10, the emerge's hit): lobbed with the emerge's cHitPar3 frames of flight (0xC7C80, gravity
  // -0x1000 per frame in 1/2048 px), then cHitPar1 ground bounces each reversing vz damped to cHitPar2 % (the integrator 0xC7FB0, run
  // from the creation frame). Its do-func (pCltDoFunc 47, 0xBB470) plays ProgSound whenever the bounce count it last saw (missile data
  // +0x28) is nonzero and has changed, i.e. on each bounce: frames 11 and 18 for the stock 2 / 50 / 10, so ticks +10 and +17.
  const boulderBounces = (n, damp, lob) => { const g = -0x1000, out = []; let z = 0, vz = Math.trunc(-Math.trunc(lob * lob * g / 2) / lob);
    for (let f = 1; f < 1000; ++f) { z += vz; vz += g; if ((z >> 11) <= 0) { vz = Math.trunc(-damp * vz / 100); z = 0; if (!n) break; --n; out.push(f); } } return out; };
  const row = (name) => (name && idx.has(name) ? idx.get(name) : 0);
  // A piercing client missile (pCltDoFunc 1, ClientCol, NextHit: twister, teeth) sounds its HitSound on each unit it strikes and flies on;
  // tornado (pSrvDoFunc 27) is excluded: its damage is the do-func's pulse, not a collision, and retail plays no impact for it (skill 245).
  // the strike shows as a unit taking damage while the missile lives (the unit's last hit class carries the skill's, not the missile's) (a retail recording, skill 240: two twister hits, two impacts).
  const pierce = new Map(rows.filter((m) => m.pCltDoFunc === '1' && m.pSrvDoFunc === '1' && m.ClientCol === '1' && m.NextHit === '1' && m.HitSound).map((m) => [Number(m.Id), m])), hpPrev = new Map();
  const live = new Map(), born = new Map(), trailAt = new Map(), ownFb = new Set(); let pend = [], cltId = 0, vol = [], volPairs = [];
  // The volcano's client debris (481 -> pair 482+483, 483 sounding at creation) is simulated here: client ids count up from 0xde
  // after the 480/479 pair, a debris born in a later unit-hash bucket (id & 0x7f > 0x5d) is updated one frame sooner (lead 1), and
  // the pair appears when the client debris ends, Lo - lead (the sound is stamped a tick before the creation). Lo, the server debris' last snapshot, is E + 15 - remaining frames.
  return {
    /** A new session replaced the world: its missiles, pending sounds and client debris are gone with it. */
    reset() { live.clear(); born.clear(); trailAt.clear(); runs.clear(); makers.clear(); hpPrev.clear(); pend = []; vol = []; volPairs = []; cltId = 0; },
    // Birth: D2Client spawns a server missile one tick after the snapshot shows it (the 0x0D cast packet reaches the client a tick late); the travel sound follows that spawn. a retail recording skills 44/48 went 391/398 -> 7/14.
    // A client missile (ClientCol) ends on its own collision on the engine missile's path and lands on the tick the engine's goes (a retail Fire Bolt recording: both end on one tick, the client's born two later).
    update(entities, tick = 0, born_ = []) {
      // an engine missile made and gone inside one step never shows in a snapshot (a Thunder Storm bolt); the client still sounds its creation
      for (const [id, cls] of born_) { const m = byId.get(cls), r = row(m?.TravelSound); if (r && m.pSrvDoFunc === '3' && !entities.some((e) => e.kind === 'missile' && e.id === id) && onClient(id)) play(r, { kind: 'missile-travel', unitId: id, classId: cls }); }
      for (const p of pend) if (p.at <= tick) play(p.r, p.ev);
      pend = pend.filter((p) => p.at > tick);
      const seen = new Map(), news = [];
      for (const e of [...entities].filter((e) => e.kind === 'missile' && cltMaker.has(e.classId)).sort((a, b) => a.id - b.id)) {
        const k = makers.get(e.id); if (k) { k.moved = k.x !== e.x || k.y !== e.y; k.x = e.x; k.y = e.y; k.used = false; k.seen = tick; }
        else if (onClient(e.id)) { const r = cltRoll(e.id) >>> 0, bolt = boltSeed(e.id); makers.set(e.id, { seed: bolt ?? [bolt ? r : cltRoll(e.id) >>> 0, 666], first: !bolt, n: 0, x: e.x, y: e.y, moved: false, used: false, seen: tick, m: cltMaker.get(e.classId) }); } }
      for (const [id, k] of makers) if (k.seen < tick - 1) makers.delete(id);   // a maker's last step makes its sub-missile the tick after the engine maker is gone
      for (const e of entities) if (e.kind === 'missile') { seen.set(e.id, e.classId); if (e.classId === 481 && !live.has(e.id) && e.missile) news.push(15 - e.missile.remaining); }
      // the pair's sound is stamped a tick before it is made: its ids are taken in the next frame, in bucket order with that frame's debris
      for (const c of vol.filter((c) => c.at === tick)) { const v = row(byId.get(483)?.TravelSound); if (v) play(v, { kind: 'missile-travel', unitId: c.id, classId: 483 }); }
      const evs = [...volPairs.map((c) => [c.id & 0x7f, 0, c]), ...news.map((life) => [0x5d, 1, life])].sort((a, b) => a[0] - b[0] || a[1] - b[1]);
      volPairs = vol.filter((c) => c.at === tick);
      for (const [, k, c] of evs) {
        if (k === 0) cltId += 2;
        else { const lead = (cltId & 0x7f) > 0x5d ? 1 : 0; vol.push({ id: cltId, at: tick + c - lead }); cltId++; }
      }
      vol = vol.filter((c) => c.at > tick);
      // Molten Boulder (pCltDoFunc 47): the client boulder leaves a trail missile (CltSubMissile1) on each subtile it enters, whose
      // travel sound plays on that step. It flies the server boulder's path one tick ahead of the snapshot the shell shows, so the step is
      // read off the 16.16 position carried one tick forward by its last move.
      for (const e of entities) if (e.kind === 'missile' && e.preciseX !== undefined) { const m = byId.get(e.classId), t = m?.pCltDoFunc === '47' && byName.get(m.CltSubMissile1);
        if (t) { const p = trailAt.get(e.id); trailAt.set(e.id, [e.preciseX, e.preciseY]);
          if (p && (Math.floor(2 * e.preciseX - p[0]) !== Math.floor(e.preciseX) || Math.floor(2 * e.preciseY - p[1]) !== Math.floor(e.preciseY)) && row(t.TravelSound) && onClient(e.id)) play(row(t.TravelSound), { kind: 'missile-travel', unitId: e.id, classId: Number(t.Id) }); } }
      for (const [id, cls] of seen) if (!live.has(id)) { born.set(id, tick); if (cls === 62 && onClient(id)) ownFb.add(id); const m0 = byId.get(cls), r = row(m0?.TravelSound); if (blasts.has(m0?.Missile)) { const g = runs.get(m0.Missile); if (g && tick - g.last <= blasts.get(m0.Missile)) g.last = tick; else if (onClient(id)) runs.set(m0.Missile, { next: tick, last: tick, id }); continue; } if (cls === 479) { cltId = 0xde; for (const c of [480, 479]) { const v = row(byId.get(c)?.TravelSound); if (v && onClient(id)) play(v, { kind: 'missile-travel', unitId: id, classId: c }); } } else if (r && cls !== 261 && onClient(id) && cls !== 483 && cls !== 114 && cls !== 68 && cls !== 69 && cls !== 0 && !cltBoulder.has(byId.get(cls)?.Missile) && !trailOwned.has(byId.get(cls)?.Missile)) { const ev = { kind: 'missile-travel', unitId: id, classId: cls };
        // 457 firestorm: the client's own maker (cltdofunc 6, 458) spawns on the engine maker's path step, the same tick, and picks firestorm / firesmall / firemedium on its own seed.
        if (cltPick.has(m0?.Missile)) { const e = entities.find((u) => u.kind === 'missile' && u.id === id), ks = [...makers.values()].filter((k) => !k.used && m0.Missile === k.m.CltSubMissile1 && k.x === e?.x && k.y === e?.y), k = ks.find((q) => q.first && !q.n) ?? ks.find((q) => !q.n) ?? ks.find((q) => q.moved) ?? ks[0];
          if (k) { k.used = true; k.n++; const r = rand(k.seed, 3), sub = byName.get(r === 0 ? k.m.CltSubMissile2 : r === 1 ? k.m.CltSubMissile3 : k.m.CltSubMissile1); if (Number(k.m.CltParam1) > 0) rand(k.seed, Number(k.m.CltParam1)); cltRoll(id);
            if (sub && row(sub.TravelSound)) play(row(sub.TravelSound), { ...ev, classId: Number(sub.Id) }); } }
        else pend.push({ at: tick + 1, r, ev }); }
        // 577 armageddon control: the client makes its own rock (578) on the cast-packet tick, which sounds its travel; the rock's hit (range - 1 ticks later) makes the explosion, which sounds its own.
        if (cls === 577 && onClient(id)) { const k = byName.get('armageddonrock'), x = byName.get(k?.CltHitSubMissile1); if (row(k?.TravelSound)) play(row(k.TravelSound), { kind: 'missile-travel', unitId: id, classId: Number(k.Id) }); if (row(x?.TravelSound)) pend.push({ at: tick + Number(k.Range) - 1, r: row(x.TravelSound), ev: { kind: 'missile-travel', unitId: id, classId: Number(x.Id) } }); } }
      for (const [name, g] of runs) { const per = blasts.get(name), k = byName.get(name); for (; g.next <= tick; g.next += per) { if (g.next - g.last > per) { runs.delete(name); break; } if (g.next === tick && row(k.TravelSound)) play(row(k.TravelSound), { kind: 'missile-travel', unitId: g.id, classId: Number(k.Id) }); } }
      // 62 fireball: where the realm client flies the player's fireball it sounds travel and impact (d2_realm_client_take_missile_sounds) and onClient is 0; otherwise the server fireball sounds them.   // 68/69 firewallmaker/firewall: the engine flies the client Fire Wall (cltdofunc 26, clmissile_b17c0.cpp), which casts even where the server refuses.   // 261 frozenorbbolt: the client orb owns it (cltdofunc 29, nova_73cb0.cpp), flown whether or not the server orb exists. the volcano (479) and its client overlay (480) both sound their travel at birth, when the client spawns them; 0 arrow: no retail trace plays weapon_bow_1 at an arrow birth; 114 teeth: the engine flies the client teeth (clmissile_b17c0.cpp)
      // meteor center (pCltDoFunc 9): the client lands it CltParam1 frames after birth and plays ProgSound then, one tick before the server missile is gone
      for (const [id, cls] of seen) { const m = byId.get(cls); if (m?.pCltDoFunc === '9' && m.ProgSound && live.has(id) && tick - born.get(id) === Number(m.CltParam1)) play(row(m.ProgSound), { kind: 'missile-hit', unitId: id, classId: cls }); }
      for (const [id, cls] of live) if (!seen.has(id)) {
        const m = byId.get(cls), r = row(m?.HitSound) || row(byName.get(m?.ExplosionMissile)?.TravelSound);
        if (m?.pCltHitFunc === '53') { const b = byName.get(m.CltHitSubMissile1); if (b) { play(row(b.TravelSound), { kind: 'missile-travel', unitId: id, classId: Number(b.Id) });
          if (b.pCltDoFunc === '47' && row(b.ProgSound)) for (const f of boulderBounces(Number(m.cHitPar1), Number(m.cHitPar2), Number(m.cHitPar3))) pend.push({ at: tick + f - 1, r: row(b.ProgSound), ev: { kind: 'missile-hit', unitId: id, classId: Number(b.Id) }, bounce: true }); const end = tick + Number(b.Range) - 2; for (const [n, k] of [[b.CltHitSubMissile1, 1], [b.CltHitSubMissile2, 10], [b.CltHitSubMissile3, 18]]) for (let i = 0; i < k; ++i) if (row(byName.get(n)?.TravelSound)) pend.push({ at: end, r: row(byName.get(n).TravelSound), ev: { kind: 'missile-hit', unitId: id, classId: Number(byName.get(n).Id) }, boulder: true }); } }
        if (cltBoulder.has(m?.Missile)) { for (const p of pend) if (p.boulder) p.at = Math.min(p.at, tick); pend = pend.filter((p) => !p.bounce || p.at <= tick); }
        const span = tick - born.get(id); born.delete(id);
        if (r && (cls !== 62 || ownFb.has(id)) && !(m?.ClientCol === "1" && span >= Number(m.Range)) && (m?.pCltDoFunc !== '1' || castMissiles.has(m.Missile))) { const ev = { kind: 'missile-hit', unitId: id, classId: cls }; play(r, ev);
          const x = cls === 62 && ownFb.delete(id) && row(m.HitSound) && byName.get(m.ExplosionMissile); if (x && row(x.TravelSound)) play(row(x.TravelSound), { kind: 'missile-travel', unitId: id, classId: Number(x.Id) }); } // 62 fireball: the client's fireball sounds its hit, then its explosion missile (fireexplode) sounds its travel
      }
      for (const e of entities) if (e.kind !== 'missile' && e.hp !== undefined) { const k = `${e.type}:${e.id}`, ph = hpPrev.get(k); hpPrev.set(k, e.hp);
        if (ph !== undefined && e.hp < ph) for (const [id, cls] of seen) { const m = pierce.get(cls); if (m && onClient(id)) { play(row(m.HitSound), { kind: 'missile-hit', unitId: id, classId: cls }); break; } } }
      live.clear(); for (const [k, v] of seen) live.set(k, v);
    },
  };
}
