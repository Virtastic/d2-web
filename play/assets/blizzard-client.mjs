// Blizzard on screen is the client's own missiles, never the server's. The server's blizzard shards
// (Missiles.txt 159-165) have no ClientSend, so a retail client never receives them: it draws the
// shards its own blizzardcenter makes and the impacts those shards end in. Those come from
// d2_client_blizzard_missiles (src/core/d2client/blizzard_bbc10.cpp), 32 bytes each:
// id, classId, x, y, direction, framePrecise, z, trans. Platform layer only: no rule lives here.
export const BLIZZARD_SERVER_ROWS = new Set([159, 160, 161, 162, 163, 164, 165]);
const CLIENT_ID = 0x80000000; // client unit ids live apart from the server's

export function parseBlizzardMissiles(bytes, count) {
  const v = bytes instanceof DataView ? bytes : new DataView(bytes.buffer, bytes.byteOffset || 0, bytes.byteLength);
  if (!Number.isInteger(count) || count < 0 || v.byteLength < count * 32) throw new RangeError('truncated blizzard missile records');
  return Array.from({ length: count }, (_, i) => { const o = i * 32; return {
    id: v.getUint32(o, true), classId: v.getUint32(o + 4, true), x: v.getInt32(o + 8, true), y: v.getInt32(o + 12, true),
    direction: v.getUint32(o + 16, true), framePrecise: v.getUint32(o + 20, true), z: v.getUint32(o + 24, true), trans: v.getUint32(o + 28, true) }; });
}

// Drops the server's blizzard rows from `entities` and appends the client's, in place.
export function applyBlizzardClient(entities, records) {
  for (let i = entities.length - 1; i >= 0; --i) if (entities[i].kind === 'missile' && BLIZZARD_SERVER_ROWS.has(entities[i].classId)) entities.splice(i, 1);
  for (const r of records) { const id = (CLIENT_ID | r.id) >>> 0;
    entities.push({ kind: 'missile', type: 3, id, classId: r.classId, mode: 0, x: r.x, y: r.y, preciseX: r.x + 0.5, preciseY: r.y + 0.5, hostile: false,
      missile: { id, classId: r.classId, mode: 0, framePrecise: r.framePrecise, direction: r.direction, trans: r.trans, z: r.z } }); }
  return entities;
}

export function blizzardClient(Module, entities) {
  if (!Module._d2_client_blizzard_missiles) return entities;
  const n = Module._d2_client_blizzard_missile_count(), p = Number(Module._d2_client_blizzard_missiles());
  return applyBlizzardClient(entities, parseBlizzardMissiles(Module.HEAPU8.subarray(p, p + n * 32), n));
}
