import test from 'node:test';
import assert from 'node:assert/strict';
import { monsterNameColour } from '../play/assets/hover-lifebar.mjs';
// D2Client+0xC0850: 0x08 gold, 0x04 blue (wins), class list gold, 0x02/0x10 plain.
test('hover plate name colour follows D2Client 0xC0850', () => {
  assert.equal(monsterNameColour(0), 0);
  assert.equal(monsterNameColour(0x08), 4);
  assert.equal(monsterNameColour(0x04), 3);
  assert.equal(monsterNameColour(0x0C), 3);
  assert.equal(monsterNameColour(0x02), 0);
  assert.equal(monsterNameColour(0x10), 0);
  assert.equal(monsterNameColour(0, 0xf3), 4);
});
