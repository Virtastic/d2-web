// The channel screen against the retail capture (vectors/drawlist/bnet-channel.txt, tick 11 of the headless
// front-end capture): one channel, four users, three chat lines, the caret showing. Skipped without the
// retail archives or the capture. The statstring class/level layout below is the capture's guess, not retail's.
// Further lobby states follow below, one test each.
import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync, writeFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { openRetail } from '../../tools/dcc-decode-bench.mjs';
import { decodeDc6, decodeDatPalette } from './d2-formats.mjs';
import { diffDrawLists, parseDrawListVector } from './drawlist.mjs';
import { createBnetLobbyScreen, portraitFrameAt } from './bnet-lobby-screen.mjs';
import { encodeGameList, parseGameList, filterGames } from './bnet-gamelist.mjs';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const vector = join(root, 'vectors', 'drawlist', 'bnet-channel.txt');
const rd = existsSync(vector) ? openRetail() : null;

// D2Lang indices as the screen asks: below 10000 string.tbl, 10000s patchstring.tbl, 20000s expansionstring.tbl (the entry index
// is the position in the table's index array).
function stringTable(read) {
  const tbl = (name, i) => {
    const d = Buffer.from(read(`data\\local\\lng\\eng\\${name}.tbl`)), n = d.readUInt16LE(2);
    if (i >= n) return undefined;
    const e = 21 + n * 2 + d.readUInt16LE(21 + i * 2) * 17, vo = d.readUInt32LE(e + 11);
    return d.toString('latin1', vo, d.indexOf(0, vo));
  };
  return { get: (i) => (i < 10000 ? tbl('string', i) : i < 20000 ? tbl('patchstring', i - 10000) : tbl('expansionstring', i - 20000)) };
}

globalThis.ImageData ??= class { constructor(data, w, h) { Object.assign(this, { data, width: w, height: h }); } };
const ctx = new Proxy({}, { get: (_, k) => (k === 'getImageData' ? () => ({ data: new Uint8ClampedArray(4) }) : () => {}), set: () => true });
const documentObject = { createElement: () => ({ width: 0, height: 0, getContext: () => ctx }) };

// The capture's statstring: "PX2D" "USEast," name "," 30 bytes, byte 2 = class + 1, byte 13 = level (as the capture builds it).
const stat = (name, cls, level) => {
  const b = new Uint8Array(30).fill(0xff); b[0] = 1; b[1] = 1; b[2] = cls + 1; b[13] = level;
  return `PX2DUSEast,${name},${String.fromCharCode(...b)}`;
};

test('channel screen draws the retail capture record for record', { skip: !rd }, () => {
  const strings = stringTable(rd);
  const screen = createBnetLobbyScreen({ read: rd, decodeDc6, decodeDatPalette, string: (i) => strings.get(i) ?? '', character: { name: 'Hero' },
    documentObject, now: () => 0, portraitFrame: (slot) => [4, 5, 0, 2][slot] });
  screen.pinClock(2750, 2750 - 8 * 40);
  screen.activate('enter-chat'); screen.hover(0, 1, 800, 600);
  const ev = (event, username, text) => ({ event, username, text, raw: text });
  screen.chatEvent(ev(7, '', 'Diablo II Test Channel'));
  [['Aelwyn', 0, 87], ['Brightstar', 1, 45], ['Cinder', 2, 12], ['Dorian', 3, 99]].forEach(([n, c, l]) => screen.chatEvent(ev(1, n, stat(n, c, l))));
  screen.chatEvent(ev(5, 'Aelwyn', 'Hello everyone'));
  screen.chatEvent(ev(5, 'Brightstar', 'Anyone for a Baal run?'));
  screen.chatEvent(ev(5, 'Cinder', 'sure, give me a minute'));
  const ours = [];
  globalThis.__d2DrawList = ours;
  try { screen.draw(ctx, 800, 600); } finally { delete globalThis.__d2DrawList; }
  // The capture's recorder has no frame bytes: hash 0 on its side, so zero ours.
  const retail = parseDrawListVector(readFileSync(vector, 'utf8')).filter((r) => r.op === 'cel' || r.op === 'celcolor');
  const d = diffDrawLists(retail, ours.map((r) => ({ ...r, hash: 0 })), { colour: (r) => /fontformal10/i.test(r.path) && r.x >= 432 && r.x < 592 && r.y >= 230 && r.y <= 392 });
  console.log(`bnet-channel: ${retail.length} retail draws, ${ours.length} ours, differing records ${d.diffs} (missing ${d.missing.length}, extra ${d.extra.length}, differing ${d.differing.length})`);
  for (const m of d.missing.slice(0, 12)) console.log('  missing', m.op, m.path, m.frame, m.x, m.y, m.mode);
  for (const m of d.extra.slice(0, 12)) console.log('  extra', m.op, m.path, m.frame, m.x, m.y, m.mode);
  for (const m of d.differing.slice(0, 12)) console.log('  differs', m.draw, m.diff);
  assert.equal(d.diffs, 0);
});

// ---- more lobby states (vectors/drawlist/bnet-<state>.txt) ----
// Retail's statstring layout (D2Multi 0xF340): after the second comma 2 header bytes, 11 gear bytes (0xFF = naked), class + 1, 11 colours,
// level, flags (0x04 hardcore, 0x08 dead, 0x20 expansion, 0x40 ladder), a word and 3 bytes.
const stat2 = (name, cls, level, flags = 0x04) => {
  const b = new Uint8Array(33).fill(1); b.fill(0xff, 2, 13); b[13] = cls + 1; b.fill(0xff, 14, 25); b[25] = level; b[26] = flags;
  return `PX2DUSEast,${name},${String.fromCharCode(...b)}`;
};
const ev = (event, username, text) => ({ event, username, text, raw: text });
const users = (n) => Array.from({ length: n }, (_, i) => { const nm = `Hero${String(i).padStart(2, '0')}`; return ev(1, nm, stat2(nm, i % 7, 1 + (i * 13) % 98, i % 3 === 1 ? 0x0c : 0x04)); });
// One user per combination of the flag bits 0x04 / 0x08 / 0x20 / 0x40, named by the flag byte, all Paladins of level 50.
const flagUsers = () => Array.from({ length: 16 }, (_, k) => { let fl = 0; [0x04, 0x08, 0x20, 0x40].forEach((b, i) => { if (k & (1 << i)) fl |= b; });
  const nm = `Flag${fl.toString(16).toUpperCase().padStart(2, '0')}`; return ev(1, nm, stat2(nm, 3, 50, fl)); });
const chatLines = Array.from({ length: 28 }, (_, i) => ev(5, `Hero${String(i % 5).padStart(2, '0')}`, `chat line ${i}: the quick brown fox jumps over`));
const RANK_FLAGS = [0, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000, 0x10000, 0x20000, 0x40000, 0x100000, 0x1000000];
const rankUsers = (n) => Array.from({ length: n }, (_, i) => { const nm = `Rank${String(i).padStart(2, '0')}`; return { ...ev(1, nm, stat2(nm, 3, 50, 0x04)), flags: RANK_FLAGS[i] }; });
const aelwyn = () => ev(1, 'Aelwyn', stat2('Aelwyn', 1, 40));

