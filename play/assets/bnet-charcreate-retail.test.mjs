// The Battle.net character-create screen's popups against retail's own frames of them (headless front-end capture of
// 1.13c D2Launch with the realm create path: the create screen built in realm mode, the name box filled, OK run with the
// reply the stub client returns). The refusal box is the create screen's own PopUpOK2 box (name taken: string 11059 with the
// name; anything else: 5214, the name, 5215); the Hardcore confirmation is PopUpOKCancel with CANCEL and OK. Identity is path +
// frame + colour shift, the anchor and the mode, in call order (the capture has no frame bytes). Skipped without the retail
// archives or the captures.
import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { openRetail } from '../../tools/dcc-decode-bench.mjs';
import { decodeDc6, decodeDatPalette } from './d2-formats.mjs';
import { diffDrawLists, parseDrawListVector } from './drawlist.mjs';
import { createCharacterCreate } from './charcreate.mjs';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const vec = (name) => join(root, 'vectors', 'drawlist', `${name}.txt`);
const have = ['idle', 'refused-taken', 'refused-other', 'hardcore-confirm', 'after-refusal-ok', 'after-hardcore-back'].every((n) => existsSync(vec(`bnet-charcreate-${n}`)));
const rd = have ? openRetail() : null;

function stringTable(read) {
  const tbl = (name, i) => { const d = Buffer.from(read(`data\\local\\lng\\eng\\${name}.tbl`)); const n = d.readUInt16LE(2);
    const e = 21 + n * 2 + d.readUInt16LE(21 + i * 2) * 17, vo = d.readUInt32LE(e + 11); return d.toString('latin1', vo, d.indexOf(0, vo)); };
  return { get: (i) => (i < 10000 ? tbl('string', i) : i < 20000 ? tbl('patchstring', i - 10000) : tbl('expansionstring', i - 20000)) };
}
globalThis.ImageData ??= class { constructor(data, w, h) { Object.assign(this, { data, width: w, height: h }); } };
const ctx = new Proxy({}, { get: (t, k) => (k in t ? t[k] : (t[k] = () => ({ data: new Uint8ClampedArray(4) }))), set: (t, k, v) => { t[k] = v; return true; } });
const documentObject = { createElement: () => ({ width: 1, height: 1, getContext: () => ctx }) };

const build = (events = {}) => {
  const strings = stringTable(rd);
  const screen = createCharacterCreate({ realm: true, read: rd, decodeDc6, decodeDatPalette, string: (i) => strings.get(i) ?? '', documentObject, ...events });
  screen.string = (i) => strings.get(i);
  return screen;
};
const identity = (l) => l.filter((r) => (r.op === 'cel' || r.op === 'celcolor') && !/ohand/i.test(r.path))
  .map((r) => ({ ...r, path: `${r.path}#${r.extra ?? 0}`, hash: 0 }));
// The capture's clock is the process's own (its zero is not the screen's), so the idle animations' phase is the one
// thing a frame does not pin: find the tick at which ours shows the heroes' and the fire's frames the capture shows,
// then compare everything. The animation rule itself is proved by the create-screen rows.
const animated = (r) => /frontend\\(\w+)\\\w+nu1|frontend\\fire/i.test(r.path);
const at = (screen, name) => {
  const want = identity(parseDrawListVector(readFileSync(vec(name), 'utf8'))).filter(animated).map((r) => `${r.path}:${r.frame}`).join();
  for (let t = 0; t < 4000; t++) {
    screen.tick = t;
    globalThis.__d2DrawList = [];
    try { screen.draw(ctx, 800, 600); } finally { var l = globalThis.__d2DrawList; delete globalThis.__d2DrawList; }
    if (identity(l).filter(animated).map((r) => `${r.path}:${r.frame}`).join() === want) return t;
  }
  assert.fail(`no tick of ours shows ${name}'s animation frames`);
};

