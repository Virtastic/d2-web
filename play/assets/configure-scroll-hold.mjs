// F218: D2Client 1.13c FUN_6FAC1A80 checks the currently hit arrow and the left-down
// latch, then permits one scroll when GetTickCount() > previous + 0x32. Both arrow
// directions share the same previous tick across pointer-down gestures. The renderer
// calls it once per frame; missed frames never accumulate a catch-up queue.
export function createConfigureScrollHold() {
  // DAT_6FBCC548 is a zero-initialized native global, so the first poll is
  // subject to the same strict > 50 ms check as every later scroll.
  let press = null, lastScrollTick = 0;
  return {
    accepts(pointerId) { return !press || press.pointerId === pointerId; },
    down(pointerId, direction, button, now) {
      if (button !== 0 || (press && press.pointerId !== pointerId)) return false;
      press = direction === 'up' || direction === 'down'
        ? { pointerId, direction, inside: true } : null;
      return Boolean(press);
    },
    move(pointerId, direction, buttons) {
      if (!press || press.pointerId !== pointerId) return;
      // A release outside the page may never deliver pointerup. Its next move ends
      // the gesture rather than merely suspending it until the arrow is hit again.
      if (!(buttons & 1)) { press = null; return; }
      press.inside = direction === press.direction;
    },
    release(pointerId, button) {
      if (!press || press.pointerId !== pointerId || button !== 0) return false;
      press = null; return true;
    },
    cancel(pointerId) {
      if (!press || (pointerId !== undefined && press.pointerId !== pointerId)) return false;
      press = null; return true;
    },
    poll(now) {
      if (!press || !press.inside || now <= lastScrollTick + 50) return 0;
      // FUN_6FAC1A80 shares DAT_6FBCC548 between both arrows and across presses.
      // A new pointer-down does not get its own repeat clock; each accepted scroll
      // advances this one native tick, and late frames still cause just one step.
      lastScrollTick = now;
      return press.direction === 'down' ? 1 : -1;
    },
  };
}