// state: [vector, tick, feed(screen)]. The clock is tick * 250 ms; the fire logo frame is read from the vector, so `created` is placed to land on it.
// `known` is the differing-record count when this file was written: the test fails only on a regression, so the drawing code can improve.
const JOIN_GAMES = [['Baal Runs 24-7', '', 2, 'fast baal runs only', 0], ['cows', '', 0, 'cow level', 0], ['NM Mephisto', 'secret', 1, 'mephisto nightmare, password', 4],
  ['Hell Ubers', '', 2, 'ubers wanted, bring keys', 0], ['HC Normal', '', 3, 'hardcore, hidden from softcore', 0], ['Tristram', '', 0, '', 0], ['Full House', '', 1, '8 of 8 players', 1],
  ['a', 'x', 0, 'short name, a password', 0], ['Pindle', '', 1, 'pindle runs', 0], ['Chaos', '', 2, 'chaos sanctuary', 0], ['Act 5 Rush', '', 0, 'rush me', 0],
  ['Magic Find', 'mf', 1, 'mf runs', 0], ['Diablo', '', 2, 'diablo runs', 0], ['Last One', '', 0, 'the last game', 0]];
// The capture's refresh variants: 0 the 14 games, 1 plus a new first game, 2 without "cows" and "Tristram", 3 the first five.
const variant = (v) => (v === 1 ? [['Aaa Fresh', '', 0, 'new at the top', 0], ...JOIN_GAMES] : v === 2 ? JOIN_GAMES.filter((g) => g[0] !== 'cows' && g[0] !== 'Tristram') : v === 3 ? JOIN_GAMES.slice(0, 5) : JOIN_GAMES);
const listOf = (v) => filterGames(parseGameList(encodeGameList(variant(v).map(([name, password, difficulty, description, status]) => ({ name, password, difficulty, description, status })))), { progression: 5 });
const mouse = (s, kind, x, y) => { if (kind === 'm') { s.hover(x, y, 800, 600); return null; } s.hover(x, y, 800, 600); return s.press(x, y, 800, 600); };
const click = (s, x, y) => { s.hover(x, y, 800, 600); const t = s.press(x, y, 800, 600); s.release(t, x, y, 800, 600); return t; };
function gameList(s, { scroll = 0, click = null, v = 0 } = {}) {
  s.chatEvent(ev(7, '', 'Diablo II Test Channel')); s.chatEvent(aelwyn()); s.activate('join');
  s.setGames(listOf(v));
  if (scroll) s.scrollGames(scroll);
  if (click) { s.hover(...click, 800, 600); s.release(s.press(...click, 800, 600), ...click, 800, 600); }
}
// The input capture's channel: twelve users (Hero00..11, class by index, all flag 0x04, the fourth squelched), three chat lines.
const uiFeed = (s) => {
  s.chatEvent(ev(7, '', 'Diablo II Test Channel'));
  Array.from({ length: 12 }, (_, i) => { const nm = `Hero${String(i).padStart(2, '0')}`; return { ...ev(1, nm, stat2(nm, i % 7, 1 + (i * 13) % 98, 0x04)), flags: i === 3 ? 0x20 : 0 }; }).forEach((e) => s.chatEvent(e));
  s.chatEvent(ev(5, 'Hero00', 'Hello everyone')); s.chatEvent(ev(5, 'Hero01', 'Anyone for a Baal run?')); s.chatEvent(ev(5, 'Hero02', 'sure, give me a minute'));
};
// The tooltip capture's channel: eleven users named by their flag word, Paladins of level 50, the last one a classic (VD2D) character.
const tipFeed = (s) => {
  s.chatEvent(ev(7, '', 'Diablo II Test Channel'));
  [0x00, 0x04, 0x0c, 0x20, 0x24, 0x40, 0x44, 0x60, 0x64, 0x28, 0x24].forEach((fl, i) => {
    const nm = `Fl${fl.toString(16).toUpperCase().padStart(2, '0')}${i === 10 ? 'c' : ''}`, st = stat2(nm, 3, 50, fl);
    s.chatEvent(ev(1, nm, i === 10 ? `VD2D${st.slice(4)}` : st));
  });
};
// The clamp capture's channel: eleven Paladins (Tip00..10) of level 50; `realms` = the realm-name lengths of the users in cells 10, 9, 8 (the rest USEast).
const clampFeed = (s, realms) => {
  s.chatEvent(ev(7, '', 'Diablo II Test Channel'));
  for (let i = 0; i < 11; i++) {
    const nm = `Tip${String(i).padStart(2, '0')}`, n = i >= 8 ? realms[10 - i] : 0;
    s.chatEvent(ev(1, nm, stat2(nm, 3, 50, 0x04).replace('USEast', n ? Array.from({ length: n }, (_, k) => String.fromCharCode(65 + k % 26)).join('') : 'USEast')));
  }
};
const typed = (s, text) => { for (const c of text) s.typeKey(`Key${c.toUpperCase()}`, c); };
// The send callback of a live chat: slash commands go out and leave no local line; plain SEND text is echoed in the log (<> + the line, the capture's character has no name).
const sentByCommand = (l) => /^\/(whisper|me) /.test(l);
// The slash captures' channel: twelve users, then per tick a command (Enter callback) and the chat lines that arrive after it.
const slashFeed = (s, cmds, replyTo) => {
  s.chatLive(true);
  s.chatEvent(ev(7, '', 'Diablo II Test Channel'));
  Array.from({ length: 12 }, (_, i) => { const nm = `Hero${String(i).padStart(2, '0')}`; return { ...ev(1, nm, stat2(nm, i % 7, 1 + (i * 13) % 98, 0x04)), flags: i === 3 ? 0x20 : 0 }; }).forEach((e) => s.chatEvent(e));
  if (replyTo) s.chatState.replyTo = replyTo;
  const chat = [['Hero00', 'Hello everyone'], ['Hero01', 'Anyone for a Baal run?'], ['Hero02', 'sure, give me a minute']];
  // Commands at ticks 12, 14, 16; chat line k arrives at tick 13 + k, after that tick's command.
  const lastTick = 12 + 2 * (cmds.length - 1);
  for (let t = 12; t <= lastTick; t++) {
    if (t % 2 === 0) for (const c of cmds[(t - 12) / 2]) { typed(s, c); s.typeKey('Enter'); }
    const k = t - 13; if (k >= 0 && k < 3) s.chatEvent(ev(5, chat[k][0], chat[k][1]));
  }
};
const STATES = {
  // Before ENTER CHAT: the character's details and the four lobby buttons.
  'prechat-t4': { tick: 4, file: 'prechat-t4', prechat: true, character: { name: 'Hero', level: 30, charClass: 3 }, known: 0, feed: () => {} },
  'prechat-t9': { tick: 9, file: 'prechat-t9', prechat: true, character: { name: 'Hero', level: 30, charClass: 3 }, known: 0, feed: () => {} },
  // The mouse held on ENTER CHAT, QUIT and HELP before chat: the pressed cel and the label 2 px left and down.
  'prechat-press-enter': { tick: 4, prechat: true, character: { name: 'Hero', level: 30, charClass: 3 }, known: 0, feed: (s) => { mouse(s, 'c', 80, 470); } },
  'prechat-press-quit': { tick: 6, prechat: true, character: { name: 'Hero', level: 30, charClass: 3 }, known: 0, feed: (s) => { mouse(s, 'c', 701, 485); } },
  'prechat-press-help': { tick: 8, prechat: true, character: { name: 'Hero', level: 30, charClass: 3 }, known: 0, feed: (s) => { mouse(s, 'c', 200, 470); } },
  // Slash commands handled locally (captured with the Enter callback driven directly at ticks 12, 14, 16; the three chat lines
  // arrive at ticks 13, 14 (after the command) and 15).
  'slash-r-nobody': { file: 'slash-r-nobody', tick: 12, send: () => true, known: 0, feed: (s) => { slashFeed(s, [['/r']]); } },
  'slash-notify-on': { file: 'slash-notify-on', tick: 14, send: () => true, known: 0, feed: (s) => { slashFeed(s, [['/r'], ['/d2notify']]); } },
  'slash-notify-off': { file: 'slash-notify-off', tick: 16, send: () => true, known: 0, feed: (s) => { slashFeed(s, [['/r'], ['/d2notify'], ['/d2notify']]); } },
  'slash-r-usage': { file: 'slash-r-usage', tick: 12, send: () => true, known: 0, feed: (s) => { slashFeed(s, [['/r']], 'Gandalf'); } },
  'slash-reply-usage': { file: 'slash-reply-usage', tick: 16, send: () => true, known: 0, feed: (s) => { slashFeed(s, [['/r'], ['/r hello there'], ['/reply']], 'Gandalf'); } },
  prechat: { tick: 12, prechat: true, character: { name: 'Hero', level: 30, charClass: 3 }, known: 0, feed: () => {} },
  'users-top': { tick: 31, known: 0, feed: (s) => { s.chatEvent(ev(7, '', 'Diablo II Test Channel')); users(26).forEach((e) => s.chatEvent(e)); s.chatEvent(ev(5, 'Hero00', 'Hello everyone')); } },
  'users-scrolled': { tick: 35, known: 0, feed: (s) => { STATES['users-top'].feed(s); s.activate('arrow-1'); } },
  'users-scroll1': { tick: 43, known: 0, feed: (s) => { STATES['users-top'].feed(s); s.activate('arrow-3'); } },
  classes: { tick: 15, known: 0, feed: (s) => { s.chatEvent(ev(7, '', 'Diablo II Test Channel')); users(7).forEach((e) => s.chatEvent(e)); s.chatEvent(ev(5, 'Hero00', 'Hello everyone')); } },
  'chat-long': { tick: 33, known: 0, feed: (s) => { s.chatEvent(ev(7, '', 'Diablo II Test Channel')); s.chatEvent(ev(1, 'Hero00', stat2('Hero00', 0, 30))); chatLines.forEach((e) => s.chatEvent(e)); } },
  'chat-kinds': { tick: 17, known: 0, feed: (s) => {
    s.chatEvent(ev(7, '', 'Diablo II Test Channel')); s.chatEvent(ev(1, 'Aelwyn', stat2('Aelwyn', 1, 40))); s.chatEvent(ev(1, 'Brightstar', stat2('Brightstar', 2, 12)));
    s.chatEvent(ev(5, 'Aelwyn', 'plain talk line')); s.chatEvent(ev(23, 'Aelwyn', 'waves hello')); s.chatEvent(ev(4, 'Brightstar', 'psst, a whisper')); s.chatEvent(ev(10, 'Brightstar', 'my reply'));
    s.chatEvent(ev(6, 'Aelwyn', 'A broadcast')); s.chatEvent(ev(18, '', 'An info line')); s.chatEvent(ev(19, '', 'An error line')); } },
  'chat-typed': { tick: 13, known: 0, feed: (s) => { s.chatEvent(ev(7, '', 'Diablo II Test Channel')); s.chatEvent(aelwyn()); s.chatEvent(ev(5, 'Aelwyn', 'Hello everyone')); for (const c of 'typed chat text') s.typeKey(`Key${c.toUpperCase()}`, c); } },
  'create-game': { tick: 11, known: 0, feed: (s) => { s.chatEvent(ev(7, '', 'Diablo II Test Channel')); s.chatEvent(aelwyn()); s.activate('create'); } },
  'join-game': { tick: 11, known: 0, feed: (s) => { s.chatEvent(ev(7, '', 'Diablo II Test Channel')); s.chatEvent(aelwyn()); s.activate('join'); } },
  // Seven games listed after retail's own filter (the hardcore "HC Normal" is hidden from a softcore character; the feed is a raw game-list reply, D2Multi 0x13F20).
  'join-games': { tick: 11, known: 0, feed: (s) => { s.chatEvent(ev(7, '', 'Diablo II Test Channel')); s.chatEvent(aelwyn()); s.activate('join');
    s.setGames(['Baal Runs 24-7', 'cows', 'NM Mephisto', 'Hell Ubers', 'Tristram', 'Full House', 'a'].map((name) => ({ name }))); } },
  // The same capture fed as the realm sends it: the raw 0x09 reply body of eight games, parsed and filtered by the client (progress 5, softcore).
  'join-games-wire': { file: 'join-games', tick: 11, known: 0, feed: (s) => { s.chatEvent(ev(7, '', 'Diablo II Test Channel')); s.chatEvent(aelwyn()); s.activate('join');
    const reply = encodeGameList([['Baal Runs 24-7', '', 2], ['cows', '', 0], ['NM Mephisto', 'secret', 1], ['Hell Ubers', '', 2], ['HC Normal', '', 3], ['Tristram', '', 0], ['Full House', '', 1], ['a', 'x', 0]]
      .map(([name, password, difficulty]) => ({ name, password, difficulty, description: '' })));
    s.setGames(filterGames(parseGameList(reply), { progression: 5 })); } },
  // Fourteen games as the realm sends them (HC Normal hidden): the list has 13 rows, nine on show, then a click (mouse down on a row) and/or a scroll.
  'join-games-13': { tick: 7, known: 0, feed: (s) => gameList(s) },
  'join-sel-first': { tick: 11, known: 0, feed: (s) => gameList(s, { click: [440, 229] }) },
  'join-sel-password': { tick: 11, known: 0, feed: (s) => gameList(s, { click: [440, 267] }) },
  'join-sel-full': { tick: 11, known: 0, feed: (s) => gameList(s, { click: [440, 324] }) },
  'join-scrolled': { tick: 11, known: 0, feed: (s) => gameList(s, { scroll: 4 }) },
  'join-scrolled-sel': { tick: 11, known: 0, feed: (s) => gameList(s, { scroll: 4, click: [440, 248] }) },
  // Join panel input the retail capture reached (vectors/drawlist/bnet-gap-*): a click past the last row, a refresh, the scrollbar, a double-click.
  'gap-below': { tick: 11, file: 'gap-below', known: 0, feed: (s) => { gameList(s, { v: 3 }); click(s, 440, 248); click(s, 440, 362); } },
  'gap-refresh-s4c': { tick: 12, file: 'gap-refresh-s4c', known: 0, feed: (s) => { gameList(s, { scroll: 4 }); click(s, 440, 229); s.setGames(listOf(1)); } },
  'gap-refresh-keep': { tick: 11, file: 'gap-refresh-keep', known: 0, feed: (s) => { gameList(s); click(s, 440, 267); s.setGames(listOf(1)); } },
  'gap-refresh-gone': { tick: 11, file: 'gap-refresh-gone', known: 0, feed: (s) => { gameList(s); click(s, 440, 248); s.setGames(listOf(2)); } },
  'gap-refresh-scrolled': { tick: 12, file: 'gap-refresh-s3c', known: 0, feed: (s) => { gameList(s, { scroll: 3 }); click(s, 440, 229); s.setGames(listOf(1)); } },
  'gap-track': { tick: 10, file: 'gap-track', known: 0, feed: (s) => { gameList(s); click(s, 597, 300); } },
  'gap-arrow-up': { tick: 11, file: 'gap-arrow-up', known: 0, feed: (s) => { gameList(s, { scroll: 2 }); mouse(s, 'c', 597, 225); } },
  'gap-arrow-down': { tick: 10, file: 'gap-arrow-down', known: 0, feed: (s) => { gameList(s); mouse(s, 'c', 597, 388); } },
  'gap-drag': { tick: 13, file: 'gap-drag', known: 0, feed: (s) => { gameList(s); mouse(s, 'c', 597, 235); mouse(s, 'm', 597, 338); s.release('scrollbar', 597, 338, 800, 600); mouse(s, 'm', 597, 226); } },
  'gap-dbl': { tick: 12, file: 'gap-dbl', known: 0, feed: (s, t) => { gameList(s); s.pinClock(t.clock - 250, t.built); click(s, 440, 229); s.pinClock(t.clock, t.built); click(s, 440, 229); } },
  'portrait-t20': { tick: 20, known: 0, feed: (s) => { s.chatEvent(ev(7, '', 'Diablo II Test Channel')); users(11).forEach((e) => s.chatEvent(e)); } },
  'portrait-t31': { tick: 31, known: 0, feed: (s) => STATES['portrait-t20'].feed(s) },
  'portrait-t40': { tick: 40, known: 0, feed: (s) => STATES['portrait-t20'].feed(s) },
  'flags-top': { tick: 21, known: 0, feed: (s) => { s.chatEvent(ev(7, '', 'Diablo II Test Channel')); flagUsers().forEach((e) => s.chatEvent(e)); } },
  'flags-scrolled': { tick: 24, known: 0, feed: (s) => { STATES['flags-top'].feed(s); s.activate('arrow-1'); } },
  // The chat log scrolled: retail's list holds its first visible row in the scrollbar child (set by D2Win#10124 on the list control); 14 is the
  // newest (29 rows, 15 visible), 7 is mid, 0 the top, and a position past the end clamps to 14.
  'chat-scroll-bottom': { tick: 38, known: 0, feed: (s) => STATES['chat-long'].feed(s) },
  'chat-scroll-top': { tick: 40, known: 0, feed: (s) => { STATES['chat-long'].feed(s); s.scrollChat?.(0); } },
  'chat-scroll-mid': { tick: 44, known: 0, feed: (s) => { STATES['chat-long'].feed(s); s.scrollChat?.(7); } },
  'chat-scroll-past': { tick: 48, known: 0, feed: (s) => { STATES['chat-long'].feed(s); s.scrollChat?.(20); } },
  // Chat-event flags (SHOWUSER / TALK data[1]): 22 Paladins, each with its own flags word.
  // The band's user input: hovering a user (the tooltip), a click picks them (the ring, SQUELCH or UNSQUELCH enabled, WHISPER once there is text), a second click drops the pick and leaves the buttons as they were.
  'ui-hover': { tick: 20, known: 0, feed: (s) => { uiFeed(s); s.hover(70, 540, 800, 600); } },
  'ui-pick': { tick: 26, known: 0, feed: (s) => { uiFeed(s); click(s, 70, 540); s.hover(600, 300, 800, 600); } },
  'ui-pick-text': { tick: 31, known: 0, feed: (s) => { uiFeed(s); click(s, 70, 540); s.hover(600, 300, 800, 600); typed(s, 'hi'); } },
  'ui-pick-squelched': { tick: 20, known: 0, feed: (s) => { uiFeed(s); click(s, 250, 540); } },
  'ui-unpick': { tick: 24, known: 0, feed: (s) => { uiFeed(s); click(s, 250, 540); click(s, 250, 540); } },
  'ui-squelch': { tick: 30, known: 0, feed: (s) => { uiFeed(s); s.chatLive(true); click(s, 70, 540); s.hover(600, 300, 800, 600); click(s, 60, 480); } },
  'ui-whisper': { tick: 34, known: 0, send: () => true, feed: (s) => { uiFeed(s); s.chatLive(true); click(s, 70, 540); s.hover(600, 300, 800, 600); typed(s, 'hi there'); click(s, 147, 460); } },
  'ui-unsquelch': { tick: 30, known: 0, feed: (s) => { uiFeed(s); s.chatLive(true); click(s, 250, 540); s.hover(600, 300, 800, 600); click(s, 140, 480); } },
  // Chat text is not trimmed: spaces count as text (SEND and WHISPER send them, EMOTE is dim on a blank line), SEND with a user picked sends the line as typed, and EMOTE clears the line.
  'ui-chat-blank-typed': { tick: 18, file: 'ui-chat-blank-typed', known: 0, name: '', send: sentByCommand, feed: (s) => { uiFeed(s); s.chatLive(true); typed(s, '   '); } },
  'ui-chat-blank-send': { tick: 22, file: 'ui-chat-blank-send', known: 0, name: '', send: sentByCommand, feed: (s) => { uiFeed(s); s.chatLive(true); typed(s, '   '); click(s, 60, 460); } },
  'ui-chat-blank-emote': { tick: 26, file: 'ui-chat-blank-emote', known: 0, name: '', send: sentByCommand, feed: (s) => { uiFeed(s); s.chatLive(true); typed(s, '   '); click(s, 60, 460); click(s, 227, 480); } },
  'ui-chat-blank-pick': { tick: 24, file: 'ui-chat-blank-pick', known: 0, name: '', send: sentByCommand, feed: (s) => { uiFeed(s); s.chatLive(true); click(s, 70, 540); s.hover(600, 300, 800, 600); typed(s, '   '); } },
  'ui-chat-blank-whisper': { tick: 28, file: 'ui-chat-blank-whisper', known: 0, name: '', send: sentByCommand, feed: (s) => { uiFeed(s); s.chatLive(true); click(s, 70, 540); s.hover(600, 300, 800, 600); typed(s, '   '); click(s, 147, 460); click(s, 60, 460); } },
  'ui-chat-blank-send-pick': { tick: 30, file: 'ui-chat-blank-send-pick', known: 0, name: '', send: sentByCommand, feed: (s) => { uiFeed(s); s.chatLive(true); click(s, 70, 540); s.hover(600, 300, 800, 600); typed(s, '   '); click(s, 147, 460); click(s, 60, 460); click(s, 227, 480); } },
  'ui-chat-blank-emote-pick': { tick: 32, file: 'ui-chat-blank-emote-pick', known: 0, name: '', send: sentByCommand, feed: (s) => { uiFeed(s); s.chatLive(true); click(s, 70, 540); s.hover(600, 300, 800, 600); typed(s, '   '); click(s, 147, 460); click(s, 60, 460); click(s, 227, 480); } },
  'ui-chat-pad-pick': { tick: 24, file: 'ui-chat-pad-pick', known: 0, name: '', send: sentByCommand, feed: (s) => { uiFeed(s); s.chatLive(true); click(s, 70, 540); s.hover(600, 300, 800, 600); typed(s, '  hi  '); } },
  'ui-chat-pad-whisper': { tick: 28, file: 'ui-chat-pad-whisper', known: 0, name: '', send: sentByCommand, feed: (s) => { uiFeed(s); s.chatLive(true); click(s, 70, 540); s.hover(600, 300, 800, 600); typed(s, '  hi  '); click(s, 147, 460); typed(s, '  yo  '); } },
  'ui-chat-pad-send': { tick: 32, file: 'ui-chat-pad-send', known: 0, name: '', send: sentByCommand, feed: (s) => { uiFeed(s); s.chatLive(true); click(s, 70, 540); s.hover(600, 300, 800, 600); typed(s, '  hi  '); click(s, 147, 460); typed(s, '  yo  '); click(s, 60, 460); typed(s, '  em  '); } },
  'ui-chat-pad-emote': { tick: 36, file: 'ui-chat-pad-emote', known: 0, name: '', send: sentByCommand, feed: (s) => { uiFeed(s); s.chatLive(true); click(s, 70, 540); s.hover(600, 300, 800, 600); typed(s, '  hi  '); click(s, 147, 460); typed(s, '  yo  '); click(s, 60, 460); typed(s, '  em  '); click(s, 227, 480); } },
  'ui-chat-emote-pad-typed': { tick: 24, file: 'ui-chat-emote-pad-typed', known: 0, name: '', send: sentByCommand, feed: (s) => { uiFeed(s); s.chatLive(true); click(s, 70, 540); s.hover(600, 300, 800, 600); typed(s, '  em  '); } },
  'ui-chat-emote-sent': { tick: 30, file: 'ui-chat-emote-sent', known: 0, name: '', send: sentByCommand, feed: (s) => { uiFeed(s); s.chatLive(true); click(s, 70, 540); s.hover(600, 300, 800, 600); typed(s, '  em  '); click(s, 227, 480); click(s, 227, 480); } },
  'ui-send': { tick: 28, known: 0, name: '', feed: (s) => { uiFeed(s); s.chatLive(true); typed(s, 'plain text'); click(s, 60, 460); } },
  'ui-emote': { tick: 28, known: 0, feed: (s) => { uiFeed(s); s.chatLive(true); typed(s, 'waves'); click(s, 227, 480); } },
  // The mouse held down on a button (no release): QUIT and the user-list arrows show the pressed cel; the disabled buttons stay as they were.
  'press-quit': { tick: 20, known: 0, feed: (s) => { uiFeed(s); mouse(s, 'c', 701, 485); } },
  'press-arrow1': { tick: 24, known: 0, feed: (s) => { uiFeed(s); mouse(s, 'c', 759, 578); } },
  'press-arrow3': { tick: 28, known: 0, feed: (s) => { uiFeed(s); mouse(s, 'c', 759, 523); } },
  // Picked Hero00 with "hi" typed: SEND, WHISPER, SQUELCH and EMOTE are enabled and show the pressed cel; UNSQUELCH is dim and ignores the mouse.
  'press-send': { tick: 22, known: 0, feed: (s) => { uiFeed(s); click(s, 70, 540); typed(s, 'hi'); mouse(s, 'c', 35, 465); } },
  'press-whisper': { tick: 24, known: 0, feed: (s) => { uiFeed(s); click(s, 70, 540); typed(s, 'hi'); mouse(s, 'c', 115, 465); } },
  'press-squelch': { tick: 26, known: 0, feed: (s) => { uiFeed(s); click(s, 70, 540); typed(s, 'hi'); mouse(s, 'c', 35, 485); } },
  'press-emote': { tick: 28, known: 0, feed: (s) => { uiFeed(s); click(s, 70, 540); typed(s, 'hi'); mouse(s, 'c', 203, 485); } },
  'press-dim-unsquelch': { tick: 30, known: 0, feed: (s) => { uiFeed(s); click(s, 70, 540); typed(s, 'hi'); mouse(s, 'c', 107, 485); } },
  // Picked the squelched Hero03: UNSQUELCH is enabled, SQUELCH is dim.
  'press-unsquelch': { tick: 22, known: 0, feed: (s) => { uiFeed(s); click(s, 250, 540); typed(s, 'hi'); mouse(s, 'c', 107, 485); } },
  'press-sq-whisper': { tick: 24, known: 0, feed: (s) => { uiFeed(s); click(s, 250, 540); typed(s, 'hi'); mouse(s, 'c', 115, 465); } },
  'press-dim-squelch': { tick: 26, known: 0, feed: (s) => { uiFeed(s); click(s, 250, 540); typed(s, 'hi'); mouse(s, 'c', 35, 485); } },
  // The tooltip held against the screen's right edge: the realm line's length decides how far the block is pushed left (cells 10, 9, 8, 7 of the capture).
  'ui-tip-r1-c10': { tick: 18, file: 'ui-tip-r1-c10', known: 0, feed: (s) => { clampFeed(s, [40, 50, 60]); s.hover(670, 540, 800, 600); } },
  'ui-tip-r1-c9': { tick: 22, file: 'ui-tip-r1-c9', known: 0, feed: (s) => { clampFeed(s, [40, 50, 60]); s.hover(610, 540, 800, 600); } },
  'ui-tip-r1-c8': { tick: 26, file: 'ui-tip-r1-c8', known: 0, feed: (s) => { clampFeed(s, [40, 50, 60]); s.hover(550, 540, 800, 600); } },
  'ui-tip-r1-c7': { tick: 30, file: 'ui-tip-r1-c7', known: 0, feed: (s) => { clampFeed(s, [40, 50, 60]); s.hover(490, 540, 800, 600); } },
  'ui-tip-r2-c10': { tick: 18, file: 'ui-tip-r2-c10', known: 0, feed: (s) => { clampFeed(s, [28, 34, 44]); s.hover(670, 540, 800, 600); } },
  'ui-tip-r3-c10': { tick: 18, file: 'ui-tip-r3-c10', known: 0, feed: (s) => { clampFeed(s, [20, 24, 30]); s.hover(670, 540, 800, 600); } },
  'ui-tip-r3-c9': { tick: 22, file: 'ui-tip-r3-c9', known: 0, feed: (s) => { clampFeed(s, [20, 24, 30]); s.hover(610, 540, 800, 600); } },
  'ui-tip-plain': { tick: 18, known: 0, feed: (s) => { tipFeed(s); s.hover(70, 540, 800, 600); } },
  'ui-tip-dead': { tick: 27, known: 0, feed: (s) => { tipFeed(s); s.hover(250, 540, 800, 600); } },
  'ui-tip-classic': { tick: 48, known: 0, feed: (s) => { tipFeed(s); s.hover(670, 540, 800, 600); } },
  // The panels HELP, CHANNEL and LADDER open on a click (the capture's cursor stays on the button): HELP's list at its top, mid-way and at its end,
  // CHANNEL with its name edit and the channel list, LADDER with its four wide buttons; the CHANNEL and LADDER buttons stay pressed while their panel is open.
  // The four latching buttons by mouse: the panel opens on the press and the button stays pressed while it is open; another panel unlatches it; a second click
  // on the latched one is swallowed; a held press on a latching button shows the same.
  'toggle-create': { tick: 21, known: 0, feed: (s) => { uiFeed(s); click(s, 590, 460); } },
  'toggle-join': { tick: 25, known: 0, feed: (s) => { uiFeed(s); click(s, 590, 460); click(s, 700, 460); } },
  'toggle-channel': { tick: 29, known: 0, feed: (s) => { uiFeed(s); s.setChannels(['Diablo II Test Channel', 'The Void', 'Op Chat', 'Clan Recruit']); click(s, 590, 460); click(s, 700, 460); click(s, 575, 480); } },
  'toggle-ladder': { tick: 33, known: 0, feed: (s) => { uiFeed(s); click(s, 590, 460); click(s, 700, 460); click(s, 575, 480); click(s, 654, 480); } },
  'toggle-create2': { tick: 37, known: 0, feed: (s) => { uiFeed(s); click(s, 590, 460); click(s, 700, 460); click(s, 575, 480); click(s, 654, 480); click(s, 590, 460); } },
  'toggle-create-reclick': { tick: 39, known: 0, feed: (s) => { uiFeed(s); click(s, 590, 460); click(s, 700, 460); click(s, 575, 480); click(s, 654, 480); click(s, 590, 460); click(s, 590, 460); } },
  'toggle-join-held': { tick: 45, known: 0, feed: (s) => { uiFeed(s); click(s, 590, 460); click(s, 700, 460); click(s, 575, 480); click(s, 654, 480); click(s, 590, 460); click(s, 590, 460); mouse(s, 'c', 700, 460); } },
  'help-open': { tick: 24, known: 0, feed: (s) => { uiFeed(s); click(s, 227, 460); } },
  'help-scroll40': { tick: 62, known: 0, feed: (s) => { uiFeed(s); click(s, 227, 460); s.scrollHelp(40); } },
  'help-end': { tick: 36, known: 0, feed: (s) => { uiFeed(s); click(s, 227, 460); s.scrollHelp(94); } },
  'channel-panel': { tick: 24, known: 0, feed: (s) => { uiFeed(s); s.setChannels(['Diablo II Test Channel', 'The Void', 'Op Chat', 'Clan Recruit']); click(s, 575, 480); } },
  'ladder-panel': { tick: 24, known: 0, feed: (s) => { uiFeed(s); click(s, 654, 480); } },
  // Clicks inside the CHANNEL and LADDER panels and the exclusion between the panels: each state is the clicks, in order, from the input capture's channel.
  ...Object.fromEntries(Object.entries({
    'chan-pick0': [26, [[575, 480], [440, 206]]], 'chan-pick1': [29, [[575, 480], [440, 206], [440, 244]]], 'chan-pick-empty': [32, [[575, 480], [440, 206], [440, 244], [440, 301]]],
    'chan-click-edit': [35, [[575, 480], [440, 206], [440, 244], [440, 301], [500, 172]]], 'chan-click-bar': [38, [[575, 480], [440, 206], [440, 244], [440, 301], [500, 172], [758, 300]]],
    'chan-pick1b': [41, [[575, 480], [440, 206], [440, 244], [440, 301], [500, 172], [758, 300], [440, 225]]],
    'chan-ok': [44, [[575, 480], [440, 206], [440, 244], [440, 301], [500, 172], [758, 300], [440, 225], [700, 420]]],
    'chan-ok-sel0': [26, [[575, 480], [700, 420]]], 'chan-twice': [26, [[575, 480], [575, 480]]], 'chan-g-pick': [25, [[575, 480], [440, 225]]], 'chan-cancel': [32, [[575, 480], [440, 225], [480, 420]]],
    'chan-then-ladder': [26, [[575, 480], [654, 480]]], 'chan-ladder-create': [30, [[575, 480], [654, 480], [590, 459]]], 'chan-ladder-create-join': [34, [[575, 480], [654, 480], [590, 459], [710, 459]]],
    'chan-lcj-channel': [38, [[575, 480], [654, 480], [590, 459], [710, 459], [575, 480]]], 'chan-lcj-cl-ladder': [42, [[575, 480], [654, 480], [590, 459], [710, 459], [575, 480], [654, 480]]],
    'chan-pick-again': [28, [[575, 480], [440, 244], [575, 480]]], 'ladder-twice': [26, [[654, 480], [654, 480]]],
    'chan-pick-ladder': [27, [[575, 480], [440, 244], [654, 480]]], 'chan-pick-ladder-chan': [30, [[575, 480], [440, 244], [654, 480], [575, 480]]],
    'create-then': [22, [[590, 459]]], 'create-channel': [26, [[590, 459], [575, 480]]], 'create-chan-ladder': [30, [[590, 459], [575, 480], [654, 480]]],
    'create-cl-join': [34, [[590, 459], [575, 480], [654, 480], [710, 459]]], 'create-clj-channel': [38, [[590, 459], [575, 480], [654, 480], [710, 459], [575, 480]]],
  }).map(([name, [tick, clicks]]) => [name, { tick, known: 0, feed: (s) => { uiFeed(s); s.setChannels(['Diablo II Test Channel', 'The Void', 'Op Chat', 'Clan Recruit']); for (const [x, y] of clicks) click(s, x, y); } }])),
  // The channel list's scrollbar: the list holds the current channel and the names; ten rows show, the bar appears past ten and the S op scrolls it five rows.
  ...Object.fromEntries(Object.entries({
    'chan-ten': [26, 10, 0], 'chan-eleven': [26, 11, 0], 'chan-many': [26, 30, 5], 'chan-many-s5': [30, 30, 5],
  }).map(([name, [tick, n, scroll]]) => [name, { tick, known: 0, feed: (s) => { uiFeed(s);
    s.setChannels(['Diablo II Test Channel', ...Array.from({ length: n }, (_, i) => `Chan ${String(i + 1).padStart(2, '0')}`)]); click(s, 575, 480); if (scroll) s.wheel(500, 300, 800, 600, scroll); } }])),
  // The wide ladder buttons never draw pressed and a click on one changes nothing on screen: the held press (mouse down only) at each, then a whole click.
  ...Object.fromEntries(Object.entries({ 'ladder-down-hc': [26, 600, 222], 'ladder-down-std': [26, 600, 172], 'ladder-down-xhc': [26, 600, 322], 'ladder-down-xp': [26, 600, 272] })
    .map(([name, [tick, x, y]]) => [name, { tick, known: 0, feed: (s) => { uiFeed(s); click(s, 654, 480); s.hover(x, y, 800, 600); s.press(x, y, 800, 600); } }])),
  'ladder-click-hc': { tick: 28, known: 0, feed: (s) => { uiFeed(s); click(s, 654, 480); click(s, 600, 222); } },
  'ladder-click-hc-later': { tick: 32, known: 0, feed: (s) => { uiFeed(s); click(s, 654, 480); click(s, 600, 222); } },
  // A name typed over the edit's text (the capture sets it whole): the list's highlight stays where the last pick left it.
  'chan-typed': { tick: 25, known: 0, feed: (s) => { uiFeed(s); s.setChannels(['Diablo II Test Channel', 'The Void', 'Op Chat', 'Clan Recruit']); click(s, 575, 480);
    for (let i = 0; i < 22; i++) s.typeKey('Backspace', ''); typed(s, 'Typed Name'); } },
  'chan-pick-typed': { tick: 26, known: 0, feed: (s) => { uiFeed(s); s.setChannels(['Diablo II Test Channel', 'The Void', 'Op Chat', 'Clan Recruit']); click(s, 575, 480); click(s, 440, 225);
    for (let i = 0; i < 8; i++) s.typeKey('Backspace', ''); typed(s, 'Typed Name'); } },
  'ranks-top': { tick: 26, known: 0, feed: (s) => { s.chatEvent(ev(7, '', 'Diablo II Test Channel')); rankUsers(22).forEach((e) => s.chatEvent(e)); } },
  'ranks-scrolled': { tick: 33, known: 0, feed: (s) => { STATES['ranks-top'].feed(s); s.activate('arrow-1'); } },
  'ranks-talk': { tick: 26, known: 0, feed: (s) => { s.chatEvent(ev(7, '', 'Diablo II Test Channel')); rankUsers(11).forEach((e) => s.chatEvent(e));
    rankUsers(11).forEach((e, i) => s.chatEvent({ ...ev(5, e.username, `line from flags ${RANK_FLAGS[i].toString(16)}`), flags: RANK_FLAGS[i] })); } },
  // CHANNEL list bar and empty OK (the `ui` capture: 31 channels, five rows scrolled, the bar pressed and released by the capture's ops).
  'chan-bar-down': { tick: 27, known: 0, feed: (s) => { chanOpen(s); s.wheel(500, 300, 800, 600, 5); bar(s, 758, 388); } },
  'chan-bar-up': { tick: 27, known: 0, feed: (s) => { chanOpen(s); s.wheel(500, 300, 800, 600, 5); bar(s, 758, 200); } },
  'chan-bar-track': { tick: 27, known: 0, feed: (s) => { chanOpen(s); s.wheel(500, 300, 800, 600, 5); bar(s, 758, 300); } },
  'chan-bar-thumb': { tick: 28, known: 0, feed: (s) => { chanOpen(s); s.wheel(500, 300, 800, 600, 5); const t = s.press(758, 250, 800, 600);
    s.hover(758, 300, 800, 600); s.hover(758, 330, 800, 600); s.release(t, 758, 330, 800, 600); } },
  'chan-ok-empty': { tick: 27, known: 0, feed: (s) => { chanOpen(s); for (let i = 0; i < 40; i++) s.typeKey('Backspace'); click(s, 700, 420); } },
};
// The `ui` capture's channel list: the 31 names of the capture, the input channel's users, then CHANNEL opened.
const chanNames31 = ['Diablo II Test Channel', ...Array.from({ length: 30 }, (_, i) => `Chan ${String(i + 1).padStart(2, '0')}`)];
const chanOpen = (s) => { uiFeed(s); s.setChannels(chanNames31); click(s, 575, 480); };
const bar = (s, x, y) => { s.hover(x, y, 800, 600); const t = s.press(x, y, 800, 600); s.release(t, x, y, 800, 600); };

