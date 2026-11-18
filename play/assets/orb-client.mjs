// Frozen Orb on screen is the client's own missiles, never the server's. The server's orb, bolt and nova (Missiles.txt 260-262)
// have no ClientSend, so a retail client never receives them: it draws the ones its own 0xB8840 / 0xB6010 / 0xB29B0 make.
// Those come from d2_client_orb_missiles (src/core/d2client/clmissile_b17c0.cpp), 32 bytes each: id, classId, x, y, direction,
// framePrecise, z, trans. Platform layer only: no rule lives here.
import { parseBlizzardMissiles } from './blizzard-client.mjs';
export const ORB_SERVER_ROWS = new Set([260, 261, 262]);
const CLIENT_ID = 0x80000000; // client unit ids live apart from the server's

// Drops the server's orb rows from `entities` and appends the client's, in place.
export function applyOrbClient(entities, records) {
  for (let i = entities.length - 1; i >= 0; --i) if (entities[i].kind === 'missile' && ORB_SERVER_ROWS.has(entities[i].classId)) entities.splice(i, 1);
  for (const r of records) { const id = (CLIENT_ID | r.id) >>> 0;
    entities.push({ kind: 'missile', type: 3, id, classId: r.classId, mode: 3, x: r.x, y: r.y, preciseX: r.x + 0.5, preciseY: r.y + 0.5, hostile: false,
      missile: { id, classId: r.classId, mode: 3, framePrecise: r.framePrecise, direction: r.direction, trans: r.trans, z: r.z } }); }
  return entities;
}

export function orbClient(Module, entities) {
  if (!Module._d2_client_orb_missiles) return entities;
  const n = Module._d2_client_orb_missile_count(), p = Number(Module._d2_client_orb_missiles());
  return applyOrbClient(entities, parseBlizzardMissiles(Module.HEAPU8.subarray(p, p + n * 32), n));
}
