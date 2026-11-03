// The Horadric Cube's close cel: a click and Escape run one close (TRADEBTN_CLOSECUBE, 23), a touch near miss snaps
// to the cel, and the window closes only when the engine accepts the command.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { TRADEBTN_CLOSECUBE, CUBE_CLOSE, cubeCloseAt, closeCubeFromPanel } from '../play/assets/cube-close.mjs';
import { panelSpec, panelControlAim, onPanelControl } from '../play/assets/touch-panel-aim.mjs';

const SHELL = readFileSync(new URL('../play/gameplay-shell.mjs', import.meta.url), 'utf8');
const cubeOnly = (w, h) => panelSpec({ cube: { geometry: null, panel: null } }, w, h);
const reachOf = (px) => px + 16;   // a finger's reach: 16 canvas px beyond the control's own half-size

test('the close command is retail 23, and the close cel is 32x32 at game px (352, 445)', () => {
  assert.equal(TRADEBTN_CLOSECUBE, 23);
  assert.deepEqual(CUBE_CLOSE, { x: 352, y: 445, w: 32, h: 32 });
});

test('a click and Escape share one close: the command goes out, and the window closes only on acceptance', () => {
  const calls = [];
  let open = true;
  const accepted = closeCubeFromPanel({ cubeOpen: open, commandButton: (b, a) => { calls.push([b, a]); return 1; }, onAccepted: () => { open = false; } });
  assert.equal(accepted, true);
  assert.deepEqual(calls, [[23, 0]]);
  assert.equal(open, false);

  open = true;
  const refused = closeCubeFromPanel({ cubeOpen: open, commandButton: (b, a) => { calls.push([b, a]); return 0; }, onAccepted: () => { open = false; } });
  assert.equal(refused, false);
  assert.deepEqual(calls[1], [23, 0]);
  assert.equal(open, true, 'a refused close leaves the cube open');
});

test('no command is sent when the cube is not open', () => {
  const calls = [];
  assert.equal(closeCubeFromPanel({ cubeOpen: false, commandButton: (b, a) => { calls.push([b, a]); return 1; }, onAccepted: () => {} }), false);
  assert.deepEqual(calls, []);
});

test('the close hit rect is the cel: inclusive top-left, exclusive bottom-right, and only while visible', () => {
  assert.equal(cubeCloseAt(352, 445, 800, 600, true), true);
  assert.equal(cubeCloseAt(383, 476, 800, 600, true), true);
  assert.equal(cubeCloseAt(384, 460, 800, 600, true), false);
  assert.equal(cubeCloseAt(360, 477, 800, 600, true), false);
  assert.equal(cubeCloseAt(360, 460, 800, 600, false), false, 'a closed cube has no close to hit');
  assert.equal(cubeCloseAt(360, 460, 0, 0, true), false);
});

test('on a 1200x700 canvas the close hit rect moves with the centred frame', () => {
  assert.equal(cubeCloseAt(552, 495, 1200, 700, true), true);   // game (352, 445) + origin (200, 50)
  assert.equal(cubeCloseAt(360, 460, 1200, 700, true), false);
});

test('the touch spec carries the cube close cel whenever the cube window is up', () => {
  const spec = cubeOnly(800, 600);
  assert.deepEqual(spec.controls, [{ x: 352, y: 445, w: 32, h: 32 }]);
  assert.equal(onPanelControl(368, 461, spec), true);
});

test('a finger a few px short of the cube close snaps to the cel centre', () => {
  const spec = cubeOnly(800, 600);
  assert.deepEqual(panelControlAim(346, 461, spec, reachOf), [368, 461]);
  assert.deepEqual(panelControlAim(360, 440, spec, reachOf), [368, 461]);
});

test('a point on the cel is delivered raw, and a point out of reach is not snapped', () => {
  const spec = cubeOnly(800, 600);
  assert.equal(panelControlAim(368, 461, spec, reachOf), null);
  assert.equal(panelControlAim(300, 461, spec, reachOf), null);
});

test('the shell closes the cube by one function from both Escape and the pointer release', () => {
  assert.equal(SHELL.match(/closeCubeWindow\(\)/g).length, 2, 'Escape and the pointer-up call the shared close, and nothing else calls it');
  assert.equal(SHELL.includes("[23, 0]"), false, 'the shell sends no raw close command; closeCubeFromPanel owns button 23');
  assert.match(SHELL, /cubeCloseAt\(x, y, canvas\.width, canvas\.height, cubeOpen && inventoryVisible\)\) \{\n\s+cubeClosePress = beginInventoryClosePress/);
});