for (const [name, st] of Object.entries(STATES)) {
  const file = join(root, 'vectors', 'drawlist', `bnet-${st.file ?? name}.txt`);
  test(`lobby state ${name} against its retail capture`, { skip: !rd || !existsSync(file) }, () => {
    const strings = stringTable(rd);
    const screen = createBnetLobbyScreen({ read: rd, decodeDc6, decodeDatPalette, string: (i) => strings.get(i) ?? '', character: st.character ?? { name: st.name ?? 'Hero' }, documentObject, now: () => 0, offline: true, ...(st.send ? { onSendChat: st.send } : {}),
      portraitFrame: (slot) => portraitFrames[slot] ?? 0, auraFrame: () => auraFrameOf });
    const retail = parseDrawListVector(readFileSync(file, 'utf8')).filter((r) => r.op === 'cel' || r.op === 'celcolor');
    // The portraits' animation phase follows the capture's wall clock, which the vector does not carry: read each slot's frame from it.
    // Everything else about a portrait (token, layer count, mode, position, draw order) is still compared.
    const portraitFrames = []; for (const r of retail) if (/^dcc:/i.test(r.path)) portraitFrames[(r.x - 70) / 60] = r.frame;
    // The squelch aura animates on the capture's wall clock as the portraits do: its frame is read from the vector.
    const auraFrameOf = retail.find((r) => /squelchaura/i.test(r.path))?.frame ?? 0;
    const logo = retail.find((r) => /d2logobnet(\.dc6)?$/i.test(r.path));
    // The capture's real clock is not its tick: the caret is on exactly when retail drew it, so put the clock on that half period.
    const caret = retail.some((r) => /fontformal10(\.dc6)?$/i.test(r.path) && r.frame === 95), clock = st.tick * 250 + (st.prechat || caret === (st.tick % 2 === 1) ? 0 : 250);
    screen.pinClock(clock, st.prechat ? 250 : clock - (logo ? logo.frame : 0) * 40);
    if (!st.prechat) screen.activate('enter-chat'); screen.hover(0, 1, 800, 600);
    st.feed(screen, { clock, built: clock - (logo ? logo.frame : 0) * 40 });
    const ours = [];
    globalThis.__d2DrawList = ours;
    try { screen.draw(ctx, 800, 600); } finally { delete globalThis.__d2DrawList; }
    const d = diffDrawLists(retail, ours.map((r) => ({ ...r, hash: 0 })), { colour: (r) => /fontformal10/i.test(r.path) && r.x >= 432 && r.x < 592 && r.y >= 230 && r.y <= 392 });
    console.log(`bnet-${name}: ${retail.length} retail draws, ${ours.length} ours, differing records ${d.diffs} (missing ${d.missing.length}, extra ${d.extra.length}, differing ${d.differing.length})`);
    for (const m of d.missing.slice(0, 3)) console.log('  first missing', m.op, m.path, m.frame, m.x, m.y, m.mode);
    for (const m of d.extra.slice(0, 3)) console.log('  first extra', m.op, m.path, m.frame, m.x, m.y, m.mode);
    for (const m of d.differing.slice(0, 20)) console.log('  first differs', m.draw, m.diff);
    if (st.known !== null) assert.ok(d.diffs <= st.known, `${d.diffs} differing records, was ${st.known}`);
  });
}

