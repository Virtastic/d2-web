// The in-game party screen (P): who is in the game, the party button, the four relation boxes and the
// close button. Layout is retail's own draw order (the party screen draw routine of 1.13c D2Client);
// the pure parts (rows, hit tests, tooltips, click -> packet) are testable in node, the renderer is the
// canvas side. Players come from the game roster; this holds no roster state of its own beyond the
// local guesses a click makes before the server answers.
import { decodeDatPalette, decodeDc6 } from './d2-formats.mjs';
import { createFont, decodeFontTbl, textColourPalette } from './frontend.mjs';
import { createPanelChrome, BORDER_LEFT } from './panel-chrome.mjs';
import { relationPacket, partyRequestPacket } from './bnet-party.mjs';

const MENU = 'data\\global\\ui\\Menu\\';
export const PARTY_ART = { panel: MENU + 'party.dc6', buttons: MENU + 'partybuttons.dc6', boxes: MENU + 'partyboxes.dc6',
  bar: MENU + 'partybar.dc6', close: 'data\\global\\ui\\PANEL\\buysellbtn.dc6' };
// The panel's top-left on an 800x600 screen; every local coordinate below is relative to it.
export const PANEL = Object.freeze({ x: 80, y: 60 });
export const PARTY_NONE = 0xffff;
export const MOST_ROWS = 8;
export const ROW = Object.freeze({ first: 0x5a, pitch: 0x26 });
export const CLOSE = Object.freeze({ x: 0x116, bottom: 0x1a5, size: 0x20 });
export const LEAVE = Object.freeze({ x: 0xe3, bottom: 0x23, w: 0x35, h: 0xb });
export const PARTY_BUTTON = Object.freeze({ x: 0xbe, w: 0x35, h: 0x14 });
// The four boxes: x, bottom offset from the row, relation flag, frames off / on (partyboxes.dc6).
export const BOXES = Object.freeze([
  { kind: 'hostile', x: 0xe, dy: 7, flag: 8, off: 4, on: 6, tip: 0xa0 },
  { kind: 'loot', x: 0xf5, dy: 0, flag: 1, off: 0xc, on: 0xe, tip: 0xff },
  { kind: 'hear', x: 0x10a, dy: 0, flag: 2, off: 0, on: 2, tip: 0x114 },
  { kind: 'squelch', x: 0x11f, dy: 0, flag: 4, off: 8, on: 10, tip: 0x128 },
].map((b) => Object.freeze(b)));
export const BOX_SIZE = 0x14;
// The party relationship the server reports for a player (0x8b): 0 none, 1 in my party, 2 invited me,
// 3 hostile-leave (shows LEAVE), 4 I asked.
export const REL = Object.freeze({ NONE: 0, MEMBER: 1, INVITES_ME: 2, LEAVE: 3, ASKED: 4 });
export const BUTTON_WORD = Object.freeze({ [REL.NONE]: 0x1036, [REL.INVITES_ME]: 0x1037, [REL.LEAVE]: 0x1038, [REL.ASKED]: 0x13ef });
export const CLASS_WORD = Object.freeze([0x103a, 0x103b, 0x103d, 0x103e, 0x103c, 0x2771, 0x2772]);
export const LEVEL_WORDS = Object.freeze([0xd28, 0xf9d, 0xf9b]);
export const LEAVE_WORD = 0x1038;
const NAME_CUT = { alone: 0x88, party: 0x60 };

// D2Win text colours: 0 white, 1 red, 2 green, 8 orange.
export function rowColour(row, own) {
  if (row.hostile) return 1;
  if (row.party !== PARTY_NONE) return row.party === own.party ? 2 : 8;
  return 0;
}