function frame(screen) {
  globalThis.__d2DrawList = [];
  try { screen.draw(ctx, 800, 600); } finally { var l = globalThis.__d2DrawList; delete globalThis.__d2DrawList; }
  return identity(l);
}
function compare(name, ours, { fromPopup = false } = {}) {
  let retail = identity(parseDrawListVector(readFileSync(vec(name), 'utf8')));
  if (fromPopup) {
    const first = (l) => l.findIndex((r) => /PopUpOK/i.test(r.path));
    retail = retail.slice(first(retail)); ours = ours.slice(first(ours));
  }
  const d = diffDrawLists(retail, ours);
  const show = (x) => `${x.op} ${x.path} #${x.frame} @${x.x},${x.y} m${x.mode}`;
  console.log(`${name}: retail ${retail.length}, ours ${ours.length}, differing records ${d.diffs} (missing ${d.missing.length}, extra ${d.extra.length}, differing ${d.differing.length})`);
  for (const m of d.missing.slice(0, 10)) console.log('  missing', show(m));
  for (const m of d.extra.slice(0, 10)) console.log('  extra', show(m));
  for (const m of d.differing.slice(0, 10)) console.log('  differs', m.draw, m.diff);
  assert.ok(diffDrawLists(retail, ours.slice(0, -1)).diffs > 0, 'the comparison cannot fail');
  assert.equal(d.diffs, 0);
}
const takenText = (screen, name) => screen.string(11059).replace('%s', name);
const otherText = (screen, name) => `${screen.string(5214)}${name}${screen.string(5215)}`;

test('the unchosen create screen draws retail tick 3', { skip: !rd }, () => {
  const s = build(); at(s, 'bnet-charcreate-idle'); compare('bnet-charcreate-idle', frame(s));
});
test('a taken name raises the create screen\'s popup with string 11059 and the name', { skip: !rd }, () => {
  const s = build(); at(s, 'bnet-charcreate-refused-taken'); s.showNotice(takenText(s, 'Aelwyn')); compare('bnet-charcreate-refused-taken', frame(s));
});
test('any other refusal raises the popup with 5214, the name and 5215', { skip: !rd }, () => {
  const s = build(); at(s, 'bnet-charcreate-refused-other'); s.showNotice(otherText(s, 'Aelwyn')); compare('bnet-charcreate-refused-other', frame(s));
});
test('OK on the refusal returns to the unchosen create screen', { skip: !rd }, () => {
  const s = build(); s.chooseByKey('barbarian'); s.tick = 40; s.setName('Aelwyn'); s.showNotice(otherText(s, 'Aelwyn'));
  const [x, y] = [400, 322]; s.click(x, y, 800, 600);
  assert.equal(s.popup, null);
  assert.equal(s.evidence.chosen, null);
  assert.equal(s.evidence.name, '');
  at(s, 'bnet-charcreate-after-refusal-ok'); compare('bnet-charcreate-after-refusal-ok', frame(s));
});
test('OK on a Hardcore character raises the confirmation, CANCEL restarts and OK creates', { skip: !rd }, () => {
  let created = null;
  const s = build({ onCreate: (c) => { created = c; } });
  s.chooseByKey('barbarian'); s.tick = 40; s.setName('Aelwyn'); s.click(...s.targets(800, 600).checkboxes.hardcore, 800, 600);
  assert.equal(s.evidence.checkboxes.states.hardcore, true);
  s.submit();
  assert.equal(created, null, 'the confirmation comes first');
  assert.equal(s.popup.kind, 'confirm');
  compare('bnet-charcreate-hardcore-confirm', frame(s), { fromPopup: true });
  s.click(330, 322, 800, 600); // CANCEL
  assert.equal(s.popup, null); assert.equal(created, null); assert.equal(s.evidence.chosen, null);
  assert.equal(s.evidence.checkboxes.states.hardcore, false);
  at(s, 'bnet-charcreate-after-hardcore-back'); compare('bnet-charcreate-after-hardcore-back', frame(s));
  s.chooseByKey('barbarian'); s.setName('Aelwyn'); s.click(...s.targets(800, 600).checkboxes.hardcore, 800, 600); s.submit();
  s.click(470, 322, 800, 600); // OK
  assert.equal(created?.name, 'Aelwyn'); assert.equal(created.checkboxes.hardcore, true);
});

test('the expiry block is drawn in Font8 (six rows), and the LADDER box sits under HARDCORE', { skip: !rd }, () => {
  const s = build(); s.chooseByKey('barbarian'); s.tick = 40;
  const l = frame(s);
  assert.ok(l.filter((r) => /font8/i.test(r.path)).length > 0);
  const boxes = l.filter((r) => /clickbox/i.test(r.path));
  assert.equal(boxes.length, 3);
  assert.equal(s.evidence.checkboxes.states.ladder, true);
});
test('the LADDER box toggles and rides to the create callback', { skip: !rd }, () => {
  let created = null;
  const s = build({ onCreate: (c) => { created = c; } });
  s.chooseByKey('barbarian'); s.tick = 40; s.setName('Aelwyn');
  s.click(...s.targets(800, 600).checkboxes.ladder, 800, 600);
  assert.equal(s.evidence.checkboxes.states.ladder, false);
  s.submit();
  assert.equal(created.checkboxes.ladder, false);
});
