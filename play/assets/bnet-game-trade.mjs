// Player-to-player trade over the game server connection: the client's trade state machine. The server's
// trade packets (0x77 action, 0x78 partner, 0x79 gold) and the player's presses go in, the bytes the client
// sends come out. Nothing here moves an item or gold: both grids and both gold figures change only on what
// the server sends. Pure of the engine and the DOM, so it is testable in node; text comes from the caller.
const cstr = (b, o, n) => { let s = ''; for (let i = o; i < o + n && b[i]; i++) s += String.fromCharCode(b[i]); return s; };
const u32 = (b, o) => (b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)) >>> 0;

// Server 0x77 actions the trade window reacts to.
export const TRADE_UI = { ASK: 0, ASKED: 1, RESET: 2, ACCEPT: 5, OPEN: 6, SELLER_NO_ROOM: 9, BUYER_NO_ROOM: 10, CLOSE: 12, DONE: 13, CHECK: 14, UNCHECK: 15 };
// Client 0x4F button ids of the trade window.
export const TRADE_BTN = { CANCEL: 2, PERFORM: 3, ACCEPT: 4, GREENCHECK: 7, SENDGOLD: 8 };
// The client's trade state: none, I asked (wait box), I was asked (prompt), trading, the partner accepted,
// both accepted, and waiting on the server after my own accept or cancel. 4 is tested by the client but never set.
export const TRADE_STATE = { NONE: 0, ASKING: 1, ASKED: 2, OPEN: 3, OWN_ACCEPTED: 4, THEY_ACCEPTED: 5, BOTH: 6, WAIT: 7 };
// String-table ids of the two boxes and the panel.
export const TRADE_STR = { ASKED: 4042, ASKING: 4043, CANCEL: 3400, ACCEPT: 3401, GOLD: 2215, TIP_ACCEPT: 4131, TIP_CANCEL: 4130 };
export const TRADE_WAIT_MS = 30000;   // a cancel the server never answers ends the game after this long
export const TRADE_ITEM_MS = 250;     // in a multiplayer game the inventory takes one item action per this long
// Sounds.txt rows the trade screen plays, and the hero voice event of a trade that does not fit.
export const TRADE_SOUND = { BOTH_ACCEPTED: 1, BOX_BUTTON: 2, MARK: 3, PRESS: 4, GOLD: 0xdd };
export const TRADE_SPEECH_NO_ROOM = 0x17;

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

// Client 0x13: interact with a player (unit type 0) -- asks that player to trade.
export function tradeRequestPacket(guid) {
  const b = new Uint8Array(9); b[0] = 0x13; new DataView(b.buffer).setUint32(5, guid >>> 0, true); return b;
}
// Client 0x4F: a trade window button. The gold amount rides as two words, high word first.
export function tradeButtonPacket(button, gold = 0) {
  const b = new Uint8Array(7), v = new DataView(b.buffer);
  b[0] = 0x4f; v.setUint16(1, button, true); v.setUint16(3, (gold >>> 16) & 0xffff, true); v.setUint16(5, gold & 0xffff, true); return b;
}
const cancelBytes = () => tradeButtonPacket(TRADE_BTN.CANCEL);

/**
 * `ui` is the trade screen's own flag (set from the request on), `panel` the two-grid window (from the server's
 * OPEN on), `box` the request box on screen ('asking' | 'asked' | null). `latch` is the accept button held green
 * by my own press; `ownAccept`/`theirAccept` are the server-driven marks; `blocked` paints my accept button red
 * while the server holds the trade after an item was taken back. `waitSince` is the cancel's clock, null when unarmed.
 */
export function newTrade() {
  return { state: 0, ui: false, panel: false, box: null, partner: null, partnerGuid: 0, ownGold: 0, theirGold: 0,
    ownAccept: false, theirAccept: false, blocked: false, latch: false, origin: false, acceptDown: false, cancelDown: false,
    cancelPressed: false, waitSince: null, noRoom: null, done: false, goldBox: false, lastItemAt: -Infinity, events: [] };
}
/** What the screen asked to be heard since the last call: { sound: row } and { speech: event, who: 'own' | 'partner', guid }. */
export function takeTradeEvents(t) { return t.events.splice(0); }
const sound = (t, row) => { t.events.push({ sound: row }); };
const clearMouse = (t) => { t.origin = false; t.latch = false; t.cancelPressed = false; t.acceptDown = false; t.cancelDown = false; };
// The screen's (re)open step: both accept marks and every mouse flag drop; `panel` shows the two grids.
function reopen(t, panel) { t.ownAccept = false; t.theirAccept = false; if (panel) { t.panel = true; t.lastItemAt = -Infinity; } clearMouse(t); }
// Close both boxes; unless a reply is awaited, close the screen and (when a trade was live) tell the server.
function closeTrade(t, out) {
  t.box = null;
  if (t.state === TRADE_STATE.WAIT) return;
  if (t.ui) { t.ui = false; t.panel = false; t.goldBox = false; t.blocked = false; clearMouse(t); }
  if (t.state !== 0) { out.push(cancelBytes()); t.state = 0; }
}
const unaccept = (t, out) => { out.push(tradeButtonPacket(TRADE_BTN.GREENCHECK)); t.state = TRADE_STATE.OPEN; };
function cancel(t, out, now) {
  out.push(cancelBytes());
  if (t.state !== TRADE_STATE.WAIT) { t.waitSince = now; t.state = TRADE_STATE.WAIT; }
}

