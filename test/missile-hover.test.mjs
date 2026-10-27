import assert from 'node:assert/strict';
import { isHoveredMissile, missileTransDrawMode } from '../play/assets/missile-renderer.mjs';
assert.equal(isHoveredMissile({ kind: 'missile', id: 5 }, 5), true);
assert.equal(isHoveredMissile({ kind: 'unit', id: 5 }, 5), false);
assert.equal(isHoveredMissile({ kind: 'missile', id: 4 }, 5), false);
assert.equal(isHoveredMissile(null, 5), false);
assert.equal(missileTransDrawMode(2, true), 7);
assert.equal(missileTransDrawMode(2, false), 4);
