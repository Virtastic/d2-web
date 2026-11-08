// What the player's input sends in a joined Battle.net game. The same clicks and keys a local game
// reads become the retail 1.13c D2GS client packets (layouts: D2MOO D2PacketDef.h / PlrMsg.cpp, and the
// size table in src/core/realm/d2gs_session.cpp); nothing here moves a unit. The server walks the
// player to a target, runs the skill and answers with its own packets, and the position the screen
// shows comes back from those packets only.

export const UNIT = Object.freeze({ PLAYER: 0, MONSTER: 1, OBJECT: 2, MISSILE: 3, ITEM: 4, TILE: 5 });
export const INV_PAGE = Object.freeze({ INVENTORY: 0, TRADE: 2, CUBE: 3, STASH: 4 });

const le16 = (v) => [v & 255, (v >>> 8) & 255];
const le32 = (v) => [v & 255, (v >>> 8) & 255, (v >>> 16) & 255, (v >>> 24) & 255];
const make = (op, ...parts) => Uint8Array.from([op, ...parts.flat()]);
const at = (op, x, y) => make(op, le16(x), le16(y));
const unit = (op, type, guid) => make(op, le32(type), le32(guid));

/** Every client packet the input sends, by what it does.  The comment names the retail opcode's size. */
export const packets = Object.freeze({
  walkTo: (x, y) => at(0x01, x, y),                                 // 5
  runTo: (x, y) => at(0x03, x, y),                                  // 5
  walkToUnit: (type, guid) => unit(0x02, type, guid),               // 9
  runToUnit: (type, guid) => unit(0x04, type, guid),                // 9
  leftSkillAt: (x, y, held = false) => at(held ? 0x08 : 0x05, x, y),          // 5
  leftSkillOnUnit: (type, guid, held = false) => unit(held ? 0x09 : 0x06, type, guid), // 9
  rightSkillAt: (x, y, held = false) => at(held ? 0x0f : 0x0c, x, y),         // 5
  rightSkillOnUnit: (type, guid, held = false) => unit(held ? 0x10 : 0x0d, type, guid), // 9
  interact: (type, guid) => unit(0x13, type, guid),                 // 9
  pickUp: (type, guid, toInventory = false) => make(0x16, le32(type), le32(guid), le32(toInventory ? 1 : 0)), // 13
  dropCursor: (guid) => make(0x17, le32(guid)),                     // 5
  toGrid: (guid, x, y, page) => make(0x18, le32(guid), le32(x), le32(y), le32(page)), // 17
  fromGrid: (guid) => make(0x19, le32(guid)),                       // 5
  equip: (guid, bodyLoc) => make(0x1a, le32(guid), le32(bodyLoc)),  // 9
  swapOneForTwoHanded: (guid, bodyLoc) => make(0x1b, le32(guid), le32(bodyLoc)), // 9
  unequip: (bodyLoc) => make(0x1c, le16(bodyLoc)),                  // 3
  swapWithBody: (guid, bodyLoc) => make(0x1d, le32(guid), le32(bodyLoc)),   // 9
  swapCursorBuffer: (cursorGuid, itemGuid, x, y) => make(0x1f, le32(cursorGuid), le32(itemGuid), le32(x), le32(y)), // 17
  useItem: (guid, x, y) => make(0x20, le32(guid), le32(x), le32(y)),        // 13
  useBeltItem: (guid, onMerc = false) => make(0x26, le32(guid), le32(onMerc ? 1 : 0), le32(0)), // 13
  npcChatStart: (type, guid) => unit(0x2f, type, guid),             // 9
  npcChatEnd: (type, guid) => unit(0x30, type, guid),               // 9
  npcAction: (action, npcGuid, itemGuid = 0) => make(0x38, le32(action), le32(npcGuid), le32(itemGuid)), // 13
  selectSkill: (skill, left, ownerGuid = 0xffffffff) => make(0x3c, le32(((left ? 0x80000000 : 0) | (skill & 0x7fff)) >>> 0), le32(ownerGuid)), // 9
  enterPortal: (portalGuid, levelId) => make(0x45, le32(portalGuid), le32(levelId)),    // 9
  waypoint: (waypointGuid, levelId) => make(0x49, le32(waypointGuid), le32(levelId)),   // 9
  swapWeapons: () => make(0x60),                                    // 1
});

