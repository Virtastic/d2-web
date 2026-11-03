// Live touch, proved on a synthetic finger sequence: each test feeds the touch layer the document-level
// pointer events a real finger produces, and asserts the exact, ordered list of engine commands the
// canvas receives (the replayed pointer and contextmenu events the shell's own handlers read).
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { installTouchInput } from '../play/assets/input-touch.mjs';

// The DOM event constructors the replay helpers call: plain carriers of their init fields.
globalThis.PointerEvent = globalThis.KeyboardEvent = globalThis.MouseEvent = class {
  constructor(type, init) { this.type = type; Object.assign(this, init); }
  stopImmediatePropagation() { this.blocked = true; }
  stopPropagation() { this.blocked = true; }
  preventDefault() { this.defaultPrevented = true; }
};

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
    setAttribute() {},
    querySelector() { return null; },
    focus() {}, blur() {},
    getBoundingClientRect() { return { left: 0, top: 0, width: 800, height: 600 }; },
  };
}

// The canvas's own record of what it receives. The shell's listeners see every replayed event as
// the browser delivers it: pointer events bubble to the document, where the touch layer's capture
// listeners sit above them; the layer's replay guard must keep those from being read back as fingers.
function world(shell = { screen: 'game' }, clock = null) {
  const sent = [];
  const docListeners = new Map();
  const canvas = Object.assign(element(), {
    width: 800, height: 600,
    dispatchEvent(e) {
      // A contextmenu passes the document's capture listeners before the canvas's own: one they stop
      // never reaches the shell. (The layer must never stop the menus it sends itself.)
      if (e.type === 'contextmenu') {
        for (const fn of docListeners.get(e.type) || []) fn(e);
        if (!e.blocked) sent.push(record(e));
        return true;
      }
      sent.push(record(e));
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
  };
  documentObject.visibilityState = 'visible';
  const winListeners = new Map();
  const windowObject = { matchMedia: () => ({ matches: true }), __d2Shell: shell,
    addEventListener(type, fn) { winListeners.set(type, [...(winListeners.get(type) || []), fn]); } };
  if (clock) windowObject.performance = { now: () => clock.t };
  const realSetInterval = globalThis.setInterval;
  globalThis.setInterval = () => 1;
  let touch;
  try { touch = installTouchInput({ documentObject, windowObject }); } finally { globalThis.setInterval = realSetInterval; }

  // A finger event at canvas pixel (x, y), as the browser delivers it to the document: on the canvas's
  // drawn box (1:1 unless a test draws the canvas at another size).
  function finger(type, pointerId, x, y, extra = {}) {
    const r = canvas.getBoundingClientRect();
    const e = {
      type, pointerType: 'touch', pointerId, clientX: (x + 0.5) * r.width / canvas.width, clientY: (y + 0.5) * r.height / canvas.height, target: canvas,
      cancelable: true, stopped: false,
      stopPropagation() { this.stopped = true; }, preventDefault() {},
      ...extra,
    };
    for (const fn of docListeners.get(type) || []) fn(e);
    return e;
  }
  // The browser's own contextmenu (a long-press, or a mouse's right button), as it reaches the canvas.
  function browserMenu(pointerType) {
    const e = new MouseEvent('contextmenu', { clientX: 400.5, clientY: 300.5, button: 2, cancelable: true, isTrusted: true });
    if (pointerType) e.pointerType = pointerType;
    canvas.dispatchEvent(e);
    return e;
  }
  return {
    canvas, shell, touch, sent, browserMenu,
    down: (id, x, y, extra) => finger('pointerdown', id, x, y, extra),
    move: (id, x, y) => finger('pointermove', id, x, y),
    up: (id, x, y) => finger('pointerup', id, x, y),
    cancel: (id, x, y) => finger('pointercancel', id, x, y),
    // The window loses focus; the page goes to the background (and comes back).
    blur: () => { for (const fn of winListeners.get('blur') || []) fn({ type: 'blur' }); },
    hide: (state = 'hidden') => { documentObject.visibilityState = state; for (const fn of docListeners.get('visibilitychange') || []) fn({ type: 'visibilitychange' }); },
  };
}

// What the canvas received, in the fields that matter; undefined fields (a contextmenu has no
// pointer id) are not recorded at all.
function record(e) {
  const r = { type: e.type, x: e.clientX - 0.5, y: e.clientY - 0.5, button: e.button, buttons: e.buttons,
    pointerType: e.pointerType, pointerId: e.pointerId, shiftKey: e.shiftKey };
  return Object.fromEntries(Object.entries(r).filter(([, v]) => v !== undefined));
}

const left = (type, x, y, pointerId = 1, shiftKey = false) => ({
  type, x, y, button: 0, buttons: type === 'pointerup' ? 0 : 1, pointerType: 'touch', pointerId, shiftKey,
});
const right = (type, x, y, pointerId = 2, shiftKey = false) => ({
  type, x, y, button: 2, buttons: type === 'pointerup' ? 0 : 2, pointerType: 'touch', pointerId, shiftKey,
});
const menu = (x, y, shiftKey = false) => ({ type: 'contextmenu', x, y, button: 2, shiftKey });

test('one finger on open ground: a press when it lands, and a release when it lifts', () => {
  const w = world({ screen: 'title' });
  const d = w.down(1, 400, 300);
  const u = w.up(1, 400, 300);
  assert.deepEqual(w.sent, [left('pointermove', 400, 300), left('pointerdown', 400, 300), left('pointerup', 400, 300)]);
  assert.equal(d.stopped, true, 'the real touch never reaches the shell');
  assert.equal(u.stopped, true);
});

test('a drag sends the press, one move per move event, and the release at the lift', () => {
  const w = world({ screen: 'title' });
  w.down(1, 400, 300);
  w.move(1, 410, 300);
  w.move(1, 420, 305);
  w.up(1, 420, 305);
  assert.deepEqual(w.sent, [
    left('pointermove', 400, 300), left('pointerdown', 400, 300),
    left('pointermove', 410, 300, 1),
    left('pointermove', 420, 305, 1),
    left('pointerup', 420, 305),
  ]);
});

test('a pointercancel calls the held press off where the finger last was: a cancel, never an up', () => {
  const w = world({ screen: 'title' });
  w.down(1, 400, 300);
  w.move(1, 405, 300);
  const c = w.cancel(1, 405, 300);
  assert.deepEqual(w.sent, [
    left('pointermove', 400, 300), left('pointerdown', 400, 300),
    left('pointermove', 405, 300),
    { ...left('pointercancel', 405, 300), buttons: 0 },
  ]);
  assert.equal(c.stopped, true, 'the shell sees one cancel, the aimed one');
  assert.equal(w.touch.isShiftArmed(), false);
});

test('a pointercancel inside the landing window sends nothing: the press never went out', () => {
  const w = world({ screen: 'title' });
  w.down(1, 400, 300);
  w.cancel(1, 400, 300);
  w.touch.flush();
  assert.deepEqual(w.sent, []);
});

test('the aim finger steers a held right button: its moves carry the right button, the release goes there', () => {
  const w = world({ screen: 'title' });
  w.down(1, 400, 300);
  w.touch.flush();
  w.down(2, 500, 400);            // right down at the aim point
  w.move(1, 430, 310);            // the aim finger moves: the held right follows it
  w.move(2, 520, 400);            // the second finger's own moves are not the mouse
  w.up(2, 520, 400);
  w.move(1, 450, 310);            // the right is up: the aim finger's left came up too, nothing
  w.up(1, 450, 310);
  assert.deepEqual(w.sent, [
    left('pointermove', 400, 300), left('pointerdown', 400, 300),
    left('pointerup', 400, 300),
    right('pointerdown', 400, 300),
    menu(400, 300),
    right('pointermove', 430, 310),
    right('pointerup', 430, 310),
  ]);
});

test('a finger left over from a lifted pair does not swallow the fingers after it', () => {
  const w = world({ screen: 'title' });
  w.down(1, 400, 300);
  w.down(2, 500, 400);
  w.down(3, 300, 200);            // swallowed while the pair is down
  w.up(2, 500, 400);
  w.up(1, 400, 300);
  w.sent.length = 0;
  w.down(4, 200, 100);            // finger 3 is still resting: a fresh finger still presses
  w.up(4, 200, 100);
  assert.deepEqual(w.sent, [left('pointermove', 200, 100, 4), left('pointerdown', 200, 100, 4), left('pointerup', 200, 100, 4)]);
});

test('a belt aim snaps the press and the release to the box centre, the finger\'s own point is not used', () => {
  const shell = { screen: 'game', beltHudBoxes: [{ left: 100, top: 540, right: 129, bottom: 569 }] };
  const w = world(shell);
  // Canvas (110, 550) is inside the half-cell ring of the one belt box, centre (114.5, 554.5).
  w.down(1, 110, 550);
  w.up(1, 110, 550);
  assert.deepEqual(w.sent, [left('pointermove', 114.5, 554.5), left('pointerdown', 114.5, 554.5), left('pointerup', 114.5, 554.5)]);
});

test('two fingers: the left comes up, then the second holds the right button at the aiming finger\'s point', () => {
  const w = world({ screen: 'title' });
  w.down(1, 400, 300);            // aim: left down
  w.move(1, 420, 300);            // aim: left moves
  w.down(2, 500, 400);            // second: left up, right down at the aim point, then the menu event
  w.move(1, 440, 300);            // the aim finger's left is up: its moves steer the held right
  w.up(2, 500, 400);              // second lifts: right up, where the aim finger last was
  w.up(1, 440, 300);              // aim lifts: its left already came up, nothing more
  assert.deepEqual(w.sent, [
    left('pointermove', 400, 300), left('pointerdown', 400, 300),
    left('pointermove', 420, 300, 1),
    left('pointerup', 420, 300),
    right('pointerdown', 420, 300),
    menu(420, 300),
    right('pointermove', 440, 300),
    right('pointerup', 440, 300),
  ]);
});

test('a cancel of the aim finger after a second finger landed sends no second left release', () => {
  const w = world({ screen: 'title' });
  w.down(1, 400, 300);
  w.touch.flush();                // the shell's update: the landing window closes, the left goes down
  w.down(2, 500, 400);
  w.cancel(1, 400, 300);
  w.cancel(2, 500, 400);
  assert.deepEqual(w.sent, [
    left('pointermove', 400, 300), left('pointerdown', 400, 300),
    left('pointerup', 400, 300),
    right('pointerdown', 400, 300),
    menu(400, 300),
    right('pointerup', 400, 300),
  ]);
});

test('the aim finger lifting first leaves the right button held until the second finger lifts', () => {
  const w = world({ screen: 'title' });
  w.down(1, 400, 300);
  w.down(2, 500, 400);
  w.up(1, 400, 300);              // aim lifts first: its left never went down, nothing is sent
  w.up(2, 500, 400);              // the second lifts: now the right release
  assert.deepEqual(w.sent, [
    left('pointermove', 400, 300),
    right('pointerdown', 400, 300),
    menu(400, 300),
    right('pointerup', 400, 300),
  ]);
});

test('the landing window: a press waits for the shell\'s next update, then goes out where the finger landed', () => {
  const w = world({ screen: 'title' });
  w.down(1, 400, 300);
  assert.deepEqual(w.sent, [], 'nothing reaches the shell before its next update');
  w.touch.flush();
  w.touch.flush();                // a second update sends nothing more: the press is out
  w.up(1, 400, 300);
  assert.deepEqual(w.sent, [left('pointermove', 400, 300), left('pointerdown', 400, 300), left('pointerup', 400, 300)]);
});

test('a second finger inside the landing window is a pure right click: no left press, so no walk', () => {
  const w = world({ screen: 'title' });
  w.down(1, 400, 300);
  w.down(2, 500, 400);
  w.touch.flush();                // the update after: the aim finger's press was taken over, nothing goes out
  w.move(1, 420, 300);            // steers the held right, never a left
  w.up(2, 500, 400);
  w.up(1, 420, 300);
  assert.deepEqual(w.sent, [
    left('pointermove', 400, 300),
    right('pointerdown', 400, 300),
    menu(400, 300),
    right('pointermove', 420, 300),
    right('pointerup', 420, 300),
  ]);
});

test('a third finger is swallowed: it sends nothing and reaches nothing', () => {
  const w = world({ screen: 'title' });
  w.down(1, 400, 300);
  w.down(2, 500, 400);
  const third = w.down(3, 300, 200);
  const thirdMove = w.move(3, 310, 200);
  const thirdUp = w.up(3, 310, 200);
  w.up(2, 500, 400);
  w.up(1, 400, 300);
  assert.deepEqual(w.sent, [
    left('pointermove', 400, 300),
    right('pointerdown', 400, 300),
    menu(400, 300),
    right('pointerup', 400, 300),
  ]);
  assert.equal(third.stopped, true);
  assert.equal(thirdMove.stopped, true);
  assert.equal(thirdUp.stopped, true);
});

test('a shift rest finger in the console gap sends nothing, and makes every other event shift', () => {
  const w = world({ screen: 'game' });
  // Canvas (400, 570): the console gap of the bottom band, on no control the frame placed.
  const rest = w.down(3, 400, 570);
  assert.equal(w.touch.isShiftArmed(), true, 'a rest finger is down');
  assert.equal(rest.stopped, true);
  w.down(1, 300, 300);            // aim finger: shift rides the press
  w.move(1, 310, 300);
  w.up(1, 310, 300);              // and the release
  w.up(3, 400, 570);              // the rest finger lifts: nothing is sent
  assert.equal(w.touch.isShiftArmed(), false, 'shift is not latched once the rest finger lifts');
  w.down(1, 300, 300);            // the next tap is a plain tap
  w.up(1, 300, 300);
  assert.deepEqual(w.sent, [
    left('pointermove', 300, 300, 1, true), left('pointerdown', 300, 300, 1, true),
    left('pointermove', 310, 300, 1, true),
    left('pointerup', 310, 300, 1, true),
    left('pointermove', 300, 300, 1, false), left('pointerdown', 300, 300, 1, false),
    left('pointerup', 300, 300, 1, false),
  ]);
});

test('a lost lift: the next primary finger calls the stale press off and is a fresh left press, not a right click', () => {
  const w = world({ screen: 'title' });
  w.down(1, 400, 300, { isPrimary: true });
  w.move(1, 410, 300);            // the press is out; then its pointerup never arrives
  w.down(2, 500, 400, { isPrimary: true });   // the browser says no other finger is down
  w.up(2, 500, 400);
  assert.deepEqual(w.sent, [
    left('pointermove', 400, 300), left('pointerdown', 400, 300),
    left('pointermove', 410, 300, 1),
    { ...left('pointercancel', 410, 300), buttons: 0 },
    left('pointermove', 500, 400, 2), left('pointerdown', 500, 400, 2),
    left('pointerup', 500, 400, 2),
  ]);
});

test('a lost lift of a rest finger: the next primary finger clears Shift', () => {
  const w = world({ screen: 'game' });
  w.down(3, 400, 570, { isPrimary: true });   // a rest, whose pointerup is lost
  assert.equal(w.touch.isShiftArmed(), true);
  w.down(1, 300, 300, { isPrimary: true });
  assert.equal(w.touch.isShiftArmed(), false, 'a stale rest does not hold Shift');
  w.up(1, 300, 300);
  assert.deepEqual(w.sent, [
    left('pointermove', 300, 300, 1, false), left('pointerdown', 300, 300, 1, false),
    left('pointerup', 300, 300, 1, false),
  ]);
});

test('a non-primary finger never clears the holds: the second finger is still the right button', () => {
  const w = world({ screen: 'title' });
  w.down(1, 400, 300, { isPrimary: true });
  w.down(2, 500, 400, { isPrimary: false });
  w.up(2, 500, 400);
  w.up(1, 400, 300);
  assert.deepEqual(w.sent, [
    left('pointermove', 400, 300), right('pointerdown', 400, 300), menu(400, 300), right('pointerup', 400, 300),
  ]);
});

// The page loses focus or goes to the background with fingers down: their lifts never arrive. A
// non-primary next finger (or one whose browser reports no isPrimary) gets no help from the
// primary-finger guard above, so the layer calls every hold off itself.
function holdWalkThen(callOff) {
  const w = world({ screen: 'title' });
  w.down(1, 400, 300, { isPrimary: true });
  w.move(1, 410, 300);            // the press is out (a hold-walk)
  callOff(w);
  w.down(2, 500, 400, { isPrimary: false });
  w.up(2, 500, 400);
  return w.sent;
}

test('blur with a hold-walk down: the press is called off, and the next finger is a fresh left press', () => {
  assert.deepEqual(holdWalkThen((w) => w.blur()), [
    left('pointermove', 400, 300), left('pointerdown', 400, 300),
    left('pointermove', 410, 300, 1),
    { ...left('pointercancel', 410, 300), buttons: 0 },
    left('pointermove', 500, 400, 2), left('pointerdown', 500, 400, 2),
    left('pointerup', 500, 400, 2),
  ]);
});

test('the page hidden with a hold-walk down: the same call-off; the page shown again changes nothing', () => {
  assert.deepEqual(holdWalkThen((w) => w.hide()), holdWalkThen((w) => w.blur()));
  const shown = holdWalkThen((w) => w.hide('visible'));
  // NEGATIVE CONTROL: with nothing called off, the stale aim makes the next finger a second finger.
  assert.deepEqual(shown, holdWalkThen(() => {}));
  assert.notDeepEqual(shown, holdWalkThen((w) => w.blur()), 'without the call-off the next finger is not a fresh press');
  assert.ok(!shown.some((e) => e.type === 'pointercancel'), 'a page shown again calls nothing off');
});

test('the page hidden with a rest finger down: Shift is not left armed for the next tap', () => {
  const w = world({ screen: 'game' });
  w.down(3, 400, 570, { isPrimary: true });   // a rest
  assert.equal(w.touch.isShiftArmed(), true);
  w.hide();
  assert.equal(w.touch.isShiftArmed(), false, 'a hidden page holds no Shift');
  w.down(1, 300, 300, { isPrimary: false });
  w.up(1, 300, 300);
  assert.deepEqual(w.sent, [
    left('pointermove', 300, 300, 1, false), left('pointerdown', 300, 300, 1, false),
    left('pointerup', 300, 300, 1, false),
  ]);
});

test('blur with a second finger held: its right button is released, so a held cast ends', () => {
  const w = world({ screen: 'title' });
  w.down(1, 400, 300, { isPrimary: true });
  w.down(2, 500, 400, { isPrimary: false });
  w.blur();
  assert.deepEqual(w.sent, [
    left('pointermove', 400, 300), right('pointerdown', 400, 300), menu(400, 300), right('pointerup', 400, 300),
  ]);
  w.up(2, 500, 400);              // a late lift after the call-off sends nothing more
  w.up(1, 400, 300);
  assert.equal(w.sent.length, 4);
});

test('a rest finger on a control the frame placed is not a rest: the control takes the press', () => {
  const shell = { screen: 'game', beltHudBoxes: [{ left: 390, top: 560, right: 419, bottom: 589 }] };
  const w = world(shell);
  w.down(3, 400, 570);            // inside the belt box: an aim, not a rest
  assert.equal(w.touch.isShiftArmed(), false);
  w.up(3, 400, 570);
  assert.deepEqual(w.sent, [left('pointermove', 404.5, 574.5, 3), left('pointerdown', 404.5, 574.5, 3), left('pointerup', 404.5, 574.5, 3)]);
});

test('scripted tap and longPress keep their replayed sequence (the seam lib/player.mjs calls)', () => {
  const w = world({ screen: 'title' });
  w.touch.tap(100, 100);
  w.touch.longPress(100, 100);
  // click(): move, down, up; a button-2 click adds the contextmenu after the release (no shiftKey on it).
  assert.deepEqual(w.sent, [
    left('pointermove', 100, 100, 9001),
    left('pointerdown', 100, 100, 9001),
    left('pointerup', 100, 100, 9001),
    right('pointermove', 100, 100, 9001),
    right('pointerdown', 100, 100, 9001),
    right('pointerup', 100, 100, 9001),
    { type: 'contextmenu', x: 100, y: 100, button: 2 },
  ]);
});

// The timed landing window, on a page clock: a first finger's press waits LAND_MS from its landing, so a second
// finger that joins the same tap (measured 40 ms behind on a phone) makes a pure right click.
test('a second finger 40 ms behind the first is a pure right click, even across a frame', () => {
  const clock = { t: 1000 };
  const w = world({ screen: 'title' }, clock);
  w.down(1, 400, 300);
  clock.t += 16; w.touch.flush();   // a frame inside the window: nothing goes out
  clock.t += 24; w.down(2, 500, 400);
  w.up(2, 500, 400);
  w.up(1, 400, 300);
  assert.deepEqual(w.sent, [
    left('pointermove', 400, 300),
    right('pointerdown', 400, 300),
    menu(400, 300),
    right('pointerup', 400, 300),
  ]);
});

test('a held finger presses at the first frame after its landing window closes', () => {
  const clock = { t: 1000 };
  const w = world({ screen: 'title' }, clock);
  w.down(1, 400, 300);
  clock.t += 59; w.touch.flush();
  assert.deepEqual(w.sent, [], 'still inside the window');
  clock.t += 1; w.touch.flush();
  assert.deepEqual(w.sent, [left('pointermove', 400, 300), left('pointerdown', 400, 300)]);
});

test('a quick tap lifts inside the window and still clicks at once', () => {
  const clock = { t: 1000 };
  const w = world({ screen: 'title' }, clock);
  w.down(1, 400, 300);
  clock.t += 30; w.up(1, 400, 300);
  assert.deepEqual(w.sent, [left('pointermove', 400, 300), left('pointerdown', 400, 300), left('pointerup', 400, 300)]);
});

test('jitter inside the slop keeps the press waiting; a real slide presses where the finger landed, then moves', () => {
  const clock = { t: 1000 };
  const w = world({ screen: 'title' }, clock);
  w.down(1, 400, 300);
  clock.t += 10; w.move(1, 405, 303);   // under 12 px: still the landing
  assert.deepEqual(w.sent, []);
  clock.t += 10; w.move(1, 430, 300);   // a slide: the press goes out at the landing point first
  assert.deepEqual(w.sent, [
    left('pointermove', 400, 300), left('pointerdown', 400, 300),
    left('pointermove', 430, 300, 1),
  ]);
});

test('after the right lifts, the aim finger keeps its point: the next second finger aims where it now is', () => {
  const w = world({ screen: 'title' });
  w.down(1, 400, 300);
  w.down(2, 500, 400);
  w.up(2, 500, 400);
  w.move(1, 460, 320);            // nothing is held: no event, but the aim point follows the finger
  w.down(5, 500, 400);
  w.up(5, 500, 400);
  w.up(1, 460, 320);
  assert.deepEqual(w.sent, [
    left('pointermove', 400, 300),
    right('pointerdown', 400, 300), menu(400, 300), right('pointerup', 400, 300),
    right('pointerdown', 460, 320, 5), menu(460, 320), right('pointerup', 460, 320, 5),
  ]);
});

const menus = (sent) => sent.filter((e) => e.type === 'contextmenu');

test("the browser's long-press menu under a held finger never reaches the shell", () => {
  const clock = { t: 1000 };
  const w = world({ screen: 'game' }, clock);
  w.down(1, 400, 300);
  clock.t += 500; w.touch.flush?.();
  const e = w.browserMenu('touch');
  assert.equal(e.blocked, true);
  assert.equal(e.defaultPrevented, true);
  assert.deepEqual(menus(w.sent), []);
  w.up(1, 400, 300);
  assert.deepEqual(menus(w.sent), []);
});

test("a mouse's or pen's contextmenu passes through untouched, even with a finger down", () => {
  const w = world({ screen: 'game' }, { t: 1000 });
  w.down(1, 400, 300);
  for (const type of ['mouse', 'pen']) {
    const e = w.browserMenu(type);
    assert.equal(e.blocked, undefined);
    assert.equal(e.defaultPrevented, undefined);
  }
  assert.equal(menus(w.sent).length, 2);
});

test('a contextmenu with no pointerType is a finger\'s while one is down or just lifted, else a mouse\'s', () => {
  const clock = { t: 1000 };
  const w = world({ screen: 'game' }, clock);
  assert.equal(w.browserMenu().blocked, undefined);        // no finger has touched: a mouse
  w.down(1, 400, 300);
  assert.equal(w.browserMenu().blocked, true);             // a finger is down
  clock.t += 100; w.up(1, 400, 300);
  clock.t += 300;
  assert.equal(w.browserMenu().blocked, true);             // 300 ms after the lift: still the finger's
  clock.t += 1000;
  assert.equal(w.browserMenu().blocked, undefined);        // well after: a mouse again
});

test("the layer's own two-finger right click still sends its contextmenu past the guard", () => {
  const w = world({ screen: 'game' });
  w.down(1, 400, 300);
  w.down(2, 500, 400);
  w.up(2, 500, 400);
  w.up(1, 400, 300);
  assert.deepEqual(menus(w.sent).length, 1);
});

test('a belt aim on a wider canvas aims at the box where the shell draws it', () => {
  // 1000x600 puts retail's screen 100 px in, so retail box 100..129 is drawn at 200..229.
  const w = world({ screen: 'game', beltHudBoxes: [{ left: 100, top: 540, right: 129, bottom: 569 }] });
  w.canvas.width = 1000;
  w.canvas.getBoundingClientRect = () => ({ left: 0, top: 0, width: 1000, height: 600 });
  w.down(1, 210, 550);
  w.up(1, 210, 550);
  assert.deepEqual(w.sent.at(-1), left('pointerup', 214.5, 554.5));
});

test('a near miss beside the run button is delivered at the button', () => {
  const w = world({ screen: 'game', controlPanelEvidence: { hud: { runButton: { left: 255, top: 571, w: 16, h: 20 } } } });
  w.down(1, 245, 580);
  w.up(1, 245, 580);
  assert.deepEqual(w.sent.at(-1), left('pointerup', 263, 581));
});

test('a finger held still on a belt potion is a right click; a quick tap there stays a left click', () => {
  const clock = { t: 1000 };
  const belt = [{ left: 100, top: 540, right: 129, bottom: 569 }];
  const w = world({ screen: 'game', beltHudBoxes: belt }, clock);
  w.down(1, 114, 554);
  clock.t += 450; w.touch.flush();
  const press = w.sent.filter((e) => e.type === 'pointerdown');
  assert.deepEqual(press.map((e) => e.button), [2]);
  assert.equal(menus(w.sent).length, 1);
  w.up(1, 114, 554);
  assert.deepEqual([w.sent.at(-1).type, w.sent.at(-1).button], ['pointerup', 2]);
  const t = world({ screen: 'game', beltHudBoxes: belt }, clock);
  t.down(1, 114, 554);
  clock.t += 100; t.up(1, 114, 554);
  assert.deepEqual(t.sent.filter((e) => e.type === 'pointerdown').map((e) => e.button), [0]);
});

test('a tap just beside a monster lands on it; a tap on open ground still walks exactly there', () => {
  // A monster occupies 400..420 x 300..320; the finger lands 6 px left of it.
  const touchPickAt = (x, y) => x >= 400 && x < 420 && y >= 300 && y < 320;
  const w = world({ screen: 'game', touchPickAt });
  w.down(1, 394, 310);
  w.up(1, 394, 310);
  assert.deepEqual(w.sent.at(-1), left('pointerup', 402, 310));
  const g = world({ screen: 'game', touchPickAt });
  g.down(1, 200, 200);
  g.up(1, 200, 200);
  assert.deepEqual(g.sent.at(-1), left('pointerup', 200, 200));
  const on = world({ screen: 'game', touchPickAt });
  on.down(1, 405, 305);
  on.up(1, 405, 305);
  assert.deepEqual(on.sent.at(-1), left('pointerup', 405, 305));
});

test('a tap just beside an open inventory close presses it; on it, or with the panel shut, it does not', () => {
  const w = world({ screen: 'game', inventoryVisible: true });
  w.down(1, 412, 460); w.up(1, 412, 460);
  assert.deepEqual(w.sent.at(-1), left('pointerup', 434.5, 460.5));
  w.down(1, 430, 460); w.up(1, 430, 460);
  assert.deepEqual(w.sent.at(-1), left('pointerup', 430, 460));
  const shut = world({ screen: 'game' });
  shut.down(1, 412, 460); shut.up(1, 412, 460);
  assert.deepEqual(shut.sent.at(-1), left('pointerup', 412, 460));
});

test('with a dialog up, a tap just beside one of its buttons presses it; no dialog, no snap', () => {
  const onButton = (x, y) => x >= 300 && x < 340 && y >= 200 && y < 215;
  const w = world({ screen: 'game', touchControlAt: (x, y) => onButton(x, y) });
  w.down(1, 320, 222); w.up(1, 320, 222);
  const e = w.sent.at(-1); assert.ok(e.type === 'pointerup' && onButton(e.x, e.y));
  const none = world({ screen: 'game', touchControlAt: () => null });
  none.down(1, 320, 222); none.up(1, 320, 222);
  assert.deepEqual(none.sent.at(-1), left('pointerup', 320, 222));
});

// The weapon-set tab geometry of the 800x600 canvas, as the inventory's probe reads it: one arm slot
// (x 400..455, y 340..452) with its tab directly above it (y 316..340).
const ARM_SLOT = { left: 400, top: 340, right: 455, bottom: 452 }, ARM_TAB = { left: 400, top: 316, right: 455, bottom: 340 };
const inside = (x, y, r) => x >= r.left && x < r.right && y >= r.top && y < r.bottom;
function tabProbe(x, y) {
  const gap = Math.hypot(Math.max(ARM_SLOT.left - x, 0, x - ARM_SLOT.right), Math.max(ARM_SLOT.top - y, 0, y - ARM_SLOT.bottom));
  return { tab: inside(x, y, ARM_TAB), slot: inside(x, y, ARM_SLOT), gap };
}

test('the inventory weapon tabs: a tap just beside a tab presses its ring point; on a tab, on a slot, or within reach of one, it lands raw', () => {
  const shell = { screen: 'game', inventoryVisible: true, touchControlAt: () => null, touchWeaponTabAt: tabProbe };
  const near = world(shell);
  near.down(1, 394, 330); near.up(1, 394, 330);
  assert.deepEqual(near.sent.at(-1), left('pointerup', 400, 330));
  for (const [x, y] of [[420, 330], [420, 360], [394, 346]]) {
    const w = world(shell); w.down(1, x, y); w.up(1, x, y);
    assert.deepEqual(w.sent.at(-1), left('pointerup', x, y));
  }
  const dialog = world({ ...shell, touchControlAt: () => false });
  dialog.down(1, 394, 330); dialog.up(1, 394, 330);
  assert.deepEqual(dialog.sent.at(-1), left('pointerup', 394, 330));
  const shut = world({ screen: 'game', inventoryVisible: false, touchWeaponTabAt: () => null });
  shut.down(1, 394, 330); shut.up(1, 394, 330);
  assert.deepEqual(shut.sent.at(-1), left('pointerup', 394, 330));
});

test('out of game, a tap within reach of a control presses its click point; far from all, it lands raw', () => {
  const w = world({ screen: 'create', uiTargets: () => ({ screen: 'create', create: { hardcore: [400, 300], ok: [700, 560] } }) });
  w.down(1, 410, 312); w.up(1, 410, 312);
  assert.deepEqual(w.sent.at(-1), left('pointerup', 400, 300));
  w.down(1, 500, 400); w.up(1, 500, 400);
  assert.deepEqual(w.sent.at(-1), left('pointerup', 500, 400));
});

// The canvas drawn cssW CSS px wide over a backing store `backing` px wide (the page's own scaling). The
// finger lands on that box; returns CSS px per canvas px, to read back what the canvas received.
function drawn(w, cssW, backing) {
  w.canvas.width = backing;
  w.canvas.getBoundingClientRect = () => ({ left: 0, top: 0, width: cssW, height: 600 * cssW / backing });
  return cssW / backing;
}
// The last pointer event the canvas received, back in canvas pixels (the inverse of the drawn box).
const onCanvas = (w, k) => { const e = w.sent.at(-1); return [(e.x + 0.5) / k - 0.5, (e.y + 0.5) / k - 0.5]; };

test('the finger reach is a physical size: the same near miss snaps on a 1:1 canvas, not on one drawn twice as large', () => {
  // The inventory close's left edge is at 418: a finger 10 px left of it is 16 CSS px of reach at 1:1.
  const near = (cssW, backing, x) => {
    const w = world({ screen: 'game', inventoryVisible: true });
    const k = drawn(w, cssW, backing);
    w.down(1, x, 460); w.up(1, x, 460);
    return onCanvas(w, k);
  };
  assert.deepEqual(near(800, 800, 408), [434.5, 460.5]);   // reach 16 canvas px: 10 is in
  assert.deepEqual(near(1600, 800, 408), [408, 460]);      // 800 canvas px drawn over 1600 CSS: reach 8, 10 is out
  assert.deepEqual(near(400, 800, 390), [434.5, 460.5]);   // drawn over 400 CSS: reach 32, 28 is in
});

test('the same reach snaps a front-screen control and the HUD run button by their drawn size', () => {
  const front = (cssW, backing, x, y) => {
    const w = world({ screen: 'create', uiTargets: () => ({ screen: 'create', create: { ok: [400, 300] } }) });
    const k = drawn(w, cssW, backing);
    w.down(1, x, y); w.up(1, x, y);
    return onCanvas(w, k);
  };
  assert.deepEqual(front(800, 800, 410, 312), [400, 300]);   // 15.6 px away, reach 16
  assert.deepEqual(front(1600, 800, 410, 312), [410, 312]);  // reach 8: raw
  assert.deepEqual(front(800, 1600, 430, 300), [400, 300]);  // backing 1600 drawn at 800: reach 32, 30 is in
  const run = (cssW, backing, x) => {
    const w = world({ screen: 'game', controlPanelEvidence: { hud: { runButton: { left: 255, top: 571, w: 16, h: 20 } } } });
    const k = drawn(w, cssW, backing);
    w.down(1, x, 580); w.up(1, x, 580);
    return w.sent.length ? onCanvas(w, k) : 'rest';
  };
  assert.deepEqual(run(800, 800, 240), [263, 581]);   // 15 px left of the button: reach 16 presses it
  // Reach 8: the raw point is in the console gap, so it is a shift rest finger, and sends nothing.
  assert.equal(run(1600, 800, 240), 'rest');
});

test('with the game menu open, a tap just beside one of its rows presses that row; off the rows, it lands raw', () => {
  // The shell answers touchControlAt from gameMenu.hitAt while the menu is open: a row's own rect.
  const rows = [{ x: 300, y: 200, w: 200, h: 24 }, { x: 300, y: 230, w: 200, h: 24 }];
  const onRow = (x, y) => rows.some((r) => x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h);
  const menu = { open: true, hitAt: (x, y) => onRow(x, y) };
  const w = world({ screen: 'game', gameMenu: menu, touchControlAt: (x, y) => (menu.open ? onRow(x, y) : null),
    uiTargets: () => ({ screen: 'game', gameMenu: { rows: [{ x: 400, y: 212 }, { x: 400, y: 242 }] } }) });
  w.down(1, 320, 226); w.up(1, 320, 226);
  const snapped = w.sent.at(-1); assert.ok(snapped.type === 'pointerup' && onRow(snapped.x, snapped.y));
  w.down(1, 296, 196); w.up(1, 296, 196);
  const near = w.sent.at(-1); assert.ok(near.type === 'pointerup' && onRow(near.x, near.y));
  w.down(1, 120, 120); w.up(1, 120, 120);
  assert.deepEqual(w.sent.at(-1), left('pointerup', 120, 120));
});

// ---- inspect before act: a phone's first tap on an item shows its tooltip ----------------------------
// The shell's hooks answer for two rectangles: an inventory cell (item 77) and a shop's stock slot
// (item 900). A tap there is inspected first: a hover (a pointermove with no button held) and no press.
const hoverAt = (x, y, pointerId = 1) => ({ type: 'pointermove', x, y, button: 0, buttons: 0, pointerType: 'touch', pointerId, shiftKey: false });
function inspectShell(extra = {}) {
  const cell = { x0: 300, y0: 200, x1: 330, y1: 230 }, stock = { x0: 500, y0: 100, x1: 540, y1: 140 };
  const inside = (r, x, y) => x >= r.x0 && x < r.x1 && y >= r.y0 && y < r.y1;
  return { screen: 'game', inventoryVisible: true, inventoryLayout: { grid: { left: 300, top: 200, columns: 1, rows: 1, cellWidth: 30, cellHeight: 30 } },
    touchItemAt: (x, y) => (inside(cell, x, y) ? 77 : 0), touchShopItemAt: (x, y) => (inside(stock, x, y) ? 900 : 0), ...extra };
}

test('a first tap on an inventory item shows its tooltip (a hover, no press); the second tap is the click', () => {
  const w = world(inspectShell());
  w.down(1, 310, 210); w.up(1, 310, 210);
  assert.deepEqual(w.sent, [hoverAt(310, 210)]);
  w.down(1, 310, 210); w.up(1, 310, 210);
  assert.deepEqual(w.sent.slice(1), [left('pointermove', 310, 210), left('pointerdown', 310, 210), left('pointerup', 310, 210)]);
  // The click consumed the inspection: the next tap on the same cell inspects it again.
  w.down(1, 310, 210); w.up(1, 310, 210);
  assert.deepEqual(w.sent.at(-1), hoverAt(310, 210));
});

test('a tap elsewhere clears the inspected item: the next tap on it inspects again instead of clicking', () => {
  const w = world(inspectShell());
  w.down(1, 310, 210); w.up(1, 310, 210);
  w.down(1, 600, 400); w.up(1, 600, 400);
  const before = w.sent.length;
  w.down(1, 310, 210); w.up(1, 310, 210);
  assert.deepEqual(w.sent.slice(before), [hoverAt(310, 210)]);
});

test('a shop item: its first tap shows the tooltip and sends no press, so nothing is bought; the second tap buys', () => {
  const w = world(inspectShell());
  w.down(1, 510, 110); w.up(1, 510, 110);
  assert.deepEqual(w.sent, [hoverAt(510, 110)]);
  w.down(1, 510, 110); w.up(1, 510, 110);
  assert.deepEqual(w.sent.slice(1), [left('pointermove', 510, 110), left('pointerdown', 510, 110), left('pointerup', 510, 110)]);
});

test('with an item on the cursor, a tap on an item or on stock acts at once: placing it takes one tap', () => {
  const w = world(inspectShell({ cursorItem: 55 }));
  w.down(1, 310, 210); w.up(1, 310, 210);
  assert.deepEqual(w.sent, [left('pointermove', 310, 210), left('pointerdown', 310, 210), left('pointerup', 310, 210)]);
  const s = world(inspectShell({ cursorItem: 55 }));
  s.down(1, 510, 110); s.up(1, 510, 110);
  assert.deepEqual(s.sent, [left('pointermove', 510, 110), left('pointerdown', 510, 110), left('pointerup', 510, 110)]);
});

test('a hold on an inventory item is its right click, and counts as inspected: the next tap on it clicks', () => {
  const clock = { t: 1000 };
  const w = world(inspectShell(), clock);
  w.down(1, 310, 210);
  clock.t += 450; w.touch.flush();
  assert.deepEqual(w.sent.filter((e) => e.type === 'pointerdown').map((e) => e.button), [2]);
  w.up(1, 310, 210);
  w.down(1, 310, 210); w.up(1, 310, 210);
  assert.deepEqual(w.sent.slice(-3), [left('pointermove', 310, 210), left('pointerdown', 310, 210), left('pointerup', 310, 210)]);
});

test('a finger that drags off an uninspected item is a drag: it presses where it landed, it is not a hover', () => {
  const w = world(inspectShell());
  w.down(1, 310, 210);
  w.move(1, 340, 210);
  assert.deepEqual(w.sent.slice(0, 2), [left('pointermove', 310, 210), left('pointerdown', 310, 210)]);
  assert.ok(!w.sent.some((e) => e.type === 'pointermove' && e.buttons === 0));
  w.up(1, 340, 210);
  assert.deepEqual(w.sent.at(-1), left('pointerup', 340, 210));
});

// ---- M18: the orbs, and the belt's tap-to-inspect -------------------------------------------------------
test('a near miss beside the life or the mana orb presses that orb\'s centre; a far miss lands where it was', () => {
  const life = world({ screen: 'game' });
  life.down(1, 24, 550); life.up(1, 24, 550);
  assert.deepEqual(life.sent, [left('pointermove', 70, 555), left('pointerdown', 70, 555), left('pointerup', 70, 555)]);
  const mana = world({ screen: 'game' });
  mana.down(1, 775, 550); mana.up(1, 775, 550);
  assert.deepEqual(mana.sent, [left('pointermove', 729, 555), left('pointerdown', 729, 555), left('pointerup', 729, 555)]);
  const far = world({ screen: 'game' });
  far.down(1, 5, 550); far.up(1, 5, 550);
  assert.deepEqual(far.sent.at(-1), left('pointerup', 5, 550));
});

test('an open inventory owns its panel: a finger on it beside the mana orb is delivered raw', () => {
  const w = world({ screen: 'game', inventoryVisible: true, inventoryPanelRect: { left: 680, top: 100, width: 120, height: 500 } });
  w.down(1, 686, 550); w.up(1, 686, 550);
  assert.deepEqual(w.sent.at(-1), left('pointerup', 686, 550));
});

// A belt of one potion-holding box at 100..129 x 540..569 (retail 800x600 coordinates, drawn 1:1).
const beltBox = { left: 100, top: 540, right: 129, bottom: 569 };
const beltShell = (extra = {}) => ({ screen: 'game', beltHudBoxes: [beltBox],
  touchBeltItemAt: (x, y) => (x >= 100 && x < 129 && y >= 540 && y < 569 ? 77 : 0), ...extra });
const beltCentre = [114.5, 554.5];

test('a first tap on a belt potion shows its tooltip (a hover at the box, no press); the second tap uses it', () => {
  const w = world(beltShell());
  w.down(1, 114, 554); w.up(1, 114, 554);
  assert.deepEqual(w.sent, [hoverAt(...beltCentre)]);
  w.down(1, 114, 554); w.up(1, 114, 554);
  assert.deepEqual(w.sent.slice(1), [left('pointermove', ...beltCentre), right('pointerdown', ...beltCentre, 1),
    menu(...beltCentre), right('pointerup', ...beltCentre, 1)]);
  // The use spent the inspection: the next tap on the potion inspects it again.
  w.down(1, 114, 554); w.up(1, 114, 554);
  assert.deepEqual(w.sent.at(-1), hoverAt(...beltCentre));
});

test('a belt box with no potion is a plain left click at once: no tooltip, no inspect', () => {
  const w = world(beltShell({ touchBeltItemAt: () => 0 }));
  w.down(1, 114, 554); w.up(1, 114, 554);
  assert.deepEqual(w.sent, [left('pointermove', ...beltCentre), left('pointerdown', ...beltCentre), left('pointerup', ...beltCentre)]);
});

test('with an item on the cursor, a tap on a belt potion acts at once: placing it takes one tap', () => {
  const w = world(beltShell({ cursorItem: 55 }));
  w.down(1, 114, 554); w.up(1, 114, 554);
  assert.deepEqual(w.sent, [left('pointermove', ...beltCentre), left('pointerdown', ...beltCentre), left('pointerup', ...beltCentre)]);
});

test('a drag off an uninspected belt potion presses where it landed; it is not a hover', () => {
  const w = world(beltShell());
  w.down(1, 114, 554);
  w.move(1, 200, 554);
  assert.deepEqual(w.sent.slice(0, 2), [left('pointermove', ...beltCentre), left('pointerdown', ...beltCentre)]);
  assert.ok(!w.sent.some((e) => e.type === 'pointermove' && e.buttons === 0));
  w.up(1, 200, 554);
  assert.deepEqual(w.sent.at(-1), left('pointerup', 200, 554));
});

test('a near miss beside a belt potion snaps to its box: the first tap shows that potion\'s tooltip', () => {
  const w = world(beltShell());
  w.down(1, 132, 554); w.up(1, 132, 554);
  assert.deepEqual(w.sent, [hoverAt(...beltCentre)]);
});

// ---- the panels' small targets: a near miss snaps to the nearest centre the shell reports --------------
// touchPanelTargets answers with canvas centres: a vendor's tabs and buttons, the waypoint menu's tabs,
// close and rows, the NPC menu's rows, the skill popup's icons. A finger within 16 px (the reach on a
// 1:1 canvas) presses the nearest centre; a finger farther from all of them lands where it was.
function tapAt(w, x, y) { w.down(1, x, y); w.up(1, x, y); return w.sent.at(-1); }
const at = (e, x, y) => e.type === 'pointerup' && e.x === x && e.y === y;

test('a vendor window: a tap just beside a tab or a button presses it; far from both, it lands raw', () => {
  const w = world({ screen: 'game', touchPanelTargets: () => ({ shop: { tabs: { 1: [100, 50], 2: [164, 50] }, buttons: { repair: [400, 500], exit: [440, 500] } } }) });
  assert.ok(at(tapAt(w, 108, 56), 100, 50));
  assert.ok(at(tapAt(w, 404, 492), 400, 500));
  assert.deepEqual(tapAt(w, 300, 300), left('pointerup', 300, 300));
});

test('a waypoint menu: a near miss presses the tab, close or row beside it; far from all of them, it lands raw', () => {
  const w = world({ screen: 'game', touchPanelTargets: () => ({ waypoint: { tabs: { 1: [118, 80] }, close: [360, 460], rows: [[200, 150], [200, 190]] } }) });
  assert.ok(at(tapAt(w, 112, 84), 118, 80));
  assert.ok(at(tapAt(w, 200, 176), 200, 190));
  assert.ok(at(tapAt(w, 366, 466), 360, 460));
  assert.deepEqual(tapAt(w, 200, 230), left('pointerup', 200, 230));
});

test('an NPC menu: of two rows a tap between them presses the nearer one', () => {
  // The rows are listed buy-first: a first-hit probe would press buy; the nearer row is talk (14 px away, buy 22 px).
  const w = world({ screen: 'game', touchPanelTargets: () => ({ npc: { buy: [300, 200], talk: [300, 236] } }) });
  assert.ok(at(tapAt(w, 300, 222), 300, 236));
  assert.ok(at(tapAt(w, 300, 208), 300, 200));
});

test('the skill popup: a near miss presses the icon it is beside; of two close icons, the nearer one', () => {
  const w = world({ screen: 'game', touchPanelTargets: () => ({ skills: { 12: [600, 400], 13: [640, 400] } }) });
  assert.ok(at(tapAt(w, 626, 402), 640, 400));
  assert.ok(at(tapAt(w, 604, 396), 600, 400));
});

test('two panels at once: the nearest centre wins across the vendor tab and the waypoint row', () => {
  // (103, 54) is 5 px from the tab and 15 from the row; (108, 62) is 5.7 px from the row and 14.4 from the tab.
  const w = world({ screen: 'game', touchPanelTargets: () => ({ shop: { tabs: { 1: [100, 50] } }, waypoint: { rows: [[112, 66]] } }) });
  assert.ok(at(tapAt(w, 103, 54), 100, 50));
  assert.ok(at(tapAt(w, 108, 62), 112, 66));
});

test('with no panel up the probe reports nothing, and a tap beside where a panel was lands raw', () => {
  const w = world({ screen: 'game', touchPanelTargets: () => null });
  assert.deepEqual(tapAt(w, 100, 56), left('pointerup', 100, 56));
});

// The stash's PlugY page buttons and close, and the cube's Transmute button and grid, in game px at 800x600
// (the origin is identity there). PlugY's buttons are 32x32 cels at BTN's top-left: prevIndex 176, prev 208,
// next 240, nextIndex 272, all at y 445; the stash close cel is at 352,445.
const stashOnly = (plugy) => ({ screen: 'game', touchControlAt: () => null, touchPanelLayout: () => ({ stash: { plugy }, cube: null }) });
const CUBE_GEOMETRY = { left: 280, top: 180, right: 580, bottom: 380, columns: 4, rows: 3, gridLeft: 300, gridTop: 200, boxW: 29, boxH: 29 };
const cubeShell = { screen: 'game', touchControlAt: () => null,
  touchPanelLayout: () => ({ stash: null, cube: { geometry: CUBE_GEOMETRY, panel: { left: 280, top: 180, width: 300, height: 200 } } }) };

test('a tap just beside a stash page button or the stash close presses it; on it, or far off, it lands raw', () => {
  const near = world(stashOnly(true));
  near.down(1, 172, 460); near.up(1, 172, 460);   // 4 px left of the index button at 176..208
  assert.deepEqual(near.sent.at(-1), left('pointerup', 192, 461));
  const close = world(stashOnly(true));
  close.down(1, 390, 460); close.up(1, 390, 460);   // 6 px right of the close cel at 352..384
  assert.deepEqual(close.sent.at(-1), left('pointerup', 368, 461));
  const on = world(stashOnly(true));
  on.down(1, 220, 460); on.up(1, 220, 460);   // inside the prev button: raw
  assert.deepEqual(on.sent.at(-1), left('pointerup', 220, 460));
  const far = world(stashOnly(true));
  far.down(1, 500, 300); far.up(1, 500, 300);
  assert.deepEqual(far.sent.at(-1), left('pointerup', 500, 300));
});

test('PlugY off: the stash close still snaps, but the page buttons are not drawn, so a tap beside where one was lands raw', () => {
  const w = world(stashOnly(false));
  w.down(1, 172, 460); w.up(1, 172, 460);
  assert.deepEqual(w.sent.at(-1), left('pointerup', 172, 460));
  const close = world(stashOnly(false));
  close.down(1, 390, 460); close.up(1, 390, 460);
  assert.deepEqual(close.sent.at(-1), left('pointerup', 368, 461));
});

test('the cube: a tap just beside the Transmute button presses it; on it, it lands raw', () => {
  const near = world(cubeShell);
  near.down(1, 350, 326); near.up(1, 350, 326);   // 5 px under the button at 300..416 x 297..321
  assert.deepEqual(near.sent.at(-1), left('pointerup', 358, 309));
  const on = world(cubeShell);
  on.down(1, 350, 310); on.up(1, 350, 310);
  assert.deepEqual(on.sent.at(-1), left('pointerup', 350, 310));
});

test('the cube grid: a tap in the ring just outside a cell snaps to that cell; a cell interior lands raw', () => {
  const ring = world(cubeShell);
  ring.down(1, 296, 210); ring.up(1, 296, 210);   // 4 px left of the grid, in its top row
  assert.deepEqual(ring.sent.at(-1), left('pointerup', 314.5, 214.5));
  const cell = world(cubeShell);
  cell.down(1, 320, 220); cell.up(1, 320, 220);
  assert.deepEqual(cell.sent.at(-1), left('pointerup', 320, 220));
  const far = world(cubeShell);
  far.down(1, 100, 100); far.up(1, 100, 100);
  assert.deepEqual(far.sent.at(-1), left('pointerup', 100, 100));
});

test('the cube closed (or the inventory shut): no cube control snaps, and a tap beside its button lands raw', () => {
  const shut = world({ screen: 'game', touchControlAt: () => null, touchPanelLayout: () => null });
  shut.down(1, 350, 326); shut.up(1, 350, 326);
  assert.deepEqual(shut.sent.at(-1), left('pointerup', 350, 326));
});

test('a stash or cube item: the first tap shows its tooltip, the second tap is the click (the same probe as the inventory)', () => {
  const cubeItem = (x, y) => (x >= 300 && x < 329 && y >= 200 && y < 229 ? 55 : 0);
  const w = world({ ...inspectShell({ touchItemAt: cubeItem }), touchPanelLayout: cubeShell.touchPanelLayout });
  w.down(1, 310, 210); w.up(1, 310, 210);
  assert.deepEqual(w.sent, [hoverAt(310, 210)]);
  w.down(1, 310, 210); w.up(1, 310, 210);
  assert.deepEqual(w.sent.slice(1), [left('pointermove', 310, 210), left('pointerdown', 310, 210), left('pointerup', 310, 210)]);
});
