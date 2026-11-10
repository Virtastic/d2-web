import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmWorld, decodeItemHeader, PAGE } from './realm-world.mjs';
import { createItemTable, createInventoryTable, createBeltTable } from './realm-tables.mjs';
import { realmViewSnapshot, realmFrameReads } from './realm-client-view.mjs';

const enc = (text) => new TextEncoder().encode(text);
const files = {
  'data\\global\\excel\\weapons.txt': 'name\tcode\ttype\tflippyfile\tinvfile\tinvwidth\tinvheight\twclass\t2handedwclass\t2handed\nHand Axe\thax\taxe\tflpax\tinvax\t1\t3\t1hs\t\t0\nWar Sword\twsd\tswor\tflpws\tinvws\t1\t3\t1hs\t\t0\nGreat Axe\tgax\taxe\tflpga\tinvga\t2\t4\t2hs\t2ht\t1\n',
  'data\\global\\excel\\armor.txt': 'name\tcode\ttype\tflippyfile\tinvfile\tinvwidth\tinvheight\tbelt\nCap\tcap\thelm\tflpcap\tinvcap\t2\t2\t\nSash\tlbl\tbelt\tflplbl\tinvlbl\t2\t1\t0\n',
  'data\\global\\excel\\misc.txt': 'name\tcode\ttype\tflippyfile\tinvfile\tinvwidth\tinvheight\nGold\tgld\tgold\tflpgld\tinvgld\t1\t1\nHealing Potion\thp1\thpot\tflphp1\tinvhp1\t1\t1\n',
  'data\\global\\excel\\objects.txt': 'Name\tId\tToken\tSelectable0\tSelectable1\tFrameCnt1\tFrameDelta1\tStart2\tOrientation\nBarrel\t0\tBA\t1\t0\t0\t0\t0\t0\nChest\t5\tCH\t1\t0\t3\t128\t4\t3\n',
  'data\\global\\excel\\inventory.txt': 'class\tinvleft\tinvright\tinvtop\tinvbottom\tgridx\tgridy\tgridleft\tgridright\tgridtop\tgridbottom\tgridboxwidth\tgridboxheight\trarmleft\nAmazon\t1\t2\t3\t4\t10\t4\t5\t6\t7\t8\t29\t29\t9\n',
  'data\\global\\excel\\belts.txt': 'name\tnumboxes\tbox1left\tbox1right\tbox1top\tbox1bottom\tbox2left\tbox2right\tbox2top\tbox2bottom\nA\t4\t0\t10\t0\t10\t10\t20\t0\t10\nB\t4\t0\t10\t0\t10\t10\t20\t0\t10\nC\t0\n',
};
const read = (path) => { const f = files[path]; if (f === undefined) throw new Error(path); return enc(f); };
const tables = () => ({ items: createItemTable(read) });
const fourcc = (code) => { const c = code.padEnd(4, ' '); return (c.charCodeAt(0) | (c.charCodeAt(1) << 8) | (c.charCodeAt(2) << 16) | (c.charCodeAt(3) << 24)) >>> 0; };

function bits() {
  const out = []; let n = 0;
  return { put(value, count) { for (let i = 0; i < count; i++, n++) { if ((n & 7) === 0) out.push(0); if (Math.floor(value / 2 ** i) % 2) out[n >> 3] |= 1 << (n & 7); } return this; },
    bytes() { return Uint8Array.from(out); } };
}
function itemBits({ mode, flags = 0, x = 0, y = 0, bodyLoc = 0, page = 0, code, quality = 2, gold }) {
  const b = bits().put(flags, 32).put(101, 10).put(mode, 3);
  if (mode === 3 || mode === 5) b.put(x, 16).put(y, 16); else b.put(bodyLoc, 4).put(x, 4).put(y, 4).put((mode === 0 ? page + 1 : 0) & 7, 3);   // only a stored item carries a page
  b.put(fourcc(code), 32);
  if (flags & 0x200000) { if (gold !== undefined) b.put(0, 1).put(gold, 12); }
  else b.put(0, 3).put(30, 7).put(quality, 4);
  return b.bytes();
}
const GUID = 0x01020304;
const u32le = (v) => [v & 255, (v >>> 8) & 255, (v >>> 16) & 255, (v >>> 24) & 255];
function itemPacket(guid, action, spec, owner = null) {
  const body = itemBits(spec);
  const nine = [0, 1, 2, 3, 4, 10, 11, 12, 13, 14, 15, 16, 18];
  if (owner === null && !nine.includes(action)) owner = GUID;   // retail routes these actions through the owned-item packet
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
  assert.deepEqual([by[12].invPage, by[12].gridX], [PAGE.NONE, 1]);
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
  feed(w, itemPacket(11, 4, { mode: 1, bodyLoc: 4, code: 'gax' }));
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
  feed(w, itemPacket(20, 9, { mode: 1, bodyLoc: 4, code: 'gax' }, 777), itemPacket(21, 9, { mode: 1, bodyLoc: 1, code: 'cap' }, GUID));
  assert.deepEqual(w.itemViews().map((i) => i.id), [21]);
  assert.deepEqual(w.wornBy(777).map((i) => i.id), [20]);
  assert.equal(w.weaponClassOf(777), '2ht');
  assert.equal(w.weaponClassOf(), 'hth');
});

