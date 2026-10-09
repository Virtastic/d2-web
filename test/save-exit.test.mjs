import test from 'node:test';
import assert from 'node:assert/strict';
import { saveBeforeExit } from '../play/assets/save-exit.mjs';

test('save and exit leaves the game open when persistence returns no saved character', async () => {
  let exited = false;
  const didExit = await saveBeforeExit(async () => null, async () => { exited = true; });
  assert.equal(didExit, false);
  assert.equal(exited, false);
});

test('save and exit leaves the game open when persistence rejects', async () => {
  let exited = false;
  const didExit = await saveBeforeExit(async () => { throw new Error('quota exceeded'); }, async () => { exited = true; });
  assert.equal(didExit, false);
  assert.equal(exited, false);
});

test('save and exit returns to character select only after persistence succeeds', async () => {
  let exitedWith = null;
  const header = { name: 'Amazon', level: 7 };
  const didExit = await saveBeforeExit(async () => header, async (saved) => { exitedWith = saved; });
  assert.equal(didExit, true);
  assert.equal(exitedWith, header);
});
