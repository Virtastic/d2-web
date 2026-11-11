import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmNpc } from './realm-npc-state.mjs';

const GUID = 0x55, KASHYA = 150, CHARSI = 154;
const info = (guid, msgs) => { const p = new Uint8Array(40); p[0] = 0x27; p[1] = 1; p[2] = guid; p[6] = msgs.length;
  msgs.forEach(([menu, id], i) => { p[8 + i * 4] = menu; p[10 + i * 4] = id & 255; p[11 + i * 4] = id >> 8; }); return p; };
// A 0x9C stocking item `guid` with a compact item header on shop page `page` (raw bitstream field).
function stockPacket(action, guid, page) {
  const bits = []; const put = (v, n) => { for (let i = 0; i < n; i++) bits.push(Math.floor(v / 2 ** i) & 1); };
  put(0x200000, 32); put(0, 10); put(0, 3); put(0, 4); put(1, 4); put(2, 4); put(page, 3); put(0x20202020, 32);
  while (bits.length % 8) bits.push(0);
  const body = []; for (let i = 0; i < bits.length; i += 8) body.push(bits.slice(i, i + 8).reduce((a, b, k) => a | (b << k), 0));
  return Uint8Array.from([0x9c, action, 8 + body.length, 0, guid, 0, 0, 0, ...body]);
}
const fresh = (deps = {}) => { const sent = []; const npc = createRealmNpc({ send: (b) => sent.push([...b]), classOf: () => deps.classId ?? CHARSI, ...deps }); return { npc, sent }; };
const ops = (sent) => sent.map((b) => b[0]);

test('an NPC with a menu opens with 0x2F and shows the retail rows', () => {
  const { npc, sent } = fresh();
  npc.handle(info(GUID, []));
  assert.deepEqual(sent, [[0x2f, 1, 0, 0, 0, GUID, 0, 0, 0]]);
  const v = npc.view();
  assert.equal(v.mode, 'menu');
  assert.deepEqual(v.entries.map((e) => e.action), ['talk', 'trade', 'cancel']);
});

test('an unknown class with no line opens nothing', () => {
  const { npc, sent } = fresh({ classId: 9999 });
  npc.handle(info(GUID, [[2, 5]]));
  assert.equal(npc.isOpen(), false); assert.deepEqual(sent, []);
});

test('a spoken line is acknowledged with 0x31 and the menu returns', () => {
  const { npc, sent } = fresh({ classId: KASHYA });
  npc.handle(info(GUID, [[0, 0x300], [0, 0x100]]));
  assert.equal(npc.view().mode, 'speech');
  assert.equal(npc.view().speech.stringId, 0x100);   // sorted by string id
  npc.dismissSpeech();
  npc.dismissSpeech();
  assert.deepEqual(sent.slice(2, 4), [[0x31, GUID, 0, 0, 0, 0, 1, 0, 0], [0x31, GUID, 0, 0, 0, 0, 3, 0, 0]]);
  // Kashya is a hire hirer: the menu build at conversation open asks for the offers, ahead of the lines
  assert.deepEqual(sent[1], [0x38, 3, 0, 0, 0, GUID, 0, 0, 0, 255, 255, 255, 255]);
  assert.equal(sent.length, 4);
  assert.equal(npc.view().mode, 'menu');
  assert.equal(npc.dismissSpeech(), false);
});

test('trade opens at the menu click, buys with the item mode and the multi flag, and the stock fills later', () => {
  const { npc, sent } = fresh();
  npc.handle(info(GUID, []));
  assert.equal(npc.buy(7), false);
  assert.ok(npc.choose(npc.view().entries[1]));
  assert.deepEqual(sent.at(-1), [0x38, 1, 0, 0, 0, GUID, 0, 0, 0, 0, 0, 0, 0]);   // the trade form carries 0 in the player field
  assert.equal(npc.shopOpen(), true);
  npc.handle(stockPacket(11, 7, 5));
  assert.equal(npc.view().stock[0].tab, 4);
  assert.ok(npc.buy(7, true));
  assert.deepEqual(sent.at(-1).slice(0, 13), [0x32, GUID, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0x80]);
  assert.equal(npc.buy(8), false);
  npc.handle(stockPacket(12, 7, 5));
  assert.equal(npc.view().stock.length, 0);
  const t = new Uint8Array(15); t[0] = 0x2a; t[2] = 9;
  npc.handle(t);
  assert.ok(npc.takeResult().refusal > 0);
  assert.equal(npc.takeResult(), null);
});

