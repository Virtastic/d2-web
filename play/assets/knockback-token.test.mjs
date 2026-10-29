// D47: a knocked-back monster/player resolves its COF with the GH token (D2Common+0x43C30), never a KB COF that does not exist.
import assert from 'node:assert/strict';
import { monsterModeToken } from './monster-renderer.mjs';
import { playerModeToken } from './unit-renderer.mjs';
assert.equal(monsterModeToken(13), 'GH');
assert.equal(monsterModeToken(3), 'GH');
assert.equal(monsterModeToken(1), 'NU');
assert.equal(playerModeToken(18), 'GH');
assert.equal(playerModeToken(19), 'GH');
assert.equal(playerModeToken(4), 'GH');
assert.equal(playerModeToken(17), 'DD');
console.log('knockback-token: 0 diffs');
