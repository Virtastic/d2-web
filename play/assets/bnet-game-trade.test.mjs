import test from 'node:test';
import assert from 'node:assert/strict';
import { tradeEventLine, newTrade, applyTradePacket, tradeRequestPacket, tradeButtonPacket, tradeAnswer, tradeMouseDown, tradeMouseUp,
  tradeSetGold, tradeItemAction, tradeTick, tradeMarks, tradeButtonAt, TRADE_BTN, TRADE_STATE as S } from './bnet-game-trade.mjs';

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

test('client trade packets', () => {
  assert.deepEqual([...tradeRequestPacket(0x01020304)], [0x13, 0, 0, 0, 0, 4, 3, 2, 1]);
  assert.deepEqual([...tradeButtonPacket(TRADE_BTN.ACCEPT)], [0x4f, 4, 0, 0, 0, 0, 0]);
  assert.deepEqual([...tradeButtonPacket(TRADE_BTN.SENDGOLD, 0x00012345)], [0x4f, 8, 0, 1, 0, 0x45, 0x23]);
});


const ui = (action) => Uint8Array.of(0x77, action);
const partner = (name, guid) => { const b = new Uint8Array(21); b[0] = 0x78; for (let i = 0; i < name.length; i++) b[1 + i] = name.charCodeAt(i); new DataView(b.buffer).setUint32(17, guid, true); return b; };
const gold = (own, amount) => { const b = new Uint8Array(6); b[0] = 0x79; b[1] = own; new DataView(b.buffer).setUint32(2, amount, true); return b; };
const hex = (list) => list.map((b) => Array.from(b));
const CANCEL = [0x4f, 2, 0, 0, 0, 0, 0], PERFORM = [0x4f, 3, 0, 0, 0, 0, 0], UNACCEPT = [0x4f, 7, 0, 0, 0, 0, 0];
const click = (t, where, now = 0) => [...hex(tradeMouseDown(t, where)), ...hex(tradeMouseUp(t, where, now))];

test('a full trade as the asked player: prompt, open, gold, both accept, done', () => {
  const t = newTrade();
  assert.deepEqual(hex(applyTradePacket(t, ui(1))), []);
  assert.deepEqual([t.state, t.box, t.ui, t.panel], [S.ASKED, 'asked', true, false]);
  assert.deepEqual(hex(tradeAnswer(t, true)), [PERFORM]);
  assert.deepEqual([t.state, t.box], [S.OPEN, null]);
  // The server opens the screen and names the partner; the client answers the open with its (zero) gold.
  assert.deepEqual(hex(applyTradePacket(t, ui(6))), [[0x4f, 8, 0, 0, 0, 0, 0]]);
  assert.deepEqual(hex(applyTradePacket(t, partner('Bob', 0x12345678))), []);
  assert.deepEqual([t.state, t.panel, t.partner, t.partnerGuid], [S.OPEN, true, 'Bob', 0x12345678]);
  // My gold: the figure on the panel moves only on the server's echo... the offer itself is what the box sent.
  assert.deepEqual(hex(tradeSetGold(t, 70000, 100000)), [[0x4f, 8, 0, 1, 0, 0x70, 0x11]]);
  applyTradePacket(t, gold(1, 70000));
  assert.equal(t.ownGold, 70000);
  // The partner's gold arrives.
  assert.equal(t.theirGold, 0);
  applyTradePacket(t, gold(0, 1000));
  assert.equal(t.theirGold, 1000);
  // The partner accepts.
  assert.deepEqual(hex(applyTradePacket(t, ui(5))), []);
  assert.deepEqual([t.state, t.theirAccept, tradeMarks(t).theirs.frame], [S.THEY_ACCEPTED, true, 17]);
  // I accept: the button latches on the press and the packet leaves on the release over it, with my gold.
  assert.deepEqual(hex(tradeMouseDown(t, 'accept')), []);
  assert.deepEqual(tradeMarks(t).accept, { frame: 17, tint: 'green' });
  assert.deepEqual(hex(tradeMouseUp(t, 'accept')), [[0x4f, 4, 0, 1, 0, 0x70, 0x11]]);
  assert.equal(t.state, S.WAIT);
  // Done: the screen closes without a packet and no leave clock is armed.
  assert.deepEqual(hex(applyTradePacket(t, ui(13))), []);
  assert.deepEqual([t.state, t.ui, t.panel, t.done, t.waitSince], [S.NONE, false, false, true, null]);
  assert.deepEqual(tradeTick(t, 1e9), { send: [], leave: false });
});

