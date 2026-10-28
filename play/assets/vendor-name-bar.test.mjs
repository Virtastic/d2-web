// D40: retail's trade window bar under the grid is the literal string "Stash" at (101,434), not the NPC name.
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
const src = readFileSync(new URL('./vendor-panel.mjs', import.meta.url), 'utf8');
for (const scene of ['vendor-akara', 'vendor-malah-act5']) {
  const v = readFileSync(new URL(`../../the retail captures${scene}.txt`, import.meta.url), 'utf8');
  const bar = v.split('\n').filter((l) => /^text .* x=101 y=434 /.test(l));
  assert.deepEqual(bar.map((l) => l.split("'")[1]), ['Stash'], scene);
}
assert.match(src, /string\('stash'\), 101, 434/);
console.log('vendor-name-bar ok');
