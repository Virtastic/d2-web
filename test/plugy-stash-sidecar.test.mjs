import test from 'node:test';
import assert from 'node:assert/strict';
import { pagesToSss, sssToPages, pagesToD2x, d2xToPages, MAX_PAGES } from '../play/assets/plugy-stash.mjs';

const jm = (n, fill = 7) => Uint8Array.from([0x4a, 0x4d, n & 255, n >> 8, ...Array(n * 2).fill(fill)]);

test('d2x round-trips pages and the live marker', () => {
  const pages = [jm(0), jm(2, 9), jm(1, 3)];
  const out = d2xToPages(pagesToD2x(pages, { page: 2, shared: false }));
  assert.deepEqual(out.pages.map((p) => [...p]), pages.map((p) => [...p]));
  assert.deepEqual(out.live, { page: 2, shared: false });
  assert.deepEqual(d2xToPages(pagesToD2x(pages, { page: 1, shared: true })).live, { page: 1, shared: true });
});

test('d2x header is the PlugY layout', () => {
  const b = pagesToD2x([jm(0)]);
  assert.equal(String.fromCharCode(...b.subarray(0, 6)), 'CSTM01');
  assert.equal(b[10] | (b[11] << 8), 1);
  assert.equal(String.fromCharCode(b[14], b[15]), 'ST');
});

test('shared sss round-trips and has the SSS header', () => {
  const pages = [jm(1), jm(0)];
  const b = pagesToSss(pages);
  assert.equal(String.fromCharCode(...b.subarray(0, 3)), 'SSS');
  assert.deepEqual(sssToPages(b).pages.map((p) => [...p]), pages.map((p) => [...p]));
});

test('missing pages are stored as empty sections', () => {
  const out = sssToPages(pagesToSss([jm(1), undefined, jm(1)]));
  assert.equal(out.pages.length, 3);
  assert.equal(out.pages[1].length, 4);
});

test('bad headers, counts and truncation are refused', () => {
  const good = pagesToD2x([jm(1)]);
  assert.throws(() => d2xToPages(Uint8Array.from([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14])));
  assert.throws(() => sssToPages(good));
  assert.throws(() => d2xToPages(good.subarray(0, good.length - 3)));
  const zero = Uint8Array.from(good); zero[10] = 0; assert.throws(() => d2xToPages(zero));
  const huge = Uint8Array.from(good); huge[10] = 0xff; huge[11] = 0xff; assert.throws(() => d2xToPages(huge));
  assert.ok(MAX_PAGES >= 1000);
});

// ---- the PlugY file layout, with fixtures built here (simple items only: 14 bytes each) ----
import { itemTables, itemSizer } from '../play/assets/d2-item-size.mjs';
const T = itemTables('Stat\tSave Bits\tSave Add\tEncode\tSave Param Bits\nstrength\t8\t32\t0\t0', 'name\tcode\nCap\tcap', 'name\tcode\tstackable\nSword\tswd\t0', 'name\tcode\tstackable\nGem\tgsw\t0');
const itemSize = itemSizer(T);
const putBits = (b, at, n, v) => { for (let i = 0; i < n; i++) if ((v >> i) & 1) b[(at + i) >> 3] |= 1 << ((at + i) & 7); };
const simple = (code) => { const b = new Uint8Array(14); b[0] = 0x4a; b[1] = 0x4d; putBits(b, 37, 1, 1); for (let i = 0; i < 4; i++) putBits(b, 76 + 8 * i, 8, (code + '   ').charCodeAt(i)); return b; };
const sect = (...codes) => Uint8Array.from([0x4a, 0x4d, codes.length & 255, codes.length >> 8, ...codes.flatMap((c) => [...simple(c)])]);
const str = (b, o, n) => String.fromCharCode(...b.subarray(o, o + n));

test('the item decoder splits the simple-item fixture', () => {
  assert.equal(itemSize(simple('gsw'), 0), 14);
  assert.equal(itemSize(sect('cap', 'swd'), 4), 14);
});

