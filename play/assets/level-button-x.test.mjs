import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { HUD_LAYOUT } from './control-panel.mjs';
// Retail's own 800x600 frames with both points pending (draw list) put the lit Level.dc6 frame 0 at the grey slots
// (y is the last pixel row, so top = y - h + 1); the drawn rect must be that, not the orb anchors.
test('lit level buttons sit where retail draws them', () => {
  const s = HUD_LAYOUT.levelButtons;
  for (const v of ['charpanel-points', 'skilltree-points']) {
    const rows = readFileSync(new URL(`../../the retail captures${v}.txt`, import.meta.url), 'utf8').split('\n')
      .filter((l) => /Panel\\Level\.dc6 frame=0 .* y=592 /i.test(l))
      .map((l) => Number(/ x=(\d+) /.exec(l)[1]));
    assert.deepEqual(rows, s.xs, v);
  }
  assert.equal(s.y + s.h - 1, 592);
});
