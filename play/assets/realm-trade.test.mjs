import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmTrade, tradePoint, tradeBoxLayout, TRADE_BOX } from './realm-trade.mjs';
import { TRADE_STATE as S } from './bnet-game-trade.mjs';

const ui = (action) => Uint8Array.of(0x77, action);
const gold = (own, amount) => { const b = new Uint8Array(6); b[0] = 0x79; b[1] = own; new DataView(b.buffer).setUint32(2, amount, true); return b; };
const STR = { 4042: 'agree to trade', 4043: 'asking to trade', 3400: 'cancel', 3401: 'ok', 2215: 'Gold' };
function harness() {
  const h = { sent: [], opened: 0, closed: 0, left: 0, time: 0 };
  h.trade = createRealmTrade({ send: (b) => h.sent.push(Array.from(b)), now: () => h.time, string: (i) => STR[i], carried: () => 5000,
    onOpen: () => h.opened++, onClose: () => h.closed++, onLeave: () => h.left++ });
  h.take = () => h.sent.splice(0);
  return h;
}
// A point of the 640x480 layout on the 800x600 canvas.
const at = (x, y) => [x + 80, y + 60, 800, 600];
const ACCEPT = at(239, 400), CANCEL = at(291, 400);

test('the layout point is the 640x480 space centred on the canvas', () => {
  assert.deepEqual(tradePoint(80, 60, 800, 600), { x: 0, y: 0 });
  assert.deepEqual(tradePoint(319, 460, 800, 600), { x: 239, y: 400 });
});

test('a full trade through the screen: the prompt, both offers, both accepts, done', () => {
  const h = harness(), t = h.trade;
  t.feed(ui(1));
  assert.deepEqual([t.box, t.open, h.opened], ['asked', false, 0]);
  assert.deepEqual(t.boxLines().map((l) => l.text), ['agree to trade', 'ok', 'cancel']);
  // A press off the buttons is swallowed and answers nothing.
  assert.equal(t.boxPress(10, 10, 800, 600), true);
  assert.deepEqual(h.take(), []);
  const ok = t.boxLines()[1];
  assert.equal(t.boxPress(ok.x + 1, ok.y - 2, 800, 600), true);
  assert.deepEqual(h.take(), [[0x4f, 3, 0, 0, 0, 0, 0]]);
  assert.equal(t.box, null);
  t.feed(ui(6));
  assert.deepEqual(h.take(), [[0x4f, 8, 0, 0, 0, 0, 0]]);
  assert.deepEqual([t.open, h.opened], [true, 1]);
  // Gold through the coin and the box: capped at what is carried; the panel's figure waits for the server.
  t.goldPress(); t.setGold(9000);
  assert.deepEqual(h.take(), [[0x4f, 8, 0, 0, 0, 0x88, 0x13]]);
  t.feed(gold(1, 5000)); t.feed(gold(0, 250));
  assert.deepEqual([t.state.ownGold, t.state.theirGold], [5000, 250]);
  t.feed(ui(5));
  assert.equal(t.state.theirAccept, true);
  t.press(...ACCEPT); t.release(...ACCEPT);
  assert.deepEqual(h.take(), [[0x4f, 4, 0, 0, 0, 0x88, 0x13]]);
  assert.equal(t.itemAction(), false);                      // an item action now withdraws the accept instead
  assert.deepEqual(h.take(), [[0x4f, 7, 0, 0, 0, 0, 0]]);
  t.press(...ACCEPT); t.release(...ACCEPT);                 // the latch is still set: this press drops it
  assert.deepEqual(h.take(), [[0x4f, 7, 0, 0, 0, 0, 0]]);
  t.press(...ACCEPT); t.release(...ACCEPT);
  assert.deepEqual(h.take(), [[0x4f, 4, 0, 0, 0, 0x88, 0x13]]);
  t.feed(ui(13));
  assert.deepEqual([t.open, t.state.state, t.state.done, h.closed, h.left], [false, S.NONE, true, 1, 0]);
  assert.deepEqual(h.take(), []);
  assert.equal(t.itemAction(), true);
});

