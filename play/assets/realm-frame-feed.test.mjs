import test from 'node:test';
import assert from 'node:assert/strict';
import { applyRealmPlayerFrame, createRealmFrameFeed, REALM_READ_MAX_AGE_MS } from './realm-player-state.mjs';
import { createRealmInput } from './realm-input.mjs';
import { realmFrameReads } from './realm-client-view.mjs';

// A stand-in for the engine's client state: every fed byte moves the units, the 25 Hz tick moves them
// again, and each read builds one record per unit, as the real readers do. `records` counts unit records built.
function fakeEngine(monsterCount) {
  let fed = 0, sum = 0, ticks = 0;
  const cost = { feeds: 0, stateReads: 0, rosterReads: 0, monsterReads: 0, records: 0 };
  const protocol = {
    d2gsClientPlayerFeed: (bytes) => { cost.feeds++; for (const b of bytes) { sum = (sum * 31 + b) >>> 0; fed++; } return bytes.length; },
    d2gsClientPlayerState: () => { cost.stateReads++; cost.records++; return { active: true, guid: 7, x: 100 + (sum & 15) + ticks, y: 200 + fed }; },
    d2gsClientPlayers: () => { cost.rosterReads++; cost.records++; return [{ active: true, guid: 7, x: 100 + (sum & 15) + ticks, y: 200 + fed, reassignments: sum & 3 }]; },
    d2gsClientMonsters: () => { cost.monsterReads++; cost.records += monsterCount;
      return Array.from({ length: monsterCount }, (_, i) => ({ guid: 1000 + i, x: (sum + i * 7 + ticks) & 0xffff, y: (fed + i) & 0xffff, components: Array.from({ length: 16 }, (_, c) => `c${c}`) })); },
    tick: () => { ticks++; },
  };
  return { protocol, cost };
}

// One second of a busy game: 25 game ticks, `perTick` server chunks spread across each, then the shell's
// tick re-read. `feedChunk` is the per-chunk path under test; the views are the shell's three variables.
function play(feedChunk, engine, perTick, clock) {
  let views = { state: null, players: [], monsters: [] };
  const drawn = [];
  for (let tick = 0; tick < 25; tick++) {
    for (let c = 0; c < perTick; c++) {
      clock.ms = tick * 40 + (c * 40) / perTick;
      const chunk = Uint8Array.from([0x0f, tick, c, (tick * 7 + c) & 255]);
      const update = feedChunk(engine.protocol, chunk, views);
      views = { state: update.state, players: update.players, monsters: update.monsters };
    }
    engine.protocol.tick();   // the shell's 25 Hz unit tick, which re-reads every record before the draw
    views = { state: engine.protocol.d2gsClientPlayerState(), players: engine.protocol.d2gsClientPlayers(), monsters: engine.protocol.d2gsClientMonsters() };
    drawn.push(JSON.stringify(views));
  }
  return drawn;
}

test('chunks inside one game tick share one read of the unit records, and every drawn frame is identical', () => {
  const perTick = 12, monsters = 40;
  const before = fakeEngine(monsters), after = fakeEngine(monsters);
  const clock = { ms: 0 };
  const drawnBefore = play((protocol, chunk) => applyRealmPlayerFrame(protocol, chunk), before, perTick, clock);
  const feed = createRealmFrameFeed({ now: () => clock.ms });
  const drawnAfter = play((protocol, chunk, views) => feed.feed(protocol, chunk, { live: true, current: views }), after, perTick, clock);
  assert.deepEqual(drawnAfter, drawnBefore);                         // the same records reach every drawn frame
  assert.equal(after.cost.feeds, before.cost.feeds);                 // every chunk is still fed, in order
  assert.equal(before.cost.feeds, 25 * perTick);
  assert.equal(before.cost.monsterReads, 25 * perTick + 25);         // one read per chunk, plus the tick's
  assert.equal(after.cost.monsterReads, 25 + 25);                    // one per 40 ms window, plus the tick's
  assert.equal(before.cost.records, (25 * perTick + 25) * (monsters + 2));
  assert.equal(after.cost.records, 50 * (monsters + 2));
  assert.deepEqual(feed.stats, { chunks: 25 * perTick, reads: 25 });
});