// The text each chat control hands the connection, as retail's headless run logged it (the Bnclient chat-send slot, one line per send):
// the line goes out as typed, blanks and all; only WHISPER addresses the picked user; EMOTE prefixes "/me " and refuses a slash; Enter on blanks sends nothing.
test('chat sends the typed text unchanged, as retail logged it', { skip: !rd }, () => {
  const strings = stringTable(rd), sent = [];
  const screen = createBnetLobbyScreen({ read: rd, decodeDc6, decodeDatPalette, string: (i) => strings.get(i) ?? '', character: { name: '' }, documentObject, now: () => 0, offline: true,
    onSendChat: (l) => { sent.push(l); return true; }, portraitFrame: () => 0 });
  screen.pinClock(0, 0); screen.activate('enter-chat'); screen.hover(0, 1, 800, 600);
  let at = 0; const gap = () => { at += 2000; screen.pinClock(at, 0); };
  uiFeed(screen); screen.chatLive(true);
  const key = (c) => screen.typeKey(c === ' ' ? 'Space' : c === 'Enter' ? 'Enter' : `Key${c.toUpperCase()}`, c === 'Enter' ? undefined : c);
  const line = (t) => { for (const c of t) key(c); };
  line('   '); click(screen, 227, 480); gap();                      // EMOTE on blanks: sent
  line('/me x'); click(screen, 227, 480); gap();                    // a slash line: refused, stays in the box
  screen.typeKey('Backspace'); screen.typeKey('Backspace'); screen.typeKey('Backspace'); screen.typeKey('Backspace'); screen.typeKey('Backspace');
  line('  em  '); click(screen, 227, 480); gap();                   // padded EMOTE
  line('   '); key('Enter');                                        // Enter on blanks: cleared, nothing sent
  line('hey there'); key('Enter');
  line('  x '); click(screen, 60, 460);                             // SEND, no pick
  click(screen, 70, 540); screen.hover(600, 300, 800, 600);
  line('  yo  '); click(screen, 60, 460);                           // SEND with a pick: not whispered
  line('  hi  '); click(screen, 147, 460);                          // WHISPER
  line('hey'); key('Enter');                                        // Enter with a pick: not whispered
  assert.deepEqual(sent, ['/me    ', '/me   em  ', 'hey there', '  x ', '  yo  ', '/whisper *Hero00   hi  ', 'hey']);
});

