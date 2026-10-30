import test from 'node:test';
import assert from 'node:assert/strict';
import { keepWhileOpen } from './charcreate-lifetime.mjs';
// Event sequence: open, pick Sorceress, type a name, Cancel, open again -- the second open must be a fresh instance.
const makeScreen = () => ({ chosen: -1, name: '' });
test('create screen reopens fresh after cancel', () => {
  let screen = 'create', inst = null;
  const tick = () => { inst = keepWhileOpen(screen === 'create', inst); if (screen === 'create' && !inst) inst = makeScreen(); };
  tick();
  inst.chosen = 1; inst.name = 'Abc';
  const first = inst;
  screen = 'title'; tick();
  assert.equal(inst, null);
  screen = 'create'; tick();
  assert.notEqual(inst, first);
  assert.deepEqual(inst, { chosen: -1, name: '' });
});
test('stays the same instance while open', () => {
  const a = makeScreen();
  assert.equal(keepWhileOpen(true, a), a);
});
