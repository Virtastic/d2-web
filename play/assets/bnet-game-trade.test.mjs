import test from 'node:test';
import assert from 'node:assert/strict';
import { tradeEventLine, newTrade, applyTradePacket, tradeRequestPacket, tradeButtonPacket, TRADE_BTN } from './bnet-game-trade.mjs';

const ev = (type, name = '') => { const b = new Uint8Array(40); b[0] = 0x5a; b[1] = type; for (let i = 0; i < name.length; i++) b[8 + i] = name.charCodeAt(i); return b; };
const lang = (id) => ({ 4044: ' is busy', 4047: 'wait', 4134: ' has items in his box.', 4135: 'You have items in your box.', 10808: 'hostile wait' })[id];

test('event lines for a blocked trade', () => {
  assert.equal(tradeEventLine(ev(8, 'Bob'), lang), 'Bob is busy');
  assert.equal(tradeEventLine(ev(9), lang), 'wait');
  assert.equal(tradeEventLine(ev(10, 'Bob'), lang), 'Bob has items in his box.');
  assert.equal(tradeEventLine(ev(10), lang), 'You have items in your box.');
  assert.equal(tradeEventLine(ev(16), lang), 'hostile wait');
  assert.equal(tradeEventLine(ev(2, 'Bob'), lang), null);
  assert.equal(tradeEventLine(ev(8).subarray(0, 10), lang), null);
});

test('trade state follows the server packets', () => {
  const t = newTrade();
  applyTradePacket(t, Uint8Array.from([0x77, 1]));
  assert.equal(t.asked, 1);
  const n = new Uint8Array(21); n[0] = 0x78; n.set([66, 111, 98], 1); n.set([0x78, 0x56, 0x34, 0x12], 17);
  applyTradePacket(t, n);
  assert.deepEqual([t.partner, t.partnerGuid], ['Bob', 0x12345678]);
  applyTradePacket(t, Uint8Array.from([0x77, 6]));
  assert.equal(t.open, true);
  applyTradePacket(t, Uint8Array.from([0x79, 1, 0xe8, 3, 0, 0]));
  assert.equal(t.gold, 1000);
  applyTradePacket(t, Uint8Array.from([0x77, 14]));
  assert.equal(t.theirCheck, true);
  applyTradePacket(t, Uint8Array.from([0x77, 15]));
  assert.equal(t.theirCheck, false);
  applyTradePacket(t, Uint8Array.from([0x77, 12]));
  assert.deepEqual(t, newTrade());
});

test('client trade packets', () => {
  assert.deepEqual([...tradeRequestPacket(0x01020304)], [0x13, 0, 0, 0, 0, 4, 3, 2, 1]);
  assert.deepEqual([...tradeButtonPacket(TRADE_BTN.ACCEPT)], [0x4f, 4, 0, 0, 0, 0, 0]);
  assert.deepEqual([...tradeButtonPacket(TRADE_BTN.SENDGOLD, 0x00012345)], [0x4f, 8, 0, 1, 0, 0x45, 0x23]);
});
