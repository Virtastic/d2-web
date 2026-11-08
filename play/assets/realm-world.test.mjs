import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmWorld, decodeItemHeader, PAGE } from './realm-world.mjs';
import { createItemTable, createObjectTable, createInventoryTable, createBeltTable } from './realm-tables.mjs';
import { realmViewSnapshot, realmFrameReads } from './realm-client-view.mjs';

const enc = (text) => new TextEncoder().encode(text);
const files = {
  'data\\global\\excel\\weapons.txt': 'name\tcode\ttype\tflippyfile\tinvfile\tinvwidth\tinvheight\twclass\t2handedwclass\t2handed\nHand Axe\thax\taxe\tflpax\tinvax\t1\t3\t1hs\t\t0\nWar Sword\twsd\tswor\tflpws\tinvws\t1\t3\t1hs\t\t0\nGreat Axe\tgax\taxe\tflpga\tinvga\t2\t4\t2hs\t2ht\t1\n',
  'data\\global\\excel\\armor.txt': 'name\tcode\ttype\tflippyfile\tinvfile\tinvwidth\tinvheight\tbelt\nCap\tcap\thelm\tflpcap\tinvcap\t2\t2\t\nSash\tlbl\tbelt\tflplbl\tinvlbl\t2\t1\t0\n',
  'data\\global\\excel\\misc.txt': 'name\tcode\ttype\tflippyfile\tinvfile\tinvwidth\tinvheight\nGold\tgld\tgold\tflpgld\tinvgld\t1\t1\nHealing Potion\thp1\thpot\tflphp1\tinvhp1\t1\t1\n',
  'data\\global\\excel\\objects.txt': 'Name\tId\tToken\nBarrel\t0\tBA\nChest\t5\tCH\n',
  'data\\global\\excel\\inventory.txt': 'class\tinvleft\tinvright\tinvtop\tinvbottom\tgridx\tgridy\tgridleft\tgridright\tgridtop\tgridbottom\tgridboxwidth\tgridboxheight\trarmleft\nAmazon\t1\t2\t3\t4\t10\t4\t5\t6\t7\t8\t29\t29\t9\n',
  'data\\global\\excel\\belts.txt': 'name\tnumboxes\tbox1left\tbox1right\tbox1top\tbox1bottom\tbox2left\tbox2right\tbox2top\tbox2bottom\nA\t4\t0\t10\t0\t10\t10\t20\t0\t10\nB\t4\t0\t10\t0\t10\t10\t20\t0\t10\nC\t0\n',
};
const read = (path) => { const f = files[path]; if (f === undefined) throw new Error(path); return enc(f); };
const tables = () => ({ items: createItemTable(read), objects: createObjectTable(read) });
const fourcc = (code) => { const c = code.padEnd(4, ' '); return (c.charCodeAt(0) | (c.charCodeAt(1) << 8) | (c.charCodeAt(2) << 16) | (c.charCodeAt(3) << 24)) >>> 0; };

function bits() {
  const out = []; let n = 0;
  return { put(value, count) { for (let i = 0; i < count; i++, n++) { if ((n & 7) === 0) out.push(0); if (Math.floor(value / 2 ** i) % 2) out[n >> 3] |= 1 << (n & 7); } return this; },
    bytes() { return Uint8Array.from(out); } };
}
function itemBits({ mode, flags = 0, x = 0, y = 0, bodyLoc = 0, page = 0, code, quality = 2, gold }) {
  const b = bits().put(flags, 32).put(101, 10).put(mode, 3);
  if (mode === 3 || mode === 5) b.put(x, 16).put(y, 16); else b.put(bodyLoc, 4).put(x, 4).put(y, 4).put(page + 1, 3);
  b.put(fourcc(code), 32);
  if (flags & 0x200000) { if (gold !== undefined) b.put(0, 1).put(gold, 12); }
  else b.put(0, 3).put(30, 7).put(quality, 4);
  return b.bytes();
}
const GUID = 0x01020304;
const u32le = (v) => [v & 255, (v >>> 8) & 255, (v >>> 16) & 255, (v >>> 24) & 255];
function itemPacket(guid, action, spec, owner = null) {
  const body = itemBits(spec);
  const head = owner === null ? [0x9c, action, 0, 0, ...u32le(guid)] : [0x9d, action, 0, 0, ...u32le(guid), 0, ...u32le(owner)];
  return Uint8Array.from([...head, ...body]);
}
// Packets are fed one at a time in these tests: the size reader answers with the buffer's length.
const whole = (b) => b.length;
const feed = (world, ...packets) => packets.forEach((p) => world.feed(p, whole));

