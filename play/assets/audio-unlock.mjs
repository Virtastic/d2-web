// A fresh AudioContext starts 'suspended' until the page has a user activation. Only some events grant one:
// keydown, mousedown, a mouse's pointerdown, and for a finger only the LIFT (pointerup, touchend) -- a touchstart
// or a finger's pointerdown does not. A resume() called without activation stays pending (Chrome) or is refused,
// so every listed event retries while the context is still suspended: on a phone the touchstart's attempt does
// nothing and the touchend's succeeds. The listeners go once the context runs; onRunning (optional) then fires.
const GESTURES = ['pointerdown', 'pointerup', 'mousedown', 'keydown', 'touchstart', 'touchend', 'click', 'gamepadconnected'];

export function unlockOnGesture(context, target = globalThis.window, onRunning = null) {
  if (typeof target?.addEventListener !== 'function' || context.state !== 'suspended') return;
  const controller = new AbortController();
  const settled = () => {
    if (context.state === 'suspended' || controller.signal.aborted) return;
    controller.abort();
    if (context.state === 'running') onRunning?.();
  };
  const unlock = () => {
    if (context.state !== 'suspended') { settled(); return; }
    let resumed;
    try { resumed = Promise.resolve(context.resume()); } catch { return; }
    resumed.then(settled, () => {});
  };
  const opts = { signal: controller.signal, passive: true };
  for (const type of GESTURES) target.addEventListener(type, unlock, opts);
}