test('sell, repair and repair-all go to the open shop only', () => {
  const { npc, sent } = fresh();
  assert.equal(npc.sell(3), false);
  npc.handle(info(GUID, [])); npc.choose({ action: 'trade' }); npc.handle(stockPacket(11, 7, 1));
  assert.ok(npc.sell(3, 4)); assert.ok(npc.repair(3)); assert.ok(npc.repairAll());
  assert.deepEqual(ops(sent).slice(-3), [0x33, 0x35, 0x35]);
  assert.equal(sent.at(-1)[16], 0x80);
});

test('closing ends the conversation with 0x30 and drops the stock', () => {
  const { npc, sent } = fresh();
  npc.handle(info(GUID, [])); npc.choose({ action: 'trade' }); npc.handle(stockPacket(11, 7, 1));
  assert.ok(npc.end());
  assert.deepEqual(sent.at(-1), [0x30, 1, 0, 0, 0, GUID, 0, 0, 0]);
  assert.equal(npc.view(), null); assert.equal(npc.end(), false);
  npc.handle(stockPacket(11, 8, 1));
  assert.equal(npc.view(), null);
});

test('cancel closes, gamble and identify and a dead mercenary pick their packets', () => {
  let r = fresh({ classId: 147 });   // Gheed: talk, trade, gamble
  r.npc.handle(info(GUID, []));
  r.npc.choose(r.npc.view().entries.find((e) => e.action === 'gamble'));
  assert.deepEqual(r.sent.at(-1), [0x38, 2, 0, 0, 0, GUID, 0, 0, 0, 0, 0, 0, 0]);   // the gamble form carries 0 too
  r = fresh({ classId: 246 });       // Deckard Cain: talk, identify
  r.npc.handle(info(GUID, []));
  r.npc.choose({ action: 'identify' });
  assert.deepEqual(r.sent.slice(1).map((b) => b[0]), [0x34, 0x30]);
  r = fresh({ classId: 198, deadMerc: () => true });   // Greiz: the menu build asks for the offers (0x38) even when the merc is dead
  r.npc.handle(info(GUID, []));
  r.npc.choose({ action: 'hire' });
  assert.deepEqual(r.sent.slice(1).map((b) => b[0]), [0x38, 0x62, 0x30]);
  r = fresh({ classId: 198 });
  r.npc.handle(info(GUID, []));
  r.npc.choose({ action: 'hire' });
  assert.ok(r.npc.hire(3));
  assert.deepEqual(r.sent.slice(1).map((b) => b[0]), [0x38, 0x36]);   // the conversation waits for the result
  const done = new Uint8Array(15); done[0] = 0x2a; done[2] = 0;
  r.npc.handle(done);
  assert.deepEqual(r.sent.slice(1).map((b) => b[0]), [0x38, 0x36, 0x30]);
  r = fresh();
  r.npc.handle(info(GUID, [])); r.npc.choose({ action: 'cancel' });
  assert.equal(r.npc.isOpen(), false);
});

test('talking to a different NPC ends the open conversation', () => {
  const { npc, sent } = fresh();
  npc.handle(info(GUID, []));
  npc.talkSent(GUID); assert.equal(npc.isOpen(), true);
  npc.talkSent(0x66); assert.equal(npc.isOpen(), false);
  assert.equal(sent.at(-1)[0], 0x30);
});

