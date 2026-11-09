import test from 'node:test';
import assert from 'node:assert/strict';
import { npcPackets, NPC_ACTION, REPAIR_ALL_FLAG, parseNpcInfo, parseTransaction, parseShopItem, parseHireOffer } from './realm-npc-packets.mjs';

test('the client packets have the 1.13c lengths and little-endian fields', () => {
  const sizes = { 0x31: 9, 0x32: 17, 0x33: 17, 0x34: 5, 0x35: 17, 0x36: 9, 0x37: 5, 0x38: 13, 0x62: 5 };
  const made = [npcPackets.messageDone(1, 2), npcPackets.buy(1, 2, 3, 4), npcPackets.sell(1, 2), npcPackets.identifyAll(1),
    npcPackets.repair(1, 2), npcPackets.repairAll(1), npcPackets.hire(1, 2), npcPackets.identifyGambled(1),
    npcPackets.request(NPC_ACTION.TRADE, 1), npcPackets.resurrectMerc(1)];
  for (const p of made) assert.equal(p.length, sizes[p[0]], `0x${p[0].toString(16)}`);
  assert.deepEqual([...npcPackets.buy(0x01020304, 0x0a0b0c0d, 2, 0x1234, { gamble: true, multi: true })],
    [0x32, 4, 3, 2, 1, 0x0d, 0x0c, 0x0b, 0x0a, 2, 0, 2, 0x80, 0x34, 0x12, 0, 0]);
  assert.deepEqual([...npcPackets.repair(1, 2, 4, 9)], [0x35, 1, 0, 0, 0, 2, 0, 0, 0, 4, 0, 0, 0, 9, 0, 0, 0]);
  assert.deepEqual([...npcPackets.sell(7, 9, 4)], [0x33, 7, 0, 0, 0, 9, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0]);
  assert.equal(new DataView(npcPackets.repairAll(5).buffer).getUint32(13, true), REPAIR_ALL_FLAG);
  assert.deepEqual([...npcPackets.request(NPC_ACTION.GAMBLE, 3, 0)], [0x38, 2, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0]);
});

test('the NPC message list parses in wire order and clamps a short packet', () => {
  const p = new Uint8Array(40); p[0] = 0x27; p[1] = 1; p[2] = 9; p[6] = 2;
  p.set([0, 0, 0x34, 0x12, 2, 0, 0x78, 0x56], 8);
  assert.deepEqual(parseNpcInfo(p), { unitType: 1, guid: 9, messages: [{ menu: 0, stringId: 0x1234 }, { menu: 2, stringId: 0x5678 }] });
  const short = p.slice(0, 12);
  assert.equal(parseNpcInfo(short).messages.length, 1);
  assert.equal(parseNpcInfo(Uint8Array.of(0x28, 0, 0, 0, 0, 0, 0, 0)), null);
});

test('the transaction result and shop item parse', () => {
  const t = new Uint8Array(15); t[0] = 0x2a; t[2] = 9; t.set([5, 0, 0, 0], 7); t.set([0x10, 0x27, 0, 0], 11);
  assert.deepEqual(parseTransaction(t), { result: 9, guid: 5, gold: 10000 });
  const s = Uint8Array.of(0x9c, 11, 16, 0, 7, 0, 0, 0, 1, 2, 3);
  const item = parseShopItem(s);
  assert.equal(item.add, true); assert.equal(item.guid, 7); assert.deepEqual([...item.bits], [1, 2, 3]);
  assert.equal(parseShopItem(Uint8Array.of(0x9c, 12, 16, 0, 7, 0, 0, 0)).add, false);
  assert.equal(parseShopItem(Uint8Array.of(0x9c, 0, 16, 0, 7, 0, 0, 0)), null);
});

test('a hire offer (0x4E) parses its merc name id and seed; anything else is not an offer', () => {
  const p = Uint8Array.of(0x4e, 0x05, 0x01, 0x78, 0x56, 0x34, 0x12);
  assert.deepEqual(parseHireOffer(p), { mercName: 0x0105, seed: 0x12345678 });
  assert.equal(parseHireOffer(Uint8Array.of(0x4e, 1, 2)), null);
  assert.equal(parseHireOffer(Uint8Array.of(0x4f, 0)), null);
});
