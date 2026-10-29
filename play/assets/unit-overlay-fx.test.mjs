import { test } from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import { createUnitOverlayFx } from './unit-overlay-fx.mjs';
const enc = (s) => new TextEncoder().encode(s);
// Overlay.txt rows are indexed from 0 (row 0 = 'null'); frozenarmor_hit is row 42 in the retail table.
const pad = (n) => Array.from({ length: n }, (_, i) => `pad${i}\tnull\t0\t2\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t16\t0\t3\n`).join('');
const header = 'overlay\tFilename\tversion\tFrames\tCharacter\tPreDraw\t1ofN\tDir\tOpen\tBeta\tXoffset\tYoffset\tHeight1\tHeight2\tHeight3\tHeight4\tAnimRate\tLoopWaitTime\tTrans\n';
const files = {
  'data\\global\\excel\\Overlay.txt': enc(header + 'null\tnull\t0\t2\n' + pad(41) + 'frozenarmor_hit\tAuraResistColdCast\t0\t11\tall\t1\t1\t0\t1\t1\t0\t0\t0\t0\t0\t0\t24\t0\t3\n'),
};
const fx = createUnitOverlayFx((p) => files[p]);
const HIT = (overlayId, id = 77, type = 1) => [{ type, id, overlayId }];

// Frozen Armor hit struck at tick 1: attach at tick 1, frame = floor(3k/2) for k = tick - 1 (AnimRate 24 << 4 = 384/256 = 1.5 frames per tick, D2Client 0x6B300).
let frames = [];
for (let tick = 1; tick <= 130; tick++) {
  const e = fx.update(tick, HIT(42));
  frames.push(e.length ? [e[0].overlay, e[0].age] : null);
}
for (let k = 0; k <= 7; k++) {
  assert.deepEqual(frames[k], [42, Math.floor(3 * k / 2)], `tick k=${k}`);
}
assert.equal(frames[1][1], 1, 'frame 1 begins at k=1');
assert.equal(frames[0][1], 0, 'frame 0 at k=0');
assert.equal(frames[8], null, 'one-shot: gone at k=8 (frame 12 >= Frames 11)');
assert.ok(frames.slice(8).every((f) => f === null), 'stays gone while the id is still on the unit');

// Effect shape feeds the overlay drawer: file, frames, trans, predraw.
fx.update(200, []);
const first = fx.update(201, HIT(42, 9))[0];
assert.deepEqual([first.file, first.frames, first.trans, first.predraw, first.age], ['AuraResistColdCast', 11, 3, true, 0]);

// A different id on the same unit restarts it; the same id does not.
fx.update(300, HIT(42, 5));
assert.equal(fx.update(304, HIT(42, 5))[0].age, Math.floor(4 * 24 * 16 / 256), 'same id: no re-attach');
assert.equal(fx.update(310, HIT(42, 5, 0)).length, 1, 'other type, same id: separate unit');
assert.equal(fx.update(311, HIT(0, 5))[0] ?? null, null, 'null overlay (row 0) draws nothing');

// Real retail table, when present: row 42 is the Frozen Armor hit.
const RETAIL = '/srv/games/d2/tables-1.13c/Overlay.txt';
if (existsSync(RETAIL)) {
  const real = createUnitOverlayFx((p) => p === 'data\\global\\excel\\Overlay.txt' ? new Uint8Array(readFileSync(RETAIL)) : (() => { throw new Error(p); })());
  const e = real.update(1, [{ type: 1, id: 1, overlayId: 42 }])[0];
  assert.deepEqual([e.file, e.frames, e.rate, e.trans, e.predraw], ['AuraResistColdCast', 11, 384, 3, true]);
}
console.log('unit-overlay-fx ok');