/**
 * Apply a server trade packet (0x77, 0x78, 0x79). Returns the packets the client answers with.
 * `env`: { now, cursorItem: an item is on the cursor, busy: another prompt box or the chat line is up }.
 * A packet that does not fit the state is answered with a cancel that also starts the wait for the server's close.
 */
export function applyTradePacket(t, bytes, env = {}) {
  const out = [];
  if (bytes[0] === 0x78) { t.partner = cstr(bytes, 1, 16); t.partnerGuid = u32(bytes, 17); return out; }
  if (bytes[0] === 0x79) { if (bytes[1]) t.ownGold = u32(bytes, 2); else t.theirGold = u32(bytes, 2); return out; }
  if (bytes[0] !== 0x77) return out;
  const now = env.now ?? 0;
  const refuse = () => { cancel(t, out, now); return out; };
  switch (bytes[1]) {
    case TRADE_UI.ASK:
      if (t.state !== 0) return refuse();
      t.ui = true; t.blocked = false; t.ownGold = 0; t.theirGold = 0; t.waitSince = null;
      t.state = TRADE_STATE.ASKING; t.box = 'asking'; reopen(t, false);
      break;
    case TRADE_UI.ASKED:
      if (t.state !== 0) return refuse();
      // Too busy to be asked: a bare cancel, the state untouched.
      // GUESS: the client also walks its table of open screens here and on ASK, closing some and refusing (with
      // the wait) for others; which screens fall in which group is that table's per-screen class, not read yet.
      if (env.cursorItem || env.busy) { out.push(cancelBytes()); return out; }
      t.ui = true; t.ownGold = 0; t.theirGold = 0; t.waitSince = null;
      t.state = TRADE_STATE.ASKED; t.box = 'asked'; reopen(t, false);
      break;
    case TRADE_UI.RESET:
      t.ownAccept = false; t.theirAccept = false; closeTrade(t, out);
      break;
    case TRADE_UI.ACCEPT:
      if (!t.ui) break;
      t.waitSince = null; reopen(t, true);
      if (t.state === TRADE_STATE.OPEN || t.state === TRADE_STATE.THEY_ACCEPTED) { t.state = TRADE_STATE.THEY_ACCEPTED; t.theirAccept = true; sound(t, TRADE_SOUND.MARK); }
      else if (t.state === TRADE_STATE.OWN_ACCEPTED) { t.state = TRADE_STATE.BOTH; t.ownAccept = true; t.theirAccept = true; sound(t, TRADE_SOUND.BOTH_ACCEPTED); }
      else if (t.state !== TRADE_STATE.BOTH && t.state !== TRADE_STATE.WAIT) return refuse();
      break;
    case TRADE_UI.OPEN:
      if (!t.ui) break;
      out.push(tradeButtonPacket(TRADE_BTN.SENDGOLD, t.ownGold));
      t.waitSince = null; reopen(t, true);
      if (t.state === TRADE_STATE.ASKING) { t.theirGold = 0; t.box = null; t.state = TRADE_STATE.OPEN; }
      else if (t.state >= TRADE_STATE.OWN_ACCEPTED && t.state <= TRADE_STATE.WAIT) t.state = TRADE_STATE.OPEN;
      else if (t.state !== TRADE_STATE.OPEN) cancel(t, out, now);
      break;
    // The trade does not fit: nothing changes on screen, the hero who lacks the room says so (the server then
    // reopens the screen with its own packets).
    case TRADE_UI.SELLER_NO_ROOM: t.noRoom = 'own'; t.events.push({ speech: TRADE_SPEECH_NO_ROOM, who: 'own', guid: 0 }); break;
    case TRADE_UI.BUYER_NO_ROOM: t.noRoom = 'theirs'; t.events.push({ speech: TRADE_SPEECH_NO_ROOM, who: 'partner', guid: t.partnerGuid }); break;
    case TRADE_UI.CLOSE: case TRADE_UI.DONE:
      t.state = 0; closeTrade(t, out); t.done = bytes[1] === TRADE_UI.DONE;
      break;
    case TRADE_UI.CHECK: t.blocked = true; break;
    case TRADE_UI.UNCHECK: t.blocked = false; break;
    default: break;
  }
  return out;
}

