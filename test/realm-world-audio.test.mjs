import test from 'node:test';
import assert from 'node:assert/strict';
import { realmAreaLevel, createRealmAreaAudio } from '../play/assets/realm-world-audio.mjs';

test('area comes from the active room, then the last reveal', () => {
  assert.equal(realmAreaLevel({ clientRoomLevel: 3 }, { mapLevel: 1 }), 3);
  assert.equal(realmAreaLevel({ clientRoomLevel: 0 }, { mapLevel: 1 }), 1);
  assert.equal(realmAreaLevel(null, null), -1);
});

test('new song waits 0x4a ticks across an area change, first song is immediate', () => {
  const music = { activeLevel: -1 }, played = [], amb = [];
  const a = createRealmAreaAudio({ music, playMusic: (l) => played.push(l), ambience: { change: (l) => amb.push(['c', l]), sync: (l, c) => amb.push(['s', l, c]) }, cycle: () => 0 });
  a.step(1, 10); assert.deepEqual(played, [1]); music.activeLevel = 1;
  a.step(1, 11); assert.deepEqual(played, [1]);
  a.step(2, 100); assert.deepEqual(played, [1]);
  a.step(2, 100 + 0x4a); assert.deepEqual(played, [1]);
  a.step(2, 100 + 0x4b); assert.deepEqual(played, [1, 2]);
  assert.deepEqual(amb, [['c', 1], ['s', 1, 0], ['c', 2], ['s', 2, 0]]);
});

test('a cycle flip resyncs ambience without a change call', () => {
  let cycle = 0; const amb = [];
  const a = createRealmAreaAudio({ ambience: { change: () => amb.push('c'), sync: (l, c) => amb.push(c) }, cycle: () => cycle });
  a.step(5, 1); cycle = 1; a.step(5, 2); a.step(5, 3);
  assert.deepEqual(amb, ['c', 0, 1]);
});