test('a cancelled trade: the button, the reply, and the leave when no reply comes', () => {
  let h = harness(), t = h.trade;
  t.feed(ui(0));
  assert.deepEqual(t.boxLines().map((l) => l.text), ['asking to trade', 'cancel']);
  t.feed(ui(6)); h.take();
  h.time = 100; t.press(...CANCEL); t.release(...CANCEL);
  assert.deepEqual(h.take(), [[0x4f, 2, 0, 0, 0, 0, 0]]);
  assert.deepEqual([t.open, t.state.state, h.closed], [true, S.WAIT, 0]);
  t.closed();                                               // the inventory going away adds no second cancel
  assert.deepEqual(h.take(), []);
  t.feed(ui(12));
  assert.deepEqual([t.open, t.state.state, h.closed, h.left], [false, S.NONE, 1, 0]);

  h = harness(); t = h.trade;
  t.feed(ui(0)); t.feed(ui(6)); h.take();
  t.closed();                                               // Escape / the inventory's close button
  assert.deepEqual(h.take(), [[0x4f, 2, 0, 0, 0, 0, 0]]);
  h.time = 30000; t.tick({ inTown: true });
  assert.equal(h.left, 0);
  h.time = 30001; t.tick({ inTown: true });
  assert.deepEqual([h.left, t.open, h.closed], [1, false, 1]);

  h = harness(); t = h.trade;
  t.feed(ui(1));
  const no = t.boxLines()[2];
  t.boxPress(no.x + 1, no.y - 2, 800, 600);
  assert.deepEqual(h.take(), [[0x4f, 2, 0, 0, 0, 0, 0]]);
  assert.deepEqual([t.box, t.state.state], [null, S.WAIT]);
  t.feed(ui(12));
  assert.deepEqual([t.state.state, h.closed], [S.NONE, 1]);
});

test('a release over my grid counts as an item put down', () => {
  const h = harness(), t = h.trade;
  t.feed(ui(0)); t.feed(ui(6)); h.take();
  t.setOwnGrid({ gridLeft: 100, gridTop: 315, columns: 10, rows: 4, boxW: 29, boxH: 29 });
  t.press(...ACCEPT); t.release(...ACCEPT); h.take();
  t.release(120, 330, 800, 600);
  assert.deepEqual(h.take(), [[0x4f, 7, 0, 0, 0, 0, 0]]);
  assert.equal(t.state.latch, false);
});

test('the request box: centred on (320, 100), 20 wider than its widest line, 15 px a line', () => {
  assert.deepEqual([TRADE_BOX.centre, TRADE_BOX.anchor], [320, 100]);
  const box = tradeBoxLayout([112, 16, 48]);
  assert.deepEqual([box.x, box.y, box.w, box.h], [254, 79, 132, 60]);
  assert.deepEqual(box.lines, [{ x: 265, y: 94, w: 112 }, { x: 313, y: 109, w: 16 }, { x: 297, y: 124, w: 48 }]);
  const odd = tradeBoxLayout([101, 7]);                                // odd widths round as the client's integer halves do
  assert.deepEqual([odd.x, odd.w, odd.lines[1].x], [260, 121, 318]);
  // The box does not move with the 640x480 layout: its buttons are hit at the same screen points.
  const h = harness(), t = h.trade;
  t.feed(ui(0));
  const cancel = t.boxLines()[1];
  assert.deepEqual([cancel.y, cancel.x], [109, 320 - 34 + Math.trunc((68 - 48 + 1) / 2) + 1]);   // 'asking to trade' is 15 chars of 8, 'cancel' 6
  assert.equal(t.boxPress(cancel.x, cancel.y - 15, 800, 600), true);  // the row above the line is outside it
  assert.equal(t.box, 'asking');
  assert.equal(t.boxPress(cancel.x, cancel.y, 800, 600), true);
  assert.deepEqual([t.box, h.take()], [null, [[0x4f, 2, 0, 0, 0, 0, 0]]]);
});

