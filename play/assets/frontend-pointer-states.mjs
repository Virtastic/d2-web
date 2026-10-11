// Front-end gestures: a press ARMS the control under the pointer, and the matching release
// COMPLETES it, but only if the pointer is still over the control its own press armed.
// Retail activates a button on mouse-up and re-hit-tests there (OpenDiablo2's mouse-up handlers
// are separate entry points from mouse-down, and a drag off the control is not a click). Two
// things follow, and both are player-visible:
//   * pressing DELETE's YES and releasing elsewhere is a CANCEL -- it must not delete the save;
//   * the control gets to draw its pressed cel between the two, which is the only moment retail
//     shows a front-end button in its down state (hover is the up cel).
// A gesture also belongs to the pointer that began it: a second finger must not be able to release
// the first finger's control, and a `pointercancel` (the browser taking the touch for a gesture)
// ends the gesture without activating anything.
export function createFrontendPointerStates() {
  const armed = new Map();

  return {
    /**
     * Arm `control` for `pointerId`. Returns the token `control.press()` handed back -- the identity
     * of the thing under this pointer -- or null when nothing was hit.
     */
    press(pointerId, control, x, y, width, height) {
      armed.delete(pointerId);   // a second press from the same pointer replaces its old gesture
      if (!control) return null;
      const token = control.press?.(x, y, width, height) ?? null;
      armed.set(pointerId, { control, token, x, y });
      return token;
    },
    /**
     * Complete `pointerId`'s gesture at the RELEASE point. `current` is the control that owns the
     * pointer right now: if the screen changed under the gesture, the stale arm is cancelled rather
     * than re-hit-tested against a screen the player has left. The control re-hit-tests the release
     * point itself and fires only when it resolves to the same token its press armed.
     */
    release(pointerId, current, x, y, width, height) {
      const active = armed.get(pointerId);
      if (!active) return null;   // a release with no matching press does nothing
      armed.delete(pointerId);
      if (active.control !== current) { active.control.cancel?.(); return null; }
      // A control that hit nothing on press has no token: still let it clear its own state.
      if (active.token == null) { active.control.cancel?.(); return null; }
      return active.control.release?.(active.token, x, y, width, height) ?? null;
    },
    /** Drop `pointerId`'s gesture without activating anything (a `pointercancel`). */
    cancel(pointerId) {
      const active = armed.get(pointerId);
      if (!active) return null;
      armed.delete(pointerId);
      active.control.cancel?.();
      return { pointerId, token: active.token };
    },
    get armed() { return [...armed.entries()].map(([pointerId, a]) => ({ pointerId, token: a.token })); },
  };
}