// The roster as rows: everyone but the local player, in roster order, at most MOST_ROWS. `roster` may be a
// Map-backed Roster (.players), have list(), or be iterable; fields are read with the defaults a server
// that has not reported them yet implies.
export function rosterPlayers(roster) {
  if (!roster) return [];
  if (roster.players?.values) return [...roster.players.values()];
  if (typeof roster.list === 'function') return roster.list();
  return [...roster];
}
export function partyRows(roster, selfName, local = new Map()) {
  const me = String(selfName || '').toLowerCase();
  const rows = [];
  for (const p of rosterPlayers(roster)) {
    if (String(p.name || '').toLowerCase() === me) continue;
    const o = local.get(p.id) || {};
    const flags = (o.flags ?? p.flags ?? 0) & 0xf;
    rows.push({ id: p.id, name: p.name || '', cls: p.cls ?? p.classId ?? 0, level: p.level ?? 0, party: p.party ?? PARTY_NONE,
      relation: o.relation ?? p.partyState ?? p.relation ?? 0, flags, hostile: (flags & 8) !== 0, hostileToMe: Boolean(p.hostileToMe), area: p.area || '' });
    if (rows.length === MOST_ROWS) break;
  }
  return rows;
}
export function ownState(roster, selfName, inTown) {
  const me = rosterPlayers(roster).find((p) => String(p.name || '').toLowerCase() === String(selfName || '').toLowerCase());
  return { id: me?.id ?? 0, name: String(selfName || ''), level: me?.level ?? 0, party: me?.party ?? PARTY_NONE, inTown: Boolean(inTown) };
}
// A party exists on screen only when two listed players share its id (the self row included).
export function partyIds(rows, own) {
  const all = [own.party, ...rows.map((r) => r.party)];
  return all.map((id) => (id !== PARTY_NONE && all.filter((x) => x === id).length > 1 ? id : PARTY_NONE));
}
export const inParty = (rows, own) => partyIds(rows, own)[0] !== PARTY_NONE;
// The bars between two parties: a bar sits above row k when the party id list changes there from a real party.
export function partyBars(rows, own) {
  const ids = partyIds(rows, own), bars = [];
  for (let k = 0; k < rows.length; k++) if (ids[k + 1] !== ids[k] && ids[k] !== PARTY_NONE) bars.push(k);
  return bars;
}
// The hostile box is washed red when it cannot be used (the draw rule); a click on it does nothing then.
export function hostileBlocked(row, own) {
  return own.level < 9 || !own.inTown || (row.level < 9 && !row.hostileToMe);
}

const inside = (x, y, bx, bottom, w, h) => x >= PANEL.x + bx && x <= PANEL.x + bx + w && y >= PANEL.y + bottom - h && y <= PANEL.y + bottom;
const rowBottom = (i) => ROW.first + ROW.pitch * i;
const origin = (width, height) => { const scale = Math.min(1, width / 800, height / 600); return { scale, x: (width - 800 * scale) / 2, y: (height - 600 * scale) / 2 }; };
export const toScreen = (px, py, width, height) => { const o = origin(width, height); return { x: (px - o.x) / o.scale, y: (py - o.y) / o.scale }; };

/** What a point (800x600 screen coordinates) lands on: {close} | {leave} | {button, row} | {box, row} | null. */
export function hitTest(x, y, rows, own) {
  if (inside(x, y, CLOSE.x, CLOSE.bottom, CLOSE.size, CLOSE.size)) return { close: true };
  if (inParty(rows, own) && inside(x, y, LEAVE.x, LEAVE.bottom, LEAVE.w, LEAVE.h)) return { leave: true };
  for (let i = 0; i < rows.length; i++) {
    const b = rowBottom(i), row = rows[i];
    if (row.relation !== REL.MEMBER && inside(x, y, PARTY_BUTTON.x, b, PARTY_BUTTON.w, PARTY_BUTTON.h)) return { button: true, row };
    for (const box of BOXES) if (inside(x, y, box.x, b + box.dy, BOX_SIZE, BOX_SIZE)) return { box, row };
  }
  return null;
}

// The party button's request: none -> invite (6), asked -> cancel (7), invites me -> accept (8), leave -> leave (9).
// `relation` is what the client assumes until the server's relationship packet arrives.
export function partyButtonClick(row) {
  const r = row.relation;
  const action = r === REL.NONE ? 6 : r === REL.ASKED ? 7 : r === REL.INVITES_ME ? 8 : 9;
  const relation = r === REL.NONE ? REL.ASKED : r === REL.ASKED ? REL.NONE : REL.MEMBER;
  return { packet: partyRequestPacket(action, row.id), patch: { relation } };
}
export const leaveClick = (own) => ({ packet: partyRequestPacket(9, own.id) });
// A relation box flips our flag toward the player and tells the server the new state.
export function boxClick(row, box, own) {
  if (box.flag === 8 && hostileBlocked(row, own)) return null;
  const flags = (row.flags ^ box.flag) & 0xf;
  return { packet: relationPacket(box.kind, (flags & box.flag) !== 0, row.id), patch: { flags } };
}
export function clickResult(hit, own) {
  if (hit?.leave) return leaveClick(own);
  if (hit?.button) return partyButtonClick(hit.row);
  if (hit?.box) return boxClick(hit.row, hit.box, own);
  return null;
}

