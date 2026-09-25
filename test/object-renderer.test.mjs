import test from 'node:test';
import assert from 'node:assert/strict';
import { parseObjectPresentations } from '../play/assets/object-renderer.mjs';

test('object presentation records retain retail token, mode, frame, and component mask', () => {
  const bytes = new Uint8Array(40), v = new DataView(bytes.buffer);
  v.setUint32(0, 9, true); v.setUint32(4, 12, true); v.setUint32(8, 0x00357931, true); v.setUint32(12, 2, true);
  v.setInt32(16, -3, true); v.setInt32(20, 8, true); v.setUint32(24, 0x300, true); v.setUint32(28, 0x800, true);
  v.setUint32(32, 0x102, true); v.setUint16(36, 128, true); v.setUint8(38, 4); v.setUint8(39, 1);
  assert.deepEqual(parseObjectPresentations(bytes, 1)[0], { id: 9, classId: 12, token: '1y5', mode: 2, x: -3, y: 8,
    framePrecise: 0x300, frameCountPrecise: 0x800, componentMask: 0x102, animSpeed: 128, direction: 4, selectable: true });
  assert.throws(() => parseObjectPresentations(bytes.subarray(1), 1), /truncated/);
});
