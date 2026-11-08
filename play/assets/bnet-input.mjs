// Platform wiring for the Battle.net login and lobby screens: the keyboard, the wheel, a drag on a
// scrollbar and a tap that raises the touch keyboard, each handed to the screen the page is showing.
// The screens own every rule (what a key does, where a control is); this only moves events to them.
// A press, a release and a hover on a control go through the shell's generic front-end pointer path.
import { canvasPoint } from './canvas-point.mjs';
import { createTouchTextEntry } from './touch-text-entry.mjs';

/** `current()` is { kind: 'login' | 'lobby', screen } for the Battle.net screen on show, else null. */
export function createBnetInput({ canvas, current: raw, render = () => {}, documentObject = globalThis.document }) {
  let drag = null;
  const current = () => { const c = raw(); return c?.screen ? c : null; };   // a screen that failed to build has nothing to take events
  const entry = createTouchTextEntry({ documentObject,
    typeKey: (code, key) => { current()?.screen?.typeKey?.(code, key); },
    getValue: () => current()?.screen?.editValue?.() ?? null,
    isSecret: () => current()?.screen?.editSecret?.() ?? false,
    onChange: render, onBlur: () => canvas.focus() });
  const enter = (e) => e.key === 'Enter' || e.code === 'Enter' || e.code === 'NumpadEnter';
  return {
    // A keydown. True when a Battle.net screen is up (it owns the keyboard whether or not a key does anything).
    key(e) {
      if (!raw()) return false;
      const c = current(); if (!c) return true;
      // The touch keyboard's own input turns Enter into a key call itself; the window must not send it twice.
      if (e.target && e.target === entry.element && enter(e)) return true;
      if (e.code === 'Escape') { if (!e.repeat) c.screen?.escape(); e.preventDefault(); render(); return true; }
      if (!e.ctrlKey && !e.metaKey && !e.altKey && c.screen?.typeKey(e.code, e.key)) { e.preventDefault(); render(); }
      return true;
    },
    // The wheel over the lobby's game list or chat log: one row per notch (more for a big delta).
    wheel(e) {
      const c = current(); if (c?.kind !== 'lobby' || !e.deltaY) return false;
      const [x, y] = canvasPoint(canvas, e), px = e.deltaY * (e.deltaMode === 1 ? 16 : e.deltaMode === 2 ? 160 : 1);
      if (!c.screen.wheel(x, y, canvas.width, canvas.height, Math.sign(px) * Math.max(1, Math.round(Math.abs(px) / 40)))) return false;
      e.preventDefault(); render(); return true;
    },
    // A primary press on a scrollbar: the screen sets the thumb and the drag follows the pointer. True when it took the press.
    down(e) {
      const c = current(); if (c?.kind !== 'lobby' || (e.button ?? 0) !== 0) return false;
      const [x, y] = canvasPoint(canvas, e), token = c.screen.scrollPress(x, y, canvas.width, canvas.height);
      if (token === null) return false;
      drag = { pointerId: e.pointerId, token };
      try { canvas.setPointerCapture?.(e.pointerId); } catch { /* a synthetic pointer */ }
      render(); return true;
    },
    move(e) {
      if (!drag || drag.pointerId !== e.pointerId) return false;
      const c = current(), [, y] = canvasPoint(canvas, e);
      if (c?.kind !== 'lobby' || !c.screen.scrollDrag(drag.token, y, canvas.width, canvas.height)) drag = null;
      render(); return true;
    },
    // The release (or cancel) of a drag; true when it was one.
    dragEnd(e) { if (!drag || drag.pointerId !== e.pointerId) return false; drag = null; current()?.screen?.scrollEnd?.(); render(); return true; },
    // Each frame: the touch keyboard stays up only while a box takes text (a screen change, a busy login or a closed panel puts it away).
    sync() { if (entry.element && documentObject.activeElement === entry.element && (current()?.screen?.editValue?.() ?? null) === null) entry.close(); },
    get dragging() { return drag !== null; },
    // After a release was handled as a click: a tap on a text box (the lobby's chat line, any touch on the lobby, a login box) raises
    // the keyboard; a click elsewhere puts it away. True when the keyboard is up for it.
    tap(e) {
      const c = current(); if (!c) return false;
      const [x, y] = canvasPoint(canvas, e);
      const wants = c.kind === 'lobby' ? c.screen.editAt(x, y, canvas.width, canvas.height) || e.pointerType === 'touch'
        : c.screen.editAt(x, y, canvas.width, canvas.height);
      if (wants && entry.open()) return true;
      entry.close(); return false;
    },
    get textEntry() { return entry; },
  };
}
