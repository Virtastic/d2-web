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
  addStat: (stat, count = 1) => make(0x3a, le16((stat & 255) | (((count - 1) & 255) << 8))),   // 3
  addSkill: (skill) => make(0x3b, le16(skill)),                     // 3
  respawn: () => make(0x41),                                        // 1
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

const MONSTER_DEATH = 0, MONSTER_DEAD = 12, PLAYER_DEAD = 17;
const near = (dx, dy, w, up, down) => Math.abs(dx) <= w && dy >= -up && dy <= down;
// Objects whose use depends on what the cursor holds: the Compelling Orb wants the Khalim's Will flail,
// the Hell Forge in its open state wants the Hellforge Hammer. Without it the click sends nothing.
const COMPELLING_ORB = 404, HELL_FORGE = 376;
const CODE_QF2 = 0x20326671, CODE_HFH = 0x20686668;

/**
 * Turn the player's input into packets.
 *   transmit(bytes)      -> false when the connection is closed
 *   scene()              -> { ready, player:{x,y}, monsters:[{guid,classId,mode,x,y,anchorX,anchorY,drawn}], objects, items,
 *                             isNpc(classId), hoverBox(dx,dy), toScreen(wx,wy) -> {x,y} in pointer space, pointerWorld(px,py),
 *                             run, selected:{left,right}, cursorId, cursorCode() -> the held item's 4-byte code }
 * The engine hooks below each answer the local player's own prediction of an action: 1 accepted, 0 refused
 * (the packet is not sent, as the client sends only what it could start itself), -1 or absent unknown (sent).
 *   move(x, y, run)               -> starts the local route to a spot. The game server sends the walk/run
 *                                    broadcast only to the OTHER clients, so the owner moves on its click.
 *   approachUnit(type, guid, run) -> starts the local route to a unit (falls back to move() to its spot)
 *   reach(type, guid)             -> true when the unit is close enough to use (falls back to a tile distance)
 *   actionDone()                  -> true once the local player has finished its current action: a waiting
 *                                    interaction then re-checks its reach and re-approaches if short
 *   castAt(x, y), castOnUnit(type, guid) -> the right-hand skill's local start
 */
