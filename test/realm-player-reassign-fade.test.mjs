import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmPlayerReassignFade, REASSIGN_FADE_MS } from '../play/assets/realm-player-reassign-fade.mjs';

const player = (guid, reassignments, reassignValue = 0) => ({ active: true, guid, reassignments, reassignValue });

test('selected nonzero reassignment fades black from opaque to clear for 500 ms', () => {
  const fade = createRealmPlayerReassignFade();
  const joinedRoster = [player(7, 0)];
  fade.installRoster(joinedRoster);
  assert.equal(fade.drawState(1000, 800, 600), null);

  // This is the first receive-loop observation after createAndJoinConfiguredGame installed its roster.
  const updated = player(7, 1, 1);
  fade.observe([updated], updated, 1200);
  assert.deepEqual(fade.drawState(1200, 800, 600), {
    x: 0, y: 0, width: 800, height: 553, color: '#000', alpha: 255,
  });
  assert.equal(fade.drawState(1450, 800, 600).alpha, 127);
  assert.equal(fade.drawState(1200 + REASSIGN_FADE_MS, 800, 600), null);
});

test('unselected and zero-valued reassignments do not start a fade', () => {
  const fade = createRealmPlayerReassignFade();
  const self = player(7, 0);
  const other = player(8, 0);
  fade.observe([self, other], self, 0);

  const otherUpdated = player(8, 1, 1);
  fade.observe([self, otherUpdated], self, 10);
  const selfUpdated = player(7, 1, 0);
  fade.observe([selfUpdated, otherUpdated], selfUpdated, 20);
  assert.equal(fade.drawState(20, 800, 600), null);
});

test('login reset clears the old fade and installs a clean joined roster baseline', () => {
  const fade = createRealmPlayerReassignFade();
  const self = player(7, 0);
  fade.installRoster([self]);
  const updated = player(7, 1, 1);
  fade.observe([updated], updated, 10);
  assert.notEqual(fade.drawState(10, 800, 600), null);

  // The login-dialog close/new-login path resets the tracker before a fresh D2GS roster arrives.
  fade.reset();
  assert.equal(fade.drawState(20, 800, 600), null);
  fade.installRoster([player(7, 0)]);
  const afterFreshJoin = player(7, 1, 1);
  fade.observe([afterFreshJoin], afterFreshJoin, 30);
  assert.notEqual(fade.drawState(30, 800, 600), null);
});

test('a reassignment consumed during join starts its fade on the first ready render', () => {
  const fade = createRealmPlayerReassignFade();
  fade.installRoster([player(7, 1, 1)]);
  fade.queue();

  assert.equal(fade.drawState(1900, 0, 600), null, 'a non-renderable frame does not consume the queued transition');
  assert.deepEqual(fade.drawState(2000, 800, 600), {
    x: 0, y: 0, width: 800, height: 553, color: '#000', alpha: 255,
  });
  assert.equal(fade.drawState(2250, 800, 600).alpha, 127);
  assert.equal(fade.drawState(2500, 800, 600), null);

  fade.queue();
  fade.reset();
  assert.equal(fade.drawState(3000, 800, 600), null, 'login reset clears an undrawn queued transition');
});
