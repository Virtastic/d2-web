import { test } from 'node:test';
import assert from 'node:assert/strict';
import { createRealmHud, STAT } from '../play/assets/realm-hud-state.mjs';

const hud = () => createRealmHud({ experience: {}, waypoints: {}, skills: {} });
const sizeOf = (b) => ({ 0x19: 2, 0x1a: 2, 0x1b: 3, 0x1c: 5 }[b[0]] ?? 0);

test('experience and gold packets add to or set the player stats', () => {
  const h = hud();
  h.feed(Uint8Array.of(0x1c, 100, 0, 0, 0, 0x1a, 5, 0x1b, 0x2c, 0x01, 0x19, 7, 0x19, 3), sizeOf);
  assert.equal(h.stat(STAT.experience), 100 + 5 + 300);
  assert.equal(h.stat(STAT.gold), 10);
  h.feed(Uint8Array.of(0x1c, 9, 0, 0, 0), sizeOf);
  assert.equal(h.stat(STAT.experience), 9, 'the dword form replaces the total');
});
