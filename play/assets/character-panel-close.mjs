// D2Client's character sheet close control. Native 1.13c draws frame 10 plus its
// held flag, and checks the release point against the same inclusive rectangle.
export const CHARACTER_PANEL_CLOSE_RECT = Object.freeze({ x1: 208, x2: 240, y1: 448, y2: 480 });
export const characterPanelCloseFrame = (pressed) => pressed ? 11 : 10;

export function characterPanelCloseAt(px, py, width, height) {
  const scale = Math.min(1, width / 800, height / 600);
  const x = (px - (width - 800 * scale) / 2) / scale;
  const y = (py - (height - 600 * scale) / 2) / scale;
  const r = CHARACTER_PANEL_CLOSE_RECT;
  return x >= r.x1 && x <= r.x2 && y >= r.y1 && y <= r.y2;
}

// Pointer ownership is the browser gesture choice. Native FUN_6FB274D0 proves
// release-side bounds but does not read the prior-down flag after clearing it.
export function createCharacterPanelCloseGesture() {
  let pointerId = null;
  return {
    press(id) { if (pointerId !== null) return false; pointerId = id; return true; },
    release(id, overClose) {
      if (pointerId !== id) return false;
      pointerId = null;
      return Boolean(overClose);
    },
    cancel(id) {
      if (pointerId !== id) return false;
      pointerId = null;
      return true;
    },
    get pointerId() { return pointerId; },
    get pressed() { return pointerId !== null; },
  };
}
