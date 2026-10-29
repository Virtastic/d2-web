// The WebGL2 presenter (play/assets/gl-present.mjs) only ever SHOWS the 2D canvas; the canvas and every
// read of it stay the parity path. These pin the two guards that keep it so: no webgl2 means no layer,
// and the shell never builds one under automation (the test runner) or beside the render worker's layer.
// The pixel proof (0 diffs on all 2^24 colours, ramp on and off) needs a GPU and was run in Chrome.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { createGlPresenter } from '../play/assets/gl-present.mjs';

test('no webgl2 context: no presenter, nothing added to the page', () => {
  let added = 0;
  const src = { ownerDocument: { createElement: () => ({ getContext: () => null }) }, after: () => { added++; } };
  assert.equal(createGlPresenter(src, new Uint8Array(256)), null);
  assert.equal(added, 0);
});

test('the shell builds the presenter only on the player page, off automation, without the render worker', () => {
  const shell = readFileSync(new URL('../play/gameplay-shell.mjs', import.meta.url), 'utf8');
  const gate = shell.match(/const glPresenter = (.*) \? createGlPresenter\(canvas, DISPLAY_RAMP\) : null;/);
  assert.ok(gate, 'glPresenter declaration');
  for (const g of ['!CSS_RAMP', '!navigator.webdriver', '__d2Entry?.player', '!RENDER_WIRE']) assert.ok(gate[1].includes(g), g);
  // The CPU ramp is skipped only for a live frame the GL layer ramps.
  assert.match(shell, /glRampOn = !movie && liveFrame && !!glPresenter\?\.ok;/);
  assert.match(shell, /if \(!movie && !css && !glRampOn\) \{/);
});
