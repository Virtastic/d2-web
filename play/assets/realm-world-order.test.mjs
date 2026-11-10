import test from 'node:test';
import assert from 'node:assert/strict';
import { createLoadCompleteOrder, worldDownScreen } from './realm-world-order.mjs';
import { createJoinLoading } from './bnet-join-loading.mjs';

// The world flags the client keeps of 0x03 (load act), 0x04 (load complete), 0x05 (unload) and 0x07 (room).
function world() {
  const w = { hasLoadAct: false, loadCompleteEvent: false, unloadCompleteEvent: false, hasMapReveal: false };
  return { w, feed(bytes) { for (const op of bytes) {
    if (op === 0x03) { w.hasLoadAct = true; w.loadCompleteEvent = false; w.hasMapReveal = false; }
    else if (op === 0x04) { w.loadCompleteEvent = true; w.unloadCompleteEvent = false; }
    else if (op === 0x05) { w.loadCompleteEvent = false; w.unloadCompleteEvent = true; }
    else if (op === 0x07) w.hasMapReveal = true;
  } } };
}
const down = (w) => w.unloadCompleteEvent || !w.hasLoadAct || !w.hasMapReveal || !w.loadCompleteEvent;

test('an act change shows the loading screen from the unload to the load complete, never the title', () => {
  const g = world(), loading = createJoinLoading({ now: () => 0 }), shown = [];
  const step = (bytes) => {
    g.feed(bytes);
    if (down(g.w)) { const s = worldDownScreen({ sessionOpen: true, hasLoadAct: g.w.hasLoadAct }); if (s === 'loading' && !loading.shown) loading.start(); shown.push(s); }
    else { loading.ready(); shown.push('game'); }
  };
  step([0x03, 0x07, 0x04]);   // the join: Act I up
  step([0x05, 0x03]);         // the server's act change: unload and load act in one tick
  step([0x07]);               // the new act's rooms
  assert.equal(loading.shown, true);
  step([0x04]);               // load complete a tick later
  assert.deepEqual(shown, ['game', 'loading', 'loading', 'game']);
  assert.equal(loading.shown, false);
});

test('a world that is down with no act loading, or with the session closed, is the title', () => {
  assert.equal(worldDownScreen({ sessionOpen: true, hasLoadAct: false }), 'title');
  assert.equal(worldDownScreen({ sessionOpen: false, hasLoadAct: true }), 'title');
  assert.equal(worldDownScreen(), 'title');
});

test('a load complete ahead of the player assignment is held until the assignment', () => {
  const order = createLoadCompleteOrder((b) => (b[0] === 0x59 ? 2 : 1));
  assert.deepEqual(Array.from(order.filter(Uint8Array.of(0x04, 0x03), false)), [0x03]);
  assert.deepEqual(Array.from(order.filter(Uint8Array.of(0x59, 0), false)), [0x59, 0, 0x04]);
});
