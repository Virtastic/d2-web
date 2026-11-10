// What a joined Battle.net game's D2GS server tells the client about the things the client state
// (src/core/realm/d2gs_client_state.cpp) does not keep: the items on the ground, the player's own inventory,
// equipment, belt, stash, cube and cursor, the gold figures, and the worn items of the other players. Fed the
// same decoded server packets the client state is fed, it shapes them into the records the shared drawFrame
// reads in a local game (the item presentations, assets/item-renderer.mjs).
//
// The items a player HOLDS (grids, body, belt, cursor) are the engine's: the wasm client seats every item
// record on the player's unit as retail does, and `heldItems` reads them back. This file's own item map then
// supplies only what the unit read does not carry (the ground items, the raw bitstream a tooltip is decoded
// from). Without `heldItems` (an engine build that lacks the read) the map answers alone, as before.
//
// Objects are not here: the engine's object unit is their one model (assets/realm-objects.mjs reads it back).
//
// Packet semantics are 1.13c's: 0x0A removes a unit, 0x9C and 0x9D carry an item as its saved-item bitstream
// (the format ITEMS_DecodeItemFromBitstream reads without the 'JM' header or the seed), 0x19 and 0x1D-0x1F,
// 0x20 carry the gold stats, 0x03 starts an act.

const ITEM_FLAG = Object.freeze({ IDENTIFIED: 0x10, SOCKETED: 0x800, EAR: 0x10000, COMPACT: 0x200000,
  ETHEREAL: 0x400000, LOW_QUALITY: 0x2000000 });
const MODE = Object.freeze({ STORED: 0, EQUIP: 1, BELT: 2, GROUND: 3, CURSOR: 4, DROPPING: 5, SOCKETED: 6 });
const ACTION = Object.freeze({ FROM_CONTAINER: 5, UNEQUIP: 8, FROM_SHOP: 12, FROM_BELT: 15, AUTO_UNEQUIP: 17, IN_SOCKET: 19, TO_SHOP: 11 });
// The actions each item packet's handler accepts; any other action is a fatal error in retail's handler, so it changes nothing here.
const ACTIONS_9C = new Set([0, 1, 2, 3, 4, 10, 11, 12, 13, 14, 15, 16, 18]);
const ACTIONS_9D = new Set([5, 6, 7, 8, 9, 17, 19, 20, 21, 22, 23]);
const GOLD_STAT = 14, STASH_GOLD_STAT = 15;
const GOLD_CODE = 0x20646c67;   // 'gld '
const NO_PAGE = 255;
export const PAGE = Object.freeze({ INVENTORY: 0, CUBE: 3, STASH: 4, NONE: NO_PAGE });

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
 * `tables` is { items: createItemTable(read) } (assets/realm-tables.mjs).
 * `heldItems(ownerGuid)` (optional) returns the engine's records of the items a player unit holds (0 = the
 * client's own player), or null when the engine cannot say: { guid, classId, code, mode, x, y, page, bodyLoc,
 * quality, flags }.
 */
