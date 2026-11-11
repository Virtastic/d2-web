import test from 'node:test';
import assert from 'node:assert/strict';
import { pagesToSss, sssToPages, pagesToD2x, d2xToPages } from './plugy-stash.mjs';

// A synthetic item record: 'J' 'M' is the section tag, so an item is five bytes here and itemSize reads it back.
const itemSize = () => 5;
const section = (items) => Uint8Array.from([0x4a, 0x4d, items.length & 255, items.length >> 8, ...items.flat()]);
const items = (sec) => { const out = []; for (let k = sec[2] | (sec[3] << 8), o = 4; k > 0; k--, o += 5) out.push(Array.from(sec.subarray(o, o + 5))); return out; };

test('open the shared stash, move an item to another page, save the .sss, reload: the item and gold are where they were left', () => {
  const a = [1, 2, 3, 4, 5], b = [9, 8, 7, 6, 5];
  let pages = [section([a, b]), section([])];
  // move item b from page 0 to page 1 (the engine's clear + insert pair in the browser)
  const moved = items(pages[0]).pop();
  pages = [section(items(pages[0]).slice(0, -1)), section([...items(pages[1]), moved])];
  const saved = pagesToSss(pages, { gold: 1234, meta: [{ flags: 0, name: 'Gems' }, { flags: 1, name: 'Runes' }], itemSize });
  const back = sssToPages(saved, itemSize);
  assert.equal(back.gold, 1234);
  assert.deepEqual(back.meta.map((m) => m.name), ['Gems', 'Runes']);
  assert.deepEqual(items(back.pages[0]), [a]);
  assert.deepEqual(items(back.pages[1]), [b]);
  assert.deepEqual(pagesToSss(back.pages, { gold: 1234, meta: back.meta, itemSize }), saved);   // reload then save is byte-identical
});

test('the personal .d2x keeps its pages and the live page across a reload', () => {
  const pages = [section([[1, 1, 1, 1, 1]]), section([[2, 2, 2, 2, 2]]), section([])];
  const saved = pagesToD2x(pages, { page: 2, shared: false }, { itemSize });
  const back = d2xToPages(saved, itemSize);
  assert.deepEqual(back.live, { page: 2, shared: false });
  assert.deepEqual(back.pages.map((p) => items(p).length), [1, 1, 0]);
  assert.deepEqual(pagesToD2x(back.pages, back.live, { meta: back.meta, itemSize }), saved);
});