test('a full trade as the asker: wait box, open, the accept is withdrawn when an offer changes', () => {
  const t = newTrade();
  assert.deepEqual(hex(applyTradePacket(t, ui(0))), []);
  assert.deepEqual([t.state, t.box], [S.ASKING, 'asking']);
  assert.deepEqual(hex(applyTradePacket(t, ui(6))), [[0x4f, 8, 0, 0, 0, 0, 0]]);
  assert.deepEqual([t.state, t.box, t.panel], [S.OPEN, null, true]);
  // I accept, then the partner changes the offer: the server resets both golds and reopens; my latch drops.
  assert.deepEqual(click(t, 'accept'), [[0x4f, 4, 0, 0, 0, 0, 0]]);
  assert.equal(t.latch, true);
  applyTradePacket(t, gold(0, 0)); applyTradePacket(t, gold(1, 0));
  assert.deepEqual(hex(applyTradePacket(t, ui(6))), [[0x4f, 8, 0, 0, 0, 0, 0]]);
  assert.deepEqual([t.state, t.latch, tradeMarks(t).accept.frame], [S.OPEN, false, 16]);
  // An item taken back holds the trade: the accept button is red until the server lets go.
  applyTradePacket(t, ui(14));
  assert.deepEqual(tradeMarks(t).accept, { frame: 16, tint: 'red' });
  applyTradePacket(t, ui(15));
  assert.deepEqual(tradeMarks(t).accept, { frame: 16, tint: null });
  // A second press on a latched accept withdraws it.
  tradeMouseDown(t, 'accept'); tradeMouseUp(t, null);
  assert.deepEqual(hex(tradeMouseDown(t, 'accept')), []);
  assert.deepEqual(click(t, 'accept').length, 1);
});

test('accept presses: a release off the button, a second press and an item on the cursor all withdraw', () => {
  const open = () => { const t = newTrade(); applyTradePacket(t, ui(0)); applyTradePacket(t, ui(6)); return t; };
  let t = open();
  tradeMouseDown(t, 'accept');
  assert.deepEqual(hex(tradeMouseUp(t, null)), [UNACCEPT]);          // dragged off: the latch drops
  assert.deepEqual([t.latch, t.state], [false, S.OPEN]);
  t = open(); click(t, 'accept');
  assert.equal(t.state, S.WAIT);
  assert.deepEqual(hex(tradeMouseDown(t, 'accept')), [UNACCEPT]);    // pressed again: withdrawn
  assert.deepEqual([t.latch, t.state], [false, S.OPEN]);
  t = open(); click(t, 'accept');
  assert.deepEqual(hex(tradeMouseDown(t, null, { cursorItem: true })), [UNACCEPT]);
  t = open(); click(t, 'accept');
  assert.deepEqual(hex(tradeMouseUp(t, 'ownGrid')), [UNACCEPT]);     // an item put down on my side
  t = open(); click(t, 'accept');
  assert.deepEqual(tradeItemAction(t).allowed, false);               // an item action while the reply is awaited
  assert.equal(t.state, S.OPEN);
  t = open(); applyTradePacket(t, ui(5));
  assert.deepEqual(hex(tradeSetGold(t, 5, 10)), [UNACCEPT]);         // the gold box with an accept standing
  assert.equal(t.ownGold, 0);
  t = open();
  assert.deepEqual(hex(tradeSetGold(t, 500, 120)), [[0x4f, 8, 0, 0, 0, 120, 0]]);   // capped at the gold carried
});

