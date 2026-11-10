import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmInput } from './realm-input.mjs';
import { realmHeroFlags, realmCorpseViews } from './realm-death.mjs';
import { deathScreenShown, respawnAllowed } from './death-screen.mjs';

const HERO = 0x01020304, CORPSE = 0x0a0b0c0d, OTHER = 0x55, OTHER_CORPSE = 0x66;
const le = (v) => [v & 255, (v >>> 8) & 255, (v >>> 16) & 255, (v >>> 24) & 255];

test('the hero flags come from the character list', () => {
  const list = [{ name: 'Soft', flags: 0x20 }, { name: 'Hard', flags: 0x24 | 0x08 }, { name: 'Classic', flags: 0 }];
  assert.equal(realmHeroFlags(list, 'soft'), 0x20);
  assert.equal(realmHeroFlags(list, 'HARD'), 0x24);
  assert.equal(realmHeroFlags(list, 'Classic'), 0);
  assert.equal(realmHeroFlags(list, 'Nobody'), 0x20);
  assert.equal(realmHeroFlags(null, undefined), 0x20);
});

test('corpse views join the corpse list to the player units in view', () => {
  const players = [{ guid: CORPSE, x: 120, y: 100, renderState: { mode: 17 } }, { guid: OTHER, x: 90, y: 90, renderState: { mode: 1 } },
    { guid: OTHER_CORPSE, x: 91, y: 91, renderState: { mode: 17 } }];
  const pairs = [{ owner: HERO, corpse: CORPSE }, { owner: OTHER, corpse: OTHER_CORPSE }, { owner: OTHER, corpse: 0x77 }];
  assert.deepEqual(realmCorpseViews(pairs, players, HERO), [
    { guid: CORPSE, owner: HERO, own: true, mode: 17, x: 120, y: 100 },
    { guid: OTHER_CORPSE, owner: OTHER, own: false, mode: 17, x: 91, y: 91 }]);
  assert.deepEqual(realmCorpseViews(undefined, players, HERO), []);
  assert.deepEqual(realmCorpseViews(pairs, [], HERO), []);
});

test('die, respawn, walk back, loot the corpse: the packets sent', () => {
  const out = [], moves = [];
  const players = [{ guid: CORPSE, x: 120, y: 100, renderState: { mode: 17 } }, { guid: OTHER_CORPSE, x: 60, y: 100, renderState: { mode: 17 } }];
  let pairs = [], me = { x: 120, y: 100 }, mode = 1;
  const scene = () => ({ ready: true, player: me, run: false, selected: { left: 0, right: 36 }, monsters: [], objects: [], items: [],
    isNpc: () => false, hoverBox: (dx, dy) => Math.abs(dx) < 10 && Math.abs(dy) < 20,
    toScreen: (x, y) => ({ x: x * 2, y }), pointerWorld: (px, py) => ({ x: px / 2, y: py }),
    corpses: realmCorpseViews(pairs, players, HERO) });
  let clock = 0;
  const input = createRealmInput({ transmit: (b) => out.push(Array.from(b)), scene, now: () => clock,
    move: (x, y, run) => { moves.push([x, y, run]); return 1; } });

  // Falling: the screen is up, the respawn key is not yet taken.
  mode = 0; assert.ok(deathScreenShown(mode)); assert.equal(respawnAllowed(mode), false);
  // Down: Esc sends the one-byte respawn.
  mode = 17; assert.ok(respawnAllowed(mode));
  assert.ok(input.respawn());
  assert.deepEqual(out, [[0x41]]);

  // In town; the server has named both corpses. Another player's corpse is not a target: the click walks.
  mode = 5; me = { x: 100, y: 100 };
  pairs = [{ owner: HERO, corpse: CORPSE }, { owner: OTHER, corpse: OTHER_CORPSE }];
  clock += 1000; input.leftClick(120, 100);
  assert.deepEqual(out.at(-1), [0x01, 60, 0, 100, 0]);

  // The hero's own corpse, far: walk to the unit, then open it on arrival, once.
  clock += 1000; const hit = input.leftClick(240, 100);
  assert.equal(hit.kind, 'corpse');
  assert.deepEqual(out.at(-1), [0x02, 0, 0, 0, 0, ...le(CORPSE)]);
  assert.deepEqual(moves.at(-1), [120, 100, false]);
  const before = out.length;
  input.tick(); assert.equal(out.length, before);
  me = { x: 119, y: 100 };
  input.tick();
  assert.deepEqual(out.at(-1), [0x13, 0, 0, 0, 0, ...le(CORPSE)]);
  input.tick(); assert.equal(out.length, before + 1);

  // Emptied: the server drops the corpse from the list, and the spot is bare ground again.
  pairs = [{ owner: OTHER, corpse: OTHER_CORPSE }];
  clock += 1000; input.leftClick(240, 100);
  assert.deepEqual(out.at(-1), [0x01, 120, 0, 100, 0]);
});

test('a corpse still falling is not opened', () => {
  const out = [];
  const corpses = [{ guid: CORPSE, owner: HERO, own: true, mode: 0, x: 101, y: 100 }];
  const input = createRealmInput({ transmit: (b) => out.push(Array.from(b)), now: () => 0,
    scene: () => ({ ready: true, player: { x: 100, y: 100 }, run: true, selected: { left: 0, right: 0 }, monsters: [], objects: [], items: [],
      isNpc: () => false, hoverBox: () => false, toScreen: (x, y) => ({ x: x * 2, y }), pointerWorld: (px, py) => ({ x: px / 2, y: py }), corpses }) });
  input.leftClick(202, 100);
  assert.deepEqual(out, [[0x03, 101, 0, 100, 0]]);
});
