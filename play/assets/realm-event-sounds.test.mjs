// Event sounds in a game read from packets (no engine session): pickup, gold, drop and the hero's footfall play from the client state.
import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { createEventSounds } from './event-sounds.mjs';
import { createRealmEventSoundFeed } from './realm-event-sounds.mjs';

const enc = (s) => new TextEncoder().encode(s);
const files = {
  'data\\global\\excel\\Sounds.txt': enc('Sound\tIndex\nitem_pickup\t235\nitem_flippy\t216\nitem_a\t10\ncursor_level_up\t7\n'),
  'data\\global\\excel\\armor.txt': enc('name\tcode\tdropsound\tdropsfxframe\nA\tcap\titem_a\t5\n'),
  'data\\global\\excel\\weapons.txt': enc('name\tcode\tdropsound\tdropsfxframe\n'),
  'data\\global\\excel\\misc.txt': enc('name\tcode\tdropsound\tdropsfxframe\n'),
};
const setup = () => { const got = []; const es = createEventSounds((p) => files[p], (row, e) => got.push([row, e.kind])); return { got, feed: createRealmEventSoundFeed(es) }; };
const item = (id, code, mode, page) => ({ id, code, mode, invPage: page, framePrecise: 0 });
const snap = (items, mode = 1) => ({ mode, level: 1, playerClass: 1, playerId: 5, player: { x: 0, y: 0 }, entities: [], objects: [], items });

test('control: the old drain gate -- a game with no session never reaches the event player', () => {
  const src = readFileSync(new URL('../gameplay-shell.mjs', import.meta.url), 'utf8');
  const drain = src.slice(src.indexOf('function drainUnitSounds()'), src.indexOf('function update(forceTick'));
  assert.ok(/!session/.test(drain.split('eventSounds?.update')[0]), 'drainUnitSounds returns without a session before eventSounds.update');
  assert.ok(/createRealmEventSoundFeed/.test(src) && /\.tick\(snapshot, readAdvancement\(\)\?\.level, realmWorldState\)/.test(src), 'the realm client tick feeds the event sounds');
});
test('the item the hero takes to the cursor plays the pickup row once; an item another player took plays none', () => {
  const { got, feed } = setup();
  const w = { cursorId: 0, gold: 0 };
  feed.tick(snap([item(1, 'cap', 3, 255), item(2, 'cap', 3, 255)]), 1, w);
  w.cursorId = 1; feed.tick(snap([item(2, 'cap', 3, 255), item(1, 'cap', 4, 0)]), 1, w);
  feed.tick(snap([item(1, 'cap', 4, 0)]), 1, w);   // item 2 left the ground with the cursor on 1: not the hero's
  assert.deepEqual(got.filter((g) => g[1] === 'pickup').map((g) => g[0]), [235]);
});
test('a gold pile vanishing plays the pickup row only when the hero gold rose', () => {
  const { got, feed } = setup();
  const w = { cursorId: 0, gold: 10 };
  feed.tick(snap([item(7, 'gld ', 3, 255), item(8, 'gld ', 3, 255)]), 1, w);
  w.gold = 60; feed.tick(snap([item(8, 'gld ', 3, 255)]), 1, w);
  feed.tick(snap([]), 1, w);   // pile 8 gone, gold unchanged: another player's
  assert.equal(got.filter((g) => g[1] === 'pickup').length, 1);
});
test('a dropped item plays the flip row and its class drop sound', () => {
  const { got, feed } = setup();
  const w = { cursorId: 0, gold: 0 };
  feed.tick(snap([]), 1, w);
  feed.tick(snap([item(3, 'cap', 5, 255)]), 1, w);
  assert.deepEqual(got.map((g) => g[0]).sort((a, b) => a - b), [10, 216]);
});
test('the hero walking plays footfalls from the unit mode', () => {
  const { got, feed } = setup();
  const w = { cursorId: 0, gold: 0 };
  for (let i = 0; i < 12; i++) feed.tick(snap([], 2), 1, w);
  assert.ok(got.filter((g) => g[1] === 'footstep').length >= 2);
  assert.equal(got.filter((g) => g[1] === 'footstep')[0][0] >= 2720, true);
});
test('a level gained plays the level-up row from the level the packets gave', () => {
  const { got, feed } = setup();
  const w = { cursorId: 0, gold: 0 };
  feed.tick(snap([]), 3, w); feed.tick(snap([]), 4, w);
  assert.deepEqual(got.filter((g) => g[1] === 'levelup').map((g) => g[0]), [7]);
});
test('deliberate-failure control: a wrong expectation fails', () => {
  const { got, feed } = setup();
  feed.tick(snap([item(1, 'cap', 3, 255)]), 1, { cursorId: 1, gold: 0 }); feed.tick(snap([]), 1, { cursorId: 1, gold: 0 });
  assert.throws(() => assert.equal(got.filter((g) => g[1] === 'pickup').length, 2));
});
