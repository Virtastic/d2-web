import test from 'node:test';
import assert from 'node:assert/strict';
import { destroyNativeSession, replaceNativeSession } from '../play/assets/session-lifecycle.mjs';
import { installUnloadAutosave } from '../play/assets/unload-autosave.mjs';

test('native session replacement clears renderer state before destroy and create', () => {
  const events = [];
  const rendererState = new Map([[42, { framePrecise: 19 * 256 }]]);
  const session = { destroy() { events.push(['destroy', rendererState.size]); } };
  const renderer = { resetSession() { events.push(['reset', rendererState.size]); rendererState.clear(); } };

  const created = replaceNativeSession(session, renderer, () => {
    events.push(['create', rendererState.size]);
    return { id: 'new-session' };
  });

  assert.deepEqual(created, { id: 'new-session' });
  assert.deepEqual(events, [['reset', 1], ['destroy', 0], ['create', 0]]);
});

test('native session replacement works when renderer has not been initialized', () => {
  const events = [];
  const session = { destroy() { events.push('destroy'); } };
  const created = replaceNativeSession(session, null, () => { events.push('create'); return true; });

  assert.equal(created, true);
  assert.deepEqual(events, ['destroy', 'create']);
});

test('fixture reset and class switch callers use the replacement lifecycle', async () => {
  const fs = await import('node:fs/promises');
  const shell = await fs.readFile(new URL('../play/gameplay-shell.mjs', import.meta.url), 'utf8');
  const lifecycleCalls = shell.match(/replaceNativeSession\(session, monsterRenderer,/g) ?? [];
  assert.equal(lifecycleCalls.length, 2, 'fixture reset and class switch both replace through the lifecycle');
  assert.match(shell, /function resetFixtureFromSelect\(\)[\s\S]*?replaceNativeSession\(session, monsterRenderer,/);
  assert.match(shell, /classSelect\.addEventListener\('change',[\s\S]*?replaceNativeSession\(session, monsterRenderer,/);

  for (const caller of ['fixture reset', 'class switch']) {
    const events = [];
    let liveActors = 1;
    replaceNativeSession({ destroy() { events.push(['destroy', liveActors]); } }, {
      resetSession() { events.push(['reset', liveActors]); liveActors = 0; },
    }, () => { events.push(['create', liveActors]); return true; });
    assert.deepEqual(events, [['reset', 1], ['destroy', 0], ['create', 0]], caller);
  }
});

test('beforeunload saves before caller teardown resets renderer and destroys session', async () => {
  const fs = await import('node:fs/promises');
  const shell = await fs.readFile(new URL('../play/gameplay-shell.mjs', import.meta.url), 'utf8');
  assert.match(shell, /teardown:\s*\(\)\s*=>\s*\{\s*if \(session\) destroyNativeSession\(session, monsterRenderer\);/);

  const events = [];
  let listener;
  const priorAddEventListener = globalThis.addEventListener;
  globalThis.addEventListener = (type, callback) => { assert.equal(type, 'beforeunload'); listener = callback; };
  try {
    installUnloadAutosave({
      shouldSave: () => true,
      saveBytes: () => { events.push('save'); return Uint8Array.of(1); },
      store: { putDuringUnload: () => events.push('store-during-unload'), put: async () => {} },
      onStored() {}, onError(error) { throw error; },
      teardown: () => destroyNativeSession({ destroy: () => events.push('destroy') }, {
        resetSession: () => events.push('reset'),
      }),
    });
    listener();
  } finally {
    globalThis.addEventListener = priorAddEventListener;
  }
  assert.deepEqual(events, ['save', 'store-during-unload', 'reset', 'destroy']);
});

import { leaveNativeSession } from '../play/assets/session-lifecycle.mjs';

test('Save and Exit destroys the session and lets the next pick start a new game', () => {
  const calls = [];
  const session = { destroy: () => calls.push('destroy') };
  const renderer = { resetSession: () => calls.push('reset') };
  const left = leaveNativeSession(session, renderer);
  assert.deepEqual(calls, ['reset', 'destroy']);
  assert.equal(left.session, null);
  assert.equal(left.bootstrapStarted, false);   // startGame() gates on this; true re-entered the old game
  assert.equal(left.currentFixture, null);
  assert.deepEqual(leaveNativeSession(null, renderer).session, null);
  assert.deepEqual(calls, ['reset', 'destroy']);
});

test('the Save and Exit menu choice leaves the game before entering character select', async () => {
  const { readFile } = await import('node:fs/promises');
  const src = await readFile(new URL('../play/gameplay-shell.mjs', import.meta.url), 'utf8');
  assert.match(src, /gameMenu\.close\(\);\s*leaveGame\(\);\s*await enterCharacterSelect\(\);/);
});