/** The request prompt's two buttons (state ASKED) and the wait box's one (state ASKING). */
export function tradeAnswer(t, accept, now = 0) {
  const out = [];
  if (t.box) sound(t, TRADE_SOUND.BOX_BUTTON);
  if (t.box === 'asked' && accept) { t.box = null; t.state = TRADE_STATE.OPEN; out.push(tradeButtonPacket(TRADE_BTN.PERFORM)); }
  else if (t.box) { t.box = null; t.state = TRADE_STATE.WAIT; t.waitSince = now; out.push(cancelBytes()); }
  return out;
}

// The panel's hit rectangles in the 640x480 layout the client tests against (an 800x600 screen adds 80, 60).
// `button*` are tested strictly inside; the cels are 32 px squares whose bottom-left is (x0, y1).
export const TRADE_RECT = Object.freeze({
  accept: { x0: 223, x1: 255, y0: 384, y1: 416 },
  cancel: { x0: 275, x1: 307, y0: 384, y1: 416 },
  theirs: { x0: 275, x1: 307, y0: 170, y1: 202 },
});
// The three hover texts of the open panel: the pointer's rectangle (edges included) and where the text plate's
// left edge and anchor row sit, all in the same 640x480 layout. The partner's mark and my accept button share a text.
export const TRADE_TIPS = Object.freeze([
  { x0: 273, x1: 307, y0: 168, y1: 202, string: TRADE_STR.TIP_ACCEPT, x: 239, y: 167 },
  { x0: 221, x1: 255, y0: 383, y1: 417, string: TRADE_STR.TIP_ACCEPT, x: 187, y: 382 },
  { x0: 273, x1: 307, y0: 383, y1: 417, string: TRADE_STR.TIP_CANCEL, x: 263, y: 382 },
]);
/** The hover text under layout point (x, y): { string, x, y } (a left-aligned plate), or null. None before the panel or under the gold box. */
export function tradeTip(t, x, y) {
  if (!t.ui || !t.panel || t.state < TRADE_STATE.OPEN || t.goldBox) return null;
  const r = TRADE_TIPS.find((r) => x >= r.x0 && x <= r.x1 && y >= r.y0 && y <= r.y1);
  return r ? { string: r.string, x: r.x, y: r.y } : null;
}
const inside = (r, x, y) => x > r.x0 && x < r.x1 && y > r.y0 && y < r.y1;
export const tradeButtonAt = (x, y) => (inside(TRADE_RECT.accept, x, y) ? 'accept' : inside(TRADE_RECT.cancel, x, y) ? 'cancel' : null);

/**
 * The left button going down on the open panel, outside the item grids (those lift and place as any grid).
 * `where`: 'accept' | 'cancel' | 'gold' (the inventory's coin) | null. Returns the packets to send.
 */
export function tradeMouseDown(t, where, { cursorItem = false } = {}) {
  const out = [];
  if (!t.panel) return out;
  if (cursorItem) { clearMouse(t); unaccept(t, out); return out; }
  if (where) sound(t, TRADE_SOUND.PRESS);
  if (where === 'accept') {
    t.acceptDown = true;
    if (!t.latch) { t.latch = true; t.origin = true; } else { t.latch = false; unaccept(t, out); }
  } else if (where === 'cancel') { t.cancelDown = true; t.cancelPressed = true; t.origin = true; }
  else { if (where === 'gold') clearMouse(t); t.cancelDown = false; t.acceptDown = false; }
  return out;
}

/**
 * The left button coming up while the panel is open. `where`: 'close' (the inventory's close button, no item
 * held), 'ownGrid' (my side of the trade), 'accept', 'cancel', or null for anywhere else.
 */
