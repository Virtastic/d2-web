// Gamepad support. A controller
// is an INPUT LAYER: it produces the SAME DOM pointer and key events a mouse and a keyboard produce,
// through input-common.mjs (whose dispatch mirrors wasm-build/scenarios/lib/player.mjs's click()), so
// the shell's own handlers issue the same engine commands. Nothing about the game is re-implemented
// here.
//
// Polled on a timer (setInterval), never requestAnimationFrame: rAF is throttled to nothing in a
// headless or background tab, which is exactly where the browser rows fake a
// pad with a page-installed navigator.getGamepads() stub -- a controller that only wakes on rAF would
// never see it.
//
// Standard mapping (https://w3c.github.io/gamepad/#remapping), recorded in (G5):
//   left stick (axes 0,1)  -- walk: the same click ahead of the hero a mouse user repeats (G2)
//   right stick (axes 2,3) -- moves a visible cursor; the move is a pointermove, as a mouse (G1)
//   A (button 0)  -- left click at the cursor       B (button 1) -- right click at the cursor
//   D-pad (12-15) -- belt columns 1-4 (Digit1-Digit4), clockwise from Up: Up=1, Right=2, Down=3, Left=4
//   Start (button 9) -- Escape (the game menu)
//   LB (button 4) -- KeyI (inventory)                RB (button 5) -- KeyC (character)
//   RT (button 7) -- KeyT (skill tree)                                        (G3, G4)
import { click, pointer, tapKey, playerOnCanvas, stickTarget } from './input-common.mjs';

const POLL_MS = 50;          // setInterval, never requestAnimationFrame -- see header
const STICK_WALK_MS = 250;   // a mouse user's own repeat rate for a held click-to-walk
const CURSOR_SPEED = 14;     // canvas px per poll at full right-stick deflection
const DEADZONE = 0.25;

const BUTTON = { A: 0, B: 1, LB: 4, RB: 5, RT: 7, START: 9, DPAD_UP: 12, DPAD_DOWN: 13, DPAD_LEFT: 14, DPAD_RIGHT: 15 };
// D-pad direction -> belt column. Retail's belt has no directional meaning, so this is simply the
// one table recorded for it; clockwise from Up.
const DPAD_BELT = { [BUTTON.DPAD_UP]: 'Digit1', [BUTTON.DPAD_RIGHT]: 'Digit2', [BUTTON.DPAD_DOWN]: 'Digit3', [BUTTON.DPAD_LEFT]: 'Digit4' };

function makeCursorEl(viewport) {
  const el = document.createElement('div');
  el.id = 'controller-cursor';
  Object.assign(el.style, {
    position: 'absolute', width: '14px', height: '14px', transform: 'translate(-50%,-50%)',
    borderRadius: '50%', border: '2px solid #fff', boxShadow: '0 0 3px #000, 0 0 1px #000 inset',
    pointerEvents: 'none', display: 'none', zIndex: 5,
  });
  viewport.appendChild(el);
  return el;
}

function start() {
  const canvas = document.getElementById('game') || document.querySelector('canvas');
  if (!canvas) { setTimeout(start, POLL_MS); return; }   // defensive only: module scripts run after the DOM parses
  const viewport = canvas.parentElement || document.body;
  if (getComputedStyle(viewport).position === 'static') viewport.style.position = 'relative';
  const cursorEl = makeCursorEl(viewport);

  const state = { connected: false, index: -1, cursor: { x: canvas.width / 2, y: canvas.height / 2 }, lastStickWalk: 0, wasPressed: {} };

  const placeCursor = () => {
    cursorEl.style.left = `${state.cursor.x / canvas.width * 100}%`;
    cursorEl.style.top = `${state.cursor.y / canvas.height * 100}%`;
  };
  placeCursor();

  // What a row asserts against: connect state and the cursor's own
  // canvas position, read back rather than the module offering a second, faked way to move it.
  window.__d2Controller = { get evidence() {
    return { connected: state.connected, index: state.index, cursor: { x: state.cursor.x, y: state.cursor.y } };
  } };

  function edge(pad, i) {
    const now = Boolean(pad.buttons[i]?.pressed), was = Boolean(state.wasPressed[i]);
    state.wasPressed[i] = now;
    return now && !was;
  }

  function poll() {
    const pads = navigator.getGamepads ? navigator.getGamepads() : [];
    const pad = pads && Array.prototype.find.call(pads, (p) => p && p.mapping === 'standard');
    if (!pad) {
      if (state.connected) { state.connected = false; state.wasPressed = {}; cursorEl.style.display = 'none'; }
      return;
    }
    if (!state.connected) { state.connected = true; state.index = pad.index; cursorEl.style.display = 'block'; }

    // Right stick: moves the cursor and sends pointermove, exactly a mouse move (G1).
    const rx = pad.axes[2] || 0, ry = pad.axes[3] || 0;
    if (Math.hypot(rx, ry) > DEADZONE) {
      state.cursor.x = Math.max(0, Math.min(canvas.width, state.cursor.x + rx * CURSOR_SPEED));
      state.cursor.y = Math.max(0, Math.min(canvas.height, state.cursor.y + ry * CURSOR_SPEED));
      placeCursor();
      pointer(canvas, 'pointermove', state.cursor.x, state.cursor.y);
    }

    // Left stick: walk -- a click at stickTarget from the hero, repeated ~every 250ms while held,
    // exactly what a mouse user does ahead of the hero (G2).
    const origin = playerOnCanvas();
    const target = origin && stickTarget(origin, pad.axes[0] || 0, pad.axes[1] || 0);
    const now = performance.now();
    if (target && now - state.lastStickWalk >= STICK_WALK_MS) { click(canvas, target[0], target[1]); state.lastStickWalk = now; }

    // A / B: a left or right click at the cursor (G3, G4 -- retail's panels hit-test the pointer).
    if (edge(pad, BUTTON.A)) click(canvas, state.cursor.x, state.cursor.y, { button: 0 });
    if (edge(pad, BUTTON.B)) click(canvas, state.cursor.x, state.cursor.y, { button: 2 });
    // D-pad: the belt's columns (G3).
    for (const [button, code] of Object.entries(DPAD_BELT)) if (edge(pad, Number(button))) tapKey(code);
    // Start / shoulders / trigger: the game menu, inventory, character, skill tree (G3).
    if (edge(pad, BUTTON.START)) tapKey('Escape');
    if (edge(pad, BUTTON.LB)) tapKey('KeyI');
    if (edge(pad, BUTTON.RB)) tapKey('KeyC');
    if (edge(pad, BUTTON.RT)) tapKey('KeyT');
  }

  setInterval(poll, POLL_MS);
}

if (document.readyState === 'loading') document.addEventListener('DOMContentLoaded', start, { once: true });
else start();
