// D5/D11: a 500-gold pile at rest vs retail trace a retail recording (cel, direction, frame per tick; main + holdout).
// Retail: cel=item:flpgld f=16 d=2 on ticks 1-160. A wrong variant must produce diffs (the control).
import assert from 'node:assert/strict';
import { readFileSync, existsSync, mkdirSync } from 'node:fs';
import { execFileSync } from 'node:child_process';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { createItemRenderer } from './item-renderer.mjs';

const dir = new URL('../../the retail captures', import.meta.url).pathname;
const tool = process.env.D2_MPQ_TOOL, retail = process.env.D2_RETAIL_DIR || 'retail/D2';
if (!tool || !existsSync(tool) || !existsSync(dir + 'trace.txt')) { console.log('gold-pile skipped: D2_MPQ_TOOL / vectors absent'); process.exit(0); }
const cache = join(tmpdir(), 'gold-pile-cache'); mkdirSync(cache, { recursive: true });
const read = (p) => { const out = join(cache, p.replace(/\\/g, '_'));
  if (!existsSync(out)) for (const a of ['patch_d2', 'd2exp', 'd2data']) { try { execFileSync(tool, ['get', join(retail, a + '.mpq'), p, out], { stdio: 'pipe' }); if (existsSync(out)) break; } catch {} }
  return new Uint8Array(readFileSync(out)); };
const r = createItemRenderer(read, {});

const ours = (framePrecise, goldAmount = 500) => {
  const item = { flippyFile: 'flpgld', goldAmount, framePrecise };
  const m = r.measure(item), perDir = 17; // flpgld: 4 directions x 17 frames
  return `cel=item:${m.flippyFile} f=${m.frame % perDir} d=${Math.floor(m.frame / perDir)}`;
};
const diffs = (file, mk) => {
  const rows = readFileSync(dir + file, 'utf8').split('\n').filter((l) => l.includes('cel=item:flpgld'));
  const ticks = rows.map((l) => l.match(/cel=\S+ f=\d+ d=\d+/)[0]);
  return { n: ticks.length, bad: ticks.filter((t) => t !== mk()).length };
};
for (const f of ['trace.txt', 'trace-holdout.txt']) {
  const ok = diffs(f, () => ours(16 << 8));
  assert.equal(ok.n, 160); assert.equal(ok.bad, 0, `${f} differing`);
  // controls: mid-animation frame (D11) and wrong gold bucket (D5) must differ
  assert.equal(diffs(f, () => ours(1 << 8)).bad, 160, 'mid-frame control');
  assert.equal(diffs(f, () => ours(16 << 8, 50)).bad, 160, 'bucket control');
}
console.log('gold-pile: 0 differing / 160 ticks x2 traces; controls differ');
