import test from 'node:test';
import assert from 'node:assert/strict';
import {
  getAutomapLayer,
  getAutomapRevealState,
  parseAutomapLevelLayers,
  serializeAutomapLayers,
} from '../play/assets/automap-overlay.mjs';

test('automap cells share the retail Levels.txt Layer identity across levels', () => {
  const levels = new TextEncoder().encode([
    'Id\tName\tLayer',
    '1\tRogue Encampment\t0',
    '2\tBlood Moor\t0',
    '8\tDen of Evil\t1',
  ].join('\r\n'));
  const layerByLevel = parseAutomapLevelLayers(levels);

  assert.equal(layerByLevel.get(1), 0);
  assert.equal(layerByLevel.get(2), 0);
  assert.equal(layerByLevel.get(8), 1);
  assert.throws(() => parseAutomapLevelLayers(new Uint8Array()), /Levels.txt is empty/);
  assert.throws(() => parseAutomapLevelLayers(new TextEncoder().encode('Id\tName\n1\tTown')),
    /Levels.txt lacks Id or Layer/);
  assert.throws(() => parseAutomapLevelLayers(new TextEncoder().encode('Id\tLayer\n1\t1oops')),
    /invalid Id\/Layer/);
});

test('automap overlay reuses a shared retail cell layer and serializes its .ma0 identity', () => {
  const levelLayers = new Map([[1, 0], [2, 0], [8, 1]]);
  const layers = new Map(), saved = new Map([[0, [{ frame: 6, x: 16, y: 24, wall: false }]]]);
  let seedCalls = 0;
  const seed = (layer, layerNo) => {
    seedCalls++;
    for (const cell of saved.get(layerNo) || []) {
      layer.set(`saved-${cell.x}-${cell.y}`, { frame: cell.frame, saved: [cell.x, cell.y], layer: cell.wall ? 1 : 0 });
    }
  };

  const townLayer = getAutomapLayer(layers, levelLayers, 1, seed);
  const bloodMoorLayer = getAutomapLayer(layers, levelLayers, 2, seed);
  assert.strictEqual(bloodMoorLayer, townLayer);
  assert.equal(seedCalls, 1, 'shared layer is seeded once');
  bloodMoorLayer.set('5,7,0', {
    frame: 42, world: [25, 35], layer: 0,
  });
  assert.equal(townLayer.get('5,7,0').frame, 42, 'a level 2 cell is visible through level 1’s shared layer');

  assert.deepEqual(serializeAutomapLayers(layers, saved), [
    0, 6, 16, 24, 0,
    0, 42, -16, 48, 0,
  ]);
  assert.throws(() => getAutomapLayer(layers, levelLayers, 99), /no Layer for level 99/);
});

test('automap reveal room state remains isolated per level while the cell layer is shared', () => {
  const revealed = new Map();
  const town = getAutomapRevealState(revealed, 1);
  town.rooms.add('0,0');
  town.hero = '0,0';

  const bloodMoor = getAutomapRevealState(revealed, 2);
  assert.notStrictEqual(bloodMoor, town);
  assert.deepEqual([...bloodMoor.rooms], []);
  assert.equal(bloodMoor.hero, null);
  assert.strictEqual(getAutomapRevealState(revealed, 1), town);
});

test('blank-Id separator row (retail "Expansion") is skipped, not fatal', () => {
  const m = parseAutomapLevelLayers(new TextEncoder().encode('Name\tId\tLayer\nA\t1\t0\nExpansion\t\t\nB\t2\t0'));
  assert.deepEqual([...m], [[1, 0], [2, 0]]);
});
