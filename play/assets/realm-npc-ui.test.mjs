import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmNpc } from './realm-npc-state.mjs';
import { createRealmNpcUi, vendorItemsFrom } from './realm-npc-ui.mjs';
import { SELL_MODE } from './realm-npc-packets.mjs';

const GUID = 0x55, KASHYA = 150, CHARSI = 154;
const info = (guid, msgs) => { const p = new Uint8Array(40); p[0] = 0x27; p[1] = 1; p[2] = guid; p[6] = msgs.length;
  msgs.forEach(([menu, id], i) => { p[8 + i * 4] = menu; p[10 + i * 4] = id & 255; p[11 + i * 4] = id >> 8; }); return p; };
function stockPacket(action, guid, page) {
  const bits = []; const put = (v, n) => { for (let i = 0; i < n; i++) bits.push(Math.floor(v / 2 ** i) & 1); };
  put(0x200000, 32); put(0, 10); put(0, 3); put(0, 4); put(1, 4); put(2, 4); put(page, 3); put(0x20202020, 32);
  while (bits.length % 8) bits.push(0);
  const body = []; for (let i = 0; i < bits.length; i += 8) body.push(bits.slice(i, i + 8).reduce((a, b, k) => a | (b << k), 0));
  return Uint8Array.from([0x9c, action, 8 + body.length, 0, guid, 0, 0, 0, ...body]);
}
const fresh = (deps = {}) => {
  const sent = [];
  const npc = createRealmNpc({ send: (b) => sent.push([...b]), classOf: () => deps.classId ?? CHARSI, ...deps });
  return { npc, sent, ui: createRealmNpcUi({ npc }) };
};
const ops = (sent) => sent.map((b) => b[0]);
// The shop opens the way the server opens it: the trade request, then the stock arrives (0x9C).
const openShop = (npc, ui) => { ui.choose(npc.view().entries.find((e) => e.action === "trade")); npc.handle(stockPacket(11, 0x78, 3)); };

test('a menu row chosen by the shell reaches the state machine: trade requests the stock with 0x38', () => {
  const { npc, sent, ui } = fresh();
  npc.handle(info(GUID, []));
  const trade = npc.view().entries.find((e) => e.action === 'trade');
  assert.equal(ui.choose(trade), true);
  assert.equal(npc.view().mode, 'trade');
  assert.equal(ops(sent).at(-1), 0x38);
});

test('cancel closes the conversation and sends 0x30', () => {
  const { npc, sent, ui } = fresh();
  npc.handle(info(GUID, []));
  ui.choose(npc.view().entries.find((e) => e.action === 'cancel'));
  assert.equal(npc.isOpen(), false);
  assert.deepEqual(ops(sent).slice(-1), [0x30]);
});

test('a speech line is dismissed with 0x31 and the menu returns', () => {
  const { npc, sent, ui } = fresh({ classId: KASHYA });
  npc.handle(info(GUID, [[0, 0x100]]));
  assert.equal(npc.view().mode, 'speech');
  assert.equal(ui.dismissSpeech(), true);
  assert.equal(npc.view().mode, 'menu');
  // the menu builder asks for the hire offers at conversation open, before the line is acknowledged
  assert.deepEqual(ops(sent).slice(1), [0x38, 0x31]);
});

test('Esc or walking away ends an open conversation once; a second end is a no-op', () => {
  const { npc, sent, ui } = fresh();
  npc.handle(info(GUID, []));
  assert.equal(ui.end(), true);
  assert.equal(ui.end(), false);
  assert.equal(sent.filter((b) => b[0] === 0x30).length, 1);
});

test('walking away ends the conversation only when the NPC has left the scene', () => {
  const { npc, ui } = fresh();
  npc.handle(info(GUID, []));
  assert.equal(ui.walkedAway(GUID, [{ guid: GUID, classId: CHARSI }]), false);
  assert.equal(npc.isOpen(), true);
  assert.equal(ui.walkedAway(GUID, [{ guid: 0x99, classId: CHARSI }]), true);
  assert.equal(npc.isOpen(), false);
});