test('cancel: the button, the prompt, the wait box and the close button; the server closes; 30 s unanswered leaves', () => {
  const open = () => { const t = newTrade(); applyTradePacket(t, ui(1)); tradeAnswer(t, true); applyTradePacket(t, ui(6)); return t; };
  let t = open();
  assert.deepEqual(hex(tradeMouseDown(t, 'cancel')), []);
  assert.equal(tradeMarks(t).cancel.frame, 11);
  assert.deepEqual(hex(tradeMouseUp(t, 'cancel', 1000)), [CANCEL]);
  assert.deepEqual([t.state, t.waitSince, t.panel], [S.WAIT, 1000, true]);   // the screen stays until the server answers
  assert.deepEqual(hex(applyTradePacket(t, ui(12))), []);
  assert.deepEqual([t.state, t.ui, t.panel, t.done], [S.NONE, false, false, false]);
  // A cancel pressed and released elsewhere does nothing.
  t = open(); tradeMouseDown(t, 'cancel');
  assert.deepEqual(hex(tradeMouseUp(t, null)), []);
  assert.equal(t.state, S.OPEN);
  // The partner cancels: no packet from here.
  t = open();
  assert.deepEqual(hex(applyTradePacket(t, ui(12))), []);
  assert.equal(t.panel, false);
  // The prompt declined and the wait box cancelled.
  t = newTrade(); applyTradePacket(t, ui(1));
  assert.deepEqual(hex(tradeAnswer(t, false, 5)), [CANCEL]);
  assert.deepEqual([t.state, t.box, t.waitSince], [S.WAIT, null, 5]);
  t = newTrade(); applyTradePacket(t, ui(0));
  assert.deepEqual(hex(tradeAnswer(t, true, 5)), [CANCEL]);           // the wait box has only a cancel
  // The inventory's close button.
  t = open();
  assert.deepEqual(hex(tradeMouseUp(t, 'close', 7)), [CANCEL]);
  assert.equal(t.state, S.WAIT);
  assert.deepEqual(tradeTick(t, 30007), { send: [], leave: false });
  assert.deepEqual(tradeTick(t, 30008), { send: [], leave: true });
  assert.deepEqual([t.state, t.ui, t.panel], [S.NONE, false, false]);
});

test('requests while busy, and leaving town or dying, are answered with cancel', () => {
  let t = newTrade(); applyTradePacket(t, ui(0));
  assert.deepEqual(hex(applyTradePacket(t, ui(1))), [CANCEL]);        // already in a trade
  assert.equal(t.state, S.ASKING);
  t = newTrade();
  assert.deepEqual(hex(applyTradePacket(t, ui(1), { cursorItem: true })), [CANCEL]);
  assert.deepEqual([t.state, t.box], [S.NONE, null]);
  assert.deepEqual(hex(applyTradePacket(t, ui(6))), []);              // an open with no trade begun is ignored
  assert.deepEqual(hex(applyTradePacket(t, ui(5))), []);
  t = newTrade(); applyTradePacket(t, ui(1)); tradeAnswer(t, true); applyTradePacket(t, ui(6));
  assert.deepEqual(hex(tradeTick(t, 0, { inTown: false }).send), [CANCEL]);
  assert.deepEqual([t.state, t.ui, t.panel], [S.NONE, false, false]);
  t = newTrade(); applyTradePacket(t, ui(1));
  assert.deepEqual(hex(tradeTick(t, 0, { dead: true }).send), [CANCEL]);
  assert.equal(t.box, null);
});

test('the buttons are hit strictly inside their 32 px squares', () => {
  assert.equal(tradeButtonAt(224, 385), 'accept');
  assert.equal(tradeButtonAt(254, 415), 'accept');
  assert.equal(tradeButtonAt(223, 400), null);
  assert.equal(tradeButtonAt(255, 400), null);
  assert.equal(tradeButtonAt(276, 400), 'cancel');
  assert.equal(tradeButtonAt(306, 384), null);
  assert.equal(tradeButtonAt(265, 400), null);
});
