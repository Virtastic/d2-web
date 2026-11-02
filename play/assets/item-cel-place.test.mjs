import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import { celAt, gridCelAt } from './item-cel-place.mjs';

// Retail (D2Client+0x6B5A0 / +0x95A54): a grid item is anchored at its top-left cell corner, never
// centred by item size against cell size; the art is one row below the anchor.
test('grid cel anchors at the cell corner, one row down, never centred', () => {
  for (const [w, h] of [[29, 29], [31, 31]])
    for (const [gx, gy] of [[0, 0], [3, 2], [9, 3]])
      assert.deepEqual(gridCelAt(418, 315, w, h, gx, gy), { left: 418 + gx * w, top: 315 + gy * h + 1 });
  assert.deepEqual(celAt(10, 20, { offsetX: 2, offsetY: -3 }), { left: 12, top: 18 });
});

// Every grid renderer (inventory, stash, cube, trade, vendor, belt) goes through the shared rule.
test('every item grid renderer places through item-cel-place', () => {
  for (const f of ['inventory-panel', 'stash-panel', 'npc-panel', 'vendor-panel', 'belt-renderer'])
    assert.match(readFileSync(new URL(`./${f}.mjs`, import.meta.url), 'utf8'), /item-cel-place\.mjs/, f);
});

// The cube window draws its own art (D2Client+0x8CB40 loads ui\panel\supertransmogrifier, +0x99440 state 0xE
// draws frames 0..3 at the left-panel anchors (80,316) (336,316) (80,492) (336,492), bottom rows, then the page-3
// grid). Instrument: the art's printed cell separators (bright columns/rows over the grid) against the grid record's
// cell edges (Transmogrify Box2: 198,199, 3x4 of 29). Under TradeStash they sat 15 px off; under this art, 0.
const RETAIL = new URL('../../build-wasm/retail/', import.meta.url);
const haveRetail = existsSync(new URL('d2data.mpq', RETAIL));
test('cube window loads supertransmogrifier and its printed cells sit on the cube grid', { skip: !haveRetail && 'no retail MPQs' }, async () => {
  const { createStashPanelRenderer, CUBE_ART, STASH_ART, INVPAGE_CUBE, INVPAGE_STASH } = await import('./stash-panel.mjs');
  const { openMpq } = await import('../../tools/mpq-node.mjs');
  const { decodeDc6, decodeDatPalette } = await import('./d2-formats.mjs');
  const qs = ['patch_d2.mpq', 'd2exp.mpq', 'd2data.mpq'].map((m) => openMpq(new URL(m, RETAIL).pathname));
  const read = (n) => { for (const q of qs) { const b = q.read(n); if (b) return b; } return null; };
  const asked = (page) => { const a = []; try { createStashPanelRenderer((n) => (a.push(n), read(n)), {}, { page }); } catch {} return a; };
  assert.ok(asked(INVPAGE_CUBE).includes(CUBE_ART) && !asked(INVPAGE_CUBE).includes(STASH_ART));
  assert.ok(asked(INVPAGE_STASH).includes(STASH_ART));
  const pal = decodeDatPalette(read('data\\global\\palette\\ACT1\\pal.dat'));
  const offBy = (path) => {
    const d = decodeDc6(read(path)), S = new Float32Array(800 * 600).fill(-1);
    [[80, 316], [336, 316], [80, 492], [336, 492]].forEach(([x, y], i) => {
      const f = d.frames[i], top = y - f.height + 1;
      for (let r = 0; r < f.height; r++) for (let c = 0; c < f.width; c++) {
        const k = r * f.width + c, p = f.indices[k] * 4;
        if (f.opaque[k]) S[(top + r) * 800 + x + c] = pal[p] + pal[p + 1] + pal[p + 2];
      }
    });
    const [gl, gt, cols, rows] = [198, 199, 3, 4];
    const col = (x) => { let s = 0; for (let y = gt; y < gt + rows * 29; y++) s += S[y * 800 + x]; return s; };
    // The brightest column within each interior cell edge's +-14 window, against the edge's own column (edge - 1).
    let worst = 0;
    for (let e = 1; e < cols; e++) {
      const edge = gl + e * 29 - 1; let best = edge;
      for (let x = edge - 14; x <= edge + 14; x++) if (col(x) > col(best)) best = x;
      worst = Math.max(worst, Math.abs(best - edge));
    }
    return worst;
  };
  assert.ok(offBy(STASH_ART) >= 10, 'TradeStash under the cube grid is the defect this pins');
  assert.ok(offBy(CUBE_ART) <= 1);
});