// The portrait phase table (vectors/drawlist/bnet-portrait-phase.txt: "tick slot token frame", ticks 20..40, D2Win clock = tick * 250):
// a portrait advances at most one frame per tick and wraps to 0; the per-slot rate is fixed. Live clock data, not drawing code.
test('portrait phase table: one frame step at most per tick, frames wrap to 0', { skip: !existsSync(join(root, 'vectors', 'drawlist', 'bnet-portrait-phase.txt')) }, () => {
  const rows = readFileSync(join(root, 'vectors', 'drawlist', 'bnet-portrait-phase.txt'), 'utf8').split('\n').filter((l) => l && l[0] !== '#').map((l) => l.split(' '));
  const bySlot = {};
  for (const [t, s, tok, f] of rows) (bySlot[s] ??= { tok, f: {} }).f[+t] = +f;
  assert.equal(Object.keys(bySlot).length, 11);
  for (const [s, { tok, f }] of Object.entries(bySlot)) {
    const max = Math.max(...Object.values(f)); let wraps = 0;
    for (let t = 21; t <= 40; t++) { const d = f[t] - f[t - 1]; if (d < 0) { wraps++; assert.equal(f[t], 0, `slot ${s} wraps to 0`); } else assert.ok(d <= 1, `slot ${s} (${tok}) steps ${d} at tick ${t}`); }
    assert.ok(new Set(Object.values(f)).size >= 4 && max >= 4, `slot ${s} (${tok}) animates`);
  }
});