test('item header: ground, stored, equipped, belt, gold pile', () => {
  const ground = decodeItemHeader(itemBits({ mode: 3, x: 5010, y: 5020, code: 'hax' }));
  assert.deepEqual([ground.mode, ground.x, ground.y, ground.code, ground.quality], [3, 5010, 5020, fourcc('hax'), 2]);
  const stored = decodeItemHeader(itemBits({ mode: 0, x: 3, y: 2, page: 0, code: 'wsd', quality: 7 }));
  assert.deepEqual([stored.mode, stored.x, stored.y, stored.page, stored.quality], [0, 3, 2, 0, 7]);
  const worn = decodeItemHeader(itemBits({ mode: 1, bodyLoc: 4, code: 'gax' }));
  assert.deepEqual([worn.mode, worn.bodyLoc], [1, 4]);
  const gold = decodeItemHeader(itemBits({ mode: 3, flags: 0x200000, x: 1, y: 2, code: 'gld', gold: 321 }));
  assert.equal(gold.goldAmount, 321);
  assert.equal(decodeItemHeader(itemBits({ mode: 3, flags: 0x10000, code: 'hax' })), null);
  assert.equal(decodeItemHeader(Uint8Array.from([1, 2])), null);
});

test('inventory, equipment, belt, cursor and ground items reach the views', () => {
  const w = createRealmWorld(tables()); w.setOwner(GUID);
  feed(w,
    itemPacket(10, 0, { mode: 0, x: 2, y: 1, page: 0, code: 'wsd' }),
    itemPacket(11, 0, { mode: 1, bodyLoc: 1, code: 'cap' }),
    itemPacket(12, 0, { mode: 2, x: 1, code: 'hp1' }),
    itemPacket(13, 0, { mode: 4, code: 'hax' }),
    itemPacket(14, 3, { mode: 3, x: 100, y: 200, code: 'hax' }));
  const by = Object.fromEntries(w.itemViews().map((i) => [i.id, i]));
  assert.equal(w.ownItemCount, 5);
  assert.deepEqual([by[10].invPage, by[10].gridX, by[10].gridY, by[10].classId], [PAGE.INVENTORY, 2, 1, 1]);
  assert.deepEqual([by[11].mode, by[11].bodyLoc, by[11].classId], [1, 1, 3]);
  assert.deepEqual([by[12].invPage, by[12].gridX], [PAGE.BELT, 1]);
  assert.equal(w.cursorId, 13);
  assert.deepEqual([by[14].landed, by[14].x, by[14].y, by[14].invPage], [true, 100, 200, 255]);
  assert.equal(by[14].flippyFile, 'flpax');
  assert.equal(by[10].invWidth * by[10].invHeight, 3);
});

test('moving an item replaces its record; removing from a container deletes it', () => {
  const w = createRealmWorld(tables()); w.setOwner(GUID);
  feed(w, itemPacket(10, 0, { mode: 0, x: 0, y: 0, page: 0, code: 'wsd' }));
  feed(w, itemPacket(10, 5, { mode: 4, code: 'wsd' }));   // picked up: now on the cursor
  assert.equal(w.cursorId, 10);
  feed(w, itemPacket(10, 1, { mode: 0, x: 4, y: 2, page: 4, code: 'wsd' }));   // put in the stash
  assert.equal(w.cursorId, 0);
  assert.deepEqual(w.itemViews().map((i) => [i.invPage, i.gridX, i.gridY]), [[PAGE.STASH, 4, 2]]);
  feed(w, itemPacket(10, 5, { mode: 0, x: 4, y: 2, page: 4, code: 'wsd' }));   // taken out, not carried
  assert.equal(w.ownItemCount, 0);
});

test('a second item at a body location replaces the first; the weapon class follows the hands', () => {
  const w = createRealmWorld(tables()); w.setOwner(GUID);
  assert.equal(w.weaponClassOf(), 'hth');
  feed(w, itemPacket(10, 0, { mode: 1, bodyLoc: 4, code: 'hax' }));
  assert.equal(w.weaponClassOf(), '1hs');
  feed(w, itemPacket(11, 9, { mode: 1, bodyLoc: 4, code: 'gax' }));
  assert.deepEqual(w.itemViews().map((i) => i.id), [11]);
  assert.equal(w.weaponClassOf(), '2ht');
  feed(w, itemPacket(11, 8, { mode: 1, bodyLoc: 4, code: 'gax' }));
  assert.equal(w.weaponClassOf(), 'hth');
});

test('belt record follows the worn belt; no belt is record 2', () => {
  const w = createRealmWorld(tables()); w.setOwner(GUID);
  assert.equal(w.beltRecord(), 2);
  feed(w, itemPacket(10, 0, { mode: 1, bodyLoc: 8, code: 'lbl' }));
  assert.equal(w.beltRecord(), 0);
});

test("other players' worn items are kept apart from the player's own", () => {
  const w = createRealmWorld(tables()); w.setOwner(GUID);
  feed(w, itemPacket(20, 0, { mode: 1, bodyLoc: 4, code: 'gax' }, 777), itemPacket(21, 0, { mode: 1, bodyLoc: 1, code: 'cap' }, GUID));
  assert.deepEqual(w.itemViews().map((i) => i.id), [21]);
  assert.deepEqual(w.wornBy(777).map((i) => i.id), [20]);
  assert.equal(w.weaponClassOf(777), '2ht');
  assert.equal(w.weaponClassOf(), 'hth');
});

