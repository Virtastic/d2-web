import test from 'node:test';
import assert from 'node:assert/strict';
import { createFrontendHoverSync } from '../play/assets/frontend-hover-sync.mjs';

// Screens model the real ones: hover() stores the pointer in 800x600 design coords through the
// letterbox view (scale = min(w/800, h/600), centred), truncated, as frontend.mjs/charselect.mjs do.
const W = 1920, H = 1200;
function design(x, y) {
  const scale = Math.min(W / 800, H / 600);
  const ox = Math.round((W - 800 * scale) / 2), oy = Math.round((H - 600 * scale) / 2);
  return [Math.trunc((x - ox) / scale), Math.trunc((y - oy) / scale)];
}
const makeScreen = () => ({ pointer: null, hover(x, y) { this.pointer = design(x, y); } });
const cur = (s) => ({ key: s, hover: (x, y) => s.hover(x, y) });

test('title, single player, back: the cursor stays under the pointer every cycle', () => {
  const sync = createFrontendHoverSync();
  const title = makeScreen();
  let screen = 'title', select = null;
  const current = () => (screen === 'title' ? cur(title) : select ? cur(select) : null);
  const moves = [[1000, 400], [300, 900], [1500, 150]];   // the mouse moves during each visit
  let last = [960, 600];
  sync.move(...last, current());
  for (let i = 0; i < 12; i++) {
    screen = 'select'; select = makeScreen();               // SINGLE PLAYER builds a fresh SELECT
    sync.frame(current());
    assert.deepEqual(select.pointer, design(...last), `select entry ${i}`);
    last = moves[i % 3];
    sync.move(...last, current());                          // the mouse moves on SELECT only
    assert.deepEqual(select.pointer, design(...last));
    screen = 'title';                                       // CANCEL back to the kept title
    sync.frame(current());
    assert.deepEqual(title.pointer, design(...last), `title cursor after return ${i}`);
  }
});

test('no pointer is invented before the first move, then a new screen is seeded', () => {
  const sync = createFrontendHoverSync();
  const a = makeScreen();
  assert.equal(sync.frame(cur(a)), false);
  assert.equal(a.pointer, null);
  sync.move(100, 100, null);
  const b = makeScreen();
  assert.equal(sync.frame(cur(b)), true);
  assert.deepEqual(b.pointer, design(100, 100));
});