// The model against the table: per slot, some start offset must reproduce every recorded frame with the class's frame count and period.
// The offsets are free (each user's arrival is not recorded), so this proves frame counts and periods, not arrival times. The class periods
// are 500 ms (AM, SO, NE, AI), 357 (PA), 520 (DZ, fits 502..555) and 576 (RH, fits 573..580 across slots); DZ and RH are the loosest.
test('portrait phase table: frame = floor(elapsed / period) % frames fits each slot', { skip: !existsSync(join(root, 'vectors', 'drawlist', 'bnet-portrait-phase.txt')) }, () => {
  const rows = readFileSync(join(root, 'vectors', 'drawlist', 'bnet-portrait-phase.txt'), 'utf8').split('\n').filter((l) => l && l[0] !== '#').map((l) => l.split(' '));
  const bySlot = {};
  for (const [t, s, tok, f] of rows) (bySlot[s] ??= { tok, f: {} }).f[+t] = +f;
  const fit = (tok, f) => { for (let off = 0; off < 9000; off++) { let ok = true; for (let t = 20; t <= 40 && ok; t++) ok = portraitFrameAt(tok, t * 250 + off) === f[t]; if (ok) return true; } return false; };
  for (const [s, { tok, f }] of Object.entries(bySlot)) assert.ok(fit(tok, f), `slot ${s} (${tok})`);
});