test('a gamble buy carries the gamble flag and the displayed cost from the injected price', () => {
  const r = fresh({ classId: 147, transactionCost: (item, kind) => (kind === 'buy' ? 777 : 0) });
  r.npc.handle(info(GUID, []));
  r.npc.choose(r.npc.view().entries.find((e) => e.action === 'gamble'));
  r.npc.handle(stockPacket(11, 7, 5));
  assert.ok(r.npc.buy(7));
  assert.deepEqual(r.sent.at(-1).slice(9, 17), [2, 0, 0, 0, 0x09, 0x03, 0, 0]);
});

const offerPacket = (mercName, seed) => Uint8Array.of(0x4e, mercName & 255, mercName >> 8, seed & 255, (seed >> 8) & 255, (seed >> 16) & 255, seed >>> 24);

test('hire offers: 0x4F restarts the list, each 0x4E appends one; unknown stats stay null, never 0', () => {
  const r = fresh({ classId: 198, hireStats: (m) => (m === 5 ? { name: 'Ahmad', level: 12, life: 40, def: 9, cost: 310, skill: 'Fire' } : null) });
  r.npc.handle(info(GUID, []));                       // the menu build: 0x38 is sent here
  r.npc.handle(offerPacket(5, 0x1234)); r.npc.handle(offerPacket(9, 0x99));
  assert.deepEqual(r.npc.view().offers.map((o) => [o.mercName, o.name, o.level]), [[5, 'Ahmad', 12], [9, '', null]]);
  r.npc.handle(Uint8Array.of(0x4f, 0));                // a new head: the list starts again
  assert.equal(r.npc.view().offers.length, 0);
});

test('the hire list request (0x38 action 3) goes at the menu build of the four hirers, not for other NPCs', () => {
  const hirer = fresh({ classId: 252 });
  hirer.npc.handle(info(GUID, []));
  assert.deepEqual(hirer.sent.map((b) => b[0]), [0x2f, 0x38]);
  assert.equal(hirer.sent[1][1], 3);
  const other = fresh({ classId: 147 });              // Gheed
  other.npc.handle(info(GUID, []));
  assert.equal(other.sent.some((b) => b[0] === 0x38 && b[1] === 3), false);
});

test('choosing Hire sends nothing: the window lists the offers already received', () => {
  const r = fresh({ classId: 198 });
  r.npc.handle(info(GUID, []));
  const before = r.sent.length;
  r.npc.choose({ action: 'hire' });
  assert.equal(r.sent.length, before);
  assert.equal(r.npc.view().mode, 'hire');
});

test('the price is unknown (null) until a seam supplies one', () => {
  const r = fresh({ classId: 147 });                 // no transactionCost given
  r.npc.handle(info(GUID, []));
  r.npc.choose(r.npc.view().entries.find((e) => e.action === 'trade'));
  r.npc.handle(stockPacket(11, 7, 5));
  assert.equal(r.npc.priceOf(7), null);
});

test('the hire request carries the hero unit id; trade and gamble carry 0', () => {
  const hirer = fresh({ classId: 252, playerGuid: () => 0x01020304 });
  hirer.npc.handle(info(GUID, []));
  assert.deepEqual(hirer.sent[1], [0x38, 3, 0, 0, 0, GUID, 0, 0, 0, 4, 3, 2, 1]);
});

test('a boat row sends the travel 0x38 (action 0) for the clicked NPC and closes without 0x30', () => {
  const r = fresh({ classId: 155, questDone: () => true });   // Warriv (Act I): Go East once Andariel is done
  r.npc.handle(info(GUID, []));
  const row = r.npc.view().entries.find((e) => e.stringId === 3382);
  assert.ok(row);
  assert.equal(r.npc.choose(row), true);
  assert.deepEqual(r.sent.at(-1), [0x38, 0, 0, 0, 0, GUID, 0, 0, 0, 0, 0, 0, 0]);
  assert.equal(r.npc.isOpen(), false);
});
