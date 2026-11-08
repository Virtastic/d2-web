import test from 'node:test';
import assert from 'node:assert/strict';
import { Roster, RELATION, PARTY_NONE, applyRosterPacket, rosterEventLine, eventLineText, eventColourCode } from './bnet-roster.mjs';

const w32 = (b, o, n) => { b[o] = n & 255; b[o + 1] = (n >>> 8) & 255; b[o + 2] = (n >>> 16) & 255; b[o + 3] = (n >>> 24) & 255; };
const w16 = (b, o, n) => { b[o] = n & 255; b[o + 1] = (n >>> 8) & 255; };
const put = (b, o, s) => { for (let i = 0; i < s.length; i++) b[o + i] = s.charCodeAt(i); };
function join(id, name, { cls = 1, level = 5, party = PARTY_NONE, title = '' } = {}) {
  const b = new Uint8Array(0x23 + title.length); b[0] = 0x5b; w16(b, 1, b.length); w32(b, 3, id); b[7] = cls; put(b, 8, name);
  w16(b, 0x18, level); w16(b, 0x1a, party); put(b, 0x22, title); return b;
}
const left = (id) => { const b = new Uint8Array(5); b[0] = 0x5c; w32(b, 1, id); return b; };
const state = (id, s) => { const b = new Uint8Array(6); b[0] = 0x8b; w32(b, 1, id); b[5] = s; return b; };
const rel = (a, o, f) => { const b = new Uint8Array(11); b[0] = 0x8c; w32(b, 1, a); w32(b, 5, o); w16(b, 9, f); return b; };
const party = (id, p) => { const b = new Uint8Array(7); b[0] = 0x8d; w32(b, 1, id); w16(b, 5, p); return b; };

test('player joined fills the roster, newest first, with the account title in the display name', () => {
  const r = new Roster();
  assert.deepEqual(applyRosterPacket(r, join(7, 'Alice', { level: 12, party: 3, cls: 4 })), { kind: 'join', id: 7 });
  applyRosterPacket(r, join(9, 'Bob', { title: 'bobacct' }));
  assert.deepEqual(r.list().map((p) => p.id), [9, 7]);
  assert.equal(r.get(7).level, 12); assert.equal(r.get(7).party, 3); assert.equal(r.get(7).cls, 4);
  assert.equal(r.displayName(9), 'Bob {bobacct}'); assert.equal(r.displayName(7), 'Alice');
  assert.equal(r.byName('bOB').id, 9); assert.equal(r.byName('nobody'), null);
});
test('a join with the unset id or a short packet changes nothing', () => {
  const r = new Roster();
  assert.equal(applyRosterPacket(r, join(0xffffffff, 'X')), null);
  assert.equal(applyRosterPacket(r, join(1, 'X').subarray(0, 0x20)), null);
  assert.equal(r.size, 0);
});
test('a rejoin moves to the head and keeps the relations set toward the player', () => {
  const r = new Roster(); r.myId = 1;
  applyRosterPacket(r, join(1, 'Me')); applyRosterPacket(r, join(2, 'Al'));
  applyRosterPacket(r, rel(1, 2, RELATION.LOOT));
  applyRosterPacket(r, join(1, 'Me'));
  assert.deepEqual(r.list().map((p) => p.id), [1, 2]);
  assert.equal(r.relation(1, 2), RELATION.LOOT);
});
test('player left removes the player and every relation toward them', () => {
  const r = new Roster(); r.myId = 1;
  applyRosterPacket(r, join(1, 'Me')); applyRosterPacket(r, join(2, 'Al'));
  applyRosterPacket(r, rel(1, 2, 4));
  assert.deepEqual(applyRosterPacket(r, left(2)), { kind: 'leave', id: 2 });
  assert.equal(r.get(2), null); assert.equal(r.relation(1, 2), 0);
  assert.equal(applyRosterPacket(r, left(0xffffffff)), null);
});
test('relation packet sets what one player holds toward another; the local view mirrors it', () => {
  const r = new Roster(); r.myId = 1;
  applyRosterPacket(r, join(1, 'Me')); applyRosterPacket(r, join(2, 'Al'));
  assert.deepEqual(applyRosterPacket(r, rel(1, 2, RELATION.HOSTILE | RELATION.LOOT)), { kind: 'relation', id: 1, other: 2, toMe: false });
  assert.equal(r.has(2, RELATION.HOSTILE), true);
  assert.deepEqual(applyRosterPacket(r, rel(2, 1, RELATION.SQUELCH)), { kind: 'relation', id: 2, other: 1, toMe: true });
  assert.equal(r.relation(2, 1), RELATION.SQUELCH);
  assert.equal(applyRosterPacket(r, rel(1, 1, 8)), null); // never toward oneself
  assert.equal(applyRosterPacket(r, rel(77, 1, 8)), null); // unknown subject
});
test('party id and party state packets update a known player only', () => {
  const r = new Roster();
  applyRosterPacket(r, join(2, 'Al'));
  assert.equal(applyRosterPacket(r, party(2, 5)).kind, 'party'); assert.equal(r.get(2).party, 5);
  assert.equal(applyRosterPacket(r, state(2, 2)).kind, 'state'); assert.equal(r.get(2).partyState, 2);
  assert.equal(applyRosterPacket(r, party(3, 5)), null); assert.equal(applyRosterPacket(r, state(3, 1)), null);
});
test('name colours: party green, other party orange, hostile either way red', () => {
  const r = new Roster(); r.myId = 1;
  applyRosterPacket(r, join(1, 'Me', { party: 4 })); applyRosterPacket(r, join(2, 'A', { party: 4 }));
  applyRosterPacket(r, join(3, 'B', { party: 6 })); applyRosterPacket(r, join(4, 'C'));
  assert.deepEqual([2, 3, 4].map((i) => r.colour(i)), ['ÿc2', 'ÿc8', 'ÿc0']);
  applyRosterPacket(r, rel(4, 1, RELATION.HOSTILE)); // C is hostile to me
  assert.equal(r.colour(4), 'ÿc1');
  applyRosterPacket(r, rel(1, 2, RELATION.HOSTILE)); // I am hostile to A
  assert.equal(r.colour(2), 'ÿc1');
});

