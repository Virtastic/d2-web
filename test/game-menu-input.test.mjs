import test from 'node:test';
import assert from 'node:assert/strict';
import {
  createGameMenu,
  createGameMenuPointerState,
  gameMenuGestureMatchesRelease,
  shouldUpdateGameMenuSelectionOnPointerMove,
} from '../play/assets/game-menu.mjs';

test('game-menu marker follows hover until a row is armed, then stays pinned', () => {
  assert.equal(shouldUpdateGameMenuSelectionOnPointerMove(null), true);
  assert.equal(shouldUpdateGameMenuSelectionOnPointerMove({ pointerId: 7, page: 'automap', row: 2 }), false);
});

test('only the owning left-button release over the armed page and row activates', () => {
  const armed = { pointerId: 7, page: 'automap', row: 2 };
  assert.equal(gameMenuGestureMatchesRelease(armed, 7, 0, 'automap', 2), true);
  assert.equal(gameMenuGestureMatchesRelease(armed, 8, 0, 'automap', 2), false);
  assert.equal(gameMenuGestureMatchesRelease(armed, 7, 2, 'automap', 2), false);
  assert.equal(gameMenuGestureMatchesRelease(armed, 7, 0, 'options', 2), false);
  assert.equal(gameMenuGestureMatchesRelease(armed, 7, 0, 'automap', 1), false);
});

test('pointer cancel clears only the gesture owned by that pointer', () => {
  const state = createGameMenuPointerState();
  const armed = { pointerId: 7, page: 'automap', row: 2 };
  state.arm(armed);
  assert.equal(state.release(8), null);
  assert.equal(state.current, armed);
  assert.equal(state.cancel(8), false);
  assert.equal(state.cancel(7), true);
  assert.equal(state.current, null);
});

test('Automap Options keeps its marker armed and changes a value only on a matching release', () => {
  const previousImageData = globalThis.ImageData;
  globalThis.ImageData ??= class ImageData { constructor(data, width, height) { Object.assign(this, { data, width, height }); } };
  try {
    const frame = { width: 100, height: 36, offsetX: 0, offsetY: 36,
      indices: new Uint8Array(3600), opaque: new Uint8Array(3600) };
    const menu = createGameMenu({
      read: () => new Uint8Array(0),
      decodeDatPalette: () => new Uint8Array(1024),
      decodeDc6: () => ({ frames: [frame, frame] }),
      documentObject: { createElement: () => ({ getContext: () => ({ putImageData() {} }) }) },
    });
    menu.toggle();
    const mainOptions = menu.targets(800, 600).OPTIONS;
    assert.equal(menu.pointerDown(...mainOptions, 800, 600, 7, 0), true);
    assert.equal(menu.evidence.page, 'main', 'press does not navigate away immediately');
    const mainReturn = menu.targets(800, 600)['RETURN TO GAME'];
    menu.hover(...mainReturn, 800, 600);
    assert.equal(menu.evidence.selected, 'OPTIONS', 'held marker remains on the pressed row');
    menu.pointerUp(...mainOptions, 800, 600, 7, 0);
    assert.equal(menu.evidence.page, 'options');

    const automap = menu.targets(800, 600)['AUTOMAP OPTIONS'];
    menu.click(...automap, 800, 600);
    const fade = menu.targets(800, 600)['AUTOMAP FADE'];
    const center = menu.targets(800, 600)['AUTOMAP CENTER'];
    assert.equal(menu.settings.fade, 'off');
    menu.pointerDown(...fade, 800, 600, 9, 0);
    menu.hover(...center, 800, 600);
    assert.equal(menu.evidence.selected, 'AUTOMAP FADE');
    assert.equal(menu.settings.fade, 'off', 'setting does not change on press');
    menu.pointerUp(...fade, 800, 600, 9, 0);
    assert.equal(menu.settings.fade, 'on');

    const party = menu.targets(800, 600)['AUTOMAP PARTY'];
    const names = menu.targets(800, 600)['AUTOMAP PARTY NAMES'];
    menu.pointerDown(...party, 800, 600, 10, 0);
    menu.pointerUp(...names, 800, 600, 10, 0);
    assert.equal(menu.settings.party, 'yes', 'release on another row cancels the armed action');
  } finally {
    if (previousImageData === undefined) delete globalThis.ImageData;
    else globalThis.ImageData = previousImageData;
  }
});
