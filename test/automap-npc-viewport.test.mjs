import test from 'node:test';
import assert from 'node:assert/strict';
import { automapNpcsInViewport, automapViewport } from '../play/assets/automap-overlay.mjs';

test('mini map culls NPC markers outside the inclusive viewport (0x6fb119e0)', () => {
  const { viewport } = automapViewport(800, 600, { mode: 'mini', left: true });
  const npcs = [{ sx: 100, sy: 200 }, { sx: 700, sy: 200 }, { sx: -16, sy: 103 }, { sx: 258, sy: 303 }, { sx: 100, sy: 102 }];
  assert.deepEqual(automapNpcsInViewport(npcs, viewport).map((n) => n.sx), [100, -16, 258]);
});
