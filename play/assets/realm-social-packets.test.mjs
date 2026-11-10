import test from 'node:test';
import assert from 'node:assert/strict';
import { routeSocialPackets, parseGameChat } from './realm-social-packets.mjs';
import { Roster, rosterEventLine, eventLineText } from './bnet-roster.mjs';
import { gameChatLine } from './bnet-game-chat.mjs';
import { newTrade, applyTradePacket } from './bnet-game-trade.mjs';

const latin = (s, n) => { const a = Array.from(s, (c) => c.charCodeAt(0)); while (a.length < n) a.push(0); return a; };
const u32 = (v) => [v & 255, (v >>> 8) & 255, (v >>> 16) & 255, (v >>> 24) & 255];
const joined = (id, name, cls, level) => { const b = [0x5b, 0x24, 0, ...u32(id), cls, ...latin(name, 16), level, 0, 0xff, 0xff, 0, 0, 0, 0, 0, 0, 0, 0]; assert.equal(b.length, 0x24); return b; };
const chat = (type, guid, name, text) => [0x26, type, 0, type === 5 ? 0 : 2, ...u32(guid), 0, 5, ...latin(name, name.length + 1), ...latin(text, text.length + 1)];
const event = (type, name, account) => [0x5a, type, 4, 0, 0, 0, 0, 0, ...latin(name, 16), ...latin(account, 16)];
const walk = [0x0f, 0, 1, 0, 0, 0, 0x17, 0, 0, 0, 0, 0, 0, 0, 0, 0];
// The sizes the client's table gives these packets (0x26 and 0x5b carry their own length).
const sizeOf = (b) => {
  if (b[0] === 0x0f) return 16;
  if (b[0] === 0x5a) return 40;
  if (b[0] === 0x5c) return 5;
  if (b[0] === 0x8d) return 7;
  if (b[0] === 0x77) return 2;
  if (b[0] === 0x5b) return b.length >= 3 ? b[1] | b[2] << 8 : 0;
  if (b[0] === 0x26) { const n = b.indexOf(0, 10); const m = n < 0 ? -1 : b.indexOf(0, n + 1); return m < 0 ? 0 : m + 1; }
  return 0;
};

function game() {
  const roster = new Roster(), lines = [], chats = [], trade = newTrade();
  const lang = () => null;
  const handlers = {
    roster: (p) => roster.feed(p),
    trade: (p) => applyTradePacket(trade, p),
    event: (p) => { const line = eventLineText(rosterEventLine(p, lang, roster, 'Me')); if (line) lines.push(line); },
    chat: (c) => { chats.push(c); const line = gameChatLine(c, lang); if (line) lines.push(line); } };
  return { roster, lines, chats, trade, feed: (bytes) => routeSocialPackets(Uint8Array.from(bytes), sizeOf, handlers) };
}

test('every social packet of one delivery is applied, not only the first', () => {
  const g = game();
  const n = g.feed([...walk, ...joined(7, 'Sorca', 1, 30), ...event(2, 'Sorca', 'acct'), ...chat(1, 7, 'Sorca', 'hello'), ...walk, ...chat(1, 7, 'Sorca', 'anyone?')]);
  assert.equal(n, 4);
  assert.deepEqual(g.roster.list().map((p) => [p.id, p.name, p.cls, p.level]), [[7, 'Sorca', 1, 30]]);
  assert.equal(g.chats.length, 2);
  assert.deepEqual(g.chats.map((c) => String.fromCharCode(...c.message)), ['hello', 'anyone?']);
  assert.equal(g.lines.length, 3);
  assert.match(g.lines[0], /Sorca/);
  assert.match(g.lines[1], /hello/);
  assert.match(g.lines[2], /anyone\?/);
});

test('the join burst seeds the roster with the players already in the game', () => {
  const g = game();
  g.feed([...walk, ...joined(1, 'Me', 0, 12), ...joined(2, 'Pal', 3, 40), ...joined(3, 'Nec', 2, 41), 0x8d, ...u32(2), 5, 0, 0x8d, ...u32(3), 5, 0]);
  assert.deepEqual(g.roster.list().map((p) => p.name), ['Nec', 'Pal', 'Me']);
  assert.equal(g.roster.sameParty(2, 3), true);
  g.feed([...walk, 0x5c, ...u32(2)]);
  assert.deepEqual(g.roster.list().map((p) => p.name), ['Nec', 'Me']);
});

test('a trade packet behind another packet still reaches the trade state', () => {
  const g = game(), before = JSON.stringify(g.trade);
  g.feed([...walk, 0x77, 1]);
  assert.notEqual(JSON.stringify(g.trade), before);
});

test('a chat packet decodes by its own bytes; a cut tail is left alone', () => {
  const c = parseGameChat(Uint8Array.from(chat(1, 0x01020304, 'Abc', 'hi there')));
  assert.deepEqual([c.messageType, c.subType, c.unitGuid, c.color], [1, 2, 0x01020304, 5]);
  assert.equal(String.fromCharCode(...c.name), 'Abc');
  assert.equal(String.fromCharCode(...c.message), 'hi there');
  const g = game();
  assert.equal(g.feed([...chat(1, 7, 'Abc', 'one'), 0x26, 1, 0, 2, 7, 0]), 1);
  assert.equal(g.chats.length, 1);
});
