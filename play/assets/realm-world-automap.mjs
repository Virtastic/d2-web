// The automap's geometry source for an online game. Single player asks its engine session for the
// level's rooms and type; online there is none, so the rooms are the ones the client DRLG built from
// MapReveal (read through its room identity export) and the level type comes from Levels.txt.
// The level rectangle, the picked town file and the seam edges come from `geometry` (the d2_realm_client_*
// exports over the same client DRLG act); without it they stay empty and the town pictures and seams are unlaid.

const ROOM_INFO_BYTES = 36;   // D2DrlgClientRoomInfo: version, size, level, tileX, tileY, tileWidth, tileHeight, sightRefs, active

/** Read one client room record at a subtile position, or null when no room holds it. */
export function readClientRoomInfo(Module, malloc = (n) => Number(Module.ccall('malloc', 'bigint', ['number'], [n]))) {
  let buf = 0;
  return (x, y) => {
    buf ||= malloc(ROOM_INFO_BYTES);
    if (!Module.ccall('d2_drlg_client_room_info', 'number', ['number', 'number', 'number', 'bigint'], [x, y, -1, BigInt(buf)])) return null;
    const v = new Uint32Array(Module.HEAPU8.buffer.slice(buf, buf + ROOM_INFO_BYTES));
    return { level: v[2], tileX: v[3], tileY: v[4], tileWidth: v[5], tileHeight: v[6] };
  };
}

/** The client DRLG act's level geometry, read through the d2_realm_client_* exports. */
export function readClientLevelGeometry(Module) {
  return {
    levelRect: (levelId) => {
      const p = Number(Module.ccall('d2_realm_client_level_rect', 'number', ['number'], [levelId]));
      return p ? Array.from(new Int32Array(Module.HEAPU8.buffer, p, 4)) : [0, 0, 0, 0];
    },
    pickedFile: (levelId) => { const n = Module.ccall('d2_realm_client_level_picked_file', 'number', ['number'], [levelId]); return n >= 0 ? n : null; },
    seamEdges: (destinationLevel, hereLevel) => {
      const count = Module.ccall('d2_realm_client_seam_edges', 'number', ['number', 'number'], [destinationLevel, hereLevel]);
      const p = Number(Module.ccall('d2_realm_client_seam_edge_data', 'number', [], []));
      return p && count > 0 ? Array.from(new Int32Array(Module.HEAPU8.buffer, p, count * 4)) : [];
    },
  };
}

export function createRealmAutomapSource({ tiles, roomInfo, levelType, currentLevel, geometry = null }) {
  let cacheFor = null, cacheCount = -1, byLevel = new Map();
  function build() {
    const list = tiles();
    if (list === cacheFor && list.length === cacheCount) return byLevel;
    cacheFor = list; cacheCount = list.length; byLevel = new Map();
    const seen = new Set();
    for (const t of list) {
      if (seen.has(t.room)) continue;
      seen.add(t.room);
      const info = roomInfo(t.worldX * 5, t.worldY * 5);
      if (!info) continue;
      const rooms = byLevel.get(info.level) ?? [];
      rooms.push({ x: info.tileX * 5, y: info.tileY * 5, w: info.tileWidth * 5, h: info.tileHeight * 5 });
      byLevel.set(info.level, rooms);
    }
    return byLevel;
  }
  return {
    levelType: () => levelType(currentLevel()),
    rooms: (levelId) => build().get(levelId) ?? [],
    levelRect: (levelId) => geometry?.levelRect(levelId) ?? [0, 0, 0, 0],
    pickedFile: (levelId) => geometry?.pickedFile(levelId) ?? null,
    seamEdges: (destinationLevel) => geometry?.seamEdges(destinationLevel, currentLevel()) ?? [],
  };
}
