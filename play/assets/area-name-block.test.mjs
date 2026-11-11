import test from 'node:test';
import assert from 'node:assert/strict';
import { areaNameBlockLines, rightAlignedX } from './area-name-block.mjs';

// These records are retail's: the parity harness draws `v 1.13` (build-version.mjs, entry.js).
globalThis.__d2Entry = { versionLabel: 'retail' };
// font16 advances of the four retail records (a3-forest-walkin, f5b-automap-town), from the header.
const ADV = { 'Spider Forest': 100, 'Rogue Encampment': 141, 'v 1.13': 41, EXPANSION: 84 };
const measure = (t) => ADV[t];
const place = (w, state) => areaNameBlockLines(state).map(({ text, y }) => [text, rightAlignedX(measure, text, w), y]);

test('800x600 reproduces retail\'s records: x = 800 - width - 16, rows 24/40/56', () => {
  assert.deepEqual(place(800, { name: 'Spider Forest' }),
    [['Spider Forest', 684, 24], ['v 1.13', 743, 40], ['EXPANSION', 700, 56]]);
  assert.deepEqual(place(800, { name: 'Rogue Encampment' })[0], ['Rogue Encampment', 643, 24]);
});

test('the right edge follows the screen width (DAT_6fba9e14 - 16), not 784', () => {
  assert.deepEqual(place(640, { name: 'Spider Forest' }).map((l) => l[1]), [524, 583, 540]);
  assert.deepEqual(place(2000, { name: 'Spider Forest' }).map((l) => l[1]), [1884, 1943, 1900]);
});

test('line selection and order follow D2Client+0x62680', () => {
  const keys = (s) => areaNameBlockLines(s).map((l) => `${l.key}@${l.y}`);
  assert.deepEqual(keys({ name: 'A', expansion: false }), ['name@24', 'version@40']);
  assert.deepEqual(areaNameBlockLines({ name: 'A', difficulty: 1 })[2], { key: 'difficulty', text: 'Difficulty: Nightmare', y: 56 });
  assert.deepEqual(areaNameBlockLines({ name: 'A', difficulty: 2 })[2].text, 'Difficulty: Hell');
  assert.deepEqual(keys({ name: 'A', gameName: 'g', password: 'p', difficulty: 2, address: '1.2.3.4' }),
    ['game@24', 'password@40', 'name@56', 'version@72', 'difficulty@88', 'address@104', 'expansion@120']);
  assert.equal(areaNameBlockLines({ gameName: 'g' })[0].text, 'Game: g');
  // 0x5FE90 advances the cursor even when it draws no version.
  assert.deepEqual(keys({ name: 'A', version: '' }), ['name@24', 'expansion@56']);
});
