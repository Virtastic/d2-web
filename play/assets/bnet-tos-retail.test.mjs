// The Terms of Service screen against retail's own frames of it (headless front-end capture of 1.13c D2Launch+0x18E10, the
// file it reads replaced by two placeholder texts of our own: test/fixtures/bnet-tos-short.txt, three lines, and
// bnet-tos-long.txt, 37 wrapped lines with the scrollbar). Tick 0 of the long capture is the screen as built; the later ticks
// scroll the list five rows, hover AGREE, press it, release it elsewhere, then hover and press DISAGREE. Identity is path +
// frame + colour shift, the anchor and the mode, in call order. AGREE leads to the create form and DISAGREE back to the
// login screen. Skipped without the retail archives or the captures.
import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { openRetail } from '../../tools/dcc-decode-bench.mjs';
import { decodeDc6, decodeDatPalette } from './d2-formats.mjs';
import { diffDrawLists, parseDrawListVector } from './drawlist.mjs';
import { createBnetLoginScreen } from './bnet-login-screen.mjs';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const vec = (name) => join(root, 'vectors', 'drawlist', `${name}.txt`);
const NAMES = ['tos', 'tos-long', 'tos-long-scrolled', 'tos-long-hover-agree', 'tos-long-press-agree', 'tos-long-release', 'tos-long-hover-cancel',
  'tos-long-press-cancel', 'tos-agree-create', 'tos-cancel-login'];
const have = NAMES.every((n) => existsSync(vec(`bnet-${n}`)));
const rd = have ? openRetail() : null;

// D2Lang indices as the screen asks: below 10000 string.tbl, 10000s patchstring.tbl, 20000s expansionstring.tbl.
function stringTable(read) {
  const tbl = (name, i) => { const d = Buffer.from(read(`data\\local\\lng\\eng\\${name}.tbl`)); const n = d.readUInt16LE(2);
    const e = 21 + n * 2 + d.readUInt16LE(21 + i * 2) * 17, vo = d.readUInt32LE(e + 11); return d.toString('latin1', vo, d.indexOf(0, vo)); };
  return { get: (i) => (i < 10000 ? tbl('string', i) : i < 20000 ? tbl('patchstring', i - 10000) : tbl('expansionstring', i - 20000)) };
}
globalThis.ImageData ??= class { constructor(data, w, h) { Object.assign(this, { data, width: w, height: h }); } };
const ctx = new Proxy({}, { get: (t, k) => (k in t ? t[k] : (t[k] = () => ({ data: new Uint8ClampedArray(4) }))), set: (t, k, v) => { t[k] = v; return true; } });
const documentObject = { createElement: () => ({ width: 1, height: 1, getContext: () => ctx }) };


const fixture = (name) => readFileSync(join(root, 'test', 'fixtures', name), 'utf8');

async function terms(text) {
  const strings = stringTable(rd);
  const screen = createBnetLoginScreen({ read: rd, decodeDc6, decodeDatPalette, string: (i) => strings.get(i) ?? '', documentObject, region: 'test',
    now: () => 0, onTermsOfService: async () => text });
  screen.activate('create');
  await new Promise((r) => setTimeout(r, 0));
  assert.equal(screen.mode, 'terms');
  return screen;
}
const identity = (l) => l.filter((r) => (r.op === 'cel' || r.op === 'celcolor') && !/ohand/i.test(r.path))
  .map((r) => ({ ...r, path: `${r.path}#${r.extra ?? 0}`, hash: 0 }));

function compare(name, screen) {
  globalThis.__d2DrawList = [];
  try { screen.draw(ctx, 800, 600); } finally { var l = globalThis.__d2DrawList; delete globalThis.__d2DrawList; }
  const retail = identity(parseDrawListVector(readFileSync(vec(name), 'utf8')));
  const ours = identity(l);
  const d = diffDrawLists(retail, ours);
  const show = (x) => `${x.op} ${x.path} #${x.frame} @${x.x},${x.y} m${x.mode}`;
  console.log(`${name}: retail ${retail.length}, ours ${ours.length}, differing records ${d.diffs} (missing ${d.missing.length}, extra ${d.extra.length}, differing ${d.differing.length})`);
  for (const m of d.missing.slice(0, 10)) console.log('  missing', show(m));
  for (const m of d.extra.slice(0, 10)) console.log('  extra', show(m));
  for (const m of d.differing.slice(0, 10)) console.log('  differs', m.draw, m.diff);
  assert.ok(diffDrawLists(retail, ours.slice(0, -1)).diffs > 0, 'the comparison cannot fail');
  assert.equal(d.diffs, 0);
}
const press = (s, x, y) => s.press(x, y, 800, 600);

test('the short terms draw retail tick 1', { skip: !rd }, async () => { compare('bnet-tos', await terms(fixture('bnet-tos-short.txt'))); });
test('the long terms draw retail tick 0 with the scrollbar', { skip: !rd }, async () => { compare('bnet-tos-long', await terms(fixture('bnet-tos-long.txt'))); });
test('the long terms scrolled five rows draw retail', { skip: !rd }, async () => {
  const s = await terms(fixture('bnet-tos-long.txt')); s.scrollTerms(5); compare('bnet-tos-long-scrolled', s);
});
test('hovering AGREE or DISAGREE changes nothing, and pressing shows the second cel', { skip: !rd }, async () => {
  const s = await terms(fixture('bnet-tos-long.txt')); s.scrollTerms(5);
  s.hover(590, 495, 800, 600); compare('bnet-tos-long-hover-agree', s);
  const token = press(s, 590, 495); assert.equal(token, 'terms-agree'); compare('bnet-tos-long-press-agree', s);
  s.cancel(); compare('bnet-tos-long-release', s);
  s.hover(200, 495, 800, 600); compare('bnet-tos-long-hover-cancel', s);
  assert.equal(press(s, 200, 495), 'terms-cancel'); compare('bnet-tos-long-press-cancel', s);
});
test('AGREE opens the create form and DISAGREE returns to the login screen, as retail', { skip: !rd }, async () => {
  const a = await terms(fixture('bnet-tos-short.txt'));
  assert.ok(a.release(press(a, 590, 495), 590, 495, 800, 600)); assert.equal(a.mode, 'create'); compare('bnet-tos-agree-create', a);
  const c = await terms(fixture('bnet-tos-short.txt'));
  assert.ok(c.release(press(c, 200, 495), 200, 495, 800, 600)); assert.equal(c.mode, 'login'); compare('bnet-tos-cancel-login', c);
});
