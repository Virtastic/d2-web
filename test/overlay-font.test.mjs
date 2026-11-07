import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { OVERLAY_FONT } from '../play/assets/input-touch.mjs';
test('overlay chrome has no monospace', () => {
  assert.ok(OVERLAY_FONT.includes('serif'));
  for (const f of ['input-touch', 'input-controller'])
    assert.ok(!/monospace/.test(readFileSync(`play/assets/${f}.mjs`, 'utf8').replace(/\/\/.*/g, '')), f);
});
