import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync, existsSync } from 'node:fs';
const rd = (p) => readFileSync(new URL(p, import.meta.url), 'utf8');
const screens = ['charselect', 'charcreate', 'credits-screen', 'cinematics-screen', 'other-multiplayer', 'frontend'];
test('every front-end screen draws and presents through widebg', () => {
  for (const s of screens) { const t = rd(`./${s}.mjs`); assert.match(t, /import \{ drawBg, present \} from '\.\/widebg\.mjs'/, s); assert.match(t, /drawBg\(/, s); assert.match(t, /present\(/, s); }
});
test('stage and serve copy .widebg', () => {
  assert.match(rd('../../ci/stage.sh'), /widebg/); assert.match(rd('../../tools/serve-build.sh'), /\.widebg\//);
});
test('local manifest (when present) covers the screen backgrounds', () => {
  const m = new URL('../../../../../.widebg/manifest.json', import.meta.url);
  if (!existsSync(m)) return;
  const k = Object.keys(JSON.parse(readFileSync(m, 'utf8')));
  for (const n of ['characterselectscreenexp', 'charactercreationscreenexp', 'creditsbckgexpand', 'gameselectscreenexp', 'trademarkscreenexp']) assert.ok(k.includes(n), n);
});
