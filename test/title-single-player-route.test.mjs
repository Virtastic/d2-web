import test from 'node:test';
import assert from 'node:assert/strict';
import { singlePlayerDestination } from '../play/assets/title-single-player-route.mjs';

test('SINGLE PLAYER enters CREATE when the save-file search is empty', () => {
  assert.equal(singlePlayerDestination({ hasSaveFiles: false }), 'create');
});

test('SINGLE PLAYER enters SELECT when a save file exists, even if it is unreadable', () => {
  assert.equal(singlePlayerDestination({ hasSaveFiles: true }), 'select');
});

test('a failed save listing remains a visible SELECT error', () => {
  assert.equal(singlePlayerDestination({ hasSaveFiles: false, error: 'storage unavailable' }), 'error');
});
