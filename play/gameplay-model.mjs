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
    else if (code === 'KeyT') this.queue.push({ type: COMMAND.TRAVEL });
    // Retail: 1-4 use the belt's columns; skills are selected on the F-keys (retail's skill hotkeys --
    // which skill sits on which F-key is this shell's default, declared in F3).
    else if (/^Digit[1-4]$/.test(code)) this.queue.push({ type: COMMAND.BELT, slot: Number(code.slice(5)) - 1 });
    else if (/^F[1-8]$/.test(code)) this.queue.push({ type: COMMAND.SKILL, slot: Number(code.slice(1)) });
    else if (code === 'Escape') this.queue.push({ type: COMMAND.CANCEL });
    else return false;
    return true;
  }

  pointer(x, y, button = 0, held = false) {
    this.queue.push(button === 2
      ? { type: COMMAND.PRIMARY, x: Math.round(x), y: Math.round(y), secondary: true, ...(held ? { held: true } : {}) }
      : { type: COMMAND.MOVE, x: Math.round(x), y: Math.round(y), ...(held ? { held: true } : {}) });
  }

  cancelPointerMoves() {
    this.queue = this.queue.filter((command) => command.type !== COMMAND.MOVE || command.directional);
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
    playerId: Math.max(0, Math.trunc(finite(s.playerId))) >>> 0,
    monsterCount: Math.max(0, Math.trunc(finite(s.monsterCount))),
    hash: Math.max(0, Math.trunc(finite(s.hash))) >>> 0,
    area: String(s.area ?? (s.level ? `Level ${s.level}` : 'session unavailable')),
    player: {
      x: finite(player.x), y: finite(player.y),
      hp: Math.max(0, finite(player.hp)), hpMax: Math.max(0, finite(player.hpMax)),
      mana: Math.max(0, finite(player.mana)), manaMax: Math.max(0, finite(player.manaMax)),
      ...(player.weaponClassId == null ? {} : { weaponClassId: Math.trunc(finite(player.weaponClassId, -1)) }),
      ...(typeof player.weaponClass === 'string' ? { weaponClass: player.weaponClass } : {}),
    },
    entities: entities.slice(0, 2048).map((e) => ({
      id: Math.trunc(finite(e?.id)), type: Math.trunc(finite(e?.type)), classId: Math.trunc(finite(e?.classId)),
      mode: Math.trunc(finite(e?.mode)), x: finite(e?.x), y: finite(e?.y),
      hp: Math.max(0, finite(e?.hp)), hpMax: Math.max(0, finite(e?.hpMax)),
      kind: String(e?.kind ?? 'entity'), hostile: Boolean(e?.hostile), roomOrdinal: Math.trunc(finite(e?.roomOrdinal)),
      // F245: the engine's own dwFlags (parseSessionUnits reads D2SessionUnit.flags) must survive the
      // normalisation, or the client cannot tell a monster the engine will refuse to let anyone attack
      // -- UNITFLAG_TARGETABLE (0x2) -- from one it will. `undefined` when the source has no flags (a
      // synthetic snapshot), never 0: 0 is a real engine word and means "not targetable".
      flags: e?.flags === undefined || e?.flags === null ? undefined : ((Math.max(0, Math.trunc(finite(e.flags))) >>> 0)),
      ...(Number.isInteger(e?.direction) && e.direction >= 0 && e.direction < 64 ? { direction: e.direction } : {}),
      ...(typeof e?.hasDynamicPath === 'boolean' ? { hasDynamicPath: e.hasDynamicPath } : {}),
      animation: e?.animation && typeof e.animation === 'object' ? e.animation : null,
      // F20: retain the engine's missile sequence/frame/facing/transparency record through the
      // snapshot boundary. Without this, the shell correctly joined it by unit id, then this
      // normalization silently discarded it and missiles fell back to frame 0/facing 0/opaque.
      missile: e?.missile && typeof e.missile === 'object' ? e.missile : null,
      // The engine's 16.16 position where the session has one (see gameplay-shell's unitAnchor):
      // null on an engine without d2_session_unit_coords, which falls back to the whole subtile.
      preciseX: Number.isFinite(Number(e?.preciseX)) ? Number(e.preciseX) : null,
      preciseY: Number.isFinite(Number(e?.preciseY)) ? Number(e.preciseY) : null,
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
      select: warp?.select ? { x: finite(warp.select.x), y: finite(warp.select.y), dx: finite(warp.select.dx), dy: finite(warp.select.dy) } : null,
      originX: finite(warp?.originX, finite(warp?.x)), originY: finite(warp?.originY, finite(warp?.y)),
      located: warp?.located,
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
  // The whole object pick-box row (F71/s438, F98c/s380) lives on the Monastery's own levels: the
  // Barracks carries doors an activation wave has already OPENED (mode 2, one opaque pixel of art)
  // and the Outer Cloister the shut class-23 door and the class-24 gate whose sprite ends 2 px above
  // its own anchor. Both are retail areas of the act the fixture list otherwise reaches only at 37.
  { seed: 1, difficulty: 0, act0: 0, level: 28, label: 'Act I · Barracks' },
  { seed: 1, difficulty: 0, act0: 0, level: 27, label: 'Act I · Outer Cloister' },
].map(Object.freeze));

// Where a saved character resumes is NOT decided here. The act it was in, the town level of that
// act, the map seed its game is generated from and the class of its unit are the header's, and the
// engine reads them: `d2_session_create_from_save` is PLRSAVE2_ReadSaveHeader (D2Game), which is
// the only thing that should ever decode a retail .d2s header into game state. This module used to
// carry a `savedCharacterFixture(bytes)` that re-derived those four fields in JavaScript; it was a
// second source of truth for the format and it is gone. `s670-create-from-save-header` asserts the
// engine's answer against the file's own bytes.

export function parseSessionSnapshot(bytes) {
  const view = bytes instanceof DataView ? bytes : new DataView(bytes.buffer, bytes.byteOffset || 0, bytes.byteLength);
  if (view.byteLength < 68 || view.getUint32(0, true) !== 1 || view.getUint32(4, true) !== 68) throw new Error('unsupported session snapshot');
  return normalizeSnapshot({
    seed: view.getUint32(8, true), tick: view.getUint32(12, true), level: view.getUint16(16, true), playerClass: view.getUint32(20, true), playerId: view.getUint32(24, true),
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

export function parseSessionWarps(bytes, count, origins = null) {
  const view = bytes instanceof DataView ? bytes : new DataView(bytes.buffer, bytes.byteOffset || 0, bytes.byteLength);
  if (!Number.isSafeInteger(count) || count < 0 || view.byteLength < count * 68) throw new Error('truncated session warps');
  return Array.from({ length: count }, (_, i) => { const o = i * 68; return {
    slot: view.getUint32(o, true), room: view.getUint32(o + 4, true), sourceLevel: view.getUint32(o + 8, true),
    destinationLevel: view.getUint32(o + 12, true), warpId: view.getUint32(o + 16, true), enabled: view.getUint32(o + 20, true) !== 0,
    direction: view.getInt32(o + 24, true), x: view.getInt32(o + 28, true), y: view.getInt32(o + 32, true),
    // room is UINT32_MAX while no room tile to the destination is generated: the exit is listed (and
    // the warp command can still take it) but its point is not known -- x,y read (0,0).
    located: view.getUint32(o + 4, true) !== 0xFFFFFFFF,
    // LvlWarp's screen-space select box (px, y down) and its anchor in subtiles (Diablerie Warp.cs).
    select: { x: view.getInt32(o + 36, true), y: view.getInt32(o + 40, true), dx: view.getInt32(o + 44, true), dy: view.getInt32(o + 48, true) },
    originX: origins ? origins[2 * i] : view.getInt32(o + 28, true), originY: origins ? origins[2 * i + 1] : view.getInt32(o + 32, true),
  }; });
}

export function viewportToWorld(screenX, screenY, width, height, player, pixelsPerTile = 32) {
  return {
    x: Math.round(finite(player?.x) + (finite(screenX) - finite(width) / 2) / pixelsPerTile),
    y: Math.round(finite(player?.y) + (finite(screenY) - finite(height) / 2) / pixelsPerTile),
  };
}

// F14, MEASURED retail 1.13c: a unit is hovered/clicked when the
// pointer is inside a SCREEN-SPACE box about its cel anchor: Warriv x -24..+19, y -78..+15 px (1 px bisection);
// Akara's wandering hull (x -22..+20, y -69..+11, +-4 px) lies inside it, so ONE box fits both. Not derived
// per unit from cel extents: only these two units were measured, and the box exceeds their sprites.
export const UNIT_HOVER_BOX = { x0: -24, x1: 19, y0: -78, y1: 15 };
// F14, MEASURED retail 1.13c: a MONSTER's hover region is NOT the
// NPC box. One Cold Plains monster (class 165, DCC family CR) is hovered for pointer offsets x -12..+54,
// y -74..+26 px from its cel anchor -- at least 67 px wide against the NPC's 44, so the one shared constant
// cannot be right for hostiles (a hostile at dx +54 is hit by retail and missed by the NPC box). The capture
// gives a LOWER BOUND (the region "may be wider in both directions"), so the hostile box is the measured
// region UNION the NPC box: it contains every measured hit and drops none the shared box already hits (a
// monotone widening -- the narrowest change that cannot regress a case the NPC box passes). The unit-overlap
// tie-break is still a hypothesis, not a rule (see the F14 row), so selectPrimaryTarget's tie-break is unchanged.
export const UNIT_HOVER_BOX_HOSTILE = { x0: -24, x1: 54, y0: -78, y1: 26 };
export const inUnitHoverBox = (dx, dy, hostile = false) => {
  const b = hostile ? UNIT_HOVER_BOX_HOSTILE : UNIT_HOVER_BOX;
  return dx >= b.x0 && dx <= b.x1 && dy >= b.y0 && dy <= b.y1;
};

export function selectPrimaryTarget(snapshot, point = snapshot?.player, maxDistance = Infinity, accept = null) {
  const s = normalizeSnapshot(snapshot);
  const x = finite(point?.x, s.player.x), y = finite(point?.y, s.player.y);
  const limit2 = Number.isFinite(maxDistance) ? Math.max(0, maxDistance) ** 2 : Infinity;
  let selected = null, distance2 = limit2;
  for (const entity of s.entities) {
    if (!entity.hostile || entity.hp <= 0 || (accept && !accept(entity))) continue;
    // A monster in MONMODE_DEATH (0) or DEAD (12) is a corpse even when its life reads above 0
    // (s000d: a Burning Soul at 2108 life in mode 12); retail refuses to attack it (reason 5).
    if (entity.type === 1 && (entity.mode === 0 || entity.mode === 12)) continue;
    // F245: a monster the ENGINE will not let anyone attack is not a target, however the client drew
    // it -- the engine's own refusal (reason 4, "target dead or untargetable") is UNITFLAG_TARGETABLE
    // (0x2), which is this entity's own `flags` (D2SessionUnit.flags is `unit->dwFlags`; the same word
    // `d2_session_unit_info` reports). It is not only the dead: retail clears it for the act V Imp
    // while it rides a Barricade Tower / Siege Beast (`sub_6FD00EC0`), for a submerged maggot and for a
    // Reanimated Horde mid-self-resurrect, and it restores it when each ends. Offering such a unit made
    // every act V attack command a refusal -- 60-86 rounds a fight in the Frigid Highlands. `flags` is
    // absent only on a synthetic snapshot, which keeps the older default (a target).
    if (entity.type === 1 && entity.flags !== undefined && ((entity.flags >>> 0) & 2) === 0) continue;
    const dx = entity.x - x, dy = entity.y - y, candidate = dx * dx + dy * dy;
    // Retail's F14 hover resolver replaces the current winner only for a strictly smaller distance.
    // On an exact tie the first eligible candidate stays selected; unit ids are not a tie-break.
    if (candidate < distance2 || (candidate === distance2 && !selected)) {
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
