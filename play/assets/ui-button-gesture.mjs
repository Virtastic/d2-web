/** Tracks one momentary UI button from pointer press to matching release. */
export function createUiButtonGesture() {
  let armed = null;
  return {
    press(pointerId, token) {
      armed = token == null ? null : { pointerId, token };
      return armed?.token ?? null;
    },
    release(pointerId, token) {
      const active = armed;
      if (!active || active.pointerId !== pointerId) return null;
      armed = null;
      return active.token === token ? active.token : null;
    },
    cancel(pointerId = null) {
      if (armed && (pointerId == null || armed.pointerId === pointerId)) { armed = null; return true; }
      return false;
    },
    get token() { return armed?.token ?? null; },
  };
}
