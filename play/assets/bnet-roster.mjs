// The in-game player roster: the server's player joined / left / party / relation packets decoded into a
// list of players, the lookups the party and chat code use, and the event (0x5a) message lines.
// Pure of the engine and the DOM, so it is testable in node. Text comes from the caller's string lookup.
import { tradeEventLine } from './bnet-game-trade.mjs';
import { chatString, fillFormat } from './bnet-game-chat.mjs';

// Relation flags one player holds toward another: 1 loot, 2 not heard, 4 squelched, 8 hostile.
export const RELATION = { LOOT: 1, MUTE: 2, SQUELCH: 4, HOSTILE: 8 };
export const PARTY_NONE = 0xffff;

const cstr = (b, o, n) => { let s = ''; for (let i = o; i < o + n && i < b.length && b[i]; i++) s += String.fromCharCode(b[i]); return s; };
const u16 = (b, o) => b[o] | (b[o + 1] << 8);
const u32 = (b, o) => (b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)) >>> 0;
const NO_ID = 0xffffffff;

// Roster of the players in the game, newest join first. Each player:
// { id, name, display, title, cls, level, party, partyState, flags, rel }
//   display = the name shown in lists ("name {title}" when the server sent a title string),
//   rel     = what this player has set toward each other player id (relation flags),
//   flags   = what the local player has set toward this player (kept for the party panel).
export class Roster {
  constructor() { this.players = new Map(); this.myId = null; }
  get(id) { return this.players.get(id) || null; }
  set(id, patch) {
    const p = this.players.get(id) || { id, name: '', display: '', title: '', cls: 0, level: 0, party: PARTY_NONE, partyState: 0, flags: 0, rel: new Map() };
    Object.assign(p, patch);
    if (patch && patch.name !== undefined && patch.display === undefined) p.display = p.title ? `${p.name} {${p.title}}` : p.name;
    this.players.set(id, p);
    return p;
  }
  remove(id) { this.players.delete(id); }
  // Players newest first, as the game's own list is kept.
  list() { return [...this.players.values()].reverse(); }
  get size() { return this.players.size; }
  // Apply one server roster packet (0x5b, 0x5c, 0x8b, 0x8c, 0x8d); true when it changed the roster.
  feed(bytes) { return applyRosterPacket(this, bytes) !== null; }
  clear() { this.players.clear(); }
  // A whisper target or any name lookup: the game compares names without regard to case.
  find(name) { return this.byName(name); }
  byName(name) {
    const n = String(name).toLowerCase();
    for (const p of this.players.values()) if (p.name.toLowerCase() === n) return p;
    return null;
  }
  displayName(id) { const p = this.players.get(id); return p ? p.display : ''; }
  // Relation flags `from` holds toward `to`.
  relation(from, to) { const p = this.players.get(from); return p ? (p.rel.get(to) || 0) : 0; }
  setRelation(from, to, flags) {
    const p = this.players.get(from);
    if (!p || from === to) return false;
    p.rel.set(to, flags & 0xffff);
    if (from === this.myId) { const q = this.players.get(to); if (q) q.flags = flags & 0xffff; }
    return true;
  }
  toggle(id, flag) {
    const p = this.players.get(id);
    if (!p) return null;
    p.flags ^= flag;
    return !!(p.flags & flag);
  }
  has(id, flag) { const p = this.players.get(id); return !!p && (p.flags & flag) !== 0; }
  sameParty(a, b) {
    const p = this.players.get(a), q = this.players.get(b);
    return !!p && !!q && p.party === q.party && p.party !== PARTY_NONE;
  }
  // Name colour: red hostile (either way), green own party, orange another party, white otherwise.
  colour(id, myId = this.myId) {
    const p = this.players.get(id);
    if (!p) return 'ÿc0';
    if ((p.flags & RELATION.HOSTILE) || (myId != null && (this.relation(id, myId) & RELATION.HOSTILE))) return 'ÿc1';
    if (p.party !== PARTY_NONE) { const me = this.players.get(myId); return me && me.party === p.party ? 'ÿc2' : 'ÿc8'; }
    return 'ÿc0';
  }
}

