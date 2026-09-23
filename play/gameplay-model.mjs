export const COMMAND = Object.freeze({
  MOVE: 1,
  PRIMARY: 2,
  SKILL: 3,
  CANCEL: 4,
  INTERACT: 5,
  RESPAWN: 6,
  TRAVEL: 7,
  BELT: 8,
});

const KEY_DIRECTIONS = Object.freeze({
  KeyW: [0, -1], ArrowUp: [0, -1],
  KeyS: [0, 1], ArrowDown: [0, 1],
  KeyA: [-1, 0], ArrowLeft: [-1, 0],
  KeyD: [1, 0], ArrowRight: [1, 0],
});

export class CommandInput {
  constructor() {
    this.keys = new Set();
    this.queue = [];
    this.lastMove = '0,0';
  }

  key(code, down, repeat = false) {
    const direction = KEY_DIRECTIONS[code];
    if (direction) {
      if (down) this.keys.add(code); else this.keys.delete(code);
      return true;
    }
    if (!down || repeat) return false;
    if (code === 'Space') this.queue.push({ type: COMMAND.PRIMARY });
    else if (code === 'KeyE') this.queue.push({ type: COMMAND.INTERACT });
    else if (code === 'KeyR') this.queue.push({ type: COMMAND.RESPAWN });
    else if (code === 'KeyT') this.queue.push({ type: COMMAND.TRAVEL });
    // Retail: 1-4 use the belt's columns; skills are selected on the F-keys (retail's skill hotkeys --
    // which skill sits on which F-key is this shell's default, declared in F3).
    else if (/^Digit[1-4]$/.test(code)) this.queue.push({ type: COMMAND.BELT, slot: Number(code.slice(5)) - 1 });
    else if (/^F[1-3]$/.test(code)) this.queue.push({ type: COMMAND.SKILL, slot: Number(code.slice(1)) });
    else if (code === 'Escape') this.queue.push({ type: COMMAND.CANCEL });
    else return false;
    return true;
  }

  pointer(x, y, button = 0) {
    this.queue.push(button === 2
      ? { type: COMMAND.PRIMARY, x: Math.round(x), y: Math.round(y), secondary: true }
      : { type: COMMAND.MOVE, x: Math.round(x), y: Math.round(y) });
  }

  drain() {
    let x = 0, y = 0;
    for (const code of this.keys) {
      const d = KEY_DIRECTIONS[code];
      x += d[0]; y += d[1];
    }
    x = Math.sign(x); y = Math.sign(y);
    const move = `${x},${y}`;
    if (move !== this.lastMove) {
      this.queue.push({ type: COMMAND.MOVE, x, y, directional: true });
      this.lastMove = move;
    }
    return this.queue.splice(0);
  }
}

const finite = (value, fallback = 0) => Number.isFinite(Number(value)) ? Number(value) : fallback;

