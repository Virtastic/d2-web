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

  // A finger event at canvas pixel (x, y), as the browser delivers it to the document.
  function finger(type, pointerId, x, y, extra = {}) {
    const e = {
      type, pointerType: 'touch', pointerId, clientX: x + 0.5, clientY: y + 0.5, target: canvas,
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