test('a buy click on a stock offer sends 0x32 only while the shop is open', () => {
  const { npc, sent, ui } = fresh();
  npc.handle(info(GUID, []));
  ui.choose(npc.view().entries.find((e) => e.action === 'trade'));
  npc.handle(stockPacket(11, 0x77, 3));
  assert.equal(ui.buyAt(0x77), true);
  assert.equal(ops(sent).at(-1), 0x32);
  assert.equal(ui.buyAt(0x12345), false);
});

test('a carried item click sells in sell mode and repairs in repair mode, and nothing otherwise', () => {
  const { npc, sent, ui } = fresh();
  npc.handle(info(GUID, []));
  openShop(npc, ui);
  assert.equal(ui.carriedClick(0x31, npc.view()), false);
  ui.buttonRelease('sell', npc.view());
  assert.equal(npc.view().sellMode, true);
  assert.equal(ui.carriedClick(0x31, npc.view()), true);
  assert.equal(ops(sent).at(-1), 0x33);
  ui.buttonRelease('sell', npc.view());
  ui.buttonRelease('repair', npc.view());
  assert.equal(npc.view().repairMode, true);
  assert.equal(npc.view().sellMode, false);
  assert.equal(ui.carriedClick(0x31, npc.view()), true);
  assert.equal(ops(sent).at(-1), 0x35);
});

test('repairAll and exit are the release actions of their buttons', () => {
  const { npc, sent, ui } = fresh();
  npc.handle(info(GUID, []));
  openShop(npc, ui);
  const before = sent.length;
  assert.equal(ui.buttonRelease('repairAll', npc.view()), true);
  assert.equal(sent.length, before + 1);
  assert.equal(ui.buttonRelease('exit', npc.view()), true);
  assert.equal(npc.isOpen(), false);
});

test('a button with no shop action (buy, blank) does nothing on release', () => {
  const { npc, ui } = fresh();
  npc.handle(info(GUID, []));
  ui.choose(npc.view().entries.find((e) => e.action === 'trade'));
  assert.equal(ui.buttonRelease('buy', npc.view()), false);
  assert.equal(ui.buttonRelease('blank', npc.view()), false);
  assert.equal(npc.isOpen(), true);
});

test('vendorItemsFrom maps stock to the vendor panel records and drops unknown codes', () => {
  const stock = [{ guid: 0x10, bits: new Uint8Array(), tab: 3, x: 2, y: 4 }];
  const table = { byCode: () => null };
  assert.deepEqual(vendorItemsFrom(stock, table), []);
  assert.deepEqual(vendorItemsFrom(null, table), []);
});

test('vendorItemsFrom reads grid, page and icon from the header and the item table', () => {
  // A compact item header on shop page 3; the stock packet's code is 0x20202020 (the table key below).
  const packet = stockPacket(11, 0x77, 3);
  const bits = packet.subarray(8);
  const table = { byCode: (code) => (code === 0x20202020 ? { invFile: 'inv.dc6', invWidth: 1, invHeight: 2 } : null) };
  const [item] = vendorItemsFrom([{ guid: 0x77, bits, tab: 3, x: 1, y: 2 }], table);
  assert.equal(item.invFile, 'inv.dc6');
  assert.equal(item.invPage, 3);
  assert.equal(item.gridX, 1);
  assert.equal(item.gridY, 2);
  assert.equal(item.invHeight, 2);
});

test('SELL_MODE values are the wire values the shell passes through', () => {
  assert.deepEqual(SELL_MODE, { STORED: 0, CURSOR: 4 });
});

test('a carried-item click sells or repairs with the item mode, price and durability it was given', () => {
  const { npc, sent, ui } = fresh();
  npc.handle(info(GUID, []));
  openShop(npc, ui);
  assert.equal(ui.carriedClick(9, { sellMode: true }, { mode: 4, cost: 50 }), true);
  assert.deepEqual(sent.at(-1).slice(9, 17), [4, 0, 0, 0, 50, 0, 0, 0]);
  assert.equal(ui.carriedClick(9, { repairMode: true }, { mode: 1, durability: 30 }), true);
  assert.deepEqual(sent.at(-1).slice(9, 17), [1, 0, 0, 0, 30, 0, 0, 0]);
  assert.equal(ui.carriedClick(9, {}, {}), false);
});
