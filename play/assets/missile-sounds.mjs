// Client missile sounds. The decisions (which class sounds at creation, what its removal plays, the do/hit-func rows) are the
// wasm client's (missile_sound_rules.cpp: D2Client's create/terminal and the pCltDoFunc/pCltHitFunc ports, read from the loaded
// Missiles.txt and Skills.txt records); this file keeps the frame clock the snapshots are read on and the seeded rolls of the
// client's makers. A client-created missile (pCltDoFunc 1) hits through the native terminal event, which plays its impact only
// for the fireball class (client_missile_sound.h). Limits (unverified): removal is seen at snapshot granularity.
const F = { TRAVEL: 0, HIT: 1, PROG: 2, DO: 4, HITFUNC: 5, RANGE: 6, PARAM1: 7, HITPAR: 8, SUB: 11, HITSUB: 14, PIERCE: 17, PICK: 18, MAKER: 19, ROCK: 21, CLTBOULDER: 22, EXPLOSION: 23 };

/** The wasm client's rules as the sound code reads them: field(id, f), birth(id) -> {mode, row, own}, born(id) -> row, removal(id, span, own) -> {row, monster, expl}. */
export function wasmMissileRules(Module) {
  let buf = 0; const out = () => { buf ||= Number(Module.ccall('malloc', 'bigint', ['number'], [16])); return buf; };
  const w = (i) => Module.HEAP32[(buf >> 2) + i];
  return {
    field: (id, f) => Module._d2_client_missile_field(id, f) | 0,
    born: (id) => Module._d2_client_missile_born_row(id) | 0,
    birth: (id) => { Module._d2_client_missile_birth(id, BigInt(out())); return { mode: w(0), row: w(1), own: w(2) === 1 }; },
    removal: (id, span, own) => { Module._d2_client_missile_removal(id, span, own ? 1 : 0, BigInt(out())); return { row: w(0), monster: w(1) === 1, expl: w(2) }; },
  };
}

