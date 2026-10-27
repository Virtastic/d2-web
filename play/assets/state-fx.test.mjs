import assert from 'node:assert/strict';
import { readFileSync, existsSync, openSync, readSync } from 'node:fs';
import { createStateFx } from './state-fx.mjs';
import { openMpqIndex } from './mpq-index.mjs';
import { table } from './skill-fx.mjs';
const enc = (s) => new TextEncoder().encode(s);
const files = {
  'data\\global\\excel\\Overlay.txt': enc('overlay\tFilename\tFrames\tPreDraw\tAnimRate\tTrans\nring\tRing\t4\t1\t256\t3\ncst\tCast\t2\t0\t256\t0\nrem\tRem\t2\t0\t256\t0\n'),
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
