import test from 'node:test';
import assert from 'node:assert/strict';
import { dispatchSkillSelectHit } from './hud-hit-test.mjs';
import { createRealmNpcScreen } from './realm-npc-screen.mjs';

// The shell passes the client view's liveness: the engine session offline, the joined game's handle online.
// Both are truthy while a game is up, and the dispatch must not tell them apart.
const LIVE = { offline: { engine: 'session' }, online: { realm: 'live' }, none: null };

function selector() {
  const picks = [];
  const renderer = { open: null,
    hitTest: (x) => (x === 1 ? { button: 'left' } : x === 2 ? { skill: { id: 36 }, side: 'left' } : null) };
  return { renderer, picks, select: (id, side) => picks.push([id, side]) };
}

test('skill selector: open and pick give the same outcome with an engine session and with a joined game', () => {
  const outcomes = {};
  for (const [name, live] of Object.entries({ offline: LIVE.offline, online: LIVE.online })) {
    const s = selector();
    const log = [];
    log.push(dispatchSkillSelectHit(s.renderer, live, 1, 0, 800, 600, [], s.select), s.renderer.open);   // the socket opens the popup
    log.push(dispatchSkillSelectHit(s.renderer, live, 2, 0, 800, 600, [], s.select), s.renderer.open);   // an icon picks and closes
    log.push(dispatchSkillSelectHit(s.renderer, live, 3, 0, 800, 600, [], s.select));                       // elsewhere falls through
    outcomes[name] = { log, picks: s.picks };
  }
  assert.deepEqual(outcomes.online, outcomes.offline);
  assert.deepEqual(outcomes.online, { log: [true, 'left', true, null, false], picks: [[36, 'left']] });
});

test('skill selector: no game up consumes nothing', () => {
  const s = selector();
  assert.equal(dispatchSkillSelectHit(s.renderer, LIVE.none, 1, 0, 800, 600, [], s.select), false);
  assert.equal(s.renderer.open, null);
});

test('shop: a joined game draws the overlay (tooltip, hint) with the state the window was drawn with, and only while it is up', () => {
  const seen = [];
  const vendor = { reset() {}, draw: (_c, _w, _h, st) => { seen.push(['draw', st.stashGold, st.items[0]?.cost, st.items[0]?.name]); return {}; },
    drawOverlay: (_c, _w, _h, st) => { seen.push(['overlay', st.stashGold, st.items[0]?.cost]); return { tooltip: null, hint: null }; } };
  let view = { guid: 9, classId: 154, mode: 'trade', stock: [{ guid: 5, bits: 'b', tab: 1, x: 0, y: 0 }], sellMode: false, repairMode: false };
  const itemTable = { byCode: () => ({ invFile: 'f', invWidth: 1, invHeight: 1 }) };
  const screen = createRealmNpcScreen({ ui: {}, view: () => view, menu: { open: false, close() {} }, vendor, speech: {}, itemTable,
    itemDetail: () => ({ name: 'Potion', cost: 50 }), stashGold: () => 700 });
  assert.equal(screen.drawOverlay({}, 800, 600), null);
  screen.draw({}, 800, 600);
  screen.drawOverlay({}, 800, 600);
  assert.deepEqual(seen.slice(-2), [['draw', 700, undefined, undefined], ['overlay', 700, undefined]]);   // item fields: realm-npc-ui.test.mjs
  view = null;
  screen.draw({}, 800, 600);
  assert.equal(screen.drawOverlay({}, 800, 600), null);
});