// onClient(id): whether the client holds its own missile for engine missile `id` (d2_session_missile_on_client); one it lacks plays no TravelSound.
// cltRoll(id): a client unit created in engine missile `id`'s room rolls that client room's seed; the roll is the new unit's init seed (d2_session_client_unit_roll).
// boltSeed(id): engine missile `id`'s seed [lo, hi] when it took a charged-bolt path (d2_session_missile_bolt_seed), else null.
export function createMissileSounds(T, play, onClient = () => true, cltRoll = () => 0, boltSeed = () => null) {
  const { field } = T, blastPeriod = 2, runs = new Map(), makers = new Map();
  // Arctic Blast and Inferno (cltdofunc 24): the client spawns one blast every 2 ticks from the cast's first server missile, for as long as the server keeps making them (a last one lands a period after the final), and sounds each as it spawns, not with the server's per-tick missiles. The period is 2 for both: Param2 is 2 for Arctic Blast but a level range (3) for Inferno, and retail's Inferno sounds every 2 ticks.
  // pCltDoFunc 6 makers (D2Client+0xB92B0): on each path step the client maker rolls rand(3) on its own seed, 0 -> CltSubMissile2,
  // 1 -> CltSubMissile3, 2 -> CltSubMissile1, then rand(CltParam1); its seed is {init, 666}, init the roll of its client room's seed
  // at its creation (D2Client+0xA7400), and the sub-missile it makes rolls that room again. The cast's first maker (the packet's own missile)
  // takes the room's second roll and steps a tick ahead of the engine's, so its first sub-missile is the first one made where it stands
  // (a retail recording, skill 225); a maker's first pick goes before a later one's. A cast's makers after its first (D2Client+0xC9A50's
  // loop) are reseeded by the charged-bolt init (D2Client+0x73B60) the engine's makers run on the same inputs: theirs is the engine maker's seed.
  const rand = (s, n) => { if (n < 1) return 0; const v = BigInt(s[0]) * 0x6ac690c5n + BigInt(s[1]); s[0] = Number(v & 0xffffffffn); s[1] = Number(v >> 32n); return (n & (n - 1)) ? s[0] % n : s[0] & (n - 1); };
  // The client boulder hops (D2Client+0xB5A10, the emerge's hit): lobbed with the emerge's cHitPar3 frames of flight (0xC7C80, gravity
  // -0x1000 per frame in 1/2048 px), then cHitPar1 ground bounces each reversing vz damped to cHitPar2 % (the integrator 0xC7FB0, run
  // from the creation frame). Its do-func (pCltDoFunc 47, 0xBB470) plays ProgSound whenever the bounce count it last saw (missile data
  // +0x28) is nonzero and has changed, i.e. on each bounce: frames 11 and 18 for the stock 2 / 50 / 10, so ticks +10 and +17.
  const boulderBounces = (n, damp, lob) => { const g = -0x1000, out = []; let z = 0, vz = Math.trunc(-Math.trunc(lob * lob * g / 2) / lob);
    for (let f = 1; f < 1000; ++f) { z += vz; vz += g; if ((z >> 11) <= 0) { vz = Math.trunc(-damp * vz / 100); z = 0; if (!n) break; --n; out.push(f); } } return out; };
  const hpPrev = new Map();
  const live = new Map(), born = new Map(), trailAt = new Map(), ownFb = new Set(); let pend = [], cltId = 0, vol = [], volPairs = [];
  // The volcano's client debris (481 -> pair 482+483, 483 sounding at creation) is simulated here: client ids count up from 0xde
  // after the 480/479 pair, a debris born in a later unit-hash bucket (id & 0x7f > 0x5d) is updated one frame sooner (lead 1), and
  // the pair appears when the client debris ends, Lo - lead (the sound is stamped a tick before the creation). Lo, the server debris' last snapshot, is E + 15 - remaining frames.
  const travel = (id, cls, ev0 = {}) => ({ kind: 'missile-travel', unitId: id, classId: cls, ...ev0 });
  return {
    /** A new session replaced the world: its missiles, pending sounds and client debris are gone with it. */
    reset() { live.clear(); born.clear(); trailAt.clear(); runs.clear(); makers.clear(); hpPrev.clear(); pend = []; vol = []; volPairs = []; cltId = 0; },
    // Birth: D2Client spawns a server missile one tick after the snapshot shows it (the 0x0D cast packet reaches the client a tick late); the travel sound follows that spawn. a retail recording skills 44/48 went 391/398 -> 7/14.
    // A client missile (ClientCol) ends on its own collision on the engine missile's path and lands on the tick the engine's goes (a retail Fire Bolt recording: both end on one tick, the client's born two later).
    update(entities, tick = 0, born_ = []) {
      // an engine missile made and gone inside one step never shows in a snapshot (a Thunder Storm bolt); the client still sounds its creation.
      // Not a blizzard shard: the server's shards never reach the client, which sounds its own (blizzard_bbc10.cpp)
      for (const [id, cls] of born_) { const r = T.born(cls); if (r && !entities.some((e) => e.kind === 'missile' && e.id === id) && onClient(id)) play(r, travel(id, cls)); }
      for (const p of pend) if (p.at <= tick) play(p.r, p.ev);
      pend = pend.filter((p) => p.at > tick);
      const seen = new Map(), news = [];
      for (const e of [...entities].filter((e) => e.kind === 'missile' && field(e.classId, F.MAKER) === 1).sort((a, b) => a.id - b.id)) {
        const k = makers.get(e.id); if (k) { k.moved = k.x !== e.x || k.y !== e.y; k.x = e.x; k.y = e.y; k.used = false; k.seen = tick; }
        else if (onClient(e.id)) { const r = cltRoll(e.id) >>> 0, bolt = boltSeed(e.id); makers.set(e.id, { seed: bolt ?? [bolt ? r : cltRoll(e.id) >>> 0, 666], first: !bolt, n: 0, x: e.x, y: e.y, moved: false, used: false, seen: tick, cls: e.classId }); } }
      for (const [id, k] of makers) if (k.seen < tick - 1) makers.delete(id);   // a maker's last step makes its sub-missile the tick after the engine maker is gone
      for (const e of entities) if (e.kind === 'missile') { seen.set(e.id, e.classId); if (e.classId === 481 && !live.has(e.id) && e.missile) news.push(15 - e.missile.remaining); }
      // the pair's sound is stamped a tick before it is made: its ids are taken in the next frame, in bucket order with that frame's debris
      for (const c of vol.filter((c) => c.at === tick)) { const v = field(483, F.TRAVEL); if (v > 0) play(v, travel(c.id, 483)); }
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
      for (const e of entities) if (e.kind === 'missile' && e.preciseX !== undefined) { const t = field(e.classId, F.DO) === 47 ? field(e.classId, F.SUB) : -1;
        if (t >= 0) { const p = trailAt.get(e.id); trailAt.set(e.id, [e.preciseX, e.preciseY]);
          if (p && (Math.floor(2 * e.preciseX - p[0]) !== Math.floor(e.preciseX) || Math.floor(2 * e.preciseY - p[1]) !== Math.floor(e.preciseY)) && field(t, F.TRAVEL) > 0 && onClient(e.id)) play(field(t, F.TRAVEL), travel(e.id, t)); } }
      for (const [id, cls] of seen) if (!live.has(id)) { born.set(id, tick); const b = T.birth(cls); if (b.own && onClient(id)) ownFb.add(id);
        if (b.mode === 1) { const g = runs.get(cls); if (g && tick - g.last <= blastPeriod) g.last = tick; else if (onClient(id)) runs.set(cls, { next: tick, last: tick, id }); continue; }
        if (b.mode === 2) { cltId = 0xde; for (const c of [480, 479]) { const v = field(c, F.TRAVEL); if (v > 0 && onClient(id)) play(v, travel(id, c)); } }
        else if (b.mode >= 3 && onClient(id)) { const ev = travel(id, cls);
          // 457 firestorm: the client's own maker (cltdofunc 6, 458) spawns on the engine maker's path step, the same tick, and picks firestorm / firesmall / firemedium on its own seed.
          if (b.mode === 5) { const e = entities.find((u) => u.kind === 'missile' && u.id === id), ks = [...makers.values()].filter((k) => !k.used && cls === field(k.cls, F.SUB) && k.x === e?.x && k.y === e?.y), k = ks.find((q) => q.first && !q.n) ?? ks.find((q) => !q.n) ?? ks.find((q) => q.moved) ?? ks[0];
            if (k) { k.used = true; k.n++; const r = rand(k.seed, 3), sub = field(k.cls, F.SUB + (r === 0 ? 1 : r === 1 ? 2 : 0)); if (field(k.cls, F.PARAM1) > 0) rand(k.seed, field(k.cls, F.PARAM1)); cltRoll(id);
              if (sub >= 0 && field(sub, F.TRAVEL) > 0) play(field(sub, F.TRAVEL), { ...ev, classId: sub }); } }
          else if (b.mode === 3) play(b.row, ev); else pend.push({ at: tick + 1, r: b.row, ev }); }
        // 577 armageddon control: the client makes its own rock on the cast-packet tick, which sounds its travel; the rock's hit (range - 1 ticks later) makes the explosion, which sounds its own.
        const rock = field(cls, F.ROCK);
        if (rock >= 0 && onClient(id)) { const x = field(rock, F.HITSUB); if (field(rock, F.TRAVEL) > 0) play(field(rock, F.TRAVEL), travel(id, rock)); if (x >= 0 && field(x, F.TRAVEL) > 0) pend.push({ at: tick + field(rock, F.RANGE) - 1, r: field(x, F.TRAVEL), ev: travel(id, x) }); } }
      for (const [name, g] of runs) { for (; g.next <= tick; g.next += blastPeriod) { if (g.next - g.last > blastPeriod) { runs.delete(name); break; } if (g.next === tick && field(name, F.TRAVEL) > 0) play(field(name, F.TRAVEL), travel(g.id, name)); } }
      // meteor center (pCltDoFunc 9): the client lands it CltParam1 frames after birth and plays ProgSound then, one tick before the server missile is gone
      for (const [id, cls] of seen) if (field(cls, F.DO) === 9 && field(cls, F.PROG) > 0 && live.has(id) && tick - born.get(id) === field(cls, F.PARAM1)) play(field(cls, F.PROG), { kind: 'missile-hit', unitId: id, classId: cls });
      for (const [id, cls] of live) if (!seen.has(id)) {
        if (field(cls, F.HITFUNC) === 53) { const b = field(cls, F.HITSUB); if (b >= 0) { play(field(b, F.TRAVEL), travel(id, b));
          if (field(b, F.DO) === 47 && field(b, F.PROG) > 0) for (const f of boulderBounces(field(cls, F.HITPAR), field(cls, F.HITPAR + 1), field(cls, F.HITPAR + 2))) pend.push({ at: tick + f - 1, r: field(b, F.PROG), ev: { kind: 'missile-hit', unitId: id, classId: b }, bounce: true }); const end = tick + field(b, F.RANGE) - 2; for (const [n, k] of [[field(b, F.HITSUB), 1], [field(b, F.HITSUB + 1), 10], [field(b, F.HITSUB + 2), 18]]) for (let i = 0; i < k; ++i) if (n >= 0 && field(n, F.TRAVEL) > 0) pend.push({ at: end, r: field(n, F.TRAVEL), ev: { kind: 'missile-hit', unitId: id, classId: n }, boulder: true }); } }
        if (field(cls, F.CLTBOULDER) === 1) { for (const p of pend) if (p.boulder) p.at = Math.min(p.at, tick); pend = pend.filter((p) => !p.bounce || p.at <= tick); }
        const span = tick - born.get(id); born.delete(id);
        const h = T.removal(cls, span, ownFb.has(id));
        if (h.row) { const ev = { kind: 'missile-hit', unitId: id, classId: cls }; if (h.monster) pend.push({ at: tick + 3, r: h.row, ev }); else play(h.row, ev);   // a monster missile lands 3 ticks later; the flight is not modelled.
          if (cls === 62) ownFb.delete(id); if (h.expl > 0) play(h.expl, travel(id, field(cls, F.EXPLOSION))); } // 62 fireball: the client's fireball sounds its hit, then its explosion missile (fireexplode) sounds its travel
      }
      // A piercing client missile (pCltDoFunc 1, ClientCol, NextHit: twister, teeth) sounds its HitSound on each unit it strikes and flies on;
      // tornado (pSrvDoFunc 27) is excluded: its damage is the do-func's pulse, not a collision, and retail plays no impact for it (skill 245).
      // the strike shows as a unit taking damage while the missile lives (the unit's last hit class carries the skill's, not the missile's) (a retail recording, skill 240: two twister hits, two impacts).
      for (const e of entities) if (e.kind !== 'missile' && e.hp !== undefined) { const k = `${e.type}:${e.id}`, ph = hpPrev.get(k); hpPrev.set(k, e.hp);
        if (ph !== undefined && e.hp < ph) for (const [id, cls] of seen) { if (field(cls, F.PIERCE) === 1 && onClient(id)) { play(field(cls, F.HIT), { kind: 'missile-hit', unitId: id, classId: cls }); break; } } }
      live.clear(); for (const [k, v] of seen) live.set(k, v);
    },
  };
}