export function createRealmWorld({ items: itemTable, heldItems = null }) {
  let ownGuid = null;
  const itemsByGuid = new Map();    // guid -> { ...decoded, id, owner: null (0x9C) | owner guid (0x9D) }
  let gold = 0, stashGold = 0, cursorId = 0, received = 0;
  let useItem = null, inventoryAsks = 0;   // 0x3f: the item the cursor is using, and how often it asked for the inventory
  const uiEvents = [];              // 0x77 UpdateUi codes not yet taken (16 opens the stash, 17 closes it, 21 opens the cube)

  let revision = 0;                 // bumped by every feed and reset: the engine reads below are cached per revision
  const heldCache = new Map();
  // The engine's items of one player as this file's item shape, or null when the engine has no answer.
  function held(owner) {
    if (!heldItems) return null;
    const key = owner === null || owner === ownGuid ? 0 : owner >>> 0;
    const cached = heldCache.get(key);
    if (cached && cached.revision === revision) return cached.items;
    const records = heldItems(key);
    const items = records ? records.flatMap((r) => {
      const entry = itemTable.byCode(r.code);
      return entry ? [{ id: r.guid, owner: key || ownGuid, classId: entry.classId, entry, code: r.code, mode: r.mode, x: r.x, y: r.y,
        page: r.page, bodyLoc: r.bodyLoc, quality: r.quality, flags: r.flags, goldAmount: itemsByGuid.get(r.guid)?.goldAmount ?? -1 }] : [];
    }) : null;
    heldCache.set(key, { revision, items });
    return items;
  }
  const onGround = (item) => item.mode === MODE.GROUND || item.mode === MODE.DROPPING;
  // The client's own items: what its unit holds plus what lies on the ground.
  function ownItems() {
    const mine = held(null);
    const copy = [...itemsByGuid.values()].filter(isOwn);
    return mine ? [...copy.filter(onGround), ...mine] : copy;
  }
  const wornItems = (owner) => held(owner)?.filter((item) => item.mode === MODE.EQUIP)
    ?? [...itemsByGuid.values()].filter((item) => item.mode === MODE.EQUIP && (owner === null ? isOwn(item) : item.owner === (owner >>> 0)));

  function reset() {
    revision++; heldCache.clear();
    itemsByGuid.clear(); uiEvents.length = 0; gold = 0; stashGold = 0; cursorId = 0; received = 0; useItem = null; inventoryAsks = 0;
  }
  const isOwn = (item) => item.owner === null || item.owner === ownGuid;

  function removeItem(guid) {
    itemsByGuid.delete(guid);
    if (cursorId === guid) cursorId = 0;
    if (useItem?.id === guid) useItem = null;
  }

  function applyItem(packet, withOwner) {
    const action = packet[1], guid = u32(packet, 4);
    const owner = withOwner ? u32(packet, 9) : null;
    const decoded = decodeItemHeader(packet.subarray(withOwner ? 13 : 8));
    received++;
    if (!(withOwner ? ACTIONS_9D : ACTIONS_9C).has(action)) return;
    if (action === ACTION.TO_SHOP || action === ACTION.FROM_SHOP || action === ACTION.IN_SOCKET) return;
    if (!decoded) { removeItem(guid); return; }
    if (decoded.mode === MODE.SOCKETED) { removeItem(guid); return; }
    const deleting = action === ACTION.FROM_CONTAINER || action === ACTION.UNEQUIP ||
      action === ACTION.FROM_BELT || action === ACTION.AUTO_UNEQUIP;
    if (deleting && decoded.mode !== MODE.CURSOR) { removeItem(guid); return; }
    const entry = itemTable.byCode(decoded.code);
    if (!entry) { removeItem(guid); return; }   // a code outside Weapons/Armor/Misc.txt cannot be drawn
    const item = { ...decoded, id: guid, owner, classId: entry.classId, entry, raw: packet.slice(withOwner ? 13 : 8) };
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
        break;   // a new act: its ground items arrive afresh
      case 0x0a: if (p[1] === 4) { const held = itemsByGuid.get(u32(p, 2)); if (held && (held.mode === MODE.GROUND || held.mode === MODE.DROPPING)) itemsByGuid.delete(u32(p, 2)); }
        break;
      case 0x19: gold = (gold + p[1]) >>> 0; break;
      case 0x1d: applyStat(p[1], p[2]); break;
      case 0x1e: applyStat(p[1], u16(p, 2)); break;
      case 0x1f: applyStat(p[1], u32(p, 2)); break;
      case 0x20: if (ownGuid !== null && u32(p, 1) === ownGuid) applyStat(p[5], u32(p, 6)); break;
      // A corpse was made (flag set): what its owner wore is on the corpse now, and the server sends no per-item removal.
      // The cursor takes up or lets go of an item it is using (a scroll or key being aimed). Letting go (kind 0xff,
      // word 0xffff) clears the item's targeting flag (4); taking up needs the item, sets the flag when the word
      // is 0xffff and keeps the kind. Kinds 0 and 2 ask for the inventory to be open.
      case 0x3f: if (p.length >= 8) { const kind = p[1], guid = u32(p, 2), word = u16(p, 6), held = itemsByGuid.get(guid);
        if (kind === 0xff && word === 0xffff) { if (held) held.flags = (held.flags & ~4) >>> 0; useItem = null; }
        else if (held) { if (word === 0xffff) held.flags = (held.flags | 4) >>> 0; useItem = { id: guid, kind }; if (kind === 0 || kind === 2) inventoryAsks++; }
      } break;
      // The local player's cursor item is gone.
      case 0x42: if (p.length >= 6 && p[1] === 0 && ownGuid !== null && u32(p, 2) === ownGuid && cursorId) removeItem(cursorId); break;
      case 0x74: if (p.length >= 10 && p[1]) { const owner = u32(p, 2);
        itemsByGuid.forEach((item, guid) => { if (item.mode === MODE.EQUIP && (item.owner === owner || (owner === ownGuid && item.owner === null))) itemsByGuid.delete(guid); });
      } break;
      case 0x77: if (p.length >= 2) uiEvents.push(p[1]); break;
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
      invPage: ground ? NO_PAGE : item.page,   // the item's own page: a belt or worn item has none, as the engine's item says
      bodyLoc: item.mode === MODE.EQUIP ? item.bodyLoc : 0, invWidth: e.invWidth, invHeight: e.invHeight, twoHanded: Boolean(e.twoHanded),
      goldAmount: item.goldAmount, sockets: undefined, tick };
  }

  return {
    reset,
    setOwner(guid) { ownGuid = guid === null || guid === undefined ? null : guid >>> 0; },
    /** Apply every complete server packet of `bytes`; `sizeOf(subarray)` is the 1.13c packet-size reader. */
    feed(bytes, sizeOf) {
      revision++;
      for (let at = 0; at < bytes.length;) {
        const n = sizeOf(bytes.subarray(at));
        if (!(n > 0) || at + n > bytes.length) break;
        handle(bytes.subarray(at, at + n));
        at += n;
      }
    },
    /** The player's own items (inventory, equipment, belt, stash, cube, cursor, ground) as presentation records. */
    itemViews() { return ownItems().map((item) => presentItem(item)); },
    /** The worn items of another player, as presentation records. */
    wornBy(guid) { return wornItems(guid >>> 0).map((item) => presentItem(item)); },
    /** The saved-item bitstream the server sent for an item (what its tooltip text is decoded from). */
    rawItem(guid) { return itemsByGuid.get(guid >>> 0)?.raw ?? null; },
    /** The 0x77 UpdateUi codes received since the last call. */
    takeUiEvents() { return uiEvents.splice(0); },
    get gold() { return gold; },
    get stashGold() { return stashGold; },
    get cursorId() { const mine = held(null); return mine ? (mine.find((item) => item.mode === MODE.CURSOR)?.id ?? 0) : cursorId; },
    get useItem() { return useItem; },
    takeInventoryAsks() { const n = inventoryAsks; inventoryAsks = 0; return n; },
    get itemPacketsSeen() { return received; },
    get ownItemCount() { return ownItems().length; },
    /** The Belts.txt row the worn belt names (its armor row's `belt` column), 2 with no belt. */
    beltRecord() {
      for (const item of wornItems(null)) if (item.bodyLoc === 8 && item.entry.belt >= 0) return item.entry.belt;
      return 2;
    },
    /** The weapon class animation takes (Weapons.txt wclass, the two-handed class for a two-hander) of an owner's hands. */
    weaponClassOf(owner = null) {
      const all = wornItems(owner);
      const worn = (loc) => all.find((item) => item.bodyLoc === loc && item.entry.kind === 'weapons');
      const weapon = worn(4) ?? worn(5);
      if (!weapon) return 'hth';
      const e = weapon.entry;
      return ((e.twoHanded && e.twoHandedWclass) || e.wclass || 'hth').toLowerCase();
    },
  };
}
