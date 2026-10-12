import test from 'node:test';
import assert from 'node:assert/strict';
import { nativeWallLightCell, nativeWallLightCells } from '../play/assets/native-wall-light.mjs';
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

test('all recovered direction/state rows preserve the native six-sample order', () => {
  const ordinary = {
    1: [[2,5],[2,3],[2,1],[2,-1],[2,-3],[2,-5]],
    2: [[-5,2],[-3,2],[-1,2],[1,2],[3,2],[5,2]],
    3: [[2,5],[2,3],[2,2],[2,2],[3,2],[5,2]],
    4: [[-5,2],[-3,2],[-1,2],[2,-1],[2,-3],[2,-5]],
  };
  const hiddenRows = {
    1: [[-2,5],[-2,3],[-2,1],[-2,-1],[-2,-3],[-2,-5]],
    2: [[-5,-2],[-3,-2],[-1,-2],[1,-2],[3,-2],[5,-2]],
    3: [[-2,5],[-2,3],[-2,-2],[-2,-2],[3,-2],[5,-2]],
    4: [[-5,-2],[-3,-2],[-1,-2],[-2,-1],[-2,-3],[-2,-5]],
  };

  for (let direction = 1; direction <= 4; direction++) {
    for (const hidden of [false, true]) {
      const offsets = hidden ? hiddenRows[direction] : ordinary[direction];
      assert.deepEqual(cells({direction}, hidden), offsets.map(([dx, dy]) => [100 + dx, 200 + dy]));
    }
  }
  assert.deepEqual(cells({direction: 5}, false), Array.from({length: 6}, () => [100, 200]));
  assert.deepEqual(cells({direction: 5}, true), Array.from({length: 6}, () => [100, 200]));
});

test('unrecovered direction metadata leaves the measured fallback available', () => {
  for (const direction of [undefined, 0, 10, 11, 1.5]) assert.equal(nativeWallLightCell({direction}, false, 100, 200, 0), null);
});

test('production wall sampling keeps the six native cells in draw-entry order', () => {
  const anchor = [4516 * 5, 1924 * 5];
  assert.deepEqual(nativeWallLightCells({direction: 3}, false, ...anchor),
    [[22582,9625],[22582,9623],[22582,9622],[22582,9622],[22583,9622],[22585,9622]]);
  assert.deepEqual(nativeWallLightCells({direction: 3}, true, ...anchor),
    [[22578,9625],[22578,9623],[22578,9618],[22578,9618],[22583,9618],[22585,9618]]);
  assert.equal(nativeWallLightCells({direction: 10}, false, 10, 20), null,
    'unrecovered directions continue through the renderer’s measured fallback');
});
