// What a joined Battle.net game's D2GS server tells the client about the things the client state
// (src/core/realm/d2gs_client_state.cpp) does not keep: the objects around the player, the items on the
// ground, the player's own inventory, equipment, belt, stash, cube and cursor, the gold figures, and the
// worn items of the other players. Fed the same decoded server packets the client state is fed, it shapes
// them into the records the shared drawFrame reads in a local game (the item and object presentations,
// assets/item-renderer.mjs and assets/object-renderer.mjs).
//
// Packet semantics are 1.13c's: 0x51 adds an object and 0x0E changes its state, 0x0A removes a unit,
// 0x9C and 0x9D carry an item as its saved-item bitstream (the format ITEMS_DecodeItemFromBitstream reads
// without the 'JM' header or the seed), 0x19 and 0x1D-0x1F, 0x20 carry the gold stats, 0x03 starts an act.

const ITEM_FLAG = Object.freeze({ IDENTIFIED: 0x10, SOCKETED: 0x800, EAR: 0x10000, COMPACT: 0x200000,
  ETHEREAL: 0x400000, LOW_QUALITY: 0x2000000 });
const MODE = Object.freeze({ STORED: 0, EQUIP: 1, BELT: 2, GROUND: 3, CURSOR: 4, DROPPING: 5, SOCKETED: 6 });
const ACTION = Object.freeze({ FROM_CONTAINER: 5, UNEQUIP: 8, FROM_SHOP: 12, FROM_BELT: 15, AUTO_UNEQUIP: 17, IN_SOCKET: 19, TO_SHOP: 11 });
const GOLD_STAT = 14, STASH_GOLD_STAT = 15;
const GOLD_CODE = 0x20646c67;   // 'gld '
const NO_PAGE = 255;
export const PAGE = Object.freeze({ INVENTORY: 0, CUBE: 3, STASH: 4, BELT: 5, NONE: NO_PAGE });

/** LSB-first bit reader over a byte array (the save-format bitstream order). */
export function bitReader(bytes) {
  let bit = 0;
  const total = bytes.length * 8;
  return {
    get position() { return bit; },
    get overrun() { return bit > total; },
    read(count) {
      let value = 0;
      for (let i = 0; i < count; i++, bit++) {
        if (bit < total && (bytes[bit >> 3] >> (bit & 7)) & 1) value += 2 ** i;
      }
      return value;
    },
  };
}

/**
 * The part of an item's bitstream the screen needs: flags, mode, position, class code, quality and, for a
 * gold pile, the amount. Returns null for an ear (not drawn) or a stream too short to hold the header.
 */
export function decodeItemHeader(bytes) {
  const r = bitReader(bytes);
  const flags = r.read(32);
  r.read(10);   // the item format version
  const mode = r.read(3);
  const item = { flags, mode, x: 0, y: 0, bodyLoc: 0, page: NO_PAGE, code: 0, quality: 0, goldAmount: -1, socketCount: 0, level: 1 };
  if (mode === MODE.GROUND || mode === MODE.DROPPING) { item.x = r.read(16); item.y = r.read(16); }
  else { item.bodyLoc = r.read(4); item.x = r.read(4); item.y = r.read(4); item.page = (r.read(3) - 1) & 255; }
  if (flags & ITEM_FLAG.EAR || r.overrun) return null;
  item.code = r.read(32) >>> 0;
  if (flags & ITEM_FLAG.COMPACT) {
    if (item.code === GOLD_CODE) item.goldAmount = r.read(r.read(1) ? 32 : 12);
    item.quality = 2;
  } else if (!(flags & ITEM_FLAG.LOW_QUALITY)) {
    item.socketCount = r.read(3);
    item.level = r.read(7);
    item.quality = r.read(4);
  }
  return r.overrun ? null : item;
}

const u16 = (p, at) => p[at] | (p[at + 1] << 8);
const u32 = (p, at) => (p[at] | (p[at + 1] << 8) | (p[at + 2] << 16) | (p[at + 3] << 24)) >>> 0;

