import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';

const src = readFileSync(new URL('../play/gameplay-shell.mjs', import.meta.url), 'utf8');
test('player corpse draws with the engine unit direction, not a fixed 0', () => {
  assert.match(src, /entities\[i\]\.direction = facings\.getUint8/);
  const line = src.split('\n').find((l) => l.includes("pass: 'player-corpse'") === false && l.includes('corpseRenderer?.draw'));
  assert.match(line, /direction: entity\.direction \?\? 0/);
  assert.doesNotMatch(src, /TODO\(client-parity F19\)/);
});
