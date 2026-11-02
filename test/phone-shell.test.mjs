// Phone page chrome, proved on faked browser objects: a touch outside fullscreen asks for fullscreen and
// then for a landscape lock, once per time the page is not fullscreen; a mouse or pen never does; a refused or throwing request is swallowed;
// the screen wake lock follows the game screen, the page's visibility and a coarse pointer.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { armFirstTouchFullscreen, createWakeLock, installPhoneShell, isCoarsePointer } from '../play/assets/phone-shell.mjs';

// Lets every promise chain queued so far run to the end.
const flush = () => new Promise((resolve) => setImmediate(resolve));

// A document with a listener registry: fire(type, event) delivers an event to what was added for it.
function fakeDocument({ fullscreen, visibilityState = 'visible' } = {}) {
  const listeners = new Map();
  const log = [];
  const documentObject = {
    visibilityState,
    fullscreenElement: null,
    documentElement: fullscreen === undefined ? {} : { requestFullscreen: (...args) => { log.push(['requestFullscreen', args]); return fullscreen(); } },
    addEventListener(type, fn, options) {
      log.push(['add', type, options]);
      listeners.set(type, [...(listeners.get(type) || []), { fn, options }]);
    },
    removeEventListener(type, fn) {
      log.push(['remove', type]);
      listeners.set(type, (listeners.get(type) || []).filter((l) => l.fn !== fn));
    },
    fire(type, event) {
      for (const l of [...(listeners.get(type) || [])]) l.fn(event);
    },
    listenerCount: (type) => (listeners.get(type) || []).length,
    log,
  };
  return documentObject;
}

// A pointer event as the page's listener would see it, with the calls that would change it recorded.
function pointer(pointerType) {
  const event = { type: 'pointerup', pointerType, defaultPrevented: false, propagationStopped: false,
    preventDefault() { this.defaultPrevented = true; }, stopPropagation() { this.propagationStopped = true; } };
  return event;
}

function fakeScreen(lock) {
  const calls = [];
  return { calls, orientation: lock === undefined ? undefined : { lock: (...args) => { calls.push(args); return lock(); } } };
}

// ---- first touch: fullscreen, then landscape --------------------------------------------------------

test('the first touch requests fullscreen, then locks landscape; touches while fullscreen do nothing', async () => {
  const order = [];
  const doc = fakeDocument({ fullscreen: () => { order.push('fullscreen'); doc.fullscreenElement = doc.documentElement; return Promise.resolve(); } });
  const screen = fakeScreen(() => { order.push('lock'); return Promise.resolve(); });
  armFirstTouchFullscreen({ documentObject: doc, windowObject: {}, screenObject: screen });

  doc.fire('pointerup', pointer('touch'));
  await flush();
  doc.fire('pointerup', pointer('touch'));
  doc.fire('pointerup', pointer('touch'));
  await flush();

  assert.deepEqual(order, ['fullscreen', 'lock']);
  assert.deepEqual(screen.calls, [['landscape']]);
  assert.equal(doc.listenerCount('pointerup'), 1, 'the listener stays for a later exit from fullscreen');
});

test('after the player leaves fullscreen, the next touch asks again', async () => {
  const doc = fakeDocument({ fullscreen: () => { doc.fullscreenElement = doc.documentElement; return Promise.resolve(); } });
  const screen = fakeScreen(() => Promise.resolve());
  armFirstTouchFullscreen({ documentObject: doc, windowObject: {}, screenObject: screen });
  const requests = () => doc.log.filter(([kind]) => kind === 'requestFullscreen').length;
  doc.fire('pointerup', pointer('touch'));
  await flush();
  assert.equal(requests(), 1);
  doc.fullscreenElement = null;            // the back gesture leaves fullscreen
  doc.fire('pointerup', pointer('mouse'));
  await flush();
  assert.equal(requests(), 1, 'a mouse lift never asks');
  doc.fire('pointerup', pointer('touch'));
  await flush();
  assert.equal(requests(), 2);
  assert.deepEqual(screen.calls, [['landscape'], ['landscape']]);
});

