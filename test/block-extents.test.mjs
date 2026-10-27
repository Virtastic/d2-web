import test from 'node:test';
import assert from 'node:assert/strict';
import { blockExtents } from '../play/assets/session-terrain.mjs';
test('blockExtents matches the per-call some() it replaced', () => {
  let s = 12345; const r = () => (s = (s * 1103515245 + 12345) & 0x7fffffff) % 200 - 100;
  for (let n = 0; n < 300; n++) {
    const blocks = Array.from({ length: n % 9 }, () => ({ x: r(), y: r(), format: (r() & 1) + 0 }));
    const e = blockExtents(blocks);
    assert.equal(blockExtents(blocks), e);
    for (const [tx, ty, ox, oy] of [[0, 0, 0, 0], [r(), r(), r(), r()], [-5, 3, 40, 80]]) {
      assert.equal(tx + e.minX - ox < 64, blocks.some((b) => tx + b.x - ox < 64));
      assert.equal(ty + e.minY1 - oy < 0, blocks.some((b) => b.format === 1 && ty + b.y - oy < 0));
    }
  }
});
