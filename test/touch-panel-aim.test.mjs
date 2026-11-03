// The stash and cube panel spec on a canvas that is not 800x600: the game-px controls come out in canvas px
// through the same centred origin the renderers draw with (panelOrigin), so a finger's control is where it is drawn.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { panelOrigin, panelSpec, onPanelControl } from '../play/assets/touch-panel-aim.mjs';
import { BTN } from '../play/assets/plugy-stash-ui.mjs';

test('the panel origin is the centred 800x600 frame, scaled down only when the canvas is smaller', () => {
  assert.deepEqual(panelOrigin(1200, 700), { scale: 1, x: 200, y: 50 });
  assert.deepEqual(panelOrigin(400, 300), { scale: 0.5, x: 0, y: 0 });
  assert.deepEqual(panelOrigin(800, 600), { scale: 1, x: 0, y: 0 });
});

test('on a wider canvas the stash close cel is at its canvas position, not its game-px one', () => {
  const spec = panelSpec({ stash: { plugy: false } }, 1200, 700);
  const close = BTN.close;
  assert.deepEqual(spec.controls.at(-1), { x: close.x + 200, y: close.y + 50, w: 32, h: 32 });
  assert.equal(onPanelControl(close.x + 200 + 4, close.y + 50 + 4, spec), true);
  assert.equal(onPanelControl(close.x + 4, close.y + 4, spec), false, 'the game-px position is not on the control on a wider canvas');
});

test('on a 400x300 canvas the controls scale by one half and sit at the scaled origin', () => {
  const spec = panelSpec({ stash: { plugy: false } }, 400, 300);
  assert.deepEqual(spec.controls.at(-1), { x: BTN.close.x * 0.5, y: BTN.close.y * 0.5, w: 16, h: 16 });
});

test('no panel open gives no spec, and onPanelControl is false for a null spec', () => {
  assert.equal(panelSpec(null, 1200, 700), null);
  assert.equal(onPanelControl(10, 10, null), false);
});

test('the shared-gold buttons are finger targets only while PlugY shared gold is on', () => {
  const cel = (n) => ({ x: BTN[n].x, y: BTN[n].y, w: 32, h: 32 });
  const on = panelSpec({ stash: { plugy: true, gold: true } }, 800, 600).controls;
  assert.ok(on.some((r) => JSON.stringify(r) === JSON.stringify(cel('putGold'))));
  assert.ok(on.some((r) => JSON.stringify(r) === JSON.stringify(cel('takeGold'))));
  const off = panelSpec({ stash: { plugy: true, gold: false } }, 800, 600).controls;
  assert.ok(!off.some((r) => JSON.stringify(r) === JSON.stringify(cel('putGold'))));
});
