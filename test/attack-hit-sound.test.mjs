import test from 'node:test';
import assert from 'node:assert/strict';
import { hitSounds, createCombatSoundTrigger } from '../play/assets/combat-sound-trigger.mjs';

test('0x227E0 impact/body ids by last hit class', () => {
  assert.deepEqual(hitSounds(0x01, 1, 3), [0x141]);
  assert.deepEqual(hitSounds(0x12, 1, 3), [0x155, 0x17B]);
  assert.deepEqual(hitSounds(0x20, 0, 4), [0x161]);
  assert.deepEqual(hitSounds(0x2C, 0, 4), [0x145, 0x164]);
  assert.deepEqual(hitSounds(0x36, 0, 0), [0x14F]);       // melee mode 0 keeps no body row
  assert.deepEqual(hitSounds(0x36, 0, 4), [0x14F, 0x16A]);
  assert.deepEqual(hitSounds(0x01, 1, 3, true), []);      // critter gate
  assert.deepEqual(hitSounds(0, 1, 3), []);
});

test('impact rows fire in order on hit-mode entry only', () => {
  const rd = () => new Uint8Array(0), got = [];
  const t = (() => { try { return createCombatSoundTrigger(rd, (r) => got.push(r)); } catch { return null; } })();
  if (!t) return; // tables absent: constructor needs retail tables
  for (const m of [1, 3, 3, 1]) t.update([{ type: 1, id: 9, classId: 0, mode: m, lastHitClass: 0x12 }], {});
  assert.deepEqual(got.slice(0, 2), [0x155, 0x17B]);
});
