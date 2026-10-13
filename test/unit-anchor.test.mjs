import test from 'node:test';
import assert from 'node:assert/strict';
import { unitAnchor } from '../play/assets/unit-anchor.mjs';

test('unit anchor uses engine precise coordinates when available', () => {
  assert.deepEqual(unitAnchor({ x: 12, y: 23, preciseX: 12.75, preciseY: 22.25 }), [12.25, 21.75]);
});

test('unit anchor keeps the session subtile for stationary units', () => {
  assert.deepEqual(unitAnchor({ x: 12, y: 23, preciseX: null, preciseY: null }), [12, 23]);
});
