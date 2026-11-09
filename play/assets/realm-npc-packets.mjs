// The town-NPC packets of a Battle.net game: what the client sends to talk, trade, repair, gamble, identify
// and hire, and how it reads the server's answers (the NPC's message list, the shop stock, the transaction
// result). Layouts are the 1.13c wire formats; every multi-byte field is little-endian.

const le32 = (v) => [v & 255, (v >>> 8) & 255, (v >>> 16) & 255, (v >>> 24) & 255];
const le16 = (v) => [v & 255, (v >>> 8) & 255];
const make = (op, ...parts) => Uint8Array.from([op, ...parts.flat()]);
const u16 = (p, at) => p[at] | (p[at + 1] << 8);
const u32 = (p, at) => (p[at] | (p[at + 1] << 8) | (p[at + 2] << 16) | (p[at + 3] << 24)) >>> 0;

/** The 0x38 action the client asks the NPC for (the server answers a trade with the stock, a hire with the offers). */
export const NPC_ACTION = Object.freeze({ TRADE: 1, GAMBLE: 2, HIRE: 3 });
/** The item mode a sold item is in (the server checks it against the item's own mode). */
export const SELL_MODE = Object.freeze({ STORED: 0, CURSOR: 4 });
/** The last field of a repair packet that asks for every item. */
export const REPAIR_ALL_FLAG = 0x80000000;

export const npcPackets = Object.freeze({
  /** Answer one of the NPC's speech lines (the server removes the quest message it carried). */
  messageDone: (npcGuid, messageId) => make(0x31, le32(npcGuid), le32(messageId)),
  /**
   * Buy the stocked item. The third field is (mode << 16) | 2 when gambling | 0x80000000 for the multi-buy; the
   * fourth is the displayed cost.
   */
  buy: (npcGuid, itemGuid, mode, cost, { gamble = false, multi = false } = {}) =>
    make(0x32, le32(npcGuid), le32(itemGuid), le32(((mode & 0xffff) << 16 | (gamble ? 2 : 0) | (multi ? 0x80000000 : 0)) >>> 0), le32(cost)),
  /** Sell an item to the NPC for `cost` (the server prices it itself). */
  sell: (npcGuid, itemGuid, mode = SELL_MODE.STORED, cost = 0) =>
    make(0x33, le32(npcGuid), le32(itemGuid), le16(mode), le16(0), le32(cost)),
  /** Ask Cain, Deckard or Anya to identify everything in the inventory. */
  identifyAll: (npcGuid) => make(0x34, le32(npcGuid)),
  /** Repair one item: its mode in the third field, its durability stat in the fourth. */
  repair: (npcGuid, itemGuid, mode = 0, durability = 0) => make(0x35, le32(npcGuid), le32(itemGuid), le32(mode & 0xffff), le32(durability)),
  repairAll: (npcGuid) => make(0x35, le32(npcGuid), le32(0), le32(0), le32(REPAIR_ALL_FLAG)),
  hire: (npcGuid, hirelingId) => make(0x36, le32(npcGuid), le32(hirelingId)),
  /** Identify the item a gamble bought (it comes back unidentified from the shop). */
  identifyGambled: (itemGuid) => make(0x37, le32(itemGuid)),
  /** Ask for the trade, gamble or hire list: the action, the NPC, then the player's guid (-1 when there is none). */
  request: (action, npcGuid, playerGuid = 0xffffffff) => make(0x38, le32(action), le32(npcGuid), le32(playerGuid)),
  resurrectMerc: (npcGuid) => make(0x62, le32(npcGuid)),
});

/** S2C 0x27: the lines an NPC offers when it is spoken to, [{ menu, stringId }], or null when malformed. */
export function parseNpcInfo(p) {
  if (p[0] !== 0x27 || p.length < 8) return null;
  const count = Math.min(p[6], 8, Math.floor((p.length - 8) / 4));
  const messages = [];
  for (let i = 0; i < count; i++) messages.push({ menu: p[8 + i * 4], stringId: u16(p, 10 + i * 4) });
  return { unitType: p[1], guid: u32(p, 2), messages };
}

/** S2C 0x2A: the transaction outcome. A result above 6 is a refusal (see vendor-repair-confirm.mjs). */
export function parseTransaction(p) {
  if (p[0] !== 0x2a || p.length < 15) return null;
  return { result: p[2], guid: u32(p, 7), gold: u32(p, 11) };
}

/** S2C 0x4F (1 byte): the head of the hire list; it restarts the offers. Its layout is the single-player one (gameplay-shell.mjs). */
export const HIRE_LIST_HEAD = 0x4f;
/** S2C 0x4E (7 bytes): one hire offer, { mercName (u16, the string.tbl id of the name), seed (u32) }, else null. */
export function parseHireOffer(p) {
  if (p[0] !== 0x4e || p.length < 7) return null;
  return { mercName: u16(p, 1), seed: u32(p, 3) };
}

/** S2C 0x9C when it stocks (action 11) or empties (action 12) a shop slot, else null. */
export function parseShopItem(p) {
  if (p[0] !== 0x9c || p.length < 8 || (p[1] !== 11 && p[1] !== 12)) return null;
  return { add: p[1] === 11, guid: u32(p, 4), bits: p.subarray(8) };
}
