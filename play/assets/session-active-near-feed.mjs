export const SESSION_ACTIVE_NEAR_FEED_BYTES = 28;

/** Read the engine-owned ordered room/floor records; see D2SessionActiveNearFeedRecord. */
export function parseSessionActiveNearFeed(heap, pointer, count) {
  if (!heap || pointer == null || !Number.isInteger(count) || count <= 0) return [];
  const view = new DataView(heap.buffer, heap.byteOffset || 0, heap.byteLength);
  const records = new Array(count);
  for (let i = 0; i < count; i++) {
    const p = pointer + i * SESSION_ACTIVE_NEAR_FEED_BYTES;
    records[i] = {
      kind: view.getUint32(p, true),
      roomOrdinal: view.getUint32(p + 4, true),
      levelId: view.getInt32(p + 8, true),
      tileX: view.getInt32(p + 12, true),
      tileY: view.getInt32(p + 16, true),
      floorOrdinal: view.getUint32(p + 20, true),
      selector: view.getUint32(p + 24, true),
    };
  }
  return records;
}

/** Report malformed or reordered records instead of silently treating them as absent candidates. */
export function auditSessionActiveNearFeed(feed) {
  const errors = [];
  if (!Array.isArray(feed)) return { available: false, roomCount: 0, floorCount: 0, errors: ['feed-unavailable'] };
  const rooms = new Map();
  const floorKeys = new Set();
  let currentRoom = null, previousSelector = 0, previousOrdinal = -1;
  for (let index = 0; index < feed.length; index++) {
    const record = feed[index];
    if (record?.kind === 0) {
      const key = `${record.roomOrdinal}:${record.levelId}:${record.tileX}:${record.tileY}`;
      if (record.roomOrdinal === 0xffffffff) errors.push(`room-${index}-unmapped`);
      if (rooms.has(record.roomOrdinal)) errors.push(`room-${index}-duplicate-ordinal`);
      rooms.set(record.roomOrdinal, key);
      currentRoom = record;
      previousSelector = 0;
      previousOrdinal = -1;
      continue;
    }
    if (record?.kind !== 1) { errors.push(`record-${index}-unknown-kind`); continue; }
    if (!currentRoom || record.roomOrdinal !== currentRoom.roomOrdinal
      || record.levelId !== currentRoom.levelId || record.tileX !== currentRoom.tileX || record.tileY !== currentRoom.tileY)
      errors.push(`floor-${index}-room-identity-mismatch`);
    if (record.roomOrdinal === 0xffffffff) errors.push(`floor-${index}-unmapped`);
    if (record.selector !== 1 && record.selector !== 2) errors.push(`floor-${index}-invalid-selector`);
    if (record.selector < previousSelector || (record.selector === previousSelector && record.floorOrdinal <= previousOrdinal))
      errors.push(`floor-${index}-out-of-order`);
    const key = `${record.roomOrdinal}:${record.selector}:${record.floorOrdinal}`;
    if (floorKeys.has(key)) errors.push(`floor-${index}-duplicate-identity`);
    floorKeys.add(key);
    previousSelector = record.selector;
    previousOrdinal = record.floorOrdinal;
  }
  return { available: true, roomCount: rooms.size, floorCount: floorKeys.size, errors };
}

export function readSessionActiveNearFeed(Module) {
  if (!Module?._d2_session_active_near_feed || !Module._d2_session_active_near_feed_count) return null;
  const count = Module.ccall('d2_session_active_near_feed_count', 'number', [], []);
  if (count <= 0) return [];
  const pointer = Number(Module.ccall('d2_session_active_near_feed', 'number', [], []));
  return parseSessionActiveNearFeed(Module.HEAPU8, pointer, count);
}

/**
 * Keep only drawn floor records also present in the engine's active-near feed, in native order.
 * Room ordinals remain unique for the current level and its seam-level suffix; the remaining key
 * fields make malformed or unmapped records fail closed instead of sorting through JS list order.
 */
export function orderFloorsByActiveNearFeed(floors, feed) {
  if (!Array.isArray(feed)) return [...(floors || [])];
  const rank = new Map();
  for (let i = 0; i < feed.length; i++) {
    const record = feed[i];
    if (record?.kind !== 1 || record.roomOrdinal === 0xffffffff || (record.selector !== 1 && record.selector !== 2)) continue;
    rank.set(`${record.roomOrdinal}:${record.floorOrdinal}:${record.selector}`, i);
  }
  return (floors || []).map((floor, sourceIndex) => {
    const tile = floor?.tile;
    const selector = ((tile?.flags ?? 0) >>> 14) & 7;
    const key = `${tile?.room}:${tile?.ordinal}:${selector}`;
    return { floor, sourceIndex, position: rank.get(key) };
  }).filter((entry) => entry.position !== undefined)
    .sort((a, b) => a.position - b.position || a.sourceIndex - b.sourceIndex)
    .map((entry) => entry.floor);
}
