import test from 'node:test';
import assert from 'node:assert/strict';
import { tradeEventLine, newTrade, applyTradePacket, tradeRequestPacket, tradeButtonPacket, tradeAnswer, tradeMouseDown, tradeMouseUp,
  tradeSetGold, tradeItemAction, tradeTick, tradeMarks, tradeButtonAt, tradeTip, tradeGoldOpen, tradeGoldClose, takeTradeEvents,
  TRADE_BTN, TRADE_SOUND, TRADE_SPEECH_NO_ROOM, TRADE_STATE as S } from './bnet-game-trade.mjs';

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

test('a request during a trade is cancelled and the reply awaited; a busy player just cancels', () => {
  let t = newTrade(); applyTradePacket(t, ui(0));
  assert.deepEqual(hex(applyTradePacket(t, ui(1), { now: 500 })), [CANCEL]);        // already in a trade
  assert.deepEqual([t.state, t.waitSince, t.box], [S.WAIT, 500, 'asking']);
  assert.deepEqual(hex(applyTradePacket(t, ui(0), { now: 900 })), [CANCEL]);        // again: the first wait keeps its clock
  assert.deepEqual([t.state, t.waitSince], [S.WAIT, 500]);
  assert.deepEqual(hex(applyTradePacket(t, ui(12))), []);                            // the server's close ends it
  assert.deepEqual([t.state, t.ui, t.box], [S.NONE, false, null]);
  for (const env of [{ cursorItem: true }, { busy: true }]) {
    t = newTrade();
    assert.deepEqual(hex(applyTradePacket(t, ui(1), env)), [CANCEL]);
    assert.deepEqual([t.state, t.box, t.ui, t.waitSince], [S.NONE, null, false, null]);
  }
  assert.deepEqual(hex(applyTradePacket(t, ui(6))), []);              // an open with no trade begun is ignored
  assert.deepEqual(hex(applyTradePacket(t, ui(5))), []);
});

test('an open or an accept that does not fit the state cancels and waits', () => {
  let t = newTrade(); applyTradePacket(t, ui(1));                     // asked, not yet answered
  assert.deepEqual(hex(applyTradePacket(t, ui(6), { now: 70 })), [[0x4f, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00], CANCEL]);
  assert.deepEqual([t.state, t.waitSince, t.panel], [S.WAIT, 70, true]);
  t = newTrade(); applyTradePacket(t, ui(0));                         // asking: the partner cannot have accepted yet
  assert.deepEqual(hex(applyTradePacket(t, ui(5), { now: 80 })), [CANCEL]);
  assert.deepEqual([t.state, t.waitSince], [S.WAIT, 80]);
});

test('leaving town or dying sends the screen cancel, and the close step a second when a trade was live', () => {
  let t = newTrade(); applyTradePacket(t, ui(1)); tradeAnswer(t, true); applyTradePacket(t, ui(6));
  assert.deepEqual(hex(tradeTick(t, 0, { inTown: false }).send), [CANCEL, CANCEL]);
  assert.deepEqual([t.state, t.ui, t.panel, t.blocked], [S.NONE, false, false, false]);
  assert.deepEqual(hex(tradeTick(t, 16, { inTown: false }).send), []);  // once: the screen is gone
  t = newTrade(); applyTradePacket(t, ui(1));
  assert.deepEqual(hex(tradeTick(t, 0, { dead: true }).send), [CANCEL, CANCEL]);
  assert.deepEqual([t.box, t.state, t.ui], [null, S.NONE, false]);
  // A reply already awaited (my own cancel press): only the screen's cancel goes.
  t = newTrade(); applyTradePacket(t, ui(0)); tradeAnswer(t, false, 10);
  assert.equal(t.state, S.WAIT);
  assert.deepEqual(tradeTick(t, 20, { dead: true }), { send: [Uint8Array.of(0x4f, 2, 0, 0, 0, 0, 0)], leave: false });
  assert.deepEqual([t.state, t.ui, t.waitSince], [S.NONE, false, null]);
  // ... and one that had already run out ends the game on that frame.
  t = newTrade(); applyTradePacket(t, ui(0)); tradeAnswer(t, false, 10);
  assert.equal(tradeTick(t, 30011, { inTown: false }).leave, true);
});

test('a trade that does not fit: the hero short of room speaks, nothing else changes', () => {
  const t = newTrade(); applyTradePacket(t, ui(1)); tradeAnswer(t, true); applyTradePacket(t, ui(6));
  const name = new Uint8Array(21); name[0] = 0x78; name[1] = 0x42; name[17] = 9;
  applyTradePacket(t, name); takeTradeEvents(t);
  assert.deepEqual(hex(applyTradePacket(t, ui(9))), []);
  assert.deepEqual(takeTradeEvents(t), [{ speech: TRADE_SPEECH_NO_ROOM, who: 'own', guid: 0 }]);
  assert.deepEqual(hex(applyTradePacket(t, ui(10))), []);
  assert.deepEqual(takeTradeEvents(t), [{ speech: 0x17, who: 'partner', guid: 9 }]);
  assert.deepEqual([t.state, t.ui, t.panel], [S.OPEN, true, true]);
  assert.deepEqual(takeTradeEvents(t), []);
});