test('hover texts come from the string table, left-aligned at the screen points the client uses', () => {
  const h = harness(), t = h.trade;
  STR[4130] = 'cancel trade'; STR[4131] = 'accept trade';
  assert.equal(t.tip(...at(239, 400)), null);
  t.feed(ui(1)); t.answer(true); t.feed(ui(6));
  assert.deepEqual(t.tip(...at(239, 400)), { text: 'accept trade', x: 267, y: 442, left: true });
  assert.deepEqual(t.tip(...at(291, 400)), { text: 'cancel trade', x: 343, y: 442, left: true });
  assert.deepEqual(t.tip(...at(291, 185)), { text: 'accept trade', x: 319, y: 227, left: true });
  assert.equal(t.tip(...at(200, 300)), null);
  assert.equal(t.goldOpen(), true);
  assert.equal(t.tip(...at(239, 400)), null);
  t.goldClosed();
  assert.notEqual(t.tip(...at(239, 400)), null);
});

test('the gold box opening withdraws the accept on the wire; a second opening and a held item are refused', () => {
  const h = harness(), t = h.trade;
  assert.equal(t.goldOpen(), false);
  t.feed(ui(1)); t.answer(true); t.feed(ui(6)); h.take();
  assert.equal(t.goldOpen({ cursorItem: true }), false);
  assert.deepEqual(h.take(), []);
  assert.equal(t.goldOpen(), true);
  assert.deepEqual(h.take(), [[0x4f, 7, 0, 0, 0, 0, 0]]);
  assert.equal(t.goldOpen(), false);
  t.setGold(123);
  assert.deepEqual(h.take(), [[0x4f, 8, 0, 0, 0, 123, 0]]);
  assert.equal(t.goldOpen(), true);
});

test('what is heard: screen sounds by row, and the no-room line in the voice of the hero short of room', () => {
  const heard = [], sent = [];
  const t = createRealmTrade({ send: (b) => sent.push(Array.from(b)), string: (i) => STR[i], onSound: (row) => heard.push(row),
    classOf: (who, guid) => (who === 'own' ? 4 : guid === 7 ? 1 : null) });
  t.feed(ui(1)); t.answer(true);
  assert.deepEqual(heard.splice(0), [2]);
  t.feed(ui(6));
  const name = new Uint8Array(21); name[0] = 0x78; name[1] = 0x42; name[17] = 7;
  t.feed(name);
  t.feed(ui(9));
  assert.deepEqual(heard.splice(0), [3012]);                           // the barbarian's own line
  t.feed(ui(10));
  assert.deepEqual(heard.splice(0), [3246]);                           // the sorceress partner's
  name[17] = 8; t.feed(name); t.feed(ui(10));
  assert.deepEqual(heard.splice(0), []);                               // a partner not in view says nothing
  t.press(...at(239, 400));
  assert.deepEqual(heard.splice(0), [4]);
  assert.deepEqual([t.state.state, t.open], [S.OPEN, true]);
});

test('a request while the chat line or another box is up is refused with a bare cancel', () => {
  const h = harness(), t = h.trade;
  t.feed(ui(1), { busy: true });
  assert.deepEqual([h.take(), t.box, t.state.state], [[[0x4f, 2, 0, 0, 0, 0, 0]], null, S.NONE]);
  h.time = 1234; t.feed(ui(0)); t.feed(ui(1));                         // asked while asking: cancel, and the wait starts now
  assert.deepEqual([h.take(), t.state.state, t.state.waitSince], [[[0x4f, 2, 0, 0, 0, 0, 0]], S.WAIT, 1234]);
});