export function normalizeSnapshot(value) {
  const s = value && typeof value === 'object' ? value : {};
  const player = s.player && typeof s.player === 'object' ? s.player : {};
  const entities = Array.isArray(s.entities) ? s.entities : [];
  const rooms = Array.isArray(s.rooms) ? s.rooms : [];
  const objects = Array.isArray(s.objects) ? s.objects : [];
  const items = Array.isArray(s.items) ? s.items : [];
  const warps = Array.isArray(s.warps) ? s.warps : [];
  return {
    tick: Math.max(0, Math.trunc(finite(s.tick))),
    seed: Math.max(0, Math.trunc(finite(s.seed))) >>> 0,
    act: Math.min(5, Math.max(1, Math.trunc(finite(s.act, 1)))),
    level: Math.max(0, Math.trunc(finite(s.level))),
    difficulty: Math.max(0, Math.trunc(finite(s.difficulty))),
    mode: Math.max(0, Math.trunc(finite(s.mode))),
    flags: Math.max(0, Math.trunc(finite(s.flags))) >>> 0,
    playerClass: Math.min(6, Math.max(0, Math.trunc(finite(s.playerClass)))),
    monsterCount: Math.max(0, Math.trunc(finite(s.monsterCount))),
    hash: Math.max(0, Math.trunc(finite(s.hash))) >>> 0,
    area: String(s.area ?? (s.level ? `Level ${s.level}` : 'session unavailable')),
    player: {
      x: finite(player.x), y: finite(player.y),
      hp: Math.max(0, finite(player.hp)), hpMax: Math.max(0, finite(player.hpMax)),
      mana: Math.max(0, finite(player.mana)), manaMax: Math.max(0, finite(player.manaMax)),
    },
    entities: entities.slice(0, 2048).map((e) => ({
      id: Math.trunc(finite(e?.id)), type: Math.trunc(finite(e?.type)), classId: Math.trunc(finite(e?.classId)),
      mode: Math.trunc(finite(e?.mode)), x: finite(e?.x), y: finite(e?.y),
      hp: Math.max(0, finite(e?.hp)), hpMax: Math.max(0, finite(e?.hpMax)),
      kind: String(e?.kind ?? 'entity'), hostile: Boolean(e?.hostile), roomOrdinal: Math.trunc(finite(e?.roomOrdinal)),
      animation: e?.animation && typeof e.animation === 'object' ? e.animation : null,
    })),
    rooms: rooms.slice(0, 2048).map((room) => ({
      ordinal: Math.trunc(finite(room?.ordinal)), level: Math.trunc(finite(room?.level)),
      x: finite(room?.x), y: finite(room?.y), width: Math.max(0, finite(room?.width)), height: Math.max(0, finite(room?.height)),
      flags: Math.max(0, Math.trunc(finite(room?.flags))) >>> 0,
      firstUnit: Math.max(0, Math.trunc(finite(room?.firstUnit))), unitCount: Math.max(0, Math.trunc(finite(room?.unitCount))),
    })),
    objects: objects.slice(0, 4096),
    items: items.slice(0, 4096),
    warps: warps.slice(0, 256).map((warp) => ({
      slot: Math.max(0, Math.trunc(finite(warp?.slot))), room: Math.max(0, Math.trunc(finite(warp?.room))) >>> 0,
      sourceLevel: Math.max(0, Math.trunc(finite(warp?.sourceLevel))), destinationLevel: Math.max(0, Math.trunc(finite(warp?.destinationLevel))),
      warpId: Math.max(0, Math.trunc(finite(warp?.warpId))), enabled: Boolean(warp?.enabled), direction: Math.trunc(finite(warp?.direction)),
      x: finite(warp?.x), y: finite(warp?.y),
    })),
  };
}

export function parseMonspawnFixtures(buffer) {
  const view = buffer instanceof DataView ? buffer : buffer instanceof ArrayBuffer ? new DataView(buffer)
    : new DataView(buffer.buffer, buffer.byteOffset || 0, buffer.byteLength);
  if (view.byteLength < 24 || view.getUint32(8, true) !== 4 || view.getUint32(12, true) !== 20 || !view.getUint32(16, true)) {
    throw new Error('invalid monspawn vector corpus');
  }
  const count = view.getUint32(16, true), recordSize = 20;
  if (count > Math.floor((view.byteLength - 24) / recordSize)) throw new Error('truncated monspawn vector corpus');
  return Array.from({ length: count }, (_, i) => {
    const offset = 24 + i * recordSize;
    return { seed: view.getUint32(offset, true), difficulty: view.getUint8(offset + 4),
      act0: view.getUint8(offset + 5), level: view.getUint16(offset + 6, true) };
  });
}

export function parseMonspawnFixture(buffer) { return parseMonspawnFixtures(buffer)[0]; }

// One interactive retail LvlWarp fixture per act. These are the all-act exit corpus from s23,
// so the browser defaults expose a real progression boundary rather than a random populated room.
export const DEFAULT_SESSION_FIXTURES = Object.freeze([
  { seed: 1, difficulty: 0, act0: 0, level: 1 }, { seed: 1, difficulty: 0, act0: 1, level: 40 },
  { seed: 1, difficulty: 0, act0: 2, level: 76 }, { seed: 1, difficulty: 0, act0: 3, level: 106 },
  { seed: 1, difficulty: 0, act0: 4, level: 112 },
  // Retail level 37's generated room corpus includes the Andariel quest boss (class 156).
  { seed: 1, difficulty: 0, act0: 0, level: 37, label: "Act I · Andariel's Lair" },
].map(Object.freeze));

export function parseSessionSnapshot(bytes) {
  const view = bytes instanceof DataView ? bytes : new DataView(bytes.buffer, bytes.byteOffset || 0, bytes.byteLength);
  if (view.byteLength < 68 || view.getUint32(0, true) !== 1 || view.getUint32(4, true) !== 68) throw new Error('unsupported session snapshot');
  return normalizeSnapshot({
    seed: view.getUint32(8, true), tick: view.getUint32(12, true), level: view.getUint16(16, true), playerClass: view.getUint32(20, true),
    difficulty: view.getUint8(18), act: view.getUint8(19) + 1,
    player: { x: view.getInt32(28, true), y: view.getInt32(32, true), hp: view.getInt32(44, true), hpMax: view.getInt32(48, true) },
    mode: view.getUint32(52, true), flags: view.getUint32(56, true), monsterCount: view.getUint32(60, true), hash: view.getUint32(64, true),
  });
}