test('a refused request is retried on the next touch, never while one is still settling', async () => {
  let refuse;
  const doc = fakeDocument({ fullscreen: () => new Promise((_, reject) => { refuse = reject; }) });
  armFirstTouchFullscreen({ documentObject: doc, windowObject: {}, screenObject: fakeScreen(() => Promise.resolve()) });
  const requests = () => doc.log.filter(([kind]) => kind === 'requestFullscreen').length;
  doc.fire('pointerup', pointer('touch'));
  doc.fire('pointerup', pointer('touch'));
  await flush();
  assert.equal(requests(), 1, 'no second request while the first is pending');
  refuse(new Error('denied'));
  await flush();
  doc.fire('pointerup', pointer('touch'));
  assert.equal(requests(), 2);
});

test('the lock waits for the fullscreen request to settle, granted or refused', async () => {
  const order = [];
  let refuse;
  const doc = fakeDocument({ fullscreen: () => new Promise((_, reject) => { refuse = reject; }) });
  const screen = fakeScreen(() => { order.push('lock'); return Promise.resolve(); });
  armFirstTouchFullscreen({ documentObject: doc, windowObject: {}, screenObject: screen });
  doc.fire('pointerup', pointer('touch'));
  await flush();
  assert.deepEqual(order, [], 'no lock while fullscreen is still pending');
  refuse(new Error('denied'));
  await flush();
  assert.deepEqual(order, ['lock'], 'a refused fullscreen still lets the lock be tried, and nothing throws');
});

test('a mouse or pen lift does neither, and leaves the listener armed for a touch', async () => {
  const doc = fakeDocument({ fullscreen: () => Promise.resolve() });
  const screen = fakeScreen(() => Promise.resolve());
  armFirstTouchFullscreen({ documentObject: doc, windowObject: {}, screenObject: screen });

  const mouse = pointer('mouse');
  const pen = pointer('pen');
  doc.fire('pointerup', mouse);
  doc.fire('pointerup', pen);
  await flush();
  assert.equal(doc.log.filter(([kind]) => kind === 'requestFullscreen').length, 0);
  assert.deepEqual(screen.calls, []);
  assert.equal(doc.listenerCount('pointerup'), 1);

  doc.fire('pointerup', pointer('touch'));
  await flush();
  assert.equal(doc.log.filter(([kind]) => kind === 'requestFullscreen').length, 1);
  assert.deepEqual(screen.calls, [['landscape']]);
});

test('the listener is passive and never prevents or stops the event', () => {
  const doc = fakeDocument({ fullscreen: () => Promise.resolve() });
  armFirstTouchFullscreen({ documentObject: doc, windowObject: {}, screenObject: fakeScreen(() => Promise.resolve()) });
  const add = doc.log.find(([kind, type]) => kind === 'add' && type === 'pointerup');
  assert.deepEqual(add[2], { capture: true, passive: true });

  const touch = pointer('touch');
  doc.fire('pointerup', touch);
  assert.equal(touch.defaultPrevented, false);
  assert.equal(touch.propagationStopped, false);
});

test('a throwing requestFullscreen is swallowed, and the lock is still tried', async () => {
  const doc = fakeDocument({ fullscreen: () => { throw new TypeError('not allowed'); } });
  const screen = fakeScreen(() => Promise.resolve());
  armFirstTouchFullscreen({ documentObject: doc, windowObject: {}, screenObject: screen });
  assert.doesNotThrow(() => doc.fire('pointerup', pointer('touch')));
  await flush();
  assert.deepEqual(screen.calls, [['landscape']]);
});

test('a throwing or rejecting orientation.lock is swallowed', async () => {
  const doc = fakeDocument({ fullscreen: () => Promise.resolve() });
  const throwing = fakeScreen(() => { throw new Error('no lock'); });
  armFirstTouchFullscreen({ documentObject: doc, windowObject: {}, screenObject: throwing });
  assert.doesNotThrow(() => doc.fire('pointerup', pointer('touch')));
  await flush();
  assert.equal(throwing.calls.length, 1);

  const doc2 = fakeDocument({ fullscreen: () => Promise.reject(new Error('denied')) });
  const rejecting = fakeScreen(() => Promise.reject(new Error('no lock')));
  armFirstTouchFullscreen({ documentObject: doc2, windowObject: {}, screenObject: rejecting });
  doc2.fire('pointerup', pointer('touch'));
  await flush();   // an unhandled rejection would fail the test run here
  assert.equal(rejecting.calls.length, 1);
});