// The arrival-aware table (vectors/drawlist/bnet-portrait-arrival.txt: ticks 0..41, one user joining per tick, slot s drawn first at tick s + 1):
// a portrait is drawn from the tick its user arrives, and from the tick after that its frames follow the class's frame count and period.
// The arrival tick itself can show the frame count (one past the last frame) for a tick, which the cel engine never shows twice.
test('portrait arrival table: drawn from the arrival tick, frames follow the class model after it', { skip: !existsSync(join(root, 'vectors', 'drawlist', 'bnet-portrait-arrival.txt')) }, () => {
  const rows = readFileSync(join(root, 'vectors', 'drawlist', 'bnet-portrait-arrival.txt'), 'utf8').split('\n').filter((l) => l && l[0] !== '#').map((l) => l.split(' '));
  const bySlot = {};
  for (const [t, s, tok, f] of rows) (bySlot[s] ??= { tok, f: {} }).f[+t] = +f;
  assert.equal(Object.keys(bySlot).length, 11);
  for (const [s, { tok, f }] of Object.entries(bySlot)) {
    const ticks = Object.keys(f).map(Number);
    assert.equal(Math.min(...ticks), +s + 1, `slot ${s} first drawn at its arrival tick`);
    assert.equal(Math.max(...ticks), 41, `slot ${s} drawn to the end`);
    // RH is the one class without a single period (its slots fit 563..583 ms), so it may slip two ticks over the run.
    const allow = tok === 'RH' ? 2 : 0; let fits = false;
    for (let off = 0; off < 9000 && !fits; off++) { let bad = 0; for (let t = +s + 2; t <= 41; t++) if (portraitFrameAt(tok, t * 250 + off) !== f[t]) bad++; fits = bad <= allow; }
    assert.ok(fits, `slot ${s} (${tok}) follows the model from the tick after arrival`);
  }
});