export function tradeMouseUp(t, where, now = 0) {
  const out = [];
  if (!t.panel) { t.origin = false; return out; }
  const drop = () => { if (t.latch) unaccept(t, out); t.origin = false; t.cancelPressed = false; t.latch = false; };
  if (where === 'close') cancel(t, out, now);
  else if (where === 'ownGrid') drop();
  else if (where === 'accept') {
    if (t.latch && t.acceptDown) { t.state = TRADE_STATE.WAIT; t.waitSince = null; out.push(tradeButtonPacket(TRADE_BTN.ACCEPT, t.ownGold)); }
    t.origin = false; t.acceptDown = false;
  } else if (where === 'cancel') {
    if (t.cancelPressed && t.cancelDown) { cancel(t, out, now); t.cancelPressed = false; }
    t.cancelDown = false; t.origin = false;
  } else if (!t.origin) drop();
  else {
    t.origin = false;
    if (t.acceptDown) { if (t.latch) { t.latch = false; unaccept(t, out); } }
    else if (t.cancelDown && t.cancelPressed) t.cancelPressed = false;
    t.cancelDown = false; t.acceptDown = false;
  }
  return out;
}

/**
 * The inventory's coin released on the open panel: the gold box may open (not over an item on the cursor, not
 * twice), and opening it withdraws any accept. Returns { open, send }.
 */
export function tradeGoldOpen(t, { cursorItem = false } = {}) {
  const send = [];
  if (!t.panel || t.goldBox || cursorItem) return { open: false, send };
  t.goldBox = true; unaccept(t, send);
  return { open: true, send };
}
/** The gold box went away without its OK (its cancel, Escape): nothing is sent. */
export function tradeGoldClose(t) { t.goldBox = false; }

/** The gold box's OK: offer `amount` (at most what is carried; 0 is sent too); with an accept standing it withdraws the accept instead. */
export function tradeSetGold(t, amount, carried) {
  const out = [];
  t.goldBox = false;
  if (amount > 0) sound(t, TRADE_SOUND.GOLD);
  if (t.state >= TRADE_STATE.OWN_ACCEPTED && t.state <= TRADE_STATE.BOTH) { unaccept(t, out); return out; }
  t.ownGold = Math.max(0, Math.min(amount >>> 0, carried >>> 0));
  out.push(tradeButtonPacket(TRADE_BTN.SENDGOLD, t.ownGold));
  return out;
}

/**
 * The inventory's item gate in a multiplayer game. While a reply is awaited (state WAIT) the action is refused and
 * the wait withdrawn with the un-accept bytes (`leave` when the cancel's wait had already run out; the bytes still
 * go). Otherwise one action passes per TRADE_ITEM_MS; the panel opening clears that clock.
 * Returns { allowed, send, leave }.
 */
export function tradeItemAction(t, now = 0) {
  if (t.state !== TRADE_STATE.WAIT) {
    if (!(now - t.lastItemAt > TRADE_ITEM_MS)) return { allowed: false, send: [], leave: false };
    t.lastItemAt = now;
    return { allowed: true, send: [], leave: false };
  }
  const leave = t.waitSince != null && now - t.waitSince > TRADE_WAIT_MS && tradeExpire(t);
  const send = []; unaccept(t, send);
  return { allowed: false, send, leave };
}
function tradeExpire(t) { const out = []; t.state = 0; closeTrade(t, out); t.waitSince = null; return true; }

/**
 * Once a frame. A dead hero or one outside town drops the trade: one cancel for the screen, and a second from
 * the close step when a trade was live and no reply is awaited. A cancel unanswered for 30 s ends the game.
 * Returns { send, leave }.
 */
export function tradeTick(t, now, { dead = false, inTown = true } = {}) {
  const send = [];
  const expired = () => t.state === TRADE_STATE.WAIT && t.waitSince != null && now - t.waitSince > TRADE_WAIT_MS;
  if (dead || !inTown) {
    if (!t.ui) return { send, leave: false };
    send.push(cancelBytes());
    t.ui = false; t.panel = false; t.goldBox = false; clearMouse(t);
    let leave = false;
    if (t.state !== TRADE_STATE.WAIT) closeTrade(t, send); else { leave = expired() && tradeExpire(t); t.box = null; }
    t.blocked = false; t.state = 0; t.waitSince = null;
    return { send, leave };
  }
  if (expired()) return { send, leave: tradeExpire(t) };
  return { send, leave: false };
}

/** What the panel paints for its three marks: [cel frame of buysellbtn, tint] each. */
export function tradeMarks(t) {
  return {
    theirs: t.theirAccept ? { frame: 17, tint: 'green' } : { frame: 16, tint: null },
    accept: t.blocked ? { frame: 16, tint: 'red' } : (t.ownAccept || t.latch) ? { frame: 17, tint: 'green' } : { frame: 16, tint: null },
    cancel: { frame: t.cancelPressed ? 11 : 10, tint: null },
  };
}