test('no requestFullscreen and no orientation (iOS) is silent', async () => {
  const doc = fakeDocument();   // documentElement has no requestFullscreen
  assert.doesNotThrow(() => armFirstTouchFullscreen({ documentObject: doc, windowObject: {}, screenObject: {} }));
  assert.doesNotThrow(() => doc.fire('pointerup', pointer('touch')));
  await flush();
  assert.equal(doc.listenerCount('pointerup'), 0);
});

// ---- wake lock --------------------------------------------------------------------------------------

// A wake lock API whose request() resolves to a sentinel the test can see released.
function fakeWakeLock({ reject = false } = {}) {
  const requests = [];
  const sentinels = [];
  return {
    requests, sentinels,
    request(type) {
      requests.push(type);
      if (reject) return Promise.reject(new Error('NotAllowedError'));
      const sentinel = { released: false, listeners: [], release() { this.released = true; return Promise.resolve(); },
        addEventListener(type, fn) { this.listeners.push([type, fn]); } };
      sentinels.push(sentinel);
      return Promise.resolve(sentinel);
    },
  };
}

// A phone (coarse pointer) with a game screen the test can move between states.
function phone({ screen = 'title', coarse = true, visibilityState = 'visible' } = {}) {
  const windowObject = {
    __d2Shell: { screen },
    matchMedia: (query) => ({ matches: coarse && query === '(pointer: coarse)' }),
  };
  const doc = fakeDocument({ visibilityState });
  return { windowObject, doc };
}

test('the wake lock is requested on the game screen and released on leaving it', async () => {
  const { windowObject, doc } = phone({ screen: 'title' });
  const navigatorObject = { wakeLock: fakeWakeLock() };
  const wake = createWakeLock({ windowObject, documentObject: doc, navigatorObject });

  wake.sync();
  await flush();
  assert.deepEqual(navigatorObject.wakeLock.requests, [], 'not requested on the title screen');

  windowObject.__d2Shell.screen = 'game';
  wake.sync();
  wake.sync();   // a second poll while the request is in flight or held does not ask again
  await flush();
  assert.deepEqual(navigatorObject.wakeLock.requests, ['screen']);
  assert.ok(wake.held());

  windowObject.__d2Shell.screen = 'create';
  wake.sync();
  await flush();
  assert.equal(navigatorObject.wakeLock.sentinels[0].released, true);
  assert.equal(wake.held(), null);
});

test('the wake lock is re-requested on visibilitychange to visible while still in game', async () => {
  const { windowObject, doc } = phone({ screen: 'game' });
  const navigatorObject = { wakeLock: fakeWakeLock() };
  const wake = createWakeLock({ windowObject, documentObject: doc, navigatorObject });
  wake.sync();
  await flush();
  assert.deepEqual(navigatorObject.wakeLock.requests, ['screen']);

  // the page is hidden: the browser lets the lock go, and the sentinel fires 'release'
  doc.visibilityState = 'hidden';
  doc.fire('visibilitychange');
  await flush();
  assert.deepEqual(navigatorObject.wakeLock.requests, ['screen'], 'nothing is requested while hidden');

  doc.visibilityState = 'visible';
  doc.fire('visibilitychange');
  await flush();
  assert.deepEqual(navigatorObject.wakeLock.requests, ['screen', 'screen']);
  assert.ok(wake.held());
});

test('a visibilitychange to visible re-asks even when the held lock was never released', async () => {
  // the browser may not fire 'release' on every path; the visible event drops the stale sentinel and asks again
  const { windowObject, doc } = phone({ screen: 'game' });
  const navigatorObject = { wakeLock: fakeWakeLock() };
  const wake = createWakeLock({ windowObject, documentObject: doc, navigatorObject });
  wake.sync();
  await flush();
  wake.onVisibility();
  await flush();
  assert.deepEqual(navigatorObject.wakeLock.requests, ['screen', 'screen']);
  assert.equal(navigatorObject.wakeLock.sentinels[0].released, false, 'the old sentinel is left to the browser, not released by us');
});

test('no wakeLock API is silent', async () => {
  const { windowObject, doc } = phone({ screen: 'game' });
  const wake = createWakeLock({ windowObject, documentObject: doc, navigatorObject: {} });
  assert.doesNotThrow(() => wake.sync());
  doc.visibilityState = 'hidden';
  assert.doesNotThrow(() => doc.fire('visibilitychange'));
  assert.equal(wake.held(), null);
});