test('a PlugY-form sss round-trips pages, flags, names and shared gold', () => {
  const pages = [sect('cap', 'swd'), sect(), sect('gsw')], meta = [{ flags: 3, name: 'Gems' }, { flags: 0, name: '' }, { flags: 1, name: 'A page name of twenty-plus chars' }];
  const b = pagesToSss(pages, { gold: 123456, meta, itemSize });
  assert.equal(str(b, 0, 3), 'SSS'); assert.equal(str(b, 4, 2), '02');
  assert.equal(b[6] | (b[7] << 8) | (b[8] << 16), 123456);
  assert.equal(b[10], 3);
  assert.equal(str(b, 14, 2), 'ST'); assert.equal(b[16], 3); assert.equal(str(b, 20, 4), 'Gems');
  const out = sssToPages(b, itemSize);
  assert.equal(out.gold, 123456);
  assert.deepEqual(out.pages.map((p) => [...p]), pages.map((p) => [...p]));
  assert.deepEqual(out.meta.map((m) => m.flags), [3, 0, 1]);
  assert.deepEqual(out.meta.map((m) => m.name), ['Gems', '', 'A page name of twent']);
  assert.throws(() => sssToPages(b), /item decoder/);
});

test('no gold writes version 01, as PlugY does', () => {
  const b = pagesToSss([sect('cap')], { itemSize });
  assert.equal(str(b, 4, 2), '01'); assert.equal(sssToPages(b, itemSize).gold, 0);
});

test('a PlugY 10.00 sss (no flags word) is read', () => {
  const sec = sect('swd');
  const b = Uint8Array.from([0x53, 0x53, 0x53, 0, 0x30, 0x31, 1, 0, 0, 0, 0x53, 0x54, ...Buffer.from('Old'), 0, ...sec]);
  const out = sssToPages(b, itemSize);
  assert.equal(out.meta[0].name, 'Old'); assert.deepEqual([...out.pages[0]], [...sec]);
});

test('a PlugY-form d2x round-trips, with the PlugY header and the shared-page marker', () => {
  const pages = [sect('cap'), sect(), sect('gsw', 'swd')];
  const b = pagesToD2x(pages, { page: 2, shared: false }, { itemSize });
  assert.equal(str(b, 0, 6), 'CSTM01'); assert.equal(b[10], 3); assert.equal(str(b, 14, 2), 'ST');
  const out = d2xToPages(b, itemSize);
  assert.deepEqual(out.pages.map((p) => [...p]), pages.map((p) => [...p]));
  assert.deepEqual(out.live, { page: 2, shared: false });
  for (const live of [{ page: 7, shared: true, personal: 1 }, { page: 999, shared: true, personal: 2 }, { page: 0, shared: true, personal: 0 }]) {
    assert.deepEqual(d2xToPages(pagesToD2x(pages, live, { itemSize }), itemSize).live, live);
  }
});

test('the writer falls back to the L<len> form when the decoder cannot confirm a page, and the reader needs no decoder for it', () => {
  const bad = Uint8Array.from([0x4a, 0x4d, 1, 0, 1, 2, 3, 4, 5, 6]);
  const b = pagesToSss([bad, sect('cap')], { gold: 5, itemSize });
  assert.equal(str(b, 14, 2), 'ST'); assert.ok(str(b, 16, 8).includes('L10'));
  const out = sssToPages(b);
  assert.equal(out.gold, 5); assert.deepEqual([...out.pages[0]], [...bad]); assert.equal(out.pages.length, 2);
  assert.deepEqual(sssToPages(pagesToSss([bad]), itemSize).pages.map((p) => [...p]), [[...bad]]);
});

test('a page whose items the decoder rejects is an error, not a silent loss', () => {
  const b = pagesToSss([sect('cap')], { itemSize });
  const broken = Uint8Array.from(b); broken[broken.length - 1] = 0;
  assert.throws(() => sssToPages(b.subarray(0, b.length - 5), itemSize));
  assert.ok(sssToPages(broken, itemSize));
});
