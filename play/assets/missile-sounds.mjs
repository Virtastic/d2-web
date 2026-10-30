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
  const row = (name) => (name && idx.has(name) ? idx.get(name) : 0);
  const live = new Map(), born = new Map(); let pend = [];
  return {
    // Birth: D2Client spawns a server missile one tick after the snapshot shows it (the 0x0D cast packet reaches the client a tick late); the travel sound follows that spawn. a retail recording skills 44/48 went 391/398 -> 7/14.
    // Retail's client missile (ClientCol) flies the engine missile's path at 75% velocity (D2Client create, realm
    // create_client_missile) and ends on its own collision, so its hit lands floor(steps / 0.75) ticks after birth, not at the
    // engine missile's removal. ponytail: the arrival ratio stands in for the client sweep (no client sim in session mode).
    update(entities, tick = 0) {
      for (const p of pend) if (p.at <= tick) play(p.r, p.ev);
      pend = pend.filter((p) => p.at > tick);
      const seen = new Map();
      for (const e of entities) if (e.kind === 'missile') seen.set(e.id, e.classId);
      for (const [id, cls] of seen) if (!live.has(id)) { born.set(id, tick); const r = row(byId.get(cls)?.TravelSound); if (r && cls !== 62 && cls !== 261) pend.push({ at: tick + 1, r, ev: { kind: 'missile-travel', unitId: id, classId: cls } }); }   // 261 frozenorbbolt: the client orb owns it (cltdofunc 29, nova_73cb0.cpp), flown whether or not the server orb exists
      for (const [id, cls] of live) if (!seen.has(id)) {
        const m = byId.get(cls), r = row(m?.HitSound) || row(byName.get(m?.ExplosionMissile)?.TravelSound);
        const steps = tick - born.get(id) - 1, d = m?.ClientCol === '1' ? Math.floor(steps / 0.75) - steps - 1 : 0; born.delete(id);
        if (r && cls !== 62 && (m?.pCltDoFunc !== '1' || castMissiles.has(m.Missile))) { const ev = { kind: 'missile-hit', unitId: id, classId: cls }; if (d > 0) pend.push({ at: tick + d, r, ev }); else play(r, ev); }
      }
      live.clear(); for (const [k, v] of seen) live.set(k, v);
    },
  };
}
