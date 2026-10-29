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

  cancelHeldSecondary() {
    this.queue = this.queue.filter((command) => !(command.secondary && command.held));
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
      ...(Number.isInteger(e?.lastHitClass) ? { lastHitClass: e.lastHitClass } : {}),
      ...(Number.isInteger(e?.typeFlags) ? { typeFlags: e.typeFlags & 0xFF } : {}),
      ...(Number.isInteger(e?.superUniqueId) ? { superUniqueId: e.superUniqueId } : {}),
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
      located: warp?.located, unitTile: warp?.unitTile,
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
    // enabled === 2: borrowed point of a sibling tile (this warp id has no generated UNIT_TILE): not pickable (D28)
    unitTile: view.getUint32(o + 20, true) === 1,
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
// monotone widening -- the narrowest change that cannot regress a case the NPC box passes). Native
// same-priority score inputs and stored-target behavior remain unresolved; selectPrimaryTarget is a
// separate nearest-world helper and does not define hover arbitration.
export const UNIT_HOVER_BOX_HOSTILE = { x0: -24, x1: 54, y0: -78, y1: 26 };
export const inUnitHoverBox = (dx, dy, hostile = false) => {
  const b = hostile ? UNIT_HOVER_BOX_HOSTILE : UNIT_HOVER_BOX;
  return dx >= b.x0 && dx <= b.x1 && dy >= b.y0 && dy <= b.y1;
};

// D2Client 1.13c's hover candidate table at 0x6FBA2190. EBX holds the incumbent and EDI
// the candidate; JGE retains the incumbent when its score is greater than or equal to the candidate.
const UNIT_TYPE_PICK_PRIORITY = new Map([[2, -6], [4, -4], [7, 2], [12, 1], [14, 1]]);
export const unitTypePickPriority = (unitType) => UNIT_TYPE_PICK_PRIORITY.get(unitType) ?? 0;

function candidateUnitType(candidate) {
  return Number.isInteger(candidate?.unitType) ? candidate.unitType : candidate?.type;
}

function candidatePickPriority(candidate, unitType) {
  // D2Client 1.13c #10648 returns true only for a type-1 unit in mode 0 or 12. The session's
  // D2SessionUnit mode is the same native dwAnimMode field; preserve an explicit probe result if
  // one is supplied, otherwise derive the exact predicate from that engine-owned field.
  if (unitType === 1 && (candidate?.ordinal10648 === true ||
      (candidate?.ordinal10648 === undefined && (candidate?.mode === 0 || candidate?.mode === 12)))) return -8;
  return unitTypePickPriority(unitType);
}

export function nativeHoverSpecialGate(candidate) {
  const type = candidateUnitType(candidate);
  if (type === 1) {
    const result = candidate?.ordinal10648 ?? (candidate?.mode === 0 || candidate?.mode === 12 ? true
      : Number.isInteger(candidate?.mode) ? false : undefined);
    const priorityGate = result === undefined
      ? { status: 'unresolved', ordinal: 10648 }
      : { status: 'resolved', ordinal: 10648, priority: result ? -8 : unitTypePickPriority(1) };
    const stateGate = candidate?.ordinal10494_8f === undefined
      ? { status: 'unresolved', ordinal: 10494, argument: 0x8f }
      : { status: candidate.ordinal10494_8f ? 'skip' : 'pass', ordinal: 10494, argument: 0x8f };
    return [priorityGate, stateGate];
  }
  if (type === 2) {
    // The picker calls #11020 and a second client helper for objects. Their outcome semantics are
    // unresolved here; this is metadata only. The browser selector ranks objects only after its
    // own hit test accepted them and does not treat this metadata as a native eligibility verdict.
    return [{ status: 'unresolved', ordinal: 11020, additionalClientCheck: true }];
  }
  return [];
}

// Compare already accepted browser hit candidates using the recovered cross-type score; greater values win.
// Equal-priority candidates use D2Client's squared game-coordinate distance from the player when supplied.
export function selectUnitTypePickCandidate(candidates, {
  unitTypeOf = candidateUnitType,
  accepted = () => true,
  reference = null,
} = {}) {
  let selected = null, selectedPriority = -Infinity;
  let selectedDistance2 = Infinity;
  for (const candidate of candidates ?? []) {
    if (!accepted(candidate)) continue;
    const unitType = unitTypeOf(candidate);
    // FUN_6fb01e80 walks every unit list but skips type 3 (missile): a missile is never a hover target.
    if (unitType === 3) continue;
    // The scan skips type 1 when #10494(unit, 0x8f) succeeds. Its meaning is not inferred from
    // the ordinal; an unknown result leaves the browser-accepted candidate in this approximation.
    if (unitType === 1 && candidate?.ordinal10494_8f === true) continue;
    const priority = candidatePickPriority(candidate, unitType);
    const x = Number(candidate?.x ?? candidate?.target?.x);
    const y = Number(candidate?.y ?? candidate?.target?.y);
    const refX = Number(reference?.x), refY = Number(reference?.y);
    const distance2 = Number.isFinite(x) && Number.isFinite(y) && Number.isFinite(refX) && Number.isFinite(refY)
      ? (x - refX) ** 2 + (y - refY) ** 2
      : Infinity;
    if (!selected || priority > selectedPriority ||
        (priority === selectedPriority && distance2 < selectedDistance2)) {
      selected = candidate;
      selectedPriority = priority;
      selectedDistance2 = distance2;
    }
  }
  return selected;
}

// The exact-cell item participates in hover arbitration; the nearby pickup-radius item is a
// separate fallback and remains available only when no accepted hit candidate won. Click policy is
// intentionally the pre-existing browser policy because the native click picker is distinct.
export function selectPointerCandidate(candidates, { click = false, reference = null, prior = null } = {}) {
  const object = click && !(candidates.units?.length || candidates.itemsOnCell?.length || candidates.objectsUnderPointer?.length) ? candidates.object : (candidates.objectsUnderPointer == null
    ? (candidates.hoverObject ?? candidates.object) : null);
  // Overlap lists arbitrate by the same type-priority walk as hover, so a click picks what hover shows.
  if (click && !(candidates.units?.length || candidates.itemsOnCell?.length || candidates.objectsUnderPointer?.length)) {
    if (candidates.itemOnCell && object) return { kind: 'object', target: object };
    if (candidates.itemOnCell) return { kind: 'item', target: candidates.itemOnCell };
    if (candidates.npc) return { kind: 'npc', target: candidates.npc };
    if (object) return { kind: 'object', target: object };
    if (candidates.item) return { kind: 'item', target: candidates.item };
    return null;
  }
  const rows = [
    ...(candidates.units ?? []).map((target) => ({ ...target, kind: target.hostile ? 'unit' : 'npc', target, unitType: 1 })),
    ...(candidates.itemsOnCell ?? (candidates.itemOnCell ? [candidates.itemOnCell] : []))
      .map((target) => ({ ...target, kind: 'item', target, unitType: 4 })),
    ...(candidates.objectsUnderPointer ?? (object ? [object] : []))
      .map((target) => ({ ...target, kind: 'object', target, unitType: 2 })),
    // Legacy single NPC candidate remains supported for existing callers and unit tests.
    ...(!candidates.units?.length && (candidates.hoverNpc ?? candidates.npc)
      ? [{ ...(candidates.hoverNpc ?? candidates.npc), kind: 'npc',
        target: candidates.hoverNpc ?? candidates.npc, unitType: 1 }] : []),
  ];
  // D2Client FUN_6FB01E80 first asks FUN_6FB01A80 for the previous hover target and keeps it while
  // the validator still accepts it, so overlapping units/items/objects do not swap under the cursor.
  const sticky = prior && rows.find((r) => r.target?.id === prior.id && r.kind === prior.kind);
  const winner = sticky || selectUnitTypePickCandidate(rows, { reference });
  if (winner) return { kind: winner.kind, target: winner.target };
  if (candidates.item) return { kind: 'item', target: candidates.item };
  return null;
}

// Click on overlapping units follows the hover walk (D2Client 0x51E80): type priority, then squared
// distance to the player, first seen on ties. Returns the winning unit (hostile or NPC) or null.
export function selectHoverUnit(units, reference) {
  return selectUnitTypePickCandidate(units, { reference });
}

export function isPrimaryTargetEligible(entity, accept = null) {
  if (!entity?.hostile || entity.hp <= 0 || (accept && !accept(entity))) return false;
  // A monster in MONMODE_DEATH (0) or DEAD (12) is a corpse even when its life reads above 0
  // (s000d: a Burning Soul at 2108 life in mode 12); retail refuses to attack it (reason 5).
  if (entity.type === 1 && (entity.mode === 0 || entity.mode === 12)) return false;
  // F245: a monster the ENGINE will not let anyone attack is not a target, however the client drew
  // it -- the engine's own refusal (reason 4, "target dead or untargetable") is UNITFLAG_TARGETABLE
  // (0x2), which is this entity's own `flags` (D2SessionUnit.flags is `unit->dwFlags`; the same word
  // `d2_session_unit_info` reports). It is not only the dead: retail clears it for the act V Imp
  // while it rides a Barricade Tower / Siege Beast (`sub_6FD00EC0`), for a submerged maggot and for a
  // Reanimated Horde mid-self-resurrect, and it restores it when each ends. Offering such a unit made
  // every act V attack command a refusal -- 60-86 rounds a fight in the Frigid Highlands. `flags` is
  // absent only on a synthetic snapshot, which keeps the older default (a target).
  if (entity.type === 1 && entity.flags !== undefined && ((entity.flags >>> 0) & 2) === 0) return false;
  return true;
}

// selectPrimaryTarget is the browser's nearest-world targeting helper. F14 pointer-hover candidates
// use selectUnitTypePickCandidate above for the recovered greater-priority comparison. Equal-priority
// encounter order is a browser fallback; native score inputs and stored-target behavior remain open.

// D2Client 1.13c hover type priorities, kept separate from click targeting.
const NATIVE_HOVER_TYPE_PRIORITY = Object.freeze([0, 0, -6, 0, -4, 0, 0, 2, 0, 0, 0, 0, 1, 0, 1]);

export function nativeDccHoverContains(px, py, box, ox, oy) {
  if (!box || !(box.width > 0) || !(box.height > 0)) return false;
  let left = Math.round(ox + box.xOffset - 12), right = Math.round(ox + box.xOffset + box.width + 12);
  let lowY = Math.round(oy + box.yOffset - 13), highY = Math.round(oy + box.yOffset + box.height + 11);
  const narrowX = box.width + 24, narrowY = box.height + 24;
  if (narrowX < 32) { const extra = (32 - narrowX) >> 1; left -= extra; right += extra; }
  if (narrowY < 32) { const extra = (32 - narrowY) >> 1; lowY -= extra; highY += extra; }
  return Math.floor(px) >= left && Math.floor(px) <= right && Math.floor(py) >= lowY && Math.floor(py) <= highY;
}

// F14/S286: which ground item the pointer is over. Items are units, so D2Client's validator
// FUN_6FB568E0 applies the SAME padded-rectangle rule as for any other unit; it never tests the
// item's grid cell. `project(item.x, item.y)` must be the projection the item was DRAWN with (the
// shell's static-grid transform) and `measure(item)` the cel box of the frame drawn: retail's rect
// builder FUN_6FB56730 receives the unit's exact screen x/y, so no cell rounding is applied here.
// The capture of the exact rect builder is still owed.
export function groundItemsUnderPointerAt(items, x, y, project, measure) {
  const hits = [];
  for (const item of items || []) {
    if (item.mode !== 3 || item.invPage !== 255) continue;
    const box = measure(item); if (!box) continue;
    const [sx, sy] = project(item.x, item.y);
    if (nativeDccHoverContains(x, y, box, sx, sy)) hits.push(item);
  }
  return hits;
}

// D2Client's object hover validator must obtain a current DCC cell before it can form a hit rect.
// Preserve the measured painted-box/anchor fallback for multi-layer compositions, but a missing
// cell cannot become a hover hit just because the object's anchor is near the pointer.
export function nativeObjectHoverContains(px, py, box, ox, oy) {
  if (!box || !(box.width > 0) || !(box.height > 0)) return false;
  if (box.layers === 1) return nativeDccHoverContains(px, py, box, ox, oy);
  const left = Math.round(ox + box.xOffset), top = Math.round(oy + box.yOffset);
  const inBox = px >= left && px < left + box.width && py >= top && py < top + box.height;
  const coversOrigin = box.xOffset <= 0 && box.xOffset + box.width > 0 &&
    box.yOffset <= 0 && box.yOffset + box.height > 0;
  return inBox || (!coversOrigin && Math.hypot(ox - px, oy - py) <= 6);
}

// Native diagnostic picker helper. With a player reference, equal priorities use squared path
// distance; candidates lacking coordinates retain the stable encounter-order fallback.
function nativeHoverDistance2(candidate, reference) {
  const x = Number(candidate?.x ?? candidate?.target?.x);
  const y = Number(candidate?.y ?? candidate?.target?.y);
  const refX = Number(reference?.x), refY = Number(reference?.y);
  return Number.isFinite(x) && Number.isFinite(y) && Number.isFinite(refX) && Number.isFinite(refY)
    ? (x - refX) ** 2 + (y - refY) ** 2
    : Infinity;
}

export function selectNativeHoverWinner(candidates, fallback = null, reference = null) {
  let winner = fallback;
  let winnerPriority = winner ? (NATIVE_HOVER_TYPE_PRIORITY[winner.type] ?? 0) : Infinity;
  for (const candidate of candidates || []) {
    const priority = NATIVE_HOVER_TYPE_PRIORITY[candidate.type] ?? 0;
    if (!winner || priority > winnerPriority ||
        (reference && priority === winnerPriority &&
         nativeHoverDistance2(candidate, reference) < nativeHoverDistance2(winner, reference))) {
      winner = candidate;
      winnerPriority = priority;
    }
  }
  return winner;
}

// Rank accepted non-hostile candidates by the same native priority and optional player reference.
export function selectNativeHoverCandidate({ object = null, itemOnCell = null, npc = null, item = null } = {}, fallback = null, reference = null) {
  const candidates = [];
  if (object) candidates.push({ kind: 'object', target: object, type: 2 });
  if (itemOnCell) candidates.push({ kind: 'item', target: itemOnCell, type: 4 });
  if (npc) candidates.push({ kind: 'npc', target: npc, type: 1 });
  if (item && item !== itemOnCell) candidates.push({ kind: 'item', target: item, type: 4 });
  return selectNativeHoverWinner(candidates, fallback, reference);
}

export function selectPrimaryTarget(snapshot, point = snapshot?.player, maxDistance = Infinity, accept = null) {
  const s = normalizeSnapshot(snapshot);
  const x = finite(point?.x, s.player.x), y = finite(point?.y, s.player.y);
  const limit2 = Number.isFinite(maxDistance) ? Math.max(0, maxDistance) ** 2 : Infinity;
  let selected = null, distance2 = limit2;
  for (const entity of s.entities) {
    if (!isPrimaryTargetEligible(entity, accept)) continue;
    const dx = entity.x - x, dy = entity.y - y, candidate = dx * dx + dy * dy;
    // This browser nearest-world helper replaces its current target only for a strictly smaller
    // world-coordinate distance. It is separate from F14 pointer-hover arbitration.
    if (candidate < distance2 || (candidate === distance2 && !selected)) {
      selected = entity; distance2 = candidate;
    }
  }
  return selected?.id ?? 0;
}

// Retail click/hover unit pick (D2Client+0x51650 with D2Common ordinal 10080):
// among eligible candidates in list order, score = floor((2*max(dx,dy)+min(dx,dy))/2) against the
// pointer's subtile; replace only on a strictly smaller score, start bound 10 (score 10 excluded).
export function retailPickScore(a, b) {
  const dx = Math.abs(a.x - b.x), dy = Math.abs(a.y - b.y);
  return Math.floor((2 * Math.max(dx, dy) + Math.min(dx, dy)) / 2);
}
export function selectRetailPickTarget(snapshot, point, accept = null) {
  let best = null, bound = 10;
  for (const entity of normalizeSnapshot(snapshot).entities) {
    if (!isPrimaryTargetEligible(entity, accept)) continue;
    const score = retailPickScore(entity, point);
    if (score < bound) { best = entity; bound = score; }
  }
  return best?.id ?? 0;
}

export function hudLines(snapshot, connected, fps = 0) {
  // PERF: only the header scalars and the entity COUNT are shown, so the lists (entities, rooms, objects,
  // items, warps: thousands of rebuilt objects a frame in a crowded town) are not normalised here.
  const n = Array.isArray(snapshot?.entities) ? Math.min(snapshot.entities.length, 2048) : 0;
  const s = normalizeSnapshot(snapshot && typeof snapshot === 'object' ? { ...snapshot, entities: undefined, rooms: undefined, objects: undefined, items: undefined, warps: undefined } : snapshot);
  return [
    connected ? 'SESSION CONNECTED' : 'WAITING FOR d2_session_*',
    `Act ${s.act} · ${s.area}`,
    `tick ${s.tick} · ${Math.round(fps)} fps · entities ${n}`,
    `life ${s.player.hp}/${s.player.hpMax} · mana ${s.player.mana}/${s.player.manaMax}`,
  ];
}
