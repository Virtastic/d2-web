// D18: client missile sounds from Missiles.txt (D2Client creates the unit -> TravelSound; removal -> HitSound, else the
// ExplosionMissile's TravelSound). Sounds.txt names -> row index. Limits (unverified): removal is seen at snapshot granularity.
const table = (read, p) => { const r = new TextDecoder('latin1').decode(read(p)).split(/\r?\n/).map((l) => l.split('\t')), h = r[0];
  return r.slice(1).filter((c) => c[0]).map((c) => Object.fromEntries(h.map((k, i) => [k, c[i]]))); };
export function createMissileSounds(read, play) {
  const idx = new Map(table(read, 'data\\global\\excel\\Sounds.txt').map((s) => [s.Sound, Number(s.Index)]));
  const rows = table(read, 'data\\global\\excel\\Missiles.txt'), byId = new Map(rows.map((m) => [Number(m.Id), m])), byName = new Map(rows.map((m) => [m.Missile, m]));
  const row = (name) => (name && idx.has(name) ? idx.get(name) : 0);
  const live = new Map();
  return {
    update(entities) {
      const seen = new Map();
      for (const e of entities) if (e.kind === 'missile') seen.set(e.id, e.classId);
      for (const [id, cls] of seen) if (!live.has(id)) { const r = row(byId.get(cls)?.TravelSound); if (r) play(r, { kind: 'missile-travel', unitId: id, classId: cls }); }
      for (const [id, cls] of live) if (!seen.has(id)) {
        const m = byId.get(cls), r = row(m?.HitSound) || row(byName.get(m?.ExplosionMissile)?.TravelSound);
        if (r) play(r, { kind: 'missile-hit', unitId: id, classId: cls });
      }
      live.clear(); for (const [k, v] of seen) live.set(k, v);
    },
  };
}
