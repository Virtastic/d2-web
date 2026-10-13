const TICK_MASK = 0xFFFFFFFF;
const elapsed32 = (now, then) => ((Math.trunc(now) & TICK_MASK) - then) >>> 0;

/** Model D2Client's render-driven stash amount spinner callback. The down step is separate. */
export function createStashSpinnerRepeat() {
  let direction = null;
  let pressedAt = 0;
  let lastCheckAt = 0;
  let heldFrames = 0;

  return {
    press(action, nowMs) {
      if (action !== 'increase' && action !== 'decrease') return false;
      direction = action;
      pressedAt = Math.trunc(nowMs) & TICK_MASK;
      lastCheckAt = pressedAt;
      heldFrames = 0;
      return true;
    },
    /** Called once from each visible dialog draw while an arrow stays armed. */
    onRender(nowMs) {
      if (!direction) return 0;
      heldFrames++;
      const now = Math.trunc(nowMs) & TICK_MASK;
      if (elapsed32(now, lastCheckAt) < 70) return 0;
      lastCheckAt = now;

      const heldMs = elapsed32(now, pressedAt);
      if (heldMs > 4096) return Math.floor(heldMs / 2048) * heldFrames;
      if (heldMs > 3000) return heldFrames >>> 2;
      if (heldMs > 2000) return heldFrames >>> 4;
      if (heldMs > 1000) return heldFrames >>> 5;
      return 1;
    },
    release() {
      const action = direction;
      direction = null;
      pressedAt = 0;
      lastCheckAt = 0;
      heldFrames = 0;
      return action;
    },
    get direction() { return direction; },
    get heldFrames() { return heldFrames; },
  };
}
