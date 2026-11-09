import test from 'node:test';
import assert from 'node:assert/strict';
import { routeRealmPackets } from './realm-packet-routes.mjs';
import { createRealmInput } from './realm-input.mjs';

const SIZES = { 0x28: 7, 0x7b: 8, 0x01: 2 };
const sizeOf = (b) => SIZES[b[0]] ?? 0;
const talk = (type, guid) => Uint8Array.from([0x28, type, guid & 255, guid >> 8, 0, 0, 3]);

function inputWith(monsters) {
  const sent = [];
  const input = createRealmInput({ transmit: (b) => { sent.push(Array.from(b)); return true; },
    scene: () => ({ ready: true, player: { x: 0, y: 0 }, monsters, objects: [], items: [] }) });
  return { input, sent };
}

test('a 0x28 for a known unit opens the conversation, an unknown one ends it', () => {
  const { input, sent } = inputWith([{ guid: 0x105, mode: 1 }]);
  const stream = Uint8Array.from([...talk(1, 0x105), 0x01, 0, ...talk(1, 0x999)]);
  const out = routeRealmPackets(stream, sizeOf, { npcInteraction: (p) => input.npcInteraction(p) });
  assert.equal(out.length, 2);
  assert.deepEqual(sent[0], [0x2f, 1, 0, 0, 0, 5, 1, 0, 0]);
  assert.deepEqual(sent[1].slice(0, 5), [0x30, 3, 0, 0, 0]);
});

test('a 0x7b reaches the hotkey table; a short tail is left alone', () => {
  const seen = [];
  const hk = Uint8Array.from([0x7b, 2, 0x36, 0x80, 1, 0, 0, 0]);
  routeRealmPackets(Uint8Array.from([...hk, 0x7b, 1]), sizeOf, { applyHotkey: (p) => seen.push(Array.from(p)) });
  assert.deepEqual(seen, [Array.from(hk)]);
});

test('an F-key sends the select-skill packet of its slot with the table owner; an unbound key sends nothing', () => {
  const { input, sent } = inputWith([]);
  const pkt = Uint8Array.from([0x3c, 0x36, 0x00, 0x00, 0x80, 0x07, 0, 0, 0]);
  assert.equal(input.hotkey(pkt), true);
  assert.equal(input.hotkey(null), false);
  assert.deepEqual(sent, [Array.from(pkt)]);
  input.selectSkill(0x36, false, 0x1234);
  assert.deepEqual(sent[1], [0x3c, 0x36, 0, 0, 0, 0x34, 0x12, 0, 0]);
});

test('the skill buttons read the server hotkey table', async () => {
  const { realmFrameReads } = await import('./realm-client-view.mjs');
  const table = Array.from({ length: 16 }, (_, i) => ({ skill: i === 1 ? 54 : -1, left: i === 1, owner: 0xffffffff }));
  const r = realmFrameReads(null, null, null, 0, null, null, table);
  assert.deepEqual(r.hotkeys[1], { skill: 54, left: true });
  assert.equal(r.hotkeys.length, 8);
  assert.deepEqual(r.hotkeys[0], { skill: -1, left: false });
});

test('gold, experience and the player\'s own unit stats reach the panels once the owner guid is set', async () => {
  const { createRealmHud, STAT } = await import('./realm-hud-state.mjs');
  const hud = createRealmHud({ experience: { next: () => 1000, start: () => 0 }, waypoints: { all: () => [] }, skills: { classSkills: () => [] } });
  const sizes = { 0x19: 2, 0x1a: 2, 0x1b: 3, 0x1c: 5, 0x20: 10 }, size = (b) => sizes[b[0]] ?? 0;
  const own = [0x20, 0x07, 0, 0, 0, STAT.level, 5, 0, 0, 0];
  const stream = Uint8Array.from([...own, 0x19, 40, 0x1a, 7, 0x1b, 0x2c, 0x01, 0x1c, 0xe8, 0x03, 0, 0]);
  hud.feed(stream, size);
  assert.equal(hud.stat(STAT.level), 0);                 // before the owner is known the stat belongs to nobody
  hud.reset(); hud.setOwner(7); hud.feed(stream, size);
  assert.equal(hud.stat(STAT.level), 5);
  assert.equal(hud.stat(STAT.gold), 40);
  assert.equal(hud.stat(STAT.experience), 1000);         // 0x1c sets the total after the byte and word deltas
  hud.setOwner(8); hud.feed(Uint8Array.from([0x20, 7, 0, 0, 0, STAT.level, 9, 0, 0, 0]), size);
  assert.equal(hud.stat(STAT.level), 5);                 // another unit's stat is ignored
});

test('a 0x97 reaches the weapon-set handler', () => {
  const seen = [];
  routeRealmPackets(Uint8Array.from([0x97, 0x97]), (b) => (b[0] === 0x97 ? 1 : 0), { applyWeaponSwitch: (p) => seen.push(p[0]) });
  assert.deepEqual(seen, [0x97, 0x97]);
});

test('a 0x28 carries the 96 quest-flag bytes: kept for type 6 and for a known unit, dropped for an unknown one', () => {
  const kept = [];
  const sent = [];
  const input = createRealmInput({ transmit: (b) => { sent.push(Array.from(b)); return true; },
    scene: () => ({ ready: true, player: { x: 0, y: 0 }, monsters: [{ guid: 0x105, mode: 1 }], objects: [], items: [] }),
    questFlags: (f) => kept.push(Array.from(f)) });
  const pkt = (type, guid) => { const p = new Uint8Array(103); p[0] = 0x28; p[1] = type; p[2] = guid & 255; p[3] = guid >> 8; p[7] = 0xAB; p[102] = 0xCD; return p; };
  input.npcInteraction(pkt(6, 0));
  input.npcInteraction(pkt(1, 0x105));
  input.npcInteraction(pkt(1, 0x999));
  assert.equal(kept.length, 2);
  assert.equal(kept[0].length, 96);
  assert.equal(kept[0][0], 0xAB);
  assert.equal(kept[0][95], 0xCD);
});
