// The press rule of D2Client's scrolling list (built by 0x67F20 and 0x67D00; the message log 0x7C770 and the
// hire window 0x49440 both use it). Coordinates are retail's 800x600 screen pixels.
//
// Down (0x679F0) does nothing outside the list box. Inside it, the press is armed and latches one control:
// the up arrow, the down arrow, the track, or the row under the pointer. Up (0x68670) clears the latch and acts
// only when the release is over the same control: an arrow scrolls one line, a row fires the row callback.
// The draw (0x67FD0) shows an armed arrow as frame 11 (up) or 10 (down), in place of 9 or 8, for as long as
// the arrow is latched, wherever the pointer is. While the latched arrow is still under the pointer, each draw
// steps a repeat counter that down resets to 0. The draw scrolls one line whenever the counter is not a
// multiple of 10.

/** The list's arrow geometry from its box (0x67D00): arrow x/y are the arrows' left edge and bottom row. */
export function listGeometry(x, y, w, h) {
  return { x, y, w, h, ax: x + w - 0x12, ay: y + 0x10, cels: h < 0x20 ? 1 : Math.trunc((h - 0x10) / 0xc) - 1 };
}
const inBox = (g, px, py) => px > g.x && px < g.x + g.w && py > g.y && py < g.y + g.h;
/** 0x679F0's up-arrow test (strict on every edge). */
export const onUpArrow = (g, px, py) => px > g.ax && px < g.ax + 0x10 && py > g.ay - 0xc && py < g.ay;
/** 0x673A0's down-arrow test. */
export const onDownArrow = (g, px, py) => { const b = g.ay + g.cels * 0xc; return px > g.ax && px < g.ax + 0x10 && py > b - 0xc && py < b; };
/** The arrow under a point: 'up' | 'down' | null. */
export const listArrowAt = (g, px, py) => (onUpArrow(g, px, py) ? 'up' : onDownArrow(g, px, py) ? 'down' : null);

/**
 * A press on one list. `rowAt(px, py)` returns the row under a point, or -1 when there is none (0x67160). It may
 * be omitted when the list has no row callback.
 */
export function createListPress(geometry, rowAt = null) {
  let g = geometry, armed = false, arrow = null, row = -1, repeat = 0;
  return {
    get geometry() { return g; },
    set geometry(v) { g = v; },
    get armed() { return armed; },
    get arrow() { return armed ? arrow : null; },
    get row() { return row; },
    /** The arrow frames the draw uses now. */
    frames() { return { up: armed && arrow === 'up' ? 11 : 9, down: armed && arrow === 'down' ? 10 : 8 }; },
    /** 0x679F0. Returns true when the press landed in the list box (the list takes it). */
    down(px, py) {
      if (!inBox(g, px, py)) return false;
      armed = true; arrow = null;
      if (onUpArrow(g, px, py)) { arrow = 'up'; repeat = 0; return true; }
      if (onDownArrow(g, px, py)) { arrow = 'down'; repeat = 0; return true; }
      if (rowAt) row = rowAt(px, py);
      return true;
    },
    /** 0x68670. Returns { scroll: -1 | 1 } or { row } for an action, otherwise null. */
    up(px, py) {
      if (!armed) { row = -1; return null; }
      armed = false;
      const a = arrow, latched = row; arrow = null; row = -1;
      if (a === 'up') return onUpArrow(g, px, py) ? { scroll: -1 } : null;
      if (a === 'down') return onDownArrow(g, px, py) ? { scroll: 1 } : null;
      if (!rowAt) return null;
      const now = rowAt(px, py);
      return now === latched && latched !== -1 ? { row: latched } : null;
    },
    /** The tail of 0x67FD0, run once per draw with the pointer. Returns { scroll } for a repeat, otherwise null. */
    tick(px, py) {
      if (!armed || !arrow) return null;
      const over = arrow === 'up' ? onUpArrow(g, px, py) : onDownArrow(g, px, py);
      if (!over) return null;
      repeat = (repeat + 1) >>> 0;
      return repeat !== 0 && repeat % 10 !== 0 ? { scroll: arrow === 'up' ? -1 : 1 } : null;
    },
    cancel() { armed = false; arrow = null; row = -1; },
  };
}