export function createRealmInput({ transmit, scene, now = () => 0, move = null, approachUnit = null, reach = null,
  actionDone = null, castAt = null, castOnUnit = null, onTalk = null, questFlags = null, leaveWait = null }) {
  const gate = createSendGate(transmit);
  const evidence = { attempts: 0, sent: 0, last: null };
  // The inventory's item actions pass the leave wait's gate first (1.13c 0x923F0): an armed wait is cancelled and the action refused.
  const itemGate = () => (leaveWait ? leaveWait.itemAction() : true);
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
  // Without the engine's reach test: five tiles each way is inside the server's talk range (fewer than 9 by
  // its size-aware distance) from every side; an item or object is used from two tiles.
  const inReach = (s, type, u) => {
    if (reach) { const r = reach(type, idOf(type, u)); if (r != null && r !== -1) return Boolean(r); }
    const d = Math.max(Math.abs(u.x - s.player.x), Math.abs(u.y - s.player.y));
    return d <= (type === UNIT.MONSTER ? 5 : 2);
  };
  const idOf = (type, u) => (type === UNIT.MONSTER ? u.guid : u.id);
  const find = (s, type, guid) => (type === UNIT.MONSTER ? s.monsters.find((m) => m.guid === guid && m.mode !== MONSTER_DEATH && m.mode !== MONSTER_DEAD)
    : type === UNIT.OBJECT ? s.objects.find((o) => o.id === guid) : type === UNIT.ITEM ? s.items.find((i) => i.id === guid && i.mode === 3) : null);
  // The local start comes first: the client sends an action only when its own player could start it.
  const predicted = (result) => { evidence.localMove = result; return result !== 0; };
  const startRoute = (x, y, run) => predicted(move ? move(x, y, Boolean(run)) : null);

  // The interaction waiting on the walk: the click stores it, and it is used on arrival, never on the click.
  let pending = null;
  // Returns null when the local start refused the route (nothing sent), else whether it went out. A refused
  // route still leaves the interaction waiting: 2AA40 stores the pending action whether or not it sent.
  function approach(s, type, u) {
    const run = Boolean(s.run), guid = idOf(type, u);
    if (!predicted(approachUnit ? approachUnit(type, guid, run) : move ? move(clamp16(u.x), clamp16(u.y), run) : null)) return null;
    return emit(run ? packets.runToUnit(type, guid) : packets.walkToUnit(type, guid), 'approach');
  }
  // The packet that uses a unit once the player has reached it.
  function use(s, type, u) {
    const guid = idOf(type, u);
    if (type === UNIT.ITEM) return emit(packets.pickUp(UNIT.ITEM, guid), 'pickup');
    if (type === UNIT.MONSTER) {
      const sent = emit(packets.interact(UNIT.MONSTER, guid), 'talk');
      if (sent && onTalk) onTalk(guid);
      return sent;
    }
    if (u.selectable !== false && u.mode === 0 && u.classId === COMPELLING_ORB && (s.cursorCode?.() >>> 0) !== CODE_QF2) return false;
    if (u.selectable !== false && u.mode === 2 && u.classId === HELL_FORGE && (s.cursorCode?.() >>> 0) !== CODE_HFH) return false;
    return emit(packets.interact(UNIT.OBJECT, guid), 'object');
  }
  // In reach, use it now; otherwise walk to it and keep it waiting.
  function arrive(s, reapproach) {
    const u = find(s, pending.type, pending.guid);
    if (!u) { pending = null; return null; }
    if (inReach(s, pending.type, u)) { const { type } = pending; pending = null; return use(s, type, u); }
    if (!reapproach) return null;
    const sent = approach(s, pending.type, u);
    return sent === null ? false : sent;
  }
  function interactWith(s, type, u) {
    pending = { type, guid: idOf(type, u) };
    return arrive(s, true);
  }

  return {
    evidence,
    get pending() { return pending ? { ...pending } : null; },
    pick: (px, py) => { const s = open(); return s ? pick(s, px, py) : null; },
    /** The left button on the world: act on what is under it, otherwise walk or run there. */
    leftClick(px, py, { held = false } = {}) {
      const s = open(); if (!s) return null;
      const hit = pick(s, px, py);
      const world = s.pointerWorld(px, py);
      if (hit.kind === 'monster') pending = null;
      if (hit.kind === 'monster') return emit(packets.leftSkillOnUnit(UNIT.MONSTER, hit.unit.guid, held), 'left-skill-unit') && hit;
      if (held && hit.kind !== 'ground') return hit;               // a held button does not repeat a pickup or a talk
      pending = null;
      const type = hit.kind === 'npc' ? UNIT.MONSTER : hit.kind === 'item' ? UNIT.ITEM : hit.kind === 'object' ? UNIT.OBJECT : null;
      if (type != null) return interactWith(s, type, hit.unit) === false ? false : hit;
      const x = clamp16(world.x), y = clamp16(world.y);
      if (!startRoute(x, y, s.run)) return false;
      return emit(s.run ? packets.runTo(x, y) : packets.walkTo(x, y), s.run ? 'run' : 'walk') && hit;
    },
    /** Once per frame: an interaction waiting on the walk is used once the unit is in reach. */
    tick() {
      if (!pending) return null;
      const s = open(); if (!s) return null;
      if (actionDone && !actionDone()) return null;
      return arrive(s, Boolean(actionDone));
    },
    /** Arrow keys: walk (or run) a step offset from where the player stands. */
    step(dx, dy) {
      const s = open(); if (!s) return null;
      pending = null;
      const x = clamp16(s.player.x + dx), y = clamp16(s.player.y + dy);
      if (!startRoute(x, y, s.run)) return false;
      return emit(s.run ? packets.runTo(x, y) : packets.walkTo(x, y), s.run ? 'run' : 'walk');
    },
    /** The interact key: talk to the nearest person, else operate the nearest object, within 7 tiles. */
    interactNearest() {
      const s = open(); if (!s) return null;
      const d = (u) => Math.max(Math.abs(u.x - s.player.x), Math.abs(u.y - s.player.y));
      const npc = s.monsters.filter((m) => s.isNpc(m.classId) && m.mode !== MONSTER_DEATH && m.mode !== MONSTER_DEAD && d(m) <= 7)
        .sort((a, b) => d(a) - d(b) || a.guid - b.guid)[0];
      if (npc) return interactWith(s, UNIT.MONSTER, npc) !== false;
      const object = s.objects.filter((o) => o.selectable !== false && d(o) <= 7).sort((a, b) => d(a) - d(b) || a.id - b.id)[0];
      if (!object) return false;
      return interactWith(s, UNIT.OBJECT, object) !== false;
    },
    /** The right button on the world: the right-hand skill at the unit under it, otherwise at the ground. */
    rightClick(px, py, { held = false } = {}) {
      const s = open(); if (!s) return null;
      const hit = pick(s, px, py);
      if (hit.kind === 'monster' || hit.kind === 'npc') {
        if (!predicted(castOnUnit ? castOnUnit(UNIT.MONSTER, hit.unit.guid) : null)) return false;
        return emit(packets.rightSkillOnUnit(UNIT.MONSTER, hit.unit.guid, held), 'right-skill-unit') && hit;
      }
      const world = s.pointerWorld(px, py), x = clamp16(world.x), y = clamp16(world.y);
      if (!predicted(castAt ? castAt(x, y) : null)) return false;
      return emit(packets.rightSkillAt(x, y, held), 'right-skill-ground') && hit;
    },
    /**
     * The server's answer to a talk (its 0x28 packet: type, guid, and a third field): the client opens the
     * conversation with 0x2F for that unit, or, when it no longer knows the unit, ends it with 0x30.
     */
    npcInteraction(bytes) {
      const type = bytes[1], guid = (bytes[2] | (bytes[3] << 8) | (bytes[4] << 16) | (bytes[5] << 24)) >>> 0, third = bytes[6];
      // The 96 quest-flag bytes at +7 replace the client's quest record: always for type 6, otherwise only when the unit is known.
      const flags = () => { if (questFlags && bytes.length >= 103) questFlags(bytes.subarray(7, 103)); };
      if (type === 6) { flags(); return null; }
      const s = open();
      if (!s || !find(s, type, guid)) { pending = null; return emit(make(0x30, le32(third), le32(guid)), 'npc-end'); }
      flags();
      return emit(packets.npcChatStart(type, guid), 'npc-start');
    },
    /** Belt key 1-4: the item in belt slot `column` exactly (the key handler passes the column as the slot). Nothing happens
     *  while the player is dead (mode 17), when the cell is empty, or while the cursor holds an item. The retail use routine
     *  also refuses an item whose row is not a plain useable one (useable byte not 1, or the unique/quest byte set), an item
     *  carrying a stat list with flag 2, and a hero in state 0x36: the scene's optional `beltRefused(item)` answers that
     *  (true = refuse; absent = sent). The game menu (retail's panel flag) never gets here: the shell drains input first. */
    useBelt(column, onMerc = false) {
      const s = open(); if (!s) return false;
      if (s.playerMode === PLAYER_DEAD || s.cursorId) return false;
      const slot = s.items.find((i) => i.mode === 2 && i.gridX === column);
      if (slot && s.beltRefused?.(slot)) return false;
      return slot ? emit(packets.useBeltItem(slot.id, onMerc), 'belt') : false;
    },
    /** The skill popup / hotkey choice. */
    selectSkill: (skill, left, owner = 0xffffffff) => emit(packets.selectSkill(skill, left, owner), 'select-skill'),
    /** An F-key's select-skill packet, composed from the server's hotkey table (d2gsHotkeySelect); null when the key is unbound. */
    hotkey: (bytes) => (bytes ? emit(bytes, 'hotkey-select') : false),
    /** The character screen's +: `count` points (1..100) into a stat, one packet; the server spends them or refuses. */
    addStat: (stat, count = 1) => (Number.isInteger(stat) && stat >= 0 && stat < 16 && count >= 1 && count <= 100
      ? emit(packets.addStat(stat, count), 'add-stat') : false),
    /** The skill tree's +: one point into a skill. */
    addSkill: (skill) => (Number.isInteger(skill) && skill >= 0 && skill <= 0xffff ? emit(packets.addSkill(skill), 'add-skill') : false),
    /** The death screen's continue. */
    respawn: () => emit(packets.respawn(), 'respawn'),
    useItem(guid) { const s = open(); return s && itemGate() ? emit(packets.useItem(guid, s.player.x, s.player.y), 'use-item') : false; },
    /** Inventory panel: lift a carried item (0x19) or a worn one (0x1C). */
    lift: (guid) => itemGate() && emit(packets.fromGrid(guid), 'lift'),
    unequip: (bodyLoc) => itemGate() && emit(packets.unequip(bodyLoc), 'unequip'),
    /** With an item on the cursor: into a grid cell (0x18), onto an empty body slot (0x1A), swapping a worn one (0x1D). */
    putInGrid: (guid, x, y, page = INV_PAGE.INVENTORY) => itemGate() && emit(packets.toGrid(guid, x, y, page), 'to-grid'),
    equip: (guid, bodyLoc) => itemGate() && emit(packets.equip(guid, bodyLoc), 'equip'),
    swapGrid: (cursorGuid, itemGuid, x, y) => itemGate() && emit(packets.swapCursorBuffer(cursorGuid, itemGuid, x, y), 'swap-grid'),
    swapBody: (guid, bodyLoc) => itemGate() && emit(packets.swapWithBody(guid, bodyLoc), 'swap-body'),
    dropCursor: (guid) => itemGate() && emit(packets.dropCursor(guid), 'drop'),
    swapWeapons: () => emit(packets.swapWeapons(), 'swap-weapons'),
    /** After the server has opened a menu: the waypoint list and a town-portal choice. */
    waypoint: (guid, levelId) => emit(packets.waypoint(guid, levelId), 'waypoint'),
    enterPortal: (guid, levelId) => emit(packets.enterPortal(guid, levelId), 'portal'),
    npcChatEnd: (guid) => emit(packets.npcChatEnd(UNIT.MONSTER, guid), 'npc-end'),
  };
}