// Tooltip under the pointer: [first string id, second id or 0, centre x] or null.
export function tooltipFor(hit, rows, own) {
  if (hit?.leave) return { first: 0x1018, second: 0, x: 0xd5 };
  if (hit?.button) {
    const first = ({ [REL.NONE]: 0x1004, [REL.INVITES_ME]: 0x1006, [REL.ASKED]: 0x1008 })[hit.row.relation] ?? 0x1018;
    return { first, second: first === 0x1018 ? 0 : first + 1, x: 0xda };
  }
  if (hit?.box) {
    const { box, row } = hit, set = (row.flags & box.flag) !== 0;
    let first = 0, x = box.tip;
    if (box.flag === 8) {
      if (row.level < 9 || own.level < 9) first = 0x101a;
      else if (!own.inTown) { first = 0x1019; x = 0x6e; }
      else first = set ? 0x100c : 0x100a;
    } else if (box.flag === 1) first = set ? 0x1010 : 0x100e;
    else if (box.flag === 2) first = set ? 0x1012 : 0x1014;
    else first = set ? 0x1017 : 0x1016;
    return { first, second: [0x1016, 0x1017, 0x1019, 0x101a].includes(first) ? 0 : first + 1, x };
  }
  return null;
}

/** The text of "Lvl: n" -- the three level words and the level as "%2u". */
export const levelText = (string, level) => LEVEL_WORDS.map((id) => string(id)).join('') + String(level).padStart(2, ' ');
/** Cut a name with "..." until it is narrower than `width`, as the row text does. */
export function cutText(measure, text, width) {
  if (measure(text) < width) return text;
  let s = text;
  while (s.length && measure(s + '...') >= width) s = s.slice(0, -1);
  return s + '...';
}
// Header (own name): Font16 up to 10 characters, Font8 up to 12, Font6 beyond (the size the name needs to fit).
export const headerFont = (name) => (name.length + 1 <= 11 ? 'font16' : name.length + 1 < 14 ? 'font8' : 'font6');