test('a rejected wake lock request is swallowed and the next sync tries again', async () => {
  const { windowObject, doc } = phone({ screen: 'game' });
  const failing = fakeWakeLock({ reject: true });
  const wake = createWakeLock({ windowObject, documentObject: doc, navigatorObject: { wakeLock: failing } });
  assert.doesNotThrow(() => wake.sync());
  await flush();   // a rejection with no handler would fail the run here
  assert.equal(wake.held(), null);
  wake.sync();
  await flush();
  assert.deepEqual(failing.requests, ['screen', 'screen']);
});

test('a desktop (fine pointer) never takes a wake lock', async () => {
  const { windowObject, doc } = phone({ screen: 'game', coarse: false });
  const navigatorObject = { wakeLock: fakeWakeLock() };
  const wake = createWakeLock({ windowObject, documentObject: doc, navigatorObject });
  wake.sync();
  doc.visibilityState = 'hidden';
  doc.fire('visibilitychange');
  await flush();
  assert.deepEqual(navigatorObject.wakeLock.requests, []);
});

test('isCoarsePointer is false without matchMedia', () => {
  assert.equal(isCoarsePointer({}), false);
  assert.equal(isCoarsePointer({ matchMedia: () => ({ matches: true }) }), true);
});

// ---- the whole page ---------------------------------------------------------------------------------

test('installPhoneShell wires the touch gesture, the visibility listener and a poll of the wake lock', async () => {
  const { windowObject, doc } = phone({ screen: 'game' });
  const screen = fakeScreen(() => Promise.resolve());
  doc.documentElement.requestFullscreen = () => Promise.resolve();
  const navigatorObject = { wakeLock: fakeWakeLock() };
  const polls = [];
  const handle = installPhoneShell({
    documentObject: doc, windowObject, navigatorObject, screenObject: screen,
    setIntervalFn: (fn, ms) => { polls.push([fn, ms]); return 1; },
  });

  assert.equal(polls.length, 1);
  assert.equal(polls[0][1], 250);
  polls[0][0]();   // one poll: the game screen asks for the lock
  await flush();
  assert.deepEqual(navigatorObject.wakeLock.requests, ['screen']);

  doc.fire('pointerup', pointer('touch'));
  await flush();
  assert.deepEqual(screen.calls, [['landscape']]);
  assert.ok(handle.wake && handle.fullscreen);
});

test('installPhoneShell on a desktop page with no APIs at all does not throw', async () => {
  const doc = fakeDocument();
  assert.doesNotThrow(() => installPhoneShell({ documentObject: doc, windowObject: {}, navigatorObject: {}, screenObject: {}, setIntervalFn: null }));
  assert.doesNotThrow(() => doc.fire('pointerup', pointer('mouse')));
});

test('the portrait rotate notice never covers the unsupported-browser screen', async () => {
  const { readFileSync } = await import('node:fs');
  const html = readFileSync(new URL('../play/index.html', import.meta.url), 'utf8');
  assert.match(html, /@media \(orientation: portrait\) and \(pointer: coarse\) and \(max-width: 600px\)\{#rotate-notice\{display:flex\}\}/);
  // An upright iPhone gets the unsupported screen; the notice must yield to it, and come after the media rule.
  const media = html.indexOf('#rotate-notice{display:flex}');
  const yieldRule = html.indexOf('body.unsupported #rotate-notice{display:none}');
  assert.ok(yieldRule > media && media > 0, 'body.unsupported hides #rotate-notice after the portrait rule');
  assert.match(html, /document\.body\.classList\.add\('unsupported'\)/);
});

test('a finger on the viewport selects no text, opens no callout and flashes no tap highlight', async () => {
  const { readFileSync } = await import('node:fs');
  const html = readFileSync(new URL('../play/index.html', import.meta.url), 'utf8');
  const rule = html.match(/#viewport\{-webkit-tap-highlight-color[^}]*\}/)?.[0] || '';
  for (const decl of ['-webkit-tap-highlight-color:transparent', '-webkit-touch-callout:none', 'user-select:none'])
    assert.ok(rule.includes(decl), decl);
  assert.match(html, /html,body\{overscroll-behavior:none\}/);
});
