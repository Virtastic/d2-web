import test from 'node:test';
import assert from 'node:assert/strict';
import { characterCardHitBox } from './charselect-card-input.mjs';
// The card is drawn from (37 + 272*col, 86 + 95*row) (charselect.mjs CARD); a click at its drawn centre must hit that slot,
// through the same letterbox mapping charselect.mjs view()/toScreen() uses.
const view = (w, h) => { const scale = Math.min(w / 800, h / 600); return { scale, ox: Math.round((w - 800 * scale) / 2), oy: Math.round((h - 600 * scale) / 2) }; };
for (const [w, h] of [[800, 600], [1786, 1390]]) test(`click at drawn card centre selects that slot at ${w}x${h}`, () => {
  const v = view(w, h);
  for (let i = 0; i < 8; i++) {
    const cx = 37 + (i % 2) * 272 + 100, cy = 86 + (i >> 1) * 95 + 46;
    const px = Math.round(v.ox + cx * v.scale), py = Math.round(v.oy + cy * v.scale);
    const p = { x: (px - v.ox) / v.scale, y: (py - v.oy) / v.scale };
    const hit = [...Array(8).keys()].filter((j) => { const b = characterCardHitBox(j); return p.x >= b.x && p.x < b.x + b.w && p.y >= b.y && p.y < b.y + b.h; });
    assert.deepEqual(hit, [i]);
  }
});
