// Player-to-player trade over the game server connection: the server's trade packets decoded into a
// small state, the event lines a refused trade produces, and the bytes the client sends. Pure of the
// engine and the DOM, so it is testable in node. Text comes from the caller's string lookup.
const cstr = (b, o, n) => { let s = ''; for (let i = o; i < o + n && b[i]; i++) s += String.fromCharCode(b[i]); return s; };
const u32 = (b, o) => (b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)) >>> 0;

// Server 0x77 actions the trade window reacts to.
export const TRADE_UI = { ASK: 0, ASKED: 1, ACCEPT: 5, OPEN: 6, SELLER_NO_ROOM: 9, BUYER_NO_ROOM: 10, CLOSE: 12, DONE: 13, CHECK: 14, UNCHECK: 15 };
// Client 0x4F button ids of the trade window.
export const TRADE_BTN = { CANCEL: 2, PERFORM: 3, ACCEPT: 4, GREENCHECK: 7, SENDGOLD: 8 };

// Event (0x5A) kinds that report a refused or blocked trade/hostile request, with their string ids.
const BUSY = 4044, WAIT = 4047, ITEMS_HIS = 4134, ITEMS_YOURS = 4135, HOSTILE_WAIT = 10808;

// The server's 0x5A event as a message-log line, or null for a kind not formatted here.
// type 8 = "<name> is busy", 9 = wait before trading, 10 = items in a box, 16 = wait before hostility.
export function tradeEventLine(bytes, lang) {
  if (!bytes || bytes[0] !== 0x5a || bytes.length < 40) return null;
  const type = bytes[1], name = cstr(bytes, 8, 16);
  switch (type) {
    case 8: return name + lang(BUSY);
    case 9: return lang(WAIT);
    case 10: return name ? name + lang(ITEMS_HIS) : lang(ITEMS_YOURS);
    case 16: return lang(HOSTILE_WAIT);
    default: return null;
  }
}

export function newTrade() { return { open: false, partner: null, partnerGuid: 0, gold: 0, goldOwner: 0, ownCheck: false, theirCheck: false, asked: null }; }

// Apply a server trade packet (0x77, 0x78, 0x79) to the state. Returns the state.
export function applyTradePacket(t, bytes) {
  switch (bytes[0]) {
    case 0x77:
      switch (bytes[1]) {
        case TRADE_UI.ASK: case TRADE_UI.ASKED: t.asked = bytes[1]; break;
        case TRADE_UI.OPEN: Object.assign(t, { open: true, gold: 0, ownCheck: false, theirCheck: false }); break;
        case TRADE_UI.CHECK: t.theirCheck = true; break;
        case TRADE_UI.UNCHECK: t.theirCheck = false; break;
        case TRADE_UI.CLOSE: case TRADE_UI.DONE: Object.assign(t, newTrade()); break;
        default: break;
      }
      break;
    case 0x78: t.partner = cstr(bytes, 1, 16); t.partnerGuid = u32(bytes, 17); break;
    case 0x79: t.gold = u32(bytes, 2); t.goldOwner = bytes[1]; break;
    default: break;
  }
  return t;
}

// Client 0x13: interact with a player (unit type 0) -- asks that player to trade.
export function tradeRequestPacket(guid) {
  const b = new Uint8Array(9); b[0] = 0x13; new DataView(b.buffer).setUint32(5, guid >>> 0, true); return b;
}
// Client 0x4F: a trade window button. The gold amount rides as two words, high word first.
export function tradeButtonPacket(button, gold = 0) {
  const b = new Uint8Array(7), v = new DataView(b.buffer);
  b[0] = 0x4f; v.setUint16(1, button, true); v.setUint16(3, (gold >>> 16) & 0xffff, true); v.setUint16(5, gold & 0xffff, true); return b;
}
