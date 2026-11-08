import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync, existsSync } from 'node:fs';
import { deriveWideBg, wideBg, setWideBgEntry } from './widebg.mjs';
const rd = (p) => readFileSync(new URL(p, import.meta.url), 'utf8');
const screens = ['bnet-login-screen', 'bnet-lobby-screen', 'charselect', 'charcreate', 'credits-screen', 'cinematics-screen', 'other-multiplayer', 'frontend'];
test('every front-end screen draws and presents through widebg', () => {
  for (const s of screens) { const t = rd(`./${s}.mjs`); assert.match(t, /import \{[^}]*\bdrawBg, present\b[^}]*\} from '\.\/widebg\.mjs'/, s); assert.match(t, /drawBg\(/, s); assert.match(t, /present\(/, s); }
});
test('stage and serve copy .widebg', () => {
  assert.match(rd('../../ci/stage.sh'), /widebg/); assert.match(rd('../../tools/serve-build.sh'), /\.widebg\//);
});
test('local manifest (when present) covers the screen backgrounds', () => {
  const m = new URL('../../../../../.widebg/manifest.json', import.meta.url);
  if (!existsSync(m)) return;
  const k = Object.keys(JSON.parse(readFileSync(m, 'utf8')));
  for (const n of ['characterselectscreenexp', 'charactercreationscreenexp', 'creditsbckgexpand', 'gameselectscreenexp', 'trademarkscreenexp', 'bnet']) assert.ok(k.includes(n), n);
});

test('the waiting room borrows the Battle.net image outside its own 800x600 and blanks the moss floor', () => {
  const calls = [];
  const canvas = () => { const c = { width: 0, height: 0, id: calls.length, getContext: () => ({ set fillStyle(v) { calls.push(['fillStyle', v]); }, set imageSmoothingEnabled(v) {}, set imageSmoothingQuality(v) {},
    drawImage: (...a) => calls.push(['drawImage', ...a.map((x) => (typeof x === 'object' ? x.tag ?? 'canvas' : x))]), fillRect: (...a) => calls.push(['fillRect', ...a]) }) }; return c; };
  const doc = { createElement: canvas };
  const waiting = { tag: 'waiting' };
  deriveWideBg('data\\global\\ui\\BIGMENU\\waitingroombkgd.dc6', 'data\\global\\ui\\BIGMENU\\bnet.dc6', waiting, { documentObject: doc, blankBelow: 494 });
  assert.equal(wideBg('data\\global\\ui\\BIGMENU\\waitingroombkgd.dc6'), null, 'no Battle.net image loaded: the retail DC6 stays');
  setWideBgEntry('bnet', { w: 2752, h: 1536, s: 2.56, ox: 352, oy: 0, img: { tag: 'bnetjpg' }, bg800: null });
  const e = wideBg('data\\global\\ui\\BIGMENU\\WaitingRoomBkgd.dc6');
  assert.ok(e && e.w === 2752 && e.h === 1536 && e.s === 2.56 && e.ox === 352 && e.oy === 0 && e.bg800 === null);
  assert.equal(wideBg('waitingroombkgd.dc6'), e, 'built once');
  assert.deepEqual(calls[0], ['drawImage', 'bnetjpg', 0, 0]);
  assert.deepEqual(calls.filter((c) => c[0] === 'fillRect').map((c) => c.map((v) => (typeof v === 'number' ? Math.round(v) : v))), [['fillRect', 40, 1265, 313, 231], ['fillRect', 2399, 1265, 313, 231]]);
  assert.deepEqual(calls.at(-1), ['drawImage', 'waiting', 352, 0, 2048, 1536]);
});