const actPalette = (act) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\pal.dat`;
const FONT_FILES = { font6: 'font6', font8: 'font8', font16: 'font16' };

export function createPartyPanelRenderer(read, documentObject = globalThis.document) {
  const chrome = createPanelChrome(read, documentObject);
  const fonts = new Map();
  const fontFor = (act, name, colour) => {
    const key = `${act}:${name}:${colour}`;
    if (fonts.has(key)) return fonts.get(key);
    const base = 'data\\local\\font\\latin\\' + FONT_FILES[name];
    const pal = decodeDatPalette(read(actPalette(act)));
    let pl2 = null; try { pl2 = read(actPalette(act).replace(/pal\.dat$/i, 'pal.pl2')); } catch { /* plain palette */ }
    const f = createFont({ dc6: decodeDc6(read(base + '.dc6')), tbl: decodeFontTbl(read(base + '.tbl')),
      palette: pl2 && colour ? textColourPalette(pl2, pal, colour) : pal, documentObject, path: base + '.dc6', drawAs: { op: 'celcolor', mode: 5, extra: colour } });
    fonts.set(key, f); return f;
  };
  return {
    hit(px, py, width, height, s) { const p = toScreen(px, py, width, height); return hitTest(p.x, p.y, s.rows, s.own); },
    /** `s`: { act, rows, own, string(id), pointer: {x, y} (canvas px), closePressed, lootShown }. */
    draw(ctx, width, height, s, open) {
      if (!open) return null;
      const o = origin(width, height), A = s.act || 1, blits = [];
      const cel = (path, f, x, bottom) => { const r = chrome.cel(ctx, A, path, f, PANEL.x + x, PANEL.y + bottom); if (r) blits.push(r); return r; };
      const text = (fontName, colour, str, x, baseline, centred = false) => {
        const font = fontFor(A, fontName, colour);
        font.drawAnchored(ctx, str, PANEL.x + (centred ? x - (font.measure(str) >> 1) : x), PANEL.y + baseline);
      };
      const measure = (fontName) => (str) => fontFor(A, fontName, 0).measure(str);
      const texts = [];
      const put = (...a) => { texts.push(a[2]); text(...a); };
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      const P = PARTY_ART;
      cel(P.panel, 0, 0, 600 - 0xe0 - 2 * PANEL.y); cel(P.panel, 1, 0x100, 600 - 0xe0 - 2 * PANEL.y);
      cel(P.panel, 2, 0, 600 - 0x30 - 2 * PANEL.y); cel(P.panel, 3, 0x100, 600 - 0x30 - 2 * PANEL.y);
      const { rows, own, string } = s, member = inParty(rows, own);
      const hf = headerFont(own.name);
      put(hf, 0, own.name, 0x69, 0x1f, true);
      if (member) { cel(P.buttons, 0, LEAVE.x, LEAVE.bottom); put('font8', 2, string(LEAVE_WORD), LEAVE.x + 0x1b, 0x21, true); }
      rows.forEach((row, i) => {
        const b = rowBottom(i), colour = rowColour(row, own), inPartyRow = row.party !== PARTY_NONE;
        put('font8', colour, cutText(measure('font8'), row.name, inPartyRow ? NAME_CUT.party : NAME_CUT.alone), 0x24, b - 2);
        put('font8', colour, string(CLASS_WORD[Math.min(6, Math.max(0, row.cls))]), 0x24, b + 12);
        put('font8', colour, levelText(string, row.level), 0x10a, b + 12);
        if (row.area && inPartyRow && row.party === own.party) put('font8', colour, cutText(measure('font8'), row.area, 0x8c), 0x7e, b + 12);
        if (row.relation !== REL.MEMBER) {
          cel(P.buttons, 0, PARTY_BUTTON.x, b);
          const word = BUTTON_WORD[row.relation];
          if (word) put('font8', 0, string(word), PARTY_BUTTON.x + 0x1b, b - 2, true);
        }
        for (const box of BOXES) {
          if (box.kind === 'loot' && s.lootShown === false) continue;
          cel(P.boxes, (row.flags & box.flag) ? box.on : box.off, box.x, b + box.dy);
          if (box.flag === 8 && hostileBlocked(row, own)) {
            ctx.fillStyle = 'rgba(255,0,0,0.5)';
            ctx.fillRect(PANEL.x + box.x, PANEL.y + b + box.dy - 0x13, BOX_SIZE, BOX_SIZE);
          }
        }
      });
      partyBars(rows, own).forEach((k) => { cel(P.bar, 0, 0xc, rowBottom(k) - 23); cel(P.bar, 1, 0x10b, rowBottom(k) - 23); });
      cel(P.close, s.closePressed ? 11 : 10, CLOSE.x, CLOSE.bottom);
      BORDER_LEFT.forEach(([i, x, y]) => chrome.cel(ctx, A, 'data\\global\\ui\\PANEL\\800BorderFrame.dc6', i, x, y));
      // The tooltip under the pointer: a dark box of the wider line + 10, one line at 0x37 or two at 0x32 / 0x41.
      const p = s.pointer ? toScreen(s.pointer.x, s.pointer.y, width, height) : null;
      const tip = p ? tooltipFor(hitTest(p.x, p.y, rows, own), rows, own) : null;
      if (tip) {
        const f8 = measure('font8'), a = string(tip.first), b2 = tip.second ? string(tip.second) : '';
        const w = Math.max(f8(a), f8(b2)), two = Boolean(b2), top = two ? 0x23 : 0x2a;
        ctx.fillStyle = 'rgba(0,0,0,0.75)';
        ctx.fillRect(PANEL.x + tip.x - (w >> 1) - 5, PANEL.y + top, w + 10, two ? 0x1e : 0xf);
        put('font8', 0, a, tip.x, two ? 0x32 : 0x37, true);
        if (two) put('font8', 0, b2, tip.x, 0x41, true);
      }
      ctx.restore();
      return { rows: rows.map((r) => ({ ...r })), texts, blits, tooltip: tip };
    },
  };
}