test('objects: 0x51 adds, 0x0E changes the state, 0x0A removes, a new act clears', () => {
  const w = createRealmWorld(tables()); w.setOwner(GUID);
  const add = Uint8Array.from([0x51, 2, ...u32le(900), 5, 0, 0x34, 0x12, 0x78, 0x56, 1, 0, 0, 0, 0, 0]);
  const add2 = Uint8Array.from([0x51, 2, ...u32le(901), 99, 0, 1, 0, 2, 0, 1, 0, 0, 0, 0, 0]);   // class 99 is not in the table
  feed(w, add, add2);
  assert.deepEqual(w.objectViews().map((o) => [o.id, o.classId, o.token, o.x, o.y, o.mode]), [[900, 5, 'CH', 0x1234, 0x5678, 1]]);
  feed(w, Uint8Array.from([0x0e, 2, ...u32le(900), 1, 1, 2, 0, 0, 0, 0, 0]).subarray(0, 12));
  feed(w, Uint8Array.from([0x0a, 2, ...u32le(900)]).subarray(0, 6));
  assert.equal(w.objectViews().length, 0);
  feed(w, add); feed(w, Uint8Array.from([0x03, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]));
  assert.equal(w.objectViews().length, 0);
});

test('ground items are removed by 0x0A; gold and stash gold from the stat packets', () => {
  const w = createRealmWorld(tables()); w.setOwner(GUID);
  feed(w, itemPacket(14, 3, { mode: 3, x: 1, y: 2, code: 'hax' }));
  feed(w, Uint8Array.from([0x0a, 4, ...u32le(14)]));
  assert.equal(w.ownItemCount, 0);
  feed(w, Uint8Array.from([0x1d, 14, 50]), Uint8Array.from([0x1e, 14, 0x2c, 0x01]));
  assert.equal(w.gold, 300);
  feed(w, Uint8Array.from([0x1f, 15, ...u32le(123456)]));
  assert.equal(w.stashGold, 123456);
  feed(w, Uint8Array.from([0x20, ...u32le(GUID), 14, ...u32le(77)]));
  assert.equal(w.gold, 77);
  feed(w, Uint8Array.from([0x20, ...u32le(5), 14, ...u32le(9)]));
  assert.equal(w.gold, 77);
});

test('feed walks a stream with the size reader and stops on a partial packet', () => {
  const w = createRealmWorld(tables()); w.setOwner(GUID);
  const a = itemPacket(10, 0, { mode: 0, x: 0, y: 0, page: 0, code: 'wsd' }), b = itemPacket(11, 0, { mode: 0, x: 1, y: 0, page: 0, code: 'wsd' });
  const stream = Uint8Array.from([...a, ...b]);
  w.feed(stream, (buf) => (buf.length >= a.length && buf[4] === 10 ? a.length : buf.length >= b.length ? b.length : 0));
  assert.equal(w.ownItemCount, 2);
  w.reset();
  assert.equal(w.ownItemCount, 0);
});

test('the client view carries items, objects and the panel geometry', () => {
  const w = createRealmWorld(tables()); w.setOwner(GUID);
  feed(w, itemPacket(10, 0, { mode: 1, bodyLoc: 4, code: 'hax' }));
  const snap = realmViewSnapshot({ base: null, resources: null, monsters: [], missiles: [], level: 1, actor: () => null, frame: 0, world: w });
  assert.equal(snap.items.length, 1);
  assert.equal(snap.player.weaponClass, '1hs');
  const inv = createInventoryTable(read), belt = createBeltTable(read);
  assert.deepEqual(inv.grid('amazon'), [1, 2, 3, 4, 10, 4, 5, 6, 7, 8, 29, 29]);
  const reads = realmFrameReads(null, w, { inventory: inv, belt, inventoryLayoutFromEngine: (g, c) => ({ g, c }), stashGeometry: (g) => g }, 0);
  assert.equal(reads.beltIndex, 2);
  assert.equal(reads.inventoryLayout.g.length, 12);
  assert.equal(realmFrameReads(null).inventoryLayout, undefined);
});

import { createLevelTable } from './realm-tables.mjs';
test('the level table answers Levels.txt Rain by level id, -1 for a level it does not list', () => {
  const text = 'Name\tId\tRain\nNull\t0\t0\nBlood Moor\t2\t1\n';
  const levels = createLevelTable(() => new TextEncoder().encode(text));
  assert.deepEqual([levels.rain(2), levels.rain(0), levels.rain(99)], [1, 0, -1]);
});

test('raw item bytes and UpdateUi codes are kept for the shell', () => {
  const w = createRealmWorld(tables()); w.setOwner(GUID);
  feed(w, itemPacket(10, 0, { mode: 0, x: 2, y: 1, page: 0, code: 'wsd' }), Uint8Array.of(0x77, 16), Uint8Array.of(0x77, 21));
  assert.ok(w.rawItem(10).length > 8);
  assert.equal(w.rawItem(11), null);
  assert.deepEqual(w.takeUiEvents(), [16, 21]);
  assert.deepEqual(w.takeUiEvents(), []);
});
