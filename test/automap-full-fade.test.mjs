import test from 'node:test';
import assert from 'node:assert/strict';
import { fullMapFadeDrawMode } from '../play/assets/automap-full-fade.mjs';

const source = 'data\\global\\ui\\automap\\maximap.dc6';
const at = (dx, dy = 0, targetX = 400) => fullMapFadeDrawMode({
  mode: 'full', fade: 'on', source,
  x: targetX + dx - 8, y: 290 + dy - 16, width: 800, height: 600, targetX,
});

test('full-map Fade selects the native PL2 draw mode from projected cell centers', () => {
  assert.equal(at(49), 0);
  assert.equal(at(50), 1);
  assert.equal(at(99), 1);
  assert.equal(at(100), 2);
  assert.equal(at(40, 20), 1); // (2 * max(40,20) + min(40,20)) / 2 = 50
  assert.equal(at(140), 2); // inclusive horizontal viewport edge
  assert.equal(at(141), 5);
  assert.equal(at(0, -149), 2);
  assert.equal(at(0, -150), 5); // inside viewport, at the outer distance tier
  assert.equal(at(0, 130), 2); // inclusive lower viewport edge
  assert.equal(at(0, 131), 5);
});

test('full-map Fade measures from the panel-shifted view centre', () => {
  assert.equal(at(0, 0, 200), 0);
  assert.equal(at(50, 0, 200), 1);
  assert.equal(at(0, 0, 600), 0);
});

test('full-map Fade leaves nonmatching mode, setting, and cel family normal', () => {
  const base = { mode: 'full', fade: 'on', source, x: 392, y: 274, width: 800, height: 600 };
  assert.equal(fullMapFadeDrawMode({ ...base, fade: 'off' }), 5);
  assert.equal(fullMapFadeDrawMode({ ...base, mode: 'mini' }), 5);
  assert.equal(fullMapFadeDrawMode({ ...base, source: 'data\\global\\ui\\automap\\act2map.dc6' }), 5);
});
