import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import vm from 'node:vm';
import { createCharacterEntryReader, singlePlayerDestination } from '../play/assets/title-single-player-route.mjs';

const source = readFileSync(new URL('../play/gameplay-shell.mjs', import.meta.url), 'utf8');
const entryCode = source.slice(source.indexOf('async function enterSavedCharacterScreen('), source.indexOf('// DELETE CHARACTER'));
function deferred() {
  let resolve, reject;
  const promise = new Promise((yes, no) => { resolve = yes; reject = no; });
  return { promise, resolve, reject };
}
function shell() {
  const reads = [];
  const state = {
    screen: 'title', SCREEN: { CREATE: 'create', SELECT: 'select' },
    charSelect: {}, charSelectError: '', savedCharacters: [], savedCharacterFilesExist: false,
    characterEntryReader: createCharacterEntryReader(), singlePlayerDestination,
    savedCharacterReadError: (error) => String(error),
    saveStore: { listProfile() { const read = deferred(); reads.push(read); return read.promise; } },
    ensureCharCreate() {}, ensureCharSelect() {},
  };
  vm.createContext(state); vm.runInContext(entryCode, state);
  return { state, reads };
}

test('a later HOST GAME entry keeps SELECT and its profile when SINGLE PLAYER finishes late', async () => {
  const { state, reads } = shell();
  const single = state.enterSinglePlayer();
  const host = state.enterCharacterSelect();
  const profile = { characters: [{ name: 'Latest' }], hasSaveFiles: true };
  reads[1].resolve(profile);
  assert.equal(await host, true);
  reads[0].resolve({ characters: [], hasSaveFiles: false });
  assert.equal(await single, false);
  assert.equal(state.screen, 'select');
  assert.equal(state.savedCharacters, profile.characters);
  assert.equal(state.savedCharacterFilesExist, true);
});

test('leaving the title while storage is pending prevents a late direct-CREATE transition', async () => {
  const { state, reads } = shell();
  const entry = state.enterSinglePlayer();
  state.screen = 'credits';
  reads[0].resolve({ characters: [], hasSaveFiles: false });
  assert.equal(await entry, false);
  assert.equal(state.screen, 'credits');
});

test('cancelling the pending title action also protects returning to the title before it settles', async () => {
  const { state, reads } = shell();
  const entry = state.enterSinglePlayer();
  state.characterEntryReader.cancel();
  state.screen = 'title';
  reads[0].reject(new Error('old storage failure'));
  assert.equal(await entry, false);
  assert.equal(state.screen, 'title');
  assert.equal(state.charSelectError, '');
});

test('the current empty profile enters CREATE and the current storage error stays on SELECT', async () => {
  const { state, reads } = shell();
  const empty = state.enterSinglePlayer();
  reads[0].resolve({ characters: [], hasSaveFiles: false });
  assert.equal(await empty, true);
  assert.equal(state.screen, 'create');
  state.screen = 'title';
  const failure = state.enterSinglePlayer();
  reads[1].reject(new Error('storage unavailable'));
  assert.equal(await failure, true);
  assert.equal(state.screen, 'select');
  assert.match(state.charSelectError, /storage unavailable/);
});
