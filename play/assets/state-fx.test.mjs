import assert from 'node:assert/strict';
import { readFileSync, existsSync, openSync, readSync } from 'node:fs';
import { createStateFx } from './state-fx.mjs';
import { openMpqIndex } from './mpq-index.mjs';
import { table } from './skill-fx.mjs';
const enc = (s) => new TextEncoder().encode(s);
const files = {
  'data\\global\\excel\\Overlay.txt': enc('overlay\tFilename\tFrames\tPreDraw\tAnimRate\tTrans\nring\tRing\t4\t1\t16\t3\ncst\tCast\t2\t0\t16\t0\nrem\tRem\t2\t0\t16\t0\n'),
  'data\\global\\excel\\states.txt': enc('state\tid\toverlay1\toverlay2\toverlay3\toverlay4\tcastoverlay\tx\tremoverlay\nx\t5\tring\t\t\t\tcst\t\trem\n'),
};
const fx = createStateFx((p) => files[p]);
let on = false; const has = () => on;
assert.equal(fx.update(has).length, 0);
on = true; let a = fx.update(has);
assert.deepEqual(a.map((e) => [e.file, e.predraw, e.trans]).sort(), [['Cast', false, 0], ['Ring', true, 3]]);
on = false; a = fx.update(has);
assert.deepEqual(a.map((e) => e.file), ['Cast', 'Rem']);
a = fx.update(has); a = fx.update(has);
assert.deepEqual(a.map((e) => e.file), []);
// Monster state overlays: a monster under Inner Sight (state 17, overlay1 = innersight) exports exactly that loop;
// another monster and the hero do not inherit it; a monster that leaves the view and returns starts clean.
const inner = createStateFx((p) => ({
  'data\\global\\excel\\Overlay.txt': enc('overlay\tFilename\tFrames\tPreDraw\tAnimRate\tTrans\ninnersight\tInnerSight\t16\t0\t16\t3\n'),
  'data\\global\\excel\\states.txt': enc('state\tid\toverlay1\tcastoverlay\tremoverlay\ninnersight\t17\tinnersight\t\t\n'),
})[p]);
const sees = (k) => (id) => k === '1:77' && id === 17;
assert.deepEqual(inner.update(sees('1:77'), '1:77', 500).map((e) => [e.file, e.kind, e.trans, e.age]), [['InnerSight', 'overlay', 3, 500 % 16]]);
assert.deepEqual(inner.update(sees('1:78'), '1:78', 500), [], 'another monster does not inherit it');
assert.deepEqual(inner.update(() => false), [], 'the hero does not inherit it');
inner.retain(new Set(['1:78']));
assert.deepEqual(inner.update(sees('1:77'), '1:77', 501).map((e) => e.age), [501 % 16], 'a monster that left the view restarts (no stale gain edge)');

// real tables: every state overlay / skill castoverlay names an Overlay.txt row; its dcc exists when MPQ extract is present
const dir = '/srv/games/d2/tables-1.13c/';
if (existsSync(dir + 'states.txt')) {
  const rd = (n) => readFileSync(dir + n), ov = new Map(table((p) => rd(p.split('\\').pop()), 'Overlay.txt').map((o) => [o.overlay, o]));
  let n = 0; const used = new Set();
  for (const [f, cols] of [['states.txt', ['overlay1', 'overlay2', 'overlay3', 'overlay4', 'pgsvoverlay', 'castoverlay', 'removerlay']], ['skills.txt', ['castoverlay', 'srvoverlay']]])
    for (const r of table((p) => rd(p), f)) for (const c of cols) if (r[c]) { assert.ok(ov.has(r[c]), `${f} ${r[Object.keys(r)[0]]}.${c}=${r[c]} not in Overlay.txt`); assert.ok(ov.get(r[c]).Filename, r[c]); used.add(r[c]); n++; }
  assert.ok(n > 50, 'tables read');
  // every referenced Overlay.txt Filename exists as data\\global\\overlays\\<name>.dcc in the retail MPQs (when present)
  const mpqs = ['d2exp', 'd2data', 'd2char', 'd2sfx', 'd2music', 'd2xvideo', 'd2xmusic'].map((m) => `retail/D2/${m}.mpq`).filter(existsSync);
  const idx = mpqs.map((m) => { const fd = openSync(m, 'r'); return openMpqIndex((pos, len) => { const b = Buffer.alloc(len); readSync(fd, b, 0, len, pos); return b; }); }).filter(Boolean);
  const miss = idx.length ? [...used].map((k) => ov.get(k)).filter((o) => o.Filename && o.Filename !== 'null' && !idx.some((f) => f(`data\\global\\overlays\\${o.Filename}.dcc`))).map((o) => o.Filename) : [];
  assert.deepEqual(miss, [], 'Overlay.txt names with no dcc in the retail MPQs');
}
console.log('state-fx ok');
// A re-set while the state holds (seq moves) restarts the loop at pos 0, as retail's fresh 0x6B300 node does.
{ let s = 0; const f = (t) => inner.update((id) => id === 17, 'r', t, () => s).map((e) => e.age)[0];
  assert.deepEqual([f(500), f(501)], [500 % 16, 501 % 16]); s = 1;
  assert.deepEqual([f(502), f(503)], [0, 1], 'restart at the re-set tick'); }
