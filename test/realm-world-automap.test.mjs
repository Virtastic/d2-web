import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmAutomapSource } from '../play/assets/realm-world-automap.mjs';

test('rooms come from the client rooms behind the revealed tiles, one per room, by level', () => {
  const tiles = [{ room: 1, worldX: 10, worldY: 20 }, { room: 1, worldX: 11, worldY: 20 }, { room: 2, worldX: 30, worldY: 20 }];
  const calls = [];
  const info = { '50,100': { level: 1, tileX: 10, tileY: 20, tileWidth: 8, tileHeight: 8 }, '150,100': { level: 2, tileX: 30, tileY: 20, tileWidth: 4, tileHeight: 6 } };
  const src = createRealmAutomapSource({ tiles: () => tiles, roomInfo: (x, y) => { calls.push(`${x},${y}`); return info[`${x},${y}`] ?? null; },
    levelType: (l) => l + 100, currentLevel: () => 1 });
  assert.deepEqual(src.rooms(1), [{ x: 50, y: 100, w: 40, h: 40 }]);
  assert.deepEqual(src.rooms(2), [{ x: 150, y: 100, w: 20, h: 30 }]);
  assert.deepEqual(src.rooms(3), []);
  assert.deepEqual(calls, ['50,100', '150,100']);   // read once per room, then cached
  tiles.push({ room: 3, worldX: 99, worldY: 99 });
  assert.deepEqual(src.rooms(1), [{ x: 50, y: 100, w: 40, h: 40 }]);
  assert.equal(src.levelType(), 101);
  assert.deepEqual(src.levelRect(1), [0, 0, 0, 0]);
  assert.equal(src.pickedFile(40), null);
});