// Apply one server packet to the roster. Returns { kind, id } for a packet that changed it, else null.
//   0x5b [size u16][id u32 @3][class @7][name 16 @8][level u16 @0x18][party u16 @0x1a][state u16 @0x1c]
//        [u16 @0x1e][u16 @0x20][title string @0x22]
//   0x5c [id u32 @1]            player left
//   0x8b [id u32 @1][state u8 @5]
//   0x8c [from u32 @1][to u32 @5][flags u16 @9]
//   0x8d [id u32 @1][party u16 @5]
export function applyRosterPacket(roster, bytes) {
  if (!bytes || !bytes.length) return null;
  switch (bytes[0]) {
    case 0x5b: {
      if (bytes.length < 0x23) return null;
      const id = u32(bytes, 3);
      if (id === NO_ID) return null;
      const old = roster.get(id);
      roster.players.delete(id); // a rejoin moves to the head of the list
      roster.set(id, {
        name: cstr(bytes, 8, 16), title: cstr(bytes, 0x22, bytes.length - 0x22), cls: bytes[7], level: u16(bytes, 0x18),
        party: u16(bytes, 0x1a), partyState: u16(bytes, 0x1c), extra: u16(bytes, 0x1e), flags: old ? old.flags : 0, rel: old ? old.rel : new Map(),
      });
      return { kind: 'join', id };
    }
    case 0x5c: {
      if (bytes.length < 5) return null;
      const id = u32(bytes, 1);
      if (id === NO_ID) return null;
      for (const p of roster.players.values()) p.rel.delete(id);
      roster.remove(id);
      return { kind: 'leave', id };
    }
    case 0x8b: {
      if (bytes.length < 6 || !roster.get(u32(bytes, 1))) return null;
      roster.get(u32(bytes, 1)).partyState = bytes[5];
      return { kind: 'state', id: u32(bytes, 1) };
    }
    case 0x8c: {
      if (bytes.length < 11 || !roster.setRelation(u32(bytes, 1), u32(bytes, 5), u16(bytes, 9))) return null;
      return { kind: 'relation', id: u32(bytes, 1), other: u32(bytes, 5), toMe: u32(bytes, 5) === roster.myId };
    }
    case 0x8d: {
      if (bytes.length < 7 || !roster.get(u32(bytes, 1))) return null;
      roster.get(u32(bytes, 1)).party = u16(bytes, 5);
      return { kind: 'party', id: u32(bytes, 1) };
    }
    default: return null;
  }
}

// Text of the 0x5a event lines; the game's own string table wins when the caller's lookup has the id.
export const EVENT_STR = {
  3639: ' dropped due to timeout.', 3640: ' dropped due to errors.',
  3641: '%s joined our world. Diablo\'s minions grow stronger.', 3642: '%s left our world. Diablo\'s minions weaken.',
  3643: '%s(%s) joined our world. Diablo\'s minions grow stronger.', 3644: '%s(%s) left our world. Diablo\'s minions weaken.',
  3645: ' is not in the game.', 3646: ' is not logged in.', 3647: ' was slain by ', 3648: ' was slain by ', 3649: ' was slain.',
  3651: 'Realm is going down in %d minutes.', 3652: ' is not listening to you.',
  4044: ' is busy', 4047: 'You must wait a short time to trade with that player.',
  4134: ' has items in his box.', 4135: 'You have items in your box.',
  1691: '                                 ', 10808: 'You must wait a short time to declare hostility with that player.',
  11004: '%d Stones of Jordan Sold to Merchants', 11005: 'Diablo Walks the Earth', 3224: 'an Exploding Barrel',
  4026: ' has expressed hostility towards you.', 4027: ' is no longer hostile towards you.',
  4028: ' invites you to ally against the forces of evil.', 4029: ' has cancelled the party invite.',
  4030: ' has joined your party to fight the forces of evil.', 4031: 'You are now allied with ', 4032: ' has left your party.',
  10106: '%s permits you to loot his corpse.', 10107: '%s permits you to loot her corpse.',
  10108: '%s no longer allows you to access his corpse.', 10109: '%s no longer allows you to access her corpse.',
};
// Sub-kinds of type 7 (a player's party, hostility or corpse-loot notice): [string id, 'n' name then text,
// 't' text then name, 'f' the text formatted with the name].
const SUB7 = { 1: [10106, 'f'], 2: [10107, 'f'], 3: [4026, 'n'], 4: [4027, 'n'], 5: [4028, 'n'], 6: [4029, 'n'], 7: [4030, 'n'], 8: [4031, 't'], 9: [4032, 'n'], 10: [10108, 'f'], 11: [10109, 'f'] };
// Message colour index (byte 2) as a text colour code; 0 is the default colour. The game's colour numbers
// are the text-colour order (0 white, 1 red, 2 green, ... 10 dark green, 11 purple).
export function eventColourCode(c) { return c > 0 && c < 12 ? `ÿc${'0123456789:;'[c]}` : ''; }
const LINE_MAX = 0x132;

