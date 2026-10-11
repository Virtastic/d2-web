import test from 'node:test';
import assert from 'node:assert/strict';
import { nativeWallLightCell } from '../play/assets/native-wall-light.mjs';
import { newFade, stepFade } from '../play/assets/roof-fade.mjs';

const cells = (entry, hidden) => Array.from({length: 6}, (_, j) => nativeWallLightCell(entry, hidden, 100, 200, j));

test('wall hidden state selects the pattern independently of DT1 transparency color', () => {
  const entry = { direction: 1, transparentColor: 0x00ff00ff };
  assert.deepEqual(cells(entry, false), [[102,205],[102,203],[102,201],[102,199],[102,197],[102,195]]);
  assert.deepEqual(cells(entry, true), [[98,205],[98,203],[98,201],[98,199],[98,197],[98,195]]);
  assert.deepEqual(cells({...entry, transparentColor: 0}, false), cells(entry, false));
  assert.deepEqual(cells({...entry, transparentColor: 0}, true), cells(entry, true));
});

test('hide and reveal switch samples before alpha has reached its destination', () => {
  const fade = newFade(), entry = {direction: 2};
  assert.equal(stepFade(fade, true, 1000), 255);
  assert.deepEqual(cells(entry, fade.hidden), [[95,198],[97,198],[99,198],[101,198],[103,198],[105,198]]);
  stepFade(fade, true, 1100);
  assert.equal(fade.alpha, 230);
  stepFade(fade, false, 1100);
  assert.ok(fade.alpha < 255);
  assert.deepEqual(cells(entry, fade.hidden), [[95,202],[97,202],[99,202],[101,202],[103,202],[105,202]]);
});

test('native corner rows and lower-wall aliases retain their ordered sample identities', () => {
  assert.deepEqual(cells({direction: 3}, true), [[98,205],[98,203],[98,198],[98,198],[103,198],[105,198]]);
  assert.deepEqual(cells({direction: 4}, false), [[95,202],[97,202],[99,202],[102,199],[102,197],[102,195]]);
  for (let direction = 6; direction <= 9; direction++) {
    for (const hidden of [false, true]) assert.deepEqual(cells({direction}, hidden), cells({direction: direction - 5}, hidden));
  }
  for (const hidden of [false, true]) assert.deepEqual(cells({direction: 5}, hidden), Array.from({length: 6}, () => [100,200]));
});

test('unrecovered direction metadata leaves the measured fallback available', () => {
  for (const direction of [undefined, 0, 10, 11, 1.5]) assert.equal(nativeWallLightCell({direction}, false, 100, 200, 0), null);
});