const ev = (type, name = '', { colour = 0, name2 = '', sub = 0, id = 0 } = {}) => {
  const b = new Uint8Array(40); b[0] = 0x5a; b[1] = type; b[2] = colour; w32(b, 3, id); b[7] = sub; put(b, 8, name); put(b, 0x18, name2); return b;
};
const none = () => undefined;   // no game strings: the built-in English text
const text = (...a) => rosterEventLine(ev(...a), none, null, 'Me')?.text;
test('the roster answers whisper lookups without case and drops players that left', () => {
  const r = new Roster();
  assert.equal(r.feed(join(5, 'Ann')), true); assert.equal(r.feed(join(6, 'Bob')), true);
  assert.equal(r.find('bob').id, 6); assert.equal(r.get(5).name, 'Ann');
  assert.equal(r.feed(left(5)), true); assert.equal(r.find('ann'), null); assert.equal(r.size, 1);
  assert.equal(r.feed(Uint8Array.from([0x26, 0])), false);
  r.clear(); assert.equal(r.size, 0);
});
test('dropped, joined, left, not in the game, not logged in', () => {
  assert.equal(text(0, 'Ann'), 'Ann dropped due to timeout.'); assert.equal(text(1, 'Ann'), 'Ann dropped due to errors.');
  assert.equal(text(2, 'Ann'), 'Ann joined our world. Diablo\'s minions grow stronger.');
  assert.equal(text(3, 'Ann'), 'Ann left our world. Diablo\'s minions weaken.');
  assert.equal(text(2, 'Ann', { name2: 'acct' }), 'Ann(acct) joined our world. Diablo\'s minions grow stronger.');
  assert.equal(text(3, 'Ann', { name2: 'acct' }), 'Ann(acct) left our world. Diablo\'s minions weaken.');
  assert.equal(text(4, 'Ann'), 'Ann is not in the game.'); assert.equal(text(5, 'Ann'), 'Ann is not logged in.');
});
test('your own arrival is not announced to you (exact name), your departure is', () => {
  assert.equal(text(2, 'Me'), undefined);
  assert.ok(text(2, 'me')); assert.ok(text(3, 'Me'));
});
test('the game string table wins over the built-in text', () => {
  assert.equal(rosterEventLine(ev(0, 'Al'), (id) => (id === 3639 ? ' timed out' : undefined), null)?.text, 'Al timed out');
});
test('slain by a player, a barrel, a named monster, or nothing to name', () => {
  assert.equal(text(6, 'Ann', { sub: 0, name2: 'Bob' }), 'Ann was slain by Bob');
  assert.equal(text(6, 'Ann', { sub: 2, id: 0xb }), 'Ann was slain by an Exploding Barrel');
  const mn = (q) => (q.classId === 7 ? 'Fallen' : q.unitId === 9 ? 'Andariel' : '');
  const m = (o) => rosterEventLine(ev(6, 'Ann', o), none, null, 'Me', mn)?.text;
  assert.equal(m({ sub: 1, name2: '\x07' }), 'Ann was slain by Fallen');
  assert.equal(m({ sub: 1, id: 9 }), 'Ann was slain by Andariel');
  assert.equal(m({ sub: 1, id: 3 }), 'Ann was slain.');
  assert.equal(text(6, 'Ann', { sub: 0 }), 'Ann was slain.');
  assert.equal(rosterEventLine(ev(6, 'Ann', { sub: 0, name2: 'x'.repeat(16) }), (id) => (id === 3648 ? 'y'.repeat(300) : undefined), null), null, 'a line that does not fit is dropped');
});
test('notices, trade refusals and world messages', () => {
  assert.equal(text(8, 'Ann'), 'Ann is busy'); assert.equal(text(9), 'You must wait a short time to trade with that player.');
  assert.equal(text(10, 'Ann'), 'Ann has items in his box.'); assert.equal(text(10), 'You have items in your box.');
  assert.equal(text(16), 'You must wait a short time to declare hostility with that player.');
  assert.equal(text(13, 'Ann'), 'Ann is not listening to you.');
  assert.equal(text(15, '', { id: 5 }), 'Realm is going down in 5 minutes.');
  assert.equal(text(17, '', { id: 12 }), '12 Stones of Jordan Sold to Merchants'); assert.equal(text(18), 'Diablo Walks the Earth');
  assert.equal(text(12, 'Al'), undefined); assert.equal(text(14, 'Al'), undefined); assert.equal(text(99, 'Al'), undefined);
});
test('type 7 names the player from the roster by name and is dropped for a squelched speaker', () => {
  const r = new Roster(); r.myId = 1;
  applyRosterPacket(r, join(1, 'Me')); applyRosterPacket(r, join(2, 'Al', { title: 'acc' }));
  const t = (sub, o = {}) => rosterEventLine(ev(7, '', { sub, id: 2, ...o }), none, r)?.text;
  assert.equal(t(1), 'Al permits you to loot his corpse.'); assert.equal(t(2), 'Al permits you to loot her corpse.');
  assert.equal(t(3), 'Al has expressed hostility towards you.'); assert.equal(t(5), 'Al invites you to ally against the forces of evil.');
  assert.equal(t(8), 'You are now allied with Al'); assert.equal(t(9), 'Al has left your party.');
  assert.equal(t(11), 'Al no longer allows you to access her corpse.');
  assert.equal(t(0), undefined); assert.equal(t(99), undefined);
  assert.equal(rosterEventLine(ev(7, '', { sub: 3, id: 99 }), none, r)?.text, ' has expressed hostility towards you.');
  applyRosterPacket(r, rel(1, 2, RELATION.SQUELCH));
  assert.equal(t(3), undefined);
});
test('the event colour byte becomes a text colour code, 0 meaning the default', () => {
  assert.equal(eventColourCode(0), ''); assert.equal(eventColourCode(1), 'ÿc1'); assert.equal(eventColourCode(10), 'ÿc:'); assert.equal(eventColourCode(11), 'ÿc;'); assert.equal(eventColourCode(12), '');
  assert.equal(eventLineText(rosterEventLine(ev(4, 'Al', { colour: 2 }), none, null)), 'ÿc2Al is not in the game.');
  assert.equal(eventLineText(null), null);
});
test('a short or foreign packet is not an event', () => {
  assert.equal(rosterEventLine(ev(4, 'Al').subarray(0, 20), none, null), null);
  assert.equal(rosterEventLine(Uint8Array.from([0x26, 0, 0]), none, null), null);
});
