// A touch at the death prompt is the Esc key. The death screen reads "PRESS ESC TO CONTINUE" (retail's
// youdiedinst), and the shell's Escape handler respawns a dead player (mode 17, D2Game Rcv0x41). A phone
// has no Escape, so a touch on the canvas while dead must send the same Escape keydown, and a touch at
// any other time must reach the game as it always did.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { installTouchInput } from '../play/assets/input-touch.mjs';

globalThis.PointerEvent = globalThis.KeyboardEvent = globalThis.MouseEvent = class {
  constructor(type, init) { this.type = type; Object.assign(this, init); }
  stopImmediatePropagation() { this.blocked = true; }
  stopPropagation() { this.blocked = true; }
  preventDefault() { this.defaultPrevented = true; }
};

const PLRMODE_DEAD = 17, PLRMODE_DEATH_ANIMATION = 0, PLRMODE_WALK = 2;

function element() {
  const classes = new Set();
  return {
    style: {}, dataset: {}, children: [],
    classList: {
      add(name) { classes.add(name); },
      contains(name) { return classes.has(name); },
      toggle(name, on) { on ? classes.add(name) : classes.delete(name); },
    },
    addEventListener() {},
    appendChild(child) { this.children.push(child); return child; },
    setAttribute() {}, querySelector() { return null; }, focus() {}, blur() {},
    getBoundingClientRect() { return { left: 0, top: 0, width: 800, height: 600 }; },
  };
}

// A minimal page: the canvas records what it receives, the window records the keys tapKey sends.
function page(shell) {
  const sent = [], keys = [], docListeners = new Map();
  const canvas = Object.assign(element(), {
    width: 800, height: 600,
    dispatchEvent(e) {
      if (e.type === 'contextmenu') { if (!e.blocked) sent.push(e.type); return true; }
      sent.push(e.type);
      if (e.type.startsWith('pointer')) for (const fn of docListeners.get(e.type) || []) fn(e);
      return true;
    },
  });
  const container = element();
  const documentObject = {
    head: element(),
    getElementById(id) { return id === 'game' ? canvas : id === 'touch-overlay' ? container : null; },
    createElement: element,
    addEventListener(type, fn) { docListeners.set(type, [...(docListeners.get(type) || []), fn]); },
    visibilityState: 'visible',
  };
  const windowObject = { matchMedia: () => ({ matches: true }), __d2Shell: shell, addEventListener() {} };
  const realSetInterval = globalThis.setInterval;
  globalThis.setInterval = () => 1;
  let touch;
  try { touch = installTouchInput({ documentObject, windowObject }); }
  finally { globalThis.setInterval = realSetInterval; }
  const finger = (type, pointerId, x, y) => {
    const e = { type, pointerType: 'touch', pointerId, clientX: x + 0.5, clientY: y + 0.5, target: canvas,
      cancelable: true, stopped: false, stopPropagation() { this.stopped = true; }, preventDefault() {} };
    for (const fn of docListeners.get(type) || []) fn(e);
    return e;
  };
  // The window's keydown is what tapKey reaches; only the presses are recorded (one per Escape).
  const withKeys = (fn) => {
    const saved = globalThis.dispatchEvent;
    globalThis.dispatchEvent = (e) => { if (e.type === 'keydown') keys.push(e.code); return true; };
    try { return fn(); } finally { globalThis.dispatchEvent = saved; }
  };
  return {
    touch, sent, keys, shell,
    down: (id, x, y) => withKeys(() => finger('pointerdown', id, x, y)),
    up: (id, x, y) => withKeys(() => finger('pointerup', id, x, y)),
  };
}

test('a touch on the canvas while dead sends the Escape key, the prompt "PRESS ESC TO CONTINUE" asks for', () => {
  const p = page({ screen: 'game', clearScreen: false, snapshot: { mode: PLRMODE_DEAD } });
  const d = p.down(1, 400, 300);
  p.up(1, 400, 300);
  assert.deepEqual(p.keys, ['Escape'], 'one Escape press per touch');
  assert.deepEqual(p.sent, [], 'the touch never reaches the canvas as a click on the dead world');
  assert.equal(d.stopped, true, 'the shell does not see the touch as a press');
});

test('a touch while dead respawns on the touch, not on the lift', () => {
  const p = page({ screen: 'game', clearScreen: false, snapshot: { mode: PLRMODE_DEAD } });
  p.down(1, 100, 100);
  assert.deepEqual(p.keys, ['Escape'], 'the Escape goes out at the press');
  p.up(1, 100, 100);
  assert.deepEqual(p.keys, ['Escape'], 'the lift sends nothing more');
});

test('a touch while alive does not send Escape and reaches the game as a press', () => {
  const p = page({ screen: 'game', clearScreen: false, snapshot: { mode: PLRMODE_WALK } });
  p.down(1, 400, 300);
  p.up(1, 400, 300);
  assert.deepEqual(p.keys, [], 'no Escape while alive');
  assert.ok(p.sent.includes('pointerdown'), 'the touch still presses the game');
});

test('a touch during the death animation (mode 0) does not respawn: the server refuses it', () => {
  const p = page({ screen: 'game', clearScreen: false, snapshot: { mode: PLRMODE_DEATH_ANIMATION } });
  p.down(1, 400, 300);
  p.up(1, 400, 300);
  assert.deepEqual(p.keys, [], 'no Escape before the body is down');
});

test('a touch while dead on another screen does not respawn', () => {
  const p = page({ screen: 'title', clearScreen: false, snapshot: { mode: PLRMODE_DEAD } });
  p.down(1, 400, 300);
  p.up(1, 400, 300);
  assert.deepEqual(p.keys, [], 'no Escape outside the game screen');
});

test('a touch while dead behind a clearing screen does not respawn', () => {
  const p = page({ screen: 'game', clearScreen: true, snapshot: { mode: PLRMODE_DEAD } });
  p.down(1, 400, 300);
  p.up(1, 400, 300);
  assert.deepEqual(p.keys, [], 'no Escape while the screen is clearing');
});