export function parseSessionRooms(bytes, count) {
  const view = bytes instanceof DataView ? bytes : new DataView(bytes.buffer, bytes.byteOffset || 0, bytes.byteLength);
  if (!Number.isSafeInteger(count) || count < 0 || view.byteLength < count * 36) throw new Error('truncated session rooms');
  return Array.from({ length: count }, (_, i) => { const o = i * 36; return {
    ordinal: view.getUint32(o, true), level: view.getUint32(o + 4, true), x: view.getInt32(o + 8, true), y: view.getInt32(o + 12, true),
    width: view.getInt32(o + 16, true), height: view.getInt32(o + 20, true), flags: view.getUint32(o + 24, true),
    firstUnit: view.getUint32(o + 28, true), unitCount: view.getUint32(o + 32, true),
  }; });
}

export function parseSessionUnits(bytes, count) {
  const view = bytes instanceof DataView ? bytes : new DataView(bytes.buffer, bytes.byteOffset || 0, bytes.byteLength);
  if (!Number.isSafeInteger(count) || count < 0 || view.byteLength < count * 44) throw new Error('truncated session units');
  return Array.from({ length: count }, (_, i) => { const o = i * 44, type = view.getUint32(o + 4, true); return {
    id: view.getUint32(o, true), type, classId: view.getInt32(o + 8, true), mode: view.getUint32(o + 12, true),
    x: view.getInt32(o + 16, true), y: view.getInt32(o + 20, true), flags: view.getUint32(o + 24, true), flagsEx: view.getUint32(o + 28, true),
    hp: view.getInt32(o + 32, true), hpMax: view.getInt32(o + 36, true), roomOrdinal: view.getUint32(o + 40, true),
    kind: type === 0 ? 'player' : type === 1 ? 'monster' : type === 2 ? 'object' : type === 3 ? 'missile' : type === 4 ? 'item' : 'tile',
    hostile: type === 1,
  }; });
}

export function parseSessionWarps(bytes, count) {
  const view = bytes instanceof DataView ? bytes : new DataView(bytes.buffer, bytes.byteOffset || 0, bytes.byteLength);
  if (!Number.isSafeInteger(count) || count < 0 || view.byteLength < count * 68) throw new Error('truncated session warps');
  return Array.from({ length: count }, (_, i) => { const o = i * 68; return {
    slot: view.getUint32(o, true), room: view.getUint32(o + 4, true), sourceLevel: view.getUint32(o + 8, true),
    destinationLevel: view.getUint32(o + 12, true), warpId: view.getUint32(o + 16, true), enabled: view.getUint32(o + 20, true) !== 0,
    direction: view.getInt32(o + 24, true), x: view.getInt32(o + 28, true), y: view.getInt32(o + 32, true),
  }; });
}

export function viewportToWorld(screenX, screenY, width, height, player, pixelsPerTile = 32) {
  return {
    x: Math.round(finite(player?.x) + (finite(screenX) - finite(width) / 2) / pixelsPerTile),
    y: Math.round(finite(player?.y) + (finite(screenY) - finite(height) / 2) / pixelsPerTile),
  };
}

export function selectPrimaryTarget(snapshot, point = snapshot?.player, maxDistance = Infinity) {
  const s = normalizeSnapshot(snapshot);
  const x = finite(point?.x, s.player.x), y = finite(point?.y, s.player.y);
  const limit2 = Number.isFinite(maxDistance) ? Math.max(0, maxDistance) ** 2 : Infinity;
  let selected = null, distance2 = limit2;
  for (const entity of s.entities) {
    if (!entity.hostile || entity.hp <= 0) continue;
    const dx = entity.x - x, dy = entity.y - y, candidate = dx * dx + dy * dy;
    if (candidate < distance2 || (candidate === distance2 && (!selected || entity.id < selected.id))) {
      selected = entity; distance2 = candidate;
    }
  }
  return selected?.id ?? 0;
}

export function hudLines(snapshot, connected, fps = 0) {
  const s = normalizeSnapshot(snapshot);
  return [
    connected ? 'SESSION CONNECTED' : 'WAITING FOR d2_session_*',
    `Act ${s.act} · ${s.area}`,
    `tick ${s.tick} · ${Math.round(fps)} fps · entities ${s.entities.length}`,
    `life ${s.player.hp}/${s.player.hpMax} · mana ${s.player.mana}/${s.player.manaMax}`,
  ];
}
