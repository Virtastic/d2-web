import test from 'node:test';
import assert from 'node:assert/strict';
import { createJoinLoading, JOIN_LOADING_TIMEOUT_MS } from './bnet-join-loading.mjs';

test('click -> loading shown -> world ready -> game scene', () => {
  let t = 0; const l = createJoinLoading({ now: () => t });
  assert.equal(l.shown, false);
  l.start(); assert.equal(l.shown, true);
  t = 5000; assert.ok(l.progress() > 0 && l.progress() < 1);
  assert.equal(l.ready(), true); assert.equal(l.shown, false);
  assert.equal(l.ready(), false);
});
test('failure returns to the lobby', () => {
  const l = createJoinLoading({ now: () => 0 });
  l.start(); assert.equal(l.fail(), true); assert.equal(l.shown, false); assert.equal(l.fail(), false);
});
test('the wait ends at three minutes', () => {
  let t = 0; const l = createJoinLoading({ now: () => t });
  l.start(); t = JOIN_LOADING_TIMEOUT_MS - 1; assert.equal(l.expired(), false);
  t = JOIN_LOADING_TIMEOUT_MS; assert.equal(l.expired(), true);
});
