// Phone page chrome: fullscreen and landscape on a touch outside fullscreen, and a screen that stays awake while
// the game is on screen. Platform-layer only -- it asks the browser for display state and carries no
// game logic. Every browser call is wrapped: iOS has neither requestFullscreen on <html> nor
// screen.orientation.lock, and a refused request is not an error the player needs to see.
//
// Desktop is untouched: the gesture listener returns at once for anything but a touch pointer, and the
// wake lock is held only on a coarse pointer (a phone or tablet), so a desktop tab never takes one.
//
// The game screen is read from window.__d2Shell.screen (gameplay-shell.mjs publishes it as a getter).
// The shell does not announce its screen changes, so the wake lock is re-checked on a short poll, the
// same way input-touch.mjs follows the character-create name box.

export const WAKE_POLL_MS = 250;

// Calls fn and returns its value, or undefined if it throws. A missing method (fn itself undefined) is
// a no-op too, which is how the iOS case is handled without a separate branch.
function attempt(fn) {
  try { return fn(); } catch { return undefined; }
}

// Resolves a value that may be a promise, a plain value or undefined; never rejects.
function settle(value, onDone) {
  const done = () => attempt(onDone);
  if (value && typeof value.then === 'function') value.then(done, done);
  else done();
}

// A touch gesture outside fullscreen: fullscreen, then landscape. It runs on the first touch and again on
// the next touch after the player leaves fullscreen (Android's back gesture or the system bar), the way a
// native game returns to immersive mode; a touch while fullscreen, or while a request is still settling,
// does nothing. Android allows orientation.lock only once the page
// is fullscreen, so the lock waits for the fullscreen request to settle (whether it was granted or
// refused). It runs on the finger's LIFT: a touch pointerdown is not a user activation (the HTML spec
// grants it to pointerup for touch), so a fullscreen request from the down is refused. It listens in the
// capture phase on the document, beside the touch layer's own listeners there: the touch layer stops the
// canvas's events from going further, which would hide them from a bubble-phase listener. A passive
// listener: it never calls preventDefault and never stops propagation. Without requestFullscreen (iOS)
// it removes itself after the first touch.
const FIRST_TOUCH = { capture: true, passive: true };
export function armFirstTouchFullscreen({ documentObject, windowObject, screenObject }) {
  const root = documentObject?.documentElement;
  let pending = false;
  const onTouch = (event) => {
    if (event?.pointerType !== 'touch' || pending) return;
    if (attempt(() => documentObject.fullscreenElement)) return;
    // An installed app already fills the screen; asking again only raises the browser's long "exit full screen" notice.
    if (attempt(() => windowObject?.matchMedia?.('(display-mode: fullscreen), (display-mode: standalone)')?.matches)) return;
    if (typeof attempt(() => root?.requestFullscreen) !== 'function') {
      documentObject.removeEventListener('pointerup', onTouch, FIRST_TOUCH);
    }
    pending = true;
    const requested = attempt(() => root?.requestFullscreen?.());
    settle(requested, () => {
      pending = false;
      const orientation = attempt(() => screenObject?.orientation);
      settle(attempt(() => orientation?.lock?.('landscape')), () => {});   // a rejected lock is ignored too
    });
  };
  attempt(() => documentObject?.addEventListener?.('pointerup', onTouch, FIRST_TOUCH));
  return { disarm: () => attempt(() => documentObject?.removeEventListener?.('pointerup', onTouch, FIRST_TOUCH)) };
}

// Whether the phone's primary pointer is coarse (a touch screen). Without matchMedia (a test fake, or an
// old browser) it answers false, which keeps desktop behaviour.
export function isCoarsePointer(windowObject) {
  return Boolean(attempt(() => windowObject?.matchMedia?.('(pointer: coarse)')?.matches));
}

// The screen wake lock. sync() is the single decision point: it holds the lock exactly while the game
// screen is up, the page is visible and the pointer is coarse; it releases the lock otherwise. A
// visibilitychange to visible drops any sentinel we still hold (the browser releases it when the page
// is hidden) and asks again.
export function createWakeLock({ windowObject, documentObject, navigatorObject }) {
  let held = null;       // the WakeLockSentinel we hold, or null
  let pending = false;   // a request is in flight; do not start another

  const wantLock = () =>
    attempt(() => windowObject?.__d2Shell?.screen) === 'game'
    && isCoarsePointer(windowObject)
    && attempt(() => documentObject?.visibilityState) !== 'hidden';

  function release() {
    const sentinel = held;
    held = null;
    if (sentinel) attempt(() => sentinel.release?.());
  }

  function request() {
    const wakeLock = attempt(() => navigatorObject?.wakeLock);
    if (!wakeLock || typeof wakeLock.request !== 'function') return;
    pending = true;
    const started = attempt(() => wakeLock.request('screen'));
    // a rejection is ignored (the handler below takes it); the next sync() tries again
    if (started && typeof started.then === 'function') {
      started.then((sentinel) => {
        pending = false;
        if (!sentinel) return;
        if (!wantLock()) { attempt(() => sentinel.release?.()); return; }
        held = sentinel;
        attempt(() => sentinel.addEventListener?.('release', () => { if (held === sentinel) held = null; }));
      }, () => { pending = false; });
    } else {
      pending = false;
    }
  }

  function sync() {
    if (wantLock()) { if (!held && !pending) request(); }
    else release();
  }

  function onVisibility() {
    if (attempt(() => documentObject?.visibilityState) !== 'visible') return;
    held = null;   // the browser let go when the page was hidden; that sentinel is dead
    sync();
  }

  attempt(() => documentObject?.addEventListener?.('visibilitychange', onVisibility));
  return {
    sync,
    onVisibility,
    held: () => held,
    dispose() {
      attempt(() => documentObject?.removeEventListener?.('visibilitychange', onVisibility));
      release();
    },
  };
}

// Wires the page: the first-touch fullscreen listener, the wake lock and its poll. Returns the handles
// so a test (or a later caller) can drive or dispose them. setIntervalFn is injectable; pass null to
// skip the poll and drive sync() by hand.
export function installPhoneShell({
  documentObject = globalThis.document,
  windowObject = globalThis.window,
  navigatorObject = globalThis.navigator,
  screenObject = globalThis.screen,
  setIntervalFn = globalThis.setInterval,
} = {}) {
  const fullscreen = armFirstTouchFullscreen({ documentObject, windowObject, screenObject });
  const wake = createWakeLock({ windowObject, documentObject, navigatorObject });
  const poll = typeof setIntervalFn === 'function' ? setIntervalFn(() => wake.sync(), WAKE_POLL_MS) : null;
  return { fullscreen, wake, poll };
}

// Self-install in a page: a module that imports without a window does nothing.
if (typeof window !== 'undefined' && typeof document !== 'undefined' && document.addEventListener) window.__d2PhoneShell = installPhoneShell();
