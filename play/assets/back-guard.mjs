// Android's Back must never leave the game. Platform layer only: no game action is taken on Back.
//
// Android turns a controller's B into Back when the page does not consume B's key event (the system
// key map's fallback for BUTTON_B), and a handheld's own pad often sends Back outright. Chrome then
// goes back in the tab's history, and with no earlier entry it closes the tab or leaves the browser.
// Two guards, both on the page's side:
//   * a history entry of our own on top of the page: Back pops it, and the popstate puts it back, so
//     the page never navigates. Chrome skips an entry a page added with no user interaction, so the
//     entry is pushed again on the next trusted press whenever it is not on top;
//   * a controller button that reaches the page as a key event with no key of its own ('Unidentified'
//     with no code), or the browser's Back key itself, is preventDefault-ed, so Android sees it consumed
//     and never makes Back of it. Every other key (Escape included) is left alone.
// Armed on a touch screen, or as soon as a controller connects; a desktop tab with a mouse is untouched.

export const GUARD_STATE = 'd2-back-guard';
const ACTIVATION = ['keydown', 'pointerup', 'mousedown', 'touchend'];

/** True for a key event Android would turn into Back if the page did not consume it. */
export function isBackKey(e) {
  if (!e) return false;
  if (e.key === 'GoBack' || e.key === 'BrowserBack' || e.code === 'BrowserBack') return true;
  if (e.key !== 'Unidentified' || e.code) return false;
  // A soft keyboard or an IME also reports 'Unidentified' with no code (keyCode 229, composing): that is typing, not a pad button.
  if (e.isComposing || e.keyCode === 229) return false;
  const t = e.target;
  return !(t && (t.isContentEditable || /^(INPUT|TEXTAREA|SELECT)$/.test(t.tagName || '')));
}

export function createBackGuard({ windowObject = globalThis.window, historyObject = windowObject?.history } = {}) {
  let armed = false, onTop = false, absorbed = 0;
  const top = () => historyObject?.state?.[GUARD_STATE] === true;
  const push = () => {
    try { historyObject.pushState({ ...(historyObject.state || {}), [GUARD_STATE]: true }, ''); onTop = true; } catch { /* a sandboxed frame */ }
  };
  const onPop = () => {
    onTop = top();
    if (onTop) return;
    absorbed++;
    push();
  };
  const onKey = (e) => { if (isBackKey(e)) e.preventDefault?.(); };
  const onActivation = (e) => {
    if (e?.isTrusted === false) return;
    if (!onTop || !top()) push();
  };
  const api = {
    arm() {
      if (armed || !historyObject?.pushState) return api;
      armed = true;
      if (!top()) push(); else onTop = true;
      windowObject.addEventListener('popstate', onPop);
      for (const t of ACTIVATION) windowObject.addEventListener(t, onActivation, { capture: true, passive: true });
      return api;
    },
    get evidence() { return { armed, onTop, absorbed }; },
  };
  // The key guard runs from load: a B that arrives before anything else must already be consumed.
  windowObject?.addEventListener?.('keydown', onKey, { capture: true });
  windowObject?.addEventListener?.('keyup', onKey, { capture: true });
  return api;
}

/** Wires the page: armed now on a touch screen, else on the first controller to connect. */
export function installBackGuard({ windowObject = globalThis.window } = {}) {
  const guard = createBackGuard({ windowObject });
  let coarse = false;
  try { coarse = Boolean(windowObject.matchMedia?.('(pointer: coarse)')?.matches); } catch { /* no matchMedia */ }
  if (coarse) guard.arm();
  else windowObject.addEventListener?.('gamepadconnected', () => guard.arm(), { once: true });
  return guard;
}

if (typeof window !== 'undefined' && window.addEventListener && window.history) window.__d2BackGuard = installBackGuard();
