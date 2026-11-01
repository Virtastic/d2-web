// A front-end screen learns the pointer only from a pointermove, so a screen that is entered (or
// rebuilt: SELECT, CREATE, a title re-made by exitToTitle) while the mouse is still, or one that sat
// behind another screen while the mouse moved, draws its ohand cel where the pointer WAS. Round
// trips between screens (TITLE, SINGLE PLAYER, back) made the cursor jump on every return.
// This keeps the last real canvas-pixel pointer and replays it into whichever screen becomes
// current, so the drawn cursor always sits under the real one. Platform plumbing only: the screens
// keep their own 800x600 mapping.
export function createFrontendHoverSync() {
  let at = null, shown = null;   // last canvas-pixel pointer; the screen object it was last delivered to
  return {
    /** A pointermove at canvas pixel (x, y); `current` is { key, hover(x, y) } or null. */
    move(x, y, current) { at = [x, y]; shown = current ? current.key : null; current?.hover(x, y); },
    /** Once per frame: a different current screen is given the pointer it has not seen. */
    frame(current) {
      const key = current ? current.key : null;
      if (key === shown) return false;
      shown = key;
      if (!current || !at) return false;
      current.hover(at[0], at[1]);
      return true;
    },
    get at() { return at && [...at]; },
  };
}