test('the hover texts: three rectangles, edges included, only on the open panel with no gold box', () => {
  const t = newTrade();
  assert.equal(tradeTip(t, 239, 400), null);                          // no trade
  applyTradePacket(t, ui(1));
  assert.equal(tradeTip(t, 239, 400), null);                          // the prompt
  tradeAnswer(t, true); applyTradePacket(t, ui(6));
  assert.deepEqual(tradeTip(t, 221, 383), { string: 4131, x: 187, y: 382 });
  assert.deepEqual(tradeTip(t, 255, 417), { string: 4131, x: 187, y: 382 });
  assert.equal(tradeTip(t, 220, 400), null);
  assert.equal(tradeTip(t, 256, 400), null);
  assert.equal(tradeTip(t, 239, 418), null);
  assert.deepEqual(tradeTip(t, 273, 383), { string: 4130, x: 263, y: 382 });
  assert.deepEqual(tradeTip(t, 307, 417), { string: 4130, x: 263, y: 382 });
  assert.deepEqual(tradeTip(t, 290, 168), { string: 4131, x: 239, y: 167 });
  assert.deepEqual(tradeTip(t, 307, 202), { string: 4131, x: 239, y: 167 });
  assert.equal(tradeTip(t, 290, 167), null);
  assert.equal(tradeGoldOpen(t).open, true);
  assert.equal(tradeTip(t, 239, 400), null);                          // under the gold box
  tradeGoldClose(t);
  assert.notEqual(tradeTip(t, 239, 400), null);
});

test('the gold box: opening withdraws an accept, opens once and never over a held item; OK sends the offer', () => {
  const t = newTrade();
  assert.deepEqual(tradeGoldOpen(t), { open: false, send: [] });      // no panel
  applyTradePacket(t, ui(1)); tradeAnswer(t, true); applyTradePacket(t, ui(6));
  assert.deepEqual(tradeGoldOpen(t, { cursorItem: true }), { open: false, send: [] });
  applyTradePacket(t, ui(5));
  assert.equal(t.state, S.THEY_ACCEPTED);
  let r = tradeGoldOpen(t);
  assert.deepEqual([r.open, hex(r.send), t.state], [true, [[0x4f, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00]], S.OPEN]);
  assert.deepEqual(tradeGoldOpen(t), { open: false, send: [] });      // already up
  takeTradeEvents(t);
  assert.deepEqual(hex(tradeSetGold(t, 70000, 66000)), [[0x4f, 0x08, 0x00, 0x01, 0x00, 0xd0, 0x01]]);   // capped at the carried 66000
  assert.deepEqual([t.ownGold, t.goldBox], [66000, false]);
  assert.deepEqual(takeTradeEvents(t), [{ sound: TRADE_SOUND.GOLD }]);
  r = tradeGoldOpen(t);
  assert.equal(r.open, true);
  assert.deepEqual(hex(tradeSetGold(t, 0, 66000)), [[0x4f, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00]]);       // 0 is an offer too, with no sound
  assert.deepEqual(takeTradeEvents(t), []);
  tradeGoldOpen(t); tradeGoldClose(t);
  assert.equal(tradeGoldOpen(t).open, true);                          // a cancelled box can be opened again
  applyTradePacket(t, ui(12));
  assert.equal(t.goldBox, false);                                     // the screen closing takes the box with it
});

test('item actions: one per 250 ms; while a reply is awaited they are refused and the accept withdrawn', () => {
  const t = newTrade();
  assert.equal(tradeItemAction(t, 1000).allowed, true);
  assert.equal(tradeItemAction(t, 1250).allowed, false);              // exactly 250 is still too soon
  assert.equal(tradeItemAction(t, 1251).allowed, true);
  assert.equal(tradeItemAction(t, 1300).allowed, false);
  applyTradePacket(t, ui(1)); tradeAnswer(t, true); applyTradePacket(t, ui(6));
  assert.equal(tradeItemAction(t, 1301).allowed, true);               // the panel opening clears the clock
  tradeMouseDown(t, 'cancel'); tradeMouseUp(t, 'cancel', 2000);
  assert.equal(t.state, S.WAIT);
  let r = tradeItemAction(t, 2100);
  assert.deepEqual([r.allowed, hex(r.send), r.leave, t.state], [false, [[0x4f, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00]], false, S.OPEN]);
  tradeMouseDown(t, 'cancel'); tradeMouseUp(t, 'cancel', 3000);
  r = tradeItemAction(t, 33001);                                      // the wait had run out: the game ends, the bytes still go
  assert.deepEqual([r.allowed, hex(r.send), r.leave], [false, [[0x4f, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00]], true]);
});

test('sounds: the box buttons, a press on the panel, the partner accepting, both accepted', () => {
  const t = newTrade(); applyTradePacket(t, ui(1));
  tradeAnswer(t, true);
  assert.deepEqual(takeTradeEvents(t), [{ sound: TRADE_SOUND.BOX_BUTTON }]);
  applyTradePacket(t, ui(6));
  tradeMouseDown(t, null);
  assert.deepEqual(takeTradeEvents(t), []);
  tradeMouseDown(t, 'accept');
  assert.deepEqual(takeTradeEvents(t), [{ sound: TRADE_SOUND.PRESS }]);
  tradeMouseUp(t, null);
  applyTradePacket(t, ui(5));
  assert.deepEqual(takeTradeEvents(t), [{ sound: TRADE_SOUND.MARK }]);
  t.state = S.OWN_ACCEPTED; applyTradePacket(t, ui(5));
  assert.deepEqual(takeTradeEvents(t), [{ sound: TRADE_SOUND.BOTH_ACCEPTED }]);
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