test('objects: the scene takes the engine object records as they are, and the world keeps none', () => {
  const w = createRealmWorld(tables()); w.setOwner(GUID);
  feed(w, Uint8Array.from([0x51, 2, ...u32le(900), 5, 0, 0x34, 0x12, 0x78, 0x56, 1, 0, 0, 0, 0, 0]));
  assert.equal(w.objectViews, undefined);
  assert.equal(w.tickObjects, undefined);
  const objects = [{ id: 900, classId: 5, token: 'CH', mode: 1 }];
  const snapshot = realmViewSnapshot({ base: null, resources: null, monsters: [], missiles: [], players: [], level: 1, actor: () => null, frame: 0, world: w, objects });
  assert.equal(snapshot.objects, objects);
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

test('belt boxes come from the 800x600 row set, seven rows after the 640x480 one', () => {
  const rowText = (name, left) => `${name}\t4\t${left}\t${left + 29}\t562\t591\n`;
  const names = ['belt', 'sash', 'default', 'girdle', 'light belt', 'heavy belt', 'uber belt'];
  const text = 'name\tnumboxes\tbox1left\tbox1right\tbox1top\tbox1bottom\n'
    + names.map((n) => rowText(n, 343)).join('') + names.map((n) => rowText(n + '2', 423)).join('');
  const belt = createBeltTable(() => new TextEncoder().encode(text));
  assert.equal(belt.boxes(2)[0].left, 423);
  assert.equal(belt.boxes(0)[0].left, 423);
  assert.equal(belt.boxes(2, 0)[0].left, 343);
});

test('a corpse made (0x74, flag set) takes what its owner wore; the pack stays', () => {
  const w = createRealmWorld(tables()); w.setOwner(GUID);
  feed(w, itemPacket(21, 4, { mode: 1, bodyLoc: 1, code: 'cap' }), itemPacket(22, 4, { mode: 0, x: 1, y: 1, code: 'hax' }),
    itemPacket(20, 9, { mode: 1, bodyLoc: 4, code: 'gax' }, 777));
  feed(w, Uint8Array.from([0x74, 0, ...u32le(GUID), ...u32le(900)]));
  assert.deepEqual(w.itemViews().map((i) => i.id).sort(), [21, 22]);
  feed(w, Uint8Array.from([0x74, 1, ...u32le(GUID), ...u32le(900)]));
  assert.deepEqual(w.itemViews().map((i) => i.id), [22]);
  assert.deepEqual(w.wornBy(777).map((i) => i.id), [20]);
  feed(w, Uint8Array.from([0x74, 1, ...u32le(777), ...u32le(901)]));
  assert.deepEqual(w.wornBy(777), []);
});

// The engine's read of a player's unit: the records d2gsClientItems returns.
const heldRecord = (guid, code, over = {}) => ({ guid, classId: 0, code: fourcc(code), mode: 0, x: 0, y: 0, page: 0, bodyLoc: 0, quality: 2, flags: 0x10, ...over });

test('held items are the engine unit\'s: the packet copy supplies only the ground items and the raw bits', () => {
  const calls = [];
  let mine = [], theirs = [];
  const w = createRealmWorld({ ...tables(), heldItems: (guid) => { calls.push(guid); return guid === 0 ? mine : guid === 0x55 ? theirs : []; } });
  w.setOwner(GUID);
  // The packets place a potion in the backpack and a pile of gold on the ground; the engine has not seated the potion.
  feed(w, itemPacket(20, 4, { mode: 0, x: 3, y: 1, page: 0, code: 'hp1' }), itemPacket(21, 0, { mode: 3, x: 100, y: 200, code: 'gld', flags: 0x200000, gold: 55 }));
  assert.deepEqual(w.itemViews().map((i) => i.id), [21]);
  assert.equal(w.itemViews()[0].goldAmount, 55);
  assert.equal(w.ownItemCount, 1);
  assert.equal(calls.length, 1, 'one engine read serves every view until the next packets');
  // Now the unit holds it, with a cursor item, a worn belt and a weapon; flags are the unit's (0x4000: requirements unmet).
  mine = [heldRecord(30, 'cap', { mode: 4, page: 255 }), heldRecord(20, 'hp1', { x: 3, y: 1 }), heldRecord(31, 'lbl', { mode: 1, bodyLoc: 8, page: 255 }),
    heldRecord(32, 'gax', { mode: 1, bodyLoc: 4, page: 255, flags: 0x4010 }), heldRecord(33, 'hp1', { mode: 2, x: 2, page: 255 }), heldRecord(34, 'zzz')];
  theirs = [heldRecord(40, 'wsd', { mode: 1, bodyLoc: 4, page: 255 }), heldRecord(41, 'hp1')];
  feed(w, Uint8Array.from([0x8a]));
  const views = w.itemViews();
  assert.deepEqual(views.map((i) => i.id), [21, 30, 20, 31, 32, 33]);   // a code the tables do not hold cannot be drawn
  const potion = views.find((i) => i.id === 20), belted = views.find((i) => i.id === 33), axe = views.find((i) => i.id === 32);
  assert.deepEqual([potion.invPage, potion.gridX, potion.gridY, potion.invFile], [PAGE.INVENTORY, 3, 1, 'invhp1']);
  assert.deepEqual([belted.invPage, belted.gridX], [PAGE.NONE, 2]);
  assert.deepEqual([axe.bodyLoc, axe.itemFlags], [4, 0x4010]);
  assert.equal(w.cursorId, 30);
  assert.equal(w.beltRecord(), 0);
  assert.equal(w.weaponClassOf(), '2ht');
  assert.equal(w.weaponClassOf(0x55), '1hs');
  assert.deepEqual(w.wornBy(0x55).map((i) => i.id), [40]);
  assert.ok(w.rawItem(20), 'the bitstream the tooltip decodes is still the packet\'s');
  assert.deepEqual(calls, [0, 0, 0x55]);
  // The engine drops the cursor item: the cursor is empty whatever the packet copy last saw.
  mine = mine.slice(1);
  feed(w, itemPacket(30, 1, { mode: 4, code: 'cap' }));
  assert.equal(w.cursorId, 0);
  w.reset();
  mine = [];
  assert.deepEqual(w.itemViews(), []);
});

test('without the engine read the packet copy answers, as an older engine build needs', () => {
  const w = createRealmWorld({ ...tables(), heldItems: () => null }); w.setOwner(GUID);
  feed(w, itemPacket(20, 4, { mode: 0, x: 3, y: 1, page: 0, code: 'hp1' }), itemPacket(22, 1, { mode: 4, code: 'cap' }));
  assert.deepEqual(w.itemViews().map((i) => i.id), [20, 22]);
  assert.equal(w.cursorId, 22);
});

test('the stash and the cube take the 800x600 Inventory.txt rows, sixteen after the 640x480 ones', () => {
  const header = 'class\tinvleft\tinvright\tinvtop\tinvbottom\tgridx\tgridy\tgridleft\tgridright\tgridtop\tgridbottom\tgridboxwidth\tgridboxheight';
  const row = (i) => [`row${i}`, i, i + 100, 0, 0, 6, i === 28 ? 8 : 4, 0, 0, 0, 0, 29, 29].join('\t');
  const text = [header, ...Array.from({ length: 32 }, (_, i) => row(i)), ''].join('\n');
  const inv = createInventoryTable((path) => (path.endsWith('inventory.txt') ? enc(text) : read(path)));
  assert.deepEqual(inv.gridAt(12, 1).slice(0, 2), [28, 128]);
  assert.deepEqual(inv.gridAt(12, 0).slice(0, 2), [12, 112]);
  assert.equal(inv.gridAt(40, 1), null);
  const w = createRealmWorld(tables());
  const reads = realmFrameReads(null, w, { inventory: inv, belt: createBeltTable(read), inventoryLayoutFromEngine: (g, c) => ({ g, c }), stashGeometry: (g) => g }, 0);
  assert.deepEqual([reads.stashLayout[0], reads.stashLayout[5]], [28, 8]);   // Big Bank Page 1 + 16: the eight-row stash
  assert.equal(reads.cubeLayout[0], 25);                                      // Transmogrify Box Page 1 + 16
test('0x42 takes the cursor item away from the local player only', () => {
  const w = createRealmWorld(tables()); w.setOwner(GUID);
  feed(w, itemPacket(13, 0, { mode: 4, code: 'hax' }));
  feed(w, Uint8Array.from([0x42, 0, ...u32le(GUID + 1)]));
  assert.equal(w.cursorId, 13);
  feed(w, Uint8Array.from([0x42, 1, ...u32le(GUID)]));
  assert.equal(w.cursorId, 13);
  feed(w, Uint8Array.from([0x42, 0, ...u32le(GUID)]));
  assert.equal(w.cursorId, 0); assert.equal(w.ownItemCount, 0);
});

test('0x3f marks the item the cursor is using and lets it go', () => {
  const w = createRealmWorld(tables()); w.setOwner(GUID);
  feed(w, itemPacket(10, 0, { mode: 0, x: 0, y: 0, page: 0, code: 'hp1' }));
  feed(w, Uint8Array.from([0x3f, 0, ...u32le(99), 0xff, 0xff]));   // an item the page does not hold: nothing
  assert.equal(w.useItem, null); assert.equal(w.takeInventoryAsks(), 0);
  feed(w, Uint8Array.from([0x3f, 0, ...u32le(10), 0xff, 0xff]));
  assert.deepEqual(w.useItem, { id: 10, kind: 0 });
  assert.equal(w.takeInventoryAsks(), 1); assert.equal(w.takeInventoryAsks(), 0);
  feed(w, Uint8Array.from([0x3f, 0xff, ...u32le(10), 0xff, 0xff]));
  assert.equal(w.useItem, null);
});

test('0x42 takes the cursor item away from the local player only', () => {
  const w = createRealmWorld(tables()); w.setOwner(GUID);
  feed(w, itemPacket(13, 0, { mode: 4, code: 'hax' }));
  feed(w, Uint8Array.from([0x42, 0, ...u32le(GUID + 1)]));
  assert.equal(w.cursorId, 13);
  feed(w, Uint8Array.from([0x42, 1, ...u32le(GUID)]));
  assert.equal(w.cursorId, 13);
  feed(w, Uint8Array.from([0x42, 0, ...u32le(GUID)]));
  assert.equal(w.cursorId, 0); assert.equal(w.ownItemCount, 0);
});

test('0x3f marks the item the cursor is using and lets it go', () => {
  const w = createRealmWorld(tables()); w.setOwner(GUID);
  feed(w, itemPacket(10, 0, { mode: 0, x: 0, y: 0, page: 0, code: 'hp1' }));
  feed(w, Uint8Array.from([0x3f, 0, ...u32le(99), 0xff, 0xff]));   // an item the page does not hold: nothing
  assert.equal(w.useItem, null); assert.equal(w.takeInventoryAsks(), 0);
  feed(w, Uint8Array.from([0x3f, 0, ...u32le(10), 0xff, 0xff]));
  assert.deepEqual(w.useItem, { id: 10, kind: 0 });
  assert.equal(w.takeInventoryAsks(), 1); assert.equal(w.takeInventoryAsks(), 0);
  feed(w, Uint8Array.from([0x3f, 0xff, ...u32le(10), 0xff, 0xff]));
  assert.equal(w.useItem, null);
});
