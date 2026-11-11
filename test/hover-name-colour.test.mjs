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

// The under-mouse routine queues a player's name 0x48 px above the unit's screen point.
test('player name plate sits 0x48 px above the unit', async () => {
  const { readFileSync } = await import('node:fs');
  const src = readFileSync(new URL('../play/gameplay-shell.mjs', import.meta.url), 'utf8');
  const body = src.slice(src.indexOf('function drawRealmPlayerPlate'), src.indexOf('function feedRealmWorld'));
  assert.match(body, /hit\.unit\.anchorY - 0x48 \+ 2/);
});
