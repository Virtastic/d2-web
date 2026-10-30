import test from 'node:test';
import assert from 'node:assert/strict';
import { createPlugyStash, pageTarget, MAX_PAGE } from '../play/assets/plugy-stash-ui.mjs';

test('prev stops at the first page, next grows without wrapping', () => {
  assert.equal(pageTarget('prev', 0), 0);
  assert.equal(pageTarget('prev', 5), 4);
  assert.equal(pageTarget('next', 5), 6);
  assert.equal(pageTarget('next', MAX_PAGE), MAX_PAGE);
});

test('shift: prev is the first page, next the last used', () => {
  assert.equal(pageTarget('prev', 7, true), 0);
  assert.equal(pageTarget('next', 2, true, 9), 9);
  assert.equal(pageTarget('next', 12, true, 9), 12);
});

test('index buttons jump to the previous / next multiple of 10 (100 with shift)', () => {
  assert.equal(pageTarget('nextIndex', 0), 9);
  assert.equal(pageTarget('nextIndex', 9), 19);
  assert.equal(pageTarget('prevIndex', 25), 19);
  assert.equal(pageTarget('prevIndex', 3), 0);
  assert.equal(pageTarget('nextIndex', 0, true), 99);
  assert.equal(pageTarget('prevIndex', 150, true), 99);
});

test('go() turns pages, grows the count, refuses on a false onPage, and each set keeps its page', () => {
  const calls = []; let refuse = false;
  const s = createPlugyStash({ enabled: true, onPage: (...a) => { calls.push(a); return !refuse; } });
  assert.equal(s.go(3), true);
  assert.equal(s.page, 3); assert.equal(s.pages, 4);
  assert.deepEqual(calls[0], [0, 3, false, false]);
  s.go(5, true);   // to the shared set
  assert.deepEqual([s.page, s.showShared, s.mem[false]], [5, true, 3]);
  refuse = true;
  assert.equal(s.go(1, false), false);
  assert.deepEqual([s.page, s.showShared], [5, true]);
  refuse = false;
  s.go(s.mem[false], false);
  assert.deepEqual([s.page, s.showShared], [3, false]);
});

test('the shared-gold buttons call onGold with put / take and never turn a page', () => {
  const calls = [], pages = [];
  const s = createPlugyStash({ enabled: true, onGold: (k) => calls.push(k), onPage: (...a) => pages.push(a) });
  s.setArt({ stash: Array(12).fill({}), gold: Array(4).fill({}) });
  const g = { left: 0, top: 0 };
  const at = (b) => [s.BTN[b].x + 5, s.BTN[b].y + 5];
  assert.equal(s.click(...at('putGold'), 800, 600, g, true), true);
  assert.equal(s.click(...at('takeGold'), 800, 600, g, true), true);
  assert.deepEqual(calls, ['put', 'take']);
  assert.deepEqual(pages, []);
});