test('before the scene is live, and once the last read is a tick old, a chunk is read at once', () => {
  const engine = fakeEngine(3); const clock = { ms: 0 };
  const feed = createRealmFrameFeed({ now: () => clock.ms });
  const chunk = Uint8Array.from([1, 2, 3]);
  let u = feed.feed(engine.protocol, chunk, { live: false, current: null });
  assert.equal(u.read, true); assert.equal(u.state.guid, 7); assert.equal(u.applied, 3);
  u = feed.feed(engine.protocol, chunk, { live: false, current: u });
  assert.equal(u.read, true);                                         // joining: every chunk is read
  const held = { state: u.state, players: u.players, monsters: u.monsters };
  clock.ms = 10; u = feed.feed(engine.protocol, chunk, { live: true, current: held });
  assert.equal(u.read, false); assert.equal(u.state, held.state); assert.equal(u.monsters, held.monsters); assert.equal(u.applied, 3);
  clock.ms = REALM_READ_MAX_AGE_MS; u = feed.feed(engine.protocol, chunk, { live: true, current: held });
  assert.equal(u.read, true); assert.notEqual(u.state, held.state);
  u = feed.feed(engine.protocol, chunk, { live: true, current: null });
  assert.equal(u.read, true);                                         // nothing held yet: read
  assert.equal(engine.cost.feeds, 5);
});

test('a rejected chunk throws on both paths', () => {
  const protocol = { d2gsClientPlayerFeed: () => -2, d2gsClientPlayerState: () => null };
  const feed = createRealmFrameFeed({ now: () => 0 });
  assert.throws(() => feed.feed(protocol, Uint8Array.of(1), { live: false }), /rejected/);
  const ok = { d2gsClientPlayerFeed: () => 1, d2gsClientPlayerState: () => ({ active: true }), d2gsClientPlayers: () => [], d2gsClientMonsters: () => [] };
  const held = feed.feed(ok, Uint8Array.of(1), { live: false });
  assert.throws(() => feed.feed(protocol, Uint8Array.of(1), { live: true, current: held }), /rejected/);
});

// The run/walk flag is one engine flag; the R key and the HUD button flip it, the click reads it.
test('flipping the run flag between clicks changes the packet and the local route from walk to run and back', () => {
  let run = false, at = 0;
  const out = [], moves = [];
  const input = createRealmInput({ transmit: (b) => out.push(Array.from(b)), now: () => at,
    move: (x, y, r) => { moves.push([x, y, r]); return 1; },
    scene: () => ({ ready: true, player: { x: 100, y: 100 }, run, selected: { left: 0, right: 36 }, monsters: [], objects: [], items: [],
      isNpc: () => false, hoverBox: () => false, toScreen: (x, y) => ({ x: x * 2, y }), pointerWorld: (px, py) => ({ x: px / 2, y: py }) }) });
  input.leftClick(200, 120);
  run = true; at = 1000; input.leftClick(200, 120);
  at = 2000; input.leftClick(240, 130);
  run = false; at = 3000; input.leftClick(240, 130);
  assert.deepEqual(out, [[0x01, 100, 0, 120, 0], [0x03, 100, 0, 120, 0], [0x03, 120, 0, 130, 0], [0x01, 120, 0, 130, 0]]);
  assert.deepEqual(moves, [[100, 120, false], [100, 120, true], [120, 130, true], [120, 130, false]]);
});

test('a realm frame does not pin the run button: the HUD reads the engine flag', () => {
  assert.equal('runToggle' in realmFrameReads({ stamina: 1 << 8, staminaMax: 1 << 8 }), false);
  assert.equal('runToggle' in realmFrameReads(null), false);
});
