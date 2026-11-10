import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmTrade, tradePoint, TRADE_BOX } from './realm-trade.mjs';
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
  assert.equal(TRADE_BOX.centre, 400);
});