/**
 * `tables` is { items: createItemTable(read), objects: createObjectTable(read) } (assets/realm-tables.mjs).
 * The state is plain data; every method answers from what the packets fed so far said.
 */
export function createRealmWorld({ items: itemTable, objects: objectTable }) {
  let ownGuid = null;
  const itemsByGuid = new Map();    // guid -> { ...decoded, id, owner: null (0x9C) | owner guid (0x9D) }
  const objectsByGuid = new Map();
  let gold = 0, stashGold = 0, cursorId = 0, received = 0;

  function reset() {
    itemsByGuid.clear(); objectsByGuid.clear(); gold = 0; stashGold = 0; cursorId = 0; received = 0;
  }
  const isOwn = (item) => item.owner === null || item.owner === ownGuid;

  function removeItem(guid) {
    itemsByGuid.delete(guid);
    if (cursorId === guid) cursorId = 0;
  }

  function applyItem(packet, withOwner) {
    const action = packet[1], guid = u32(packet, 4);
    const owner = withOwner ? u32(packet, 9) : null;
    const decoded = decodeItemHeader(packet.subarray(withOwner ? 13 : 8));
    received++;
    if (action === ACTION.TO_SHOP || action === ACTION.FROM_SHOP || action === ACTION.IN_SOCKET) return;
    if (!decoded) { removeItem(guid); return; }
    if (decoded.mode === MODE.SOCKETED) { removeItem(guid); return; }
    const deleting = action === ACTION.FROM_CONTAINER || action === ACTION.UNEQUIP ||
      action === ACTION.FROM_BELT || action === ACTION.AUTO_UNEQUIP;
    if (deleting && decoded.mode !== MODE.CURSOR) { removeItem(guid); return; }
    const entry = itemTable.byCode(decoded.code);
    if (!entry) { removeItem(guid); return; }   // a code outside Weapons/Armor/Misc.txt cannot be drawn
    const item = { ...decoded, id: guid, owner, classId: entry.classId, entry };
    if (item.mode === MODE.EQUIP) {   // one item per body location of an owner
      for (const [other, held] of itemsByGuid) {
        if (other !== guid && held.mode === MODE.EQUIP && held.bodyLoc === item.bodyLoc && held.owner === owner) itemsByGuid.delete(other);
      }
    }
    itemsByGuid.set(guid, item);
    if (item.mode === MODE.CURSOR && (owner === null || owner === ownGuid)) cursorId = guid;
    else if (cursorId === guid) cursorId = 0;
  }

  function applyStat(stat, value) {
    if (stat === GOLD_STAT) gold = value >>> 0;
    else if (stat === STASH_GOLD_STAT) stashGold = value >>> 0;
  }

  function handle(p) {
    switch (p[0]) {
      case 0x03: itemsByGuid.forEach((item, guid) => { if (!isOwn(item) || item.mode === MODE.GROUND || item.mode === MODE.DROPPING) itemsByGuid.delete(guid); });
        objectsByGuid.clear(); break;   // a new act: its objects and ground items arrive afresh
      case 0x0a: if (p[1] === 2) objectsByGuid.delete(u32(p, 2));
        else if (p[1] === 4) { const held = itemsByGuid.get(u32(p, 2)); if (held && (held.mode === MODE.GROUND || held.mode === MODE.DROPPING)) itemsByGuid.delete(u32(p, 2)); }
        break;
      case 0x51: if (p[1] === 2) {
        const classId = u16(p, 6), cls = objectTable.byClass(classId);
        if (cls) objectsByGuid.set(u32(p, 2), { id: u32(p, 2), classId, token: cls.token, mode: p[12], x: u16(p, 8), y: u16(p, 10), selectable: p[13] !== 0 });
        } break;
      case 0x0e: if (p[1] === 2) { const object = objectsByGuid.get(u32(p, 2));
        if (object) { object.mode = u32(p, 8); object.selectable = p[7] !== 0; } } break;
      case 0x19: gold = (gold + p[1]) >>> 0; break;
      case 0x1d: applyStat(p[1], p[2]); break;
      case 0x1e: applyStat(p[1], u16(p, 2)); break;
      case 0x1f: applyStat(p[1], u32(p, 2)); break;
      case 0x20: if (ownGuid !== null && u32(p, 1) === ownGuid) applyStat(p[5], u32(p, 6)); break;
      case 0x9c: if (p.length >= 8) applyItem(p, false); break;
      case 0x9d: if (p.length >= 13) applyItem(p, true); break;
      default: break;
    }
  }

  function presentItem(item, tick = 0) {
    const e = item.entry, ground = item.mode === MODE.GROUND || item.mode === MODE.DROPPING;
    return { colourByte: 0xff, id: item.id, classId: item.classId, code: item.code, mode: item.mode,
      x: ground ? item.x : item.x, y: ground ? item.y : item.y, quality: item.quality, itemFlags: item.flags,
      framePrecise: 0xff00, frameCountPrecise: 0xff00, landed: ground,
      flippyFile: e.flippyFile, invFile: e.invFile,
      gridX: ground ? 0 : item.x, gridY: ground ? 0 : item.y,
      invPage: ground ? NO_PAGE : item.mode === MODE.BELT ? PAGE.BELT : item.page,
      bodyLoc: item.mode === MODE.EQUIP ? item.bodyLoc : 0, invWidth: e.invWidth, invHeight: e.invHeight,
      goldAmount: item.goldAmount, sockets: undefined, tick };
  }

  return {
    reset,
    setOwner(guid) { ownGuid = guid === null || guid === undefined ? null : guid >>> 0; },
    /** Apply every complete server packet of `bytes`; `sizeOf(subarray)` is the 1.13c packet-size reader. */
    feed(bytes, sizeOf) {
      for (let at = 0; at < bytes.length;) {
        const n = sizeOf(bytes.subarray(at));
        if (!(n > 0) || at + n > bytes.length) break;
        handle(bytes.subarray(at, at + n));
        at += n;
      }
    },
    /** The player's own items (inventory, equipment, belt, stash, cube, cursor, ground) as presentation records. */
    itemViews() { return [...itemsByGuid.values()].filter(isOwn).map((item) => presentItem(item)); },
    /** The worn items of another player, as presentation records. */
    wornBy(guid) { return [...itemsByGuid.values()].filter((item) => item.owner === (guid >>> 0) && item.mode === MODE.EQUIP).map((item) => presentItem(item)); },
    objectViews() {
      return [...objectsByGuid.values()].map((o) => ({ id: o.id, classId: o.classId, token: o.token, mode: o.mode, x: o.x, y: o.y,
        framePrecise: 0, frameCountPrecise: 0, componentMask: 0, animSpeed: 0, direction: 0, selectable: o.selectable,
        portalLevel: 0, locked: false, inClientRoom: true }));
    },
    get gold() { return gold; },
    get stashGold() { return stashGold; },
    get cursorId() { return cursorId; },
    get itemPacketsSeen() { return received; },
    get ownItemCount() { let n = 0; for (const item of itemsByGuid.values()) if (isOwn(item)) n++; return n; },
    /** The Belts.txt row the worn belt names (its armor row's `belt` column), 2 with no belt. */
    beltRecord() {
      for (const item of itemsByGuid.values()) if (isOwn(item) && item.mode === MODE.EQUIP && item.bodyLoc === 8 && item.entry.belt >= 0) return item.entry.belt;
      return 2;
    },
    /** The weapon class animation takes (Weapons.txt wclass, the two-handed class for a two-hander) of an owner's hands. */
    weaponClassOf(owner = null) {
      const worn = (loc) => [...itemsByGuid.values()].find((item) => item.mode === MODE.EQUIP && item.bodyLoc === loc &&
        (owner === null ? isOwn(item) : item.owner === (owner >>> 0)) && item.entry.kind === 'weapons');
      const weapon = worn(4) ?? worn(5);
      if (!weapon) return 'hth';
      const e = weapon.entry;
      return ((e.twoHanded && e.twoHandedWclass) || e.wclass || 'hth').toLowerCase();
    },
  };
}