// D2Client+0x143E0: the one outgoing-packet gate.  A packet equal to the last one sent, inside its
// opcode's window, is dropped; the last-packet store is shared by every opcode.  0x3A has no window,
// 5..0x0A and 0x0C..0x11 wait 50 ms, everything else 200 ms.
export function sendWindow(opcode) {
  const t = (opcode - 5) >>> 0;
  if (t > 0x35) return 200;
  if (t === 0x35) return 0;
  return t === 6 || t >= 13 ? 200 : 50;
}
export function createSendGate(transmit) {
  let lastTick = 0, last = null;
  return {
    /** Send `bytes` unless the gate drops it; returns true when it went out. */
    send(bytes, now) {
      const time = Number(now) >>> 0, win = sendWindow(bytes[0]);
      if (win && last && ((time - lastTick) >>> 0) < win && last.length === bytes.length && last.every((b, i) => b === bytes[i])) return false;
      last = Uint8Array.from(bytes); lastTick = time;
      return transmit(bytes) !== false;
    },
  };
}

const MONSTER_DEATH = 0, MONSTER_DEAD = 12;
const near = (dx, dy, w, up, down) => Math.abs(dx) <= w && dy >= -up && dy <= down;

/**
 * Turn the player's input into packets.
 *   transmit(bytes)      -> false when the connection is closed
 *   scene()              -> { ready, player:{x,y}, monsters:[{guid,classId,mode,x,y,anchorX,anchorY,drawn}], objects, items,
 *                             isNpc(classId), hoverBox(dx,dy), toScreen(wx,wy) -> {x,y} in pointer space, pointerWorld(px,py),
 *                             run, selected:{left,right}, cursorId }
 */
