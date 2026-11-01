// D65: the PlugY stash reaches a bring-your-own-files player. The release image runs as a byo host and serves no
// PlugY art, and the shell used to switch the whole feature (pages, shared stash, shared gold, sidecars) off there.
// Now the door keeps PlugY's files from the player's D2SE folder and the shell reads them through plugySource.
// Also, end to end over the pure pieces the shell uses: a fixed click sequence pages the stash, an item placed on a
// page survives the .d2x/.sss save and reload, and the files carry PlugY's headers.
import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { isPlugyFile, plugyFilesOf, plugySource, PLUGY_FILES } from '../play/assets/plugy-files.mjs';
import { createCollection } from '../play/assets/byo-source.mjs';
import { createPlugyStash } from '../play/assets/plugy-stash-ui.mjs';
import { pagesToSss, sssToPages, pagesToD2x, d2xToPages } from '../play/assets/plugy-stash.mjs';

const DC6 = (tag) => Uint8Array.from([6, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0xEE, 0xEE, 0xEE, 0xEE, 1, 0, 0, 0, 1, 0, 0, 0, tag, 0, 0, 0]);
const INI = '[GENERAL]\nActivePlugin=1\n[STASH]\nActiveBigStash=1\nActiveMultiPageStash=1\nActiveSharedStash=1\nActiveSharedGold=1\n';
const given = () => [new File([DC6(1)], 'StashBtns.dc6'), new File([DC6(2)], 'SharedGoldBtns.dc6'), new File([DC6(3)], 'TradeStash.dc6'), new File([INI], 'PlugY.ini')];

test('PlugY files are recognised by base name, any case; archives and other PlugY art are not', () => {
  assert.deepEqual(PLUGY_FILES, ['StashBtns.dc6', 'SharedGoldBtns.dc6', 'TradeStash.dc6', 'PlugY.ini']);
  for (const n of ['stashbtns.DC6', 'D2SE/PlugY/TradeStash.dc6', 'D2SE\\PlugY.ini']) assert.equal(isPlugyFile(n), true, n);
  for (const n of ['d2data.mpq', 'StatsBackground.dc6', 'PlugY.dll', 'Game.exe']) assert.equal(isPlugyFile(n), false, n);
  assert.deepEqual(Object.keys(plugyFilesOf([...given(), new File([1], 'd2exp.mpq')])).sort(), ['plugy.ini', 'sharedgoldbtns.dc6', 'stashbtns.dc6', 'tradestash.dc6']);
});

test('byo: the files the player gave are the art and the ini (no fetch)', async () => {
  const src = plugySource({ byo: true, given: plugyFilesOf(given()), fetch: () => { throw new Error('a byo host must not fetch'); } });
  assert.equal(await src.ini(), INI);
  assert.equal((await src.art('StashBtns'))[24], 1);
  assert.equal((await src.art('TradeStash'))[24], 3);
});

test('byo without PlugY: art rejects with a reason the player can act on, ini is null', async () => {
  const src = plugySource({ byo: true, given: {} });
  assert.equal(await src.ini(), null);
  await assert.rejects(src.art('StashBtns'), /StashBtns\.dc6 not given \(choose the Diablo II folder that holds D2SE\/PlugY\)/);
  const bad = plugySource({ byo: true, given: { 'stashbtns.dc6': new File(['<!doctype html>.........................'], 'StashBtns.dc6') } });
  await assert.rejects(bad.art('StashBtns'), /not given/);
});

test('hosted: D2SE/ is read before the game root, a page served for a missing file is not art', async () => {
  const served = { '/retail/D2SE/PlugY.ini': INI, '/retail/PlugY/StashBtns.dc6': DC6(9), '/retail/D2SE/PlugY/StashBtns.dc6': '<html>not found</html>' };
  const asked = [];
  const fetch = async (u) => { asked.push(u); const b = served[u]; return { ok: b !== undefined, text: async () => String(b), arrayBuffer: async () => (typeof b === 'string' ? new TextEncoder().encode(b) : b ?? new Uint8Array()).buffer }; };
  const src = plugySource({ fetch });
  assert.equal(await src.ini(), INI);
  assert.deepEqual(asked, ['/retail/D2SE/PlugY.ini']);
  assert.equal((await src.art('StashBtns'))[24], 9);
  await assert.rejects(src.art('TradeStash'), /TradeStash\.dc6 not served/);
});

test('the door keeps PlugY files from a dropped folder apart from the archives and hands them to the source', async () => {
  const col = createCollection();
  await col.add({ files: given() });
  assert.deepEqual(Object.keys(col.plugy).sort(), ['plugy.ini', 'sharedgoldbtns.dc6', 'stashbtns.dc6', 'tradestash.dc6']);
  assert.equal(col.entries.length, 0, 'no PlugY file is inspected as an archive');
  assert.deepEqual(col.mounts(), []);
  const src = plugySource({ byo: true, given: { ...col.plugy } });
  assert.equal((await src.art('SharedGoldBtns'))[24], 2);
});

