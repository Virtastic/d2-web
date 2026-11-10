import test from 'node:test';
import assert from 'node:assert/strict';
import { realmTerrainFor } from './live-level-terrain.mjs';

// A joined game re-reads its tile list on every room the server reveals or hides. The terrain (decoded
// libraries and tile bitmaps) must survive those re-reads, or the next draw decodes every visible tile again.
test('one terrain serves every map revision of an act, and a new act or a new engine gets its own', () => {
  const engine = {}; let made = 0;
  const make = () => ({ id: ++made, bitmaps: new Map() });
  const first = realmTerrainFor(engine, 0, make);
  first.bitmaps.set('floor:1', 'decoded');
  for (let revision = 0; revision < 50; revision++) assert.equal(realmTerrainFor(engine, 0, make), first);
  assert.equal(made, 1);                                  // 51 revisions, one terrain
  assert.equal(first.bitmaps.get('floor:1'), 'decoded');  // and its decoded tiles are still there
  const act2 = realmTerrainFor(engine, 1, make);
  assert.notEqual(act2, first); assert.equal(made, 2);    // another act is another palette
  assert.equal(realmTerrainFor(engine, 1, make), act2);
  assert.notEqual(realmTerrainFor({}, 1, make), act2);    // another engine instance shares nothing
  assert.equal(made, 3);
});
