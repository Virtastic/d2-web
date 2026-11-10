// The two connect-failure notices against retail's own frames (headless front-end capture of 1.13c D2Launch with the realm
// connection failing): the large popup raised for a dead connection at LOG IN (string 5189 with the small print 5190), its OK
// hovered, pressed and released into the main menu; and the small notice raised when the terms cannot be fetched (string 5347),
// its OK leading to the same menu. Identity is path + frame + colour shift, the anchor and the mode, in call order. Skipped
// without the retail archives or the captures.
import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { openRetail } from '../../tools/dcc-decode-bench.mjs';
import { decodeDc6, decodeDatPalette } from './d2-formats.mjs';
import { diffDrawLists, parseDrawListVector } from './drawlist.mjs';
import { createBnetLoginScreen } from './bnet-login-screen.mjs';
import { createBnetConnectFailureNotice } from './bnet-connecting.mjs';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const vec = (name) => join(root, 'vectors', 'drawlist', `${name}.txt`);
const NAMES = ['connect-fail', 'connect-fail-hover', 'connect-fail-press', 'connect-fail-menu', 'tos-fail-notice', 'tos-fail-menu', 'event-0d', 'event-01', 'event-04', 'event-03', 'event-07', 'event-0c', 'event-12'];
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


const transport = (m) => Object.assign(new Error(m), { transport: true });
const tick = () => new Promise((r) => setTimeout(r, 0));
const clock = { ms: 0 };
async function failed(which, reject = transport('x')) {
  clock.ms = 0;
  const strings = stringTable(rd);
  const screen = createBnetLoginScreen({ read: rd, decodeDc6, decodeDatPalette, string: (i) => strings.get(i) ?? '', documentObject, region: 'test', now: () => clock.ms,
    onTermsOfService: async () => { throw transport('x'); }, onAuthenticate: async () => { throw reject; }, onExit: () => { screen.left = (screen.left ?? 0) + 1; } });
  if (which === 'terms') { screen.activate('create'); await tick(); } else { screen.typeKey('KeyA', 'a'); screen.typeKey('Tab', 'Tab'); screen.typeKey('KeyP', 'p'); screen.activate('login'); await tick(); }
  return screen;
}
// The capture's login fields are empty (LOG IN greyed); ours hold the two characters that let the request go, so the
// login form's own records (login button, edit text) are left out -- bnet-login-retail covers them.
const identity = (l) => l.filter((r) => (r.op === 'cel' || r.op === 'celcolor') && !/ohand|fontformal12|3widebuttonblank/i.test(r.path))
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

test('a dead connection at LOG IN raises the large popup; hover changes nothing, press moves the OK label, OK leaves for the menu', { skip: !rd }, async () => {
  const s = await failed('login');
  compare('bnet-connect-fail', s);
  s.hover(380, 440, 800, 600); compare('bnet-connect-fail-hover', s);
  clock.ms = 40; // the capture draws the logos at frame 1 on this tick
  const token = s.press(380, 440, 800, 600); assert.equal(token, 'notice-ok'); compare('bnet-connect-fail-press', s);
  assert.ok(s.release(token, 380, 440, 800, 600)); assert.equal(s.left, 1);
});
test('terms that cannot be fetched raise the disconnect notice, OK leaves for the menu', { skip: !rd }, async () => {
  const s = await failed('terms');
  compare('bnet-tos-fail-notice', s);
  const token = s.press(400, 330, 800, 600); assert.equal(token, 'notice-ok');
  assert.ok(s.release(token, 400, 330, 800, 600)); assert.equal(s.left, 1);
});

test('the main menu connect failure raises the same large popup, sized to its wrapped text, as retail draws', { skip: !rd }, () => {
  const strings = stringTable(rd);
  const n = createBnetConnectFailureNotice({ read: rd, decodeDc6, decodeDatPalette, string: (i) => strings.get(i) ?? '', documentObject });
  globalThis.__d2DrawList = [];
  try { n.draw(ctx); } finally { var l = globalThis.__d2DrawList; delete globalThis.__d2DrawList; }
  const all = identity(parseDrawListVector(readFileSync(vec('bnet-connect-fail'), 'utf8')));
  const retail = all.slice(all.findIndex((r) => /PopUpLargest/i.test(r.path)));
  const ours = identity(l);
  const d = diffDrawLists(retail, ours);
  console.log(`menu connect-fail popup: retail ${retail.length}, ours ${ours.length}, differing records ${d.diffs}`);
  assert.ok(retail.length > 100);
  assert.ok(diffDrawLists(retail, ours.slice(0, -1)).diffs > 0, 'the comparison cannot fail');
  assert.equal(d.diffs, 0);
});

// The failure events the login screen raises from its own reply (retail captured headless at tick 2, logo frame 6, the same
// typed name and password): 0x101 version (event 1, the large popup with its detail), 0x202 disabled (event 4, the
// small popup), and a logon result 1 (event 0xD, the small popup). Each is compared as the first frame it shows.
const EVENT_CASES = [
  ['event 1: a refused version raises the large popup with its detail as small print', 'bnet-event-01', 240, { authResult: 0x101 }],
  ['event 4: a disabled key raises the small popup with the disabled text', 'bnet-event-04', 240, { authResult: 0x202 }],
  ['event 3: an invalid key raises the large popup with its detail as small print', 'bnet-event-03', 0, { authResult: 0x200 }],
  ['event 7: an invalid expansion key raises the large popup with its detail as small print', 'bnet-event-07', 0, { authResult: 0x210 }],
  ['event 0xC: a refused connection raises the large popup with the modem small print', 'bnet-event-0c', 0, { failureEvent: 12 }],
  ['event 0x12: an unrecognised logon result raises the wide popup', 'bnet-event-12', 0, { logonResult: 9 }],
  ['event 0xD: a logon refusal (unknown account) raises the small popup', 'bnet-event-0d', 240, { logonResult: 1 }],
];
for (const [title, name, ms, reject] of EVENT_CASES) {
  test(title, { skip: !rd }, async () => {
    const s = await failed('login', Object.assign(new Error('refused'), reject));
    clock.ms = ms; // events 1, 4 and 0xD draw the logos at frame 6 on tick 2, the later captures at frame 0
    compare(name, s);
  });
}