test('the shell no longer turns PlugY off on a byo host; it reads the door\'s files', () => {
  const shell = readFileSync(new URL('../play/gameplay-shell.mjs', import.meta.url), 'utf8');
  const door = readFileSync(new URL('../play/assets/byo-door.mjs', import.meta.url), 'utf8');
  assert.doesNotMatch(shell, /window\.__d2\.byo\)\s*\{\s*plugyStash\.enabled = false/);
  assert.match(shell, /plugySource\(\{ byo: Boolean\(window\.__d2\?\.byo\), given: window\.__d2PlugyFiles \}\)/);
  assert.match(door, /window\.__d2PlugyFiles = \{ \.\.\.col\.plugy \};[^\n]*\n\s*resolveDoor\(col\.mounts\(\)\)/);
});

// The shell's page swap (gameplay-shell.mjs plugySwap) over a model engine stash: the live page is exported into its
// set and the target page inserted. jm(n, b) is a JM item list of n two-byte items filled with b.
const jm = (n, fill = 7) => Uint8Array.from([0x4a, 0x4d, n & 255, n >> 8, ...Array(n * 2).fill(fill)]);
function model() {
  const sets = { false: [], true: [] }; let engine = jm(0), live = { shared: false, page: 0 };
  const lastUsed = (sh) => { let l = 0; sets[sh].forEach((p, i) => { if (p && p[2]) l = i; }); if (live.shared === sh && engine[2]) l = Math.max(l, live.page); return l; };
  const ui = createPlugyStash({ enabled: true, lastUsed,
    onPage: (from, to, fs, ts) => { sets[fs][from] = engine; engine = sets[ts][to] || jm(0); live = { shared: ts, page: to }; return true; } });
  ui.setArt({ stash: Array(12).fill({}), gold: Array(4).fill({}) });
  return { ui, sets, get engine() { return engine; }, set engine(v) { engine = v; }, get live() { return live; } };
}
// A button release through the control's own click handler, at the button's centre (800x600, the 10x10 grid record).
const G = { left: 80, top: 61, recordTop: 280, columns: 10, rows: 10, gridLeft: 96, gridTop: 143, boxW: 29, boxH: 29 };
const press = (m, name, shift = false) => assert.equal(m.ui.click(m.ui.BTN[name].x + 16, m.ui.BTN[name].y + 16, 800, 600, G, true, shift), true, name);

test('page navigation over a fixed input sequence', () => {
  const m = model(), trail = [];
  const seq = [['next'], ['next'], ['nextIndex'], ['prev'], ['nextIndex', true], ['prevIndex'], ['prev', true], ['next', true], ['shared'], ['next'], ['shared']];
  m.engine = jm(1, 0x11);   // something on page 0, so shift+next has a last used page
  for (const [b, sh] of seq) { press(m, b, sh); trail.push(`${m.ui.showShared ? 'S' : 'P'}${m.ui.page}`); }
  assert.deepEqual(trail, ['P1', 'P2', 'P9', 'P8', 'P99', 'P89', 'P0', 'P0', 'S0', 'S1', 'P0']);
  assert.deepEqual(m.ui.mem, { false: 0, true: 1 });
  assert.equal(m.ui.pages, 100);
});

test('an item placed on page N survives save and reload (.d2x personal, .sss shared, with shared gold)', () => {
  const m = model();
  for (let i = 0; i < 4; i++) press(m, 'next');           // personal page 4
  m.engine = jm(1, 0x42);                                 // the player drops an item there
  press(m, 'next');                                       // page 5: page 4 is stored
  press(m, 'shared'); press(m, 'next'); press(m, 'next'); // shared page 2
  m.engine = jm(2, 0x5A);
  press(m, 'shared');                                     // back to personal page 5; shared page 2 stored
  assert.deepEqual([m.live.shared, m.live.page], [false, 5]);
  m.sets[false][5] = m.engine;                            // persist exports the live page first (plugyPersist)
  const d2x = pagesToD2x(m.sets[false], m.live), sss = pagesToSss(m.sets[true], { gold: 123456 });
  assert.equal(String.fromCharCode(...d2x.subarray(0, 6)), 'CSTM01');
  assert.equal(String.fromCharCode(...sss.subarray(0, 4)), 'SSS\0');
  const own = d2xToPages(d2x), shared = sssToPages(sss);
  assert.deepEqual([...own.pages[4]], [...jm(1, 0x42)]);
  assert.deepEqual([...shared.pages[2]], [...jm(2, 0x5A)]);
  assert.equal(shared.gold, 123456);
  assert.deepEqual(own.live, { page: 5, shared: false });
  for (const [i, p] of own.pages.entries()) if (i !== 4) assert.equal(p.length <= 4 || p[2] === 0, true, `personal page ${i} empty`);
});
