import test from 'node:test';
import assert from 'node:assert/strict';
import { plugyStashMode } from '../play/assets/plugy-stash-mode.mjs';

const mode = (overrides = {}) => plugyStashMode({
  enabled: true,
  screen: 'game',
  gameScreen: 'game',
  realmState: 'idle',
  realmSceneActive: false,
  ...overrides,
});

test('PlugY grid activation requires opt-in and an idle local game screen', () => {
  assert.deepEqual(mode(), { activeBigStash: true, singlePlayer: true });
  assert.deepEqual(mode({ enabled: false }), { activeBigStash: false, singlePlayer: true });
  for (const realmState of ['login-required', 'authenticating', 'characters', 'session-open', 'server-disconnected']) {
    assert.deepEqual(mode({ realmState }), { activeBigStash: false, singlePlayer: false });
  }
  assert.deepEqual(mode({ realmSceneActive: true }), { activeBigStash: false, singlePlayer: false });
  assert.deepEqual(mode({ screen: 'title' }), { activeBigStash: false, singlePlayer: false });
});