export function createRealmInput({ transmit, scene, now = () => 0 }) {
  const gate = createSendGate(transmit);
  const evidence = { attempts: 0, sent: 0, last: null };
  function emit(bytes, what) {
    evidence.attempts++;
    const sent = gate.send(bytes, now());
    if (sent) evidence.sent++;
    evidence.last = { what, opcode: bytes[0], sent, packet: Array.from(bytes) };
    return sent;
  }
  const open = () => { const s = scene(); return s?.ready ? s : null; };

  // What is under the pointer, nearest the player first: a unit, then a ground item, then an object.
  function pick(s, px, py) {
    const me = s.player, d2 = (u) => (u.x - me.x) ** 2 + (u.y - me.y) ** 2;
    const units = s.monsters.filter((m) => m.drawn && m.mode !== MONSTER_DEATH && m.mode !== MONSTER_DEAD &&
      s.hoverBox(px - m.anchorX, py - m.anchorY)).sort((a, b) => d2(a) - d2(b));
    if (units[0]) return s.isNpc(units[0].classId) ? { kind: 'npc', unit: units[0] } : { kind: 'monster', unit: units[0] };
    const spot = (u) => { const p = s.toScreen(u.x, u.y); return near(px - p.x, py - p.y, 22, 40, 12); };
    const item = s.items.filter((i) => i.mode === 3 && spot(i)).sort((a, b) => d2(a) - d2(b))[0];
    if (item) return { kind: 'item', unit: item };
    const object = s.objects.filter((o) => o.selectable !== false && spot(o)).sort((a, b) => d2(a) - d2(b))[0];
    if (object) return { kind: 'object', unit: object };
    return { kind: 'ground' };
  }
  const clamp16 = (v) => Math.max(0, Math.min(0xffff, v | 0));

  return {
    evidence,
    pick: (px, py) => { const s = open(); return s ? pick(s, px, py) : null; },
    /** The left button on the world: act on what is under it, otherwise walk or run there. */
    leftClick(px, py, { held = false } = {}) {
      const s = open(); if (!s) return null;
      const hit = pick(s, px, py);
      const world = s.pointerWorld(px, py);
      if (hit.kind === 'monster') return emit(packets.leftSkillOnUnit(UNIT.MONSTER, hit.unit.guid, held), 'left-skill-unit') && hit;
      if (held && hit.kind !== 'ground') return hit;               // a held button does not repeat a pickup or a talk
      if (hit.kind === 'npc') return emit(packets.interact(UNIT.MONSTER, hit.unit.guid), 'talk') && hit;
      if (hit.kind === 'item') return emit(packets.pickUp(UNIT.ITEM, hit.unit.id), 'pickup') && hit;
      if (hit.kind === 'object') return emit(packets.interact(UNIT.OBJECT, hit.unit.id), 'object') && hit;
      const x = clamp16(world.x), y = clamp16(world.y);
      return emit(s.run ? packets.runTo(x, y) : packets.walkTo(x, y), s.run ? 'run' : 'walk') && hit;
    },
    /** The right button on the world: the right-hand skill at the unit under it, otherwise at the ground. */
    rightClick(px, py, { held = false } = {}) {
      const s = open(); if (!s) return null;
      const hit = pick(s, px, py);
      if (hit.kind === 'monster' || hit.kind === 'npc') return emit(packets.rightSkillOnUnit(UNIT.MONSTER, hit.unit.guid, held), 'right-skill-unit') && hit;
      const world = s.pointerWorld(px, py);
      return emit(packets.rightSkillAt(clamp16(world.x), clamp16(world.y), held), 'right-skill-ground') && hit;
    },
    /** Belt key 1-4: the bottom item of that belt column. */
    useBelt(column) {
      const s = open(); if (!s) return false;
      const slot = s.items.filter((i) => i.mode === 2 && i.gridX % 4 === column)
        .sort((a, b) => a.gridX - b.gridX)[0];
      return slot ? emit(packets.useBeltItem(slot.id), 'belt') : false;
    },
    /** The skill popup / hotkey choice. */
    selectSkill: (skill, left) => emit(packets.selectSkill(skill, left), 'select-skill'),
    useItem(guid) { const s = open(); return s ? emit(packets.useItem(guid, s.player.x, s.player.y), 'use-item') : false; },
    /** Inventory panel: lift a carried item (0x19) or a worn one (0x1C). */
    lift: (guid) => emit(packets.fromGrid(guid), 'lift'),
    unequip: (bodyLoc) => emit(packets.unequip(bodyLoc), 'unequip'),
    /** With an item on the cursor: into a grid cell (0x18), onto an empty body slot (0x1A), swapping a worn one (0x1D). */
    putInGrid: (guid, x, y, page = INV_PAGE.INVENTORY) => emit(packets.toGrid(guid, x, y, page), 'to-grid'),
    equip: (guid, bodyLoc) => emit(packets.equip(guid, bodyLoc), 'equip'),
    swapGrid: (cursorGuid, itemGuid, x, y) => emit(packets.swapCursorBuffer(cursorGuid, itemGuid, x, y), 'swap-grid'),
    swapBody: (guid, bodyLoc) => emit(packets.swapWithBody(guid, bodyLoc), 'swap-body'),
    dropCursor: (guid) => emit(packets.dropCursor(guid), 'drop'),
    swapWeapons: () => emit(packets.swapWeapons(), 'swap-weapons'),
    /** After the server has opened a menu: the waypoint list and a town-portal choice. */
    waypoint: (guid, levelId) => emit(packets.waypoint(guid, levelId), 'waypoint'),
    enterPortal: (guid, levelId) => emit(packets.enterPortal(guid, levelId), 'portal'),
    npcChatEnd: (guid) => emit(packets.npcChatEnd(UNIT.MONSTER, guid), 'npc-end'),
  };
}
