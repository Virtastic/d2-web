// D25: item drop = flippy frames, then Items.txt dropsound at dropsfxframe, then the item settles.
// Part 1 is synthetic (event order). Part 2 reads the retail MPQs through the native tool (D2_MPQ_TOOL,
// D2_RETAIL_DIR) and checks EVERY Items.txt row resolves a flippy DC6 and a dropsound whose wav exists; skipped without them.
import test from 'node:test';
import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { existsSync, mkdirSync, readFileSync } from 'node:fs';
import { homedir, tmpdir } from 'node:os';
import { join } from 'node:path';
import { createEventSounds } from './event-sounds.mjs';

const enc = (s) => new TextEncoder().encode(s);
const codeNum = (c) => [...c.padEnd(4)].reduce((n, ch, i) => n + ch.charCodeAt(0) * 2 ** (8 * i), 0);
const files = {
  'data\\global\\excel\\Sounds.txt': enc('Sound\tIndex\nitem_a\t10\nitem_b\t11\n'),
  'data\\global\\excel\\armor.txt': enc('name\tcode\tdropsound\tdropsfxframe\nA\tcap\titem_a\t5\n'),
  'data\\global\\excel\\weapons.txt': enc('name\tcode\tdropsound\tdropsfxframe\nB\thax\titem_b\t0\n'),
  'data\\global\\excel\\misc.txt': enc('name\tcode\tdropsound\tdropsfxframe\n'),
};
const item = (code, mode, frame) => ({ id: 1, code: codeNum(code), mode, invPage: 255, framePrecise: frame << 8 });
function run(frames, expectedSound = true) {
  const got = [], s = createEventSounds((p) => files[p], (row, e) => got.push([row, e.kind, current, e.delay]));
  let current = -1;
  frames.forEach((it, i) => { current = i; s.update({ items: [it] }, 0); });
  return got.filter((g) => g[1] === 'drop');
}
const flip = (code, last) => [...Array.from({ length: 17 }, (_, f) => item(code, 5, f)), item(code, 3, 16), item(code, 3, 16)];

test('drop sound is submitted on the first dropping frame, with dropsfxframe as its delay, not at landing', () => {
  const got = run(flip('cap'));
  assert.deepEqual(got.map((g) => [g[0], g[2], g[3]]), [[10, 0, 5]]); // retail submits on the flip tick (Q 244,c at the flip); delay 5 == dropsfxframe
});
test('dropsfxframe 0 plays on the first dropping frame', () => {
  assert.deepEqual(run(flip('hax')).map((g) => [g[0], g[2]]), [[11, 0]]);
});
test('control: a wrong expectation fails (deliberate-failure control)', () => {
  assert.throws(() => assert.deepEqual(run(flip('cap')).map((g) => g[2]), [6]));
});

const retail = process.env.D2_RETAIL_DIR || join(homedir(), 'd2-retail');
const tool = process.env.D2_MPQ_TOOL;
const skip = !tool || !existsSync(tool) || !existsSync(join(retail, 'patch_d2.mpq')) ? 'D2_MPQ_TOOL / retail MPQs absent' : false;
const cache = join(process.env.D2_MPQ_CACHE || tmpdir(), 'd2-item-drop-cache'); mkdirSync(cache, { recursive: true });
const archives = ['patch_d2', 'd2exp', 'd2char', 'd2data', 'd2sfx'].map((a) => join(retail, `${a}.mpq`));
const read = (path) => {
  const out = join(cache, path.toLowerCase().replace(/[\\/]/g, '_'));
  if (!existsSync(out)) for (const a of archives) { try { execFileSync(tool, ['get', a, path, out], { stdio: 'pipe' }); if (existsSync(out)) break; } catch {} }
  if (!existsSync(out)) throw new Error(`missing ${path}`);
  return new Uint8Array(readFileSync(out));
};
const rows = (f) => new TextDecoder('latin1').decode(read(`data\\global\\excel\\${f}.txt`)).split(/\r?\n/).map((l) => l.split('\t'));

test('every Items.txt row resolves a flippy DC6 and a dropsound wav', { skip }, () => {
  const sounds = new Map(rows('Sounds').slice(1).filter((c) => c[0]).map((c) => [c[0], c[2]]));
  let n = 0; const bad = [];
  for (const f of ['armor', 'weapons', 'misc']) {
    const [h, ...r] = rows(f), col = (k) => h.indexOf(k);
    for (const c of r) {
      if (!c[col('code')] || /^not used$/i.test(c[col('name')])) continue; n++; // "Not used" rows are never instantiated
      const flip = c[col('flippyfile')], snd = c[col('dropsound')], id = `${f}/${c[col('code')]}`;
      try { assert.ok(flip, 'no flippyfile'); read(`data\\global\\items\\${flip}.dc6`); } catch (e) { bad.push(`${id} flippy ${flip}`); }
      try { assert.ok(sounds.has(snd), 'not in Sounds.txt'); read(`data\\global\\sfx\\${sounds.get(snd).replace(/^\//, '')}`); } catch (e) { bad.push(`${id} dropsound ${snd}`); }
    }
  }
  assert.ok(n > 400, `only ${n} item rows`);
  assert.deepEqual(bad, []);
});