// The server's 0x5a event as { text, colour } for the message log, or null when it shows nothing.
// Layout: [1] type, [2] colour, [3..6] a number or unit id, [7] sub kind, [8..23] the player (15 characters
// count), [24..39] a second name. Types 2/3 are joined/left (the second name is the account), 4 and 5 not in
// the game / not logged in, 6 slain, 7 party notices, 8-10 and 16 the trade and hostility refusals (their
// lines live with the trade code), 11 and 13 and 15-18 notices; 12 and 14 print nothing.
// `lang(id)` is the string lookup, `roster` the current roster, `myName` the local character name (your own
// arrival is not announced to you; compared exactly), `monsterName` names a monster for a type 6 line.
export function rosterEventLine(bytes, lang, roster, myName = '', monsterName = null) {
  if (!bytes || bytes[0] !== 0x5a || bytes.length < 40) return null;
  const type = bytes[1], colour = bytes[2], num = u32(bytes, 3), name = cstr(bytes, 8, 15), other = cstr(bytes, 0x18, 16);
  const S = (id) => chatString(id, lang, EVENT_STR);
  let text = null;
  switch (type) {
    case 0: text = name + S(3639); break;
    case 1: text = name + S(3640); break;
    case 2: case 3:
      if (type === 2 && myName && name === myName) return null;
      text = other ? fillFormat(S(type === 2 ? 3643 : 3644), name, other) : fillFormat(S(type === 2 ? 3641 : 3642), name);
      break;
    case 4: text = name + S(3645); break;
    case 5: text = name + S(3646); break;
    case 6: {
      const sub = bytes[7];
      let victim = '', sep = 3647;
      if (sub === 0) { victim = other; sep = 3648; }
      else if (sub === 1) { victim = monsterName?.(bytes[0x18] === 0 ? { unitId: num } : { classId: bytes[0x18] | (bytes[0x19] << 8) }) ?? ''; sep = bytes[0x18] === 0 ? 3647 : 3648; }
      else if (sub === 2) victim = num === 0xb ? S(3224) : monsterName?.({ objectId: num }) ?? '';
      if (!victim) { text = name + S(3649); break; }
      if (S(sep).length + victim.length + 16 >= LINE_MAX) return null;   // too long: the game drops the line
      text = name + S(sep) + victim;
      break;
    }
    case 7: {
      const sub = SUB7[bytes[7]];
      if (!sub) return null;
      if (roster && roster.myId != null && (roster.relation(roster.myId, num) & RELATION.SQUELCH)) return null;
      const who = roster?.get(num)?.name ?? '', s = S(sub[0]);
      text = sub[1] === 'f' ? fillFormat(s, who) : sub[1] === 't' ? s + who : who + s;
      break;
    }
    case 11: text = fillFormat(S(1691), name); break;
    case 13: text = name + S(3652); break;
    case 15: text = fillFormat(S(3651), num); break;
    case 17: text = fillFormat(S(11004), num); break;
    case 18: text = S(11005); break;
    default: text = tradeEventLine(bytes, S); break;
  }
  return text == null ? null : { text, colour };
}

// A line for the message log, with its colour code in front.
export function eventLineText(ev) { return ev ? eventColourCode(ev.colour) + ev.text : null; }
