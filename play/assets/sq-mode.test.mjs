// A PLRMODE_SEQUENCE (18) unit draws its current sequence frame's mode and frame. Whirlwind's record
// (src/core/d2moo/src/DataTbls/SequenceTbls.cpp, gPlayerSequenceWhirlwind): eight frames, all A1 (7).
import assert from 'node:assert/strict';
import { drawnPlayerState, playerModeToken } from './unit-renderer.mjs';
const WHIRLWIND = [[7, 0], [7, 1], [7, 2], [7, 3], [7, 3], [7, 4], [7, 5], [7, 6]];
const record = WHIRLWIND.map(([seqMode, seqFrame]) => {
  const s = drawnPlayerState({ playerClass: 0, mode: 18, seqMode, seqFrame });
  return [playerModeToken(s.mode), s.framePrecise >> 8];
});
assert.deepEqual(record, [['A1', 0], ['A1', 1], ['A1', 2], ['A1', 3], ['A1', 3], ['A1', 4], ['A1', 5], ['A1', 6]]);
assert.equal(playerModeToken(drawnPlayerState({ mode: 18 }).mode), 'GH', 'no published sequence keeps the GH stand-in');
assert.equal(drawnPlayerState({ mode: 19, seqMode: 7, seqFrame: 2 }).mode, 19, 'knockback is not a sequence');
console.log('sq-mode: 0 diffs');
