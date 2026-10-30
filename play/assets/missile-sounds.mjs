// D18: client missile sounds from Missiles.txt (D2Client creates the unit -> TravelSound; removal -> HitSound, else the
// ExplosionMissile's TravelSound). A client-created missile (pCltDoFunc 1) hits through the native terminal event, which plays
// its impact only for the fireball class (client_missile_sound.h), so the removal sound is not played here. Sounds.txt names -> row index. Limits (unverified): removal is seen at snapshot granularity.
const table = (read, p) => { const r = new TextDecoder('latin1').decode(read(p)).split(/\r?\n/).map((l) => l.split('\t')), h = r[0];
  return r.slice(1).filter((c) => c[0]).map((c) => Object.fromEntries(h.map((k, i) => [k, c[i]]))); };
export function createMissileSounds(read, play) {
  const idx = new Map(table(read, 'data\\global\\excel\\Sounds.txt').map((s) => [s.Sound, Number(s.Index)]));
  const rows = table(read, 'data\\global\\excel\\Missiles.txt'), byId = new Map(rows.map((m) => [Number(m.Id), m])), byName = new Map(rows.map((m) => [m.Missile, m]));
  // A class skill's own client missile (Skills.txt cltmissile of a charclass skill, e.g. Fire Bolt's firebolt) is a pCltDoFunc 1
  // missile whose impact the session plays at removal; the other pCltDoFunc 1 missiles (Hydra's 247 is HydraMissile 337's, no charclass) stay silent.
  const castMissiles = new Set(table(read, 'data\\global\\excel\\Skills.txt').filter((k) => k.charclass && k.cltmissile).map((k) => k.cltmissile));
  // Molten Boulder: the client emerge's hit (pCltHitFunc 53) creates the client boulder, whose travel sound plays on that tick (not a tick
  // after the server boulder appears); the server's trail missiles (SubMissile1 of a pCltDoFunc 47 missile) are the client's own, not
  // sounded from the server. The boulder ends two ticks short of its Range, with its hit submissiles (explode x1, rocks x10, firepath x18).
  const cltBoulder = new Set(rows.filter((m) => m.pCltHitFunc === '53').map((m) => m.CltHitSubMissile1)), trailOwned = new Set(rows.filter((m) => m.pCltDoFunc === '47').map((m) => m.SubMissile1));
  const row = (name) => (name && idx.has(name) ? idx.get(name) : 0);
  const live = new Map(), born = new Map(); let pend = [], cltId = 0, vol = [];
  // The volcano's client debris (481 -> pair 482+483, 483 sounding at creation) is simulated here: client ids count up from 0xde
  // after the 480/479 pair, a debris born in a later unit-hash bucket (id & 0x7f > 0x5d) is updated one frame sooner (lead 1), and
  // the pair appears when the client debris ends, Lo - lead (the sound is stamped a tick before the creation). Lo, the server debris' last snapshot, is E + 15 - remaining frames.
  return {
    // Birth: D2Client spawns a server missile one tick after the snapshot shows it (the 0x0D cast packet reaches the client a tick late); the travel sound follows that spawn. a retail recording skills 44/48 went 391/398 -> 7/14.
    // Retail's client missile (ClientCol) flies the engine missile's path at 75% velocity (D2Client create, realm
    // create_client_missile) and ends on its own collision, so its hit lands floor(steps / 0.75) ticks after birth, not at the
    // engine missile's removal. ponytail: the arrival ratio stands in for the client sweep (no client sim in session mode).
    update(entities, tick = 0) {
      for (const p of pend) if (p.at <= tick) play(p.r, p.ev);
      pend = pend.filter((p) => p.at > tick);
      const seen = new Map(), news = [];
      for (const e of entities) if (e.kind === 'missile') { seen.set(e.id, e.classId); if (e.classId === 481 && !live.has(e.id) && e.missile) news.push(15 - e.missile.remaining); }
      const evs = [...vol.filter((c) => c.at === tick).map((c) => [c.id & 0x7f, 0, c]), ...news.map((life) => [0x5d, 1, life])].sort((a, b) => a[0] - b[0] || a[1] - b[1]);
      for (const [, k, c] of evs) {
        if (k === 0) { cltId += 2; const v = row(byId.get(483)?.TravelSound); if (v) play(v, { kind: 'missile-travel', unitId: cltId - 1, classId: 483 }); }
        else { const lead = (cltId & 0x7f) > 0x5d ? 1 : 0; vol.push({ id: cltId, at: tick + c - lead }); cltId++; }
      }
      vol = vol.filter((c) => c.at > tick);
      for (const [id, cls] of seen) if (!live.has(id)) { born.set(id, tick); const r = row(byId.get(cls)?.TravelSound); if (cls === 479) { cltId = 0xde; for (const c of [480, 479]) { const v = row(byId.get(c)?.TravelSound); if (v) play(v, { kind: 'missile-travel', unitId: id, classId: c }); } } else if (r && cls !== 261 && cls !== 483 && cls !== 114 && cls !== 0 && !cltBoulder.has(byId.get(cls)?.Missile) && !trailOwned.has(byId.get(cls)?.Missile)) pend.push({ at: tick + 1, r, ev: { kind: 'missile-travel', unitId: id, classId: cls } }); }   // 261 frozenorbbolt: the client orb owns it (cltdofunc 29, nova_73cb0.cpp), flown whether or not the server orb exists. the volcano (479) and its client overlay (480) both sound their travel at birth, when the client spawns them; 0 arrow: no retail trace plays weapon_bow_1 at an arrow birth; 114 teeth: the engine flies the client teeth (clmissile_b17c0.cpp)
      for (const [id, cls] of live) if (!seen.has(id)) {
        const m = byId.get(cls), r = row(m?.HitSound) || row(byName.get(m?.ExplosionMissile)?.TravelSound);
        if (m?.pCltHitFunc === '53') { const b = byName.get(m.CltHitSubMissile1); if (b) { play(row(b.TravelSound), { kind: 'missile-travel', unitId: id, classId: Number(b.Id) }); const end = tick + Number(b.Range) - 2; for (const [n, k] of [[b.CltHitSubMissile1, 1], [b.CltHitSubMissile2, 10], [b.CltHitSubMissile3, 18]]) for (let i = 0; i < k; ++i) if (row(byName.get(n)?.TravelSound)) pend.push({ at: end, r: row(byName.get(n).TravelSound), ev: { kind: 'missile-hit', unitId: id, classId: Number(byName.get(n).Id) }, boulder: true }); } }
        if (cltBoulder.has(m?.Missile)) { for (const p of pend) if (p.boulder) p.at = Math.min(p.at, tick); }
        const steps = tick - born.get(id) - 1, d = m?.ClientCol === '1' ? Math.floor(steps / 0.75) - steps - 1 : 0; born.delete(id);
        if (r && cls !== 62 && (m?.pCltDoFunc !== '1' || castMissiles.has(m.Missile))) { const ev = { kind: 'missile-hit', unitId: id, classId: cls }; if (d > 0) pend.push({ at: tick + d, r, ev }); else play(r, ev); }
      }
      live.clear(); for (const [k, v] of seen) live.set(k, v);
    },
  };
}
