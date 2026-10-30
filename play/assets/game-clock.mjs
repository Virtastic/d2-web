// The shell's two clocks, pure so a test can feed fixed rAF timestamps.
// Shell updates run at 60 Hz (STEP); the engine ticks at retail's 25 Hz (GAME_STEP) on whole slices of that time.
export const STEP = 1000 / 60, GAME_STEP = 1000 / 25;
// A stall longer than this is dropped, not replayed (a backgrounded tab must not fast-forward the hero).
// 15 updates = 250 ms = 6 engine ticks: a heavy frame (>83 ms, the old 5-update cap) no longer runs the hero slow.
export const MAX_CATCHUP_UPDATES = 15;

export function createGameClock() {
  let accumulator = 0, gameClock = 0;
  return {
    // How many shell updates a frame that took `dt` ms owes.
    updatesFor(dt) {
      accumulator = Math.min(accumulator + dt, STEP * MAX_CATCHUP_UPDATES);
      let n = 0;
      while (accumulator >= STEP) { accumulator -= STEP; n++; }
      return n;
    },
    // Engine ticks owed by one shell update.
    ticksForUpdate() {
      gameClock += STEP;
      let n = 0;
      while (gameClock >= GAME_STEP) { gameClock -= GAME_STEP; n++; }
      return n;
    },
    reset() { gameClock = 0; },
  };
}

// requestAnimationFrame can slow to ~1 Hz on a page that is still visible (an occluded or power-saved window).
// Each such frame owed 1000 ms but the cap above paid 250 ms, so the game ran at a quarter speed. Retail's
// loop keeps its 25 Hz ticks whatever the window does: past this gap a timer pays the updates rAF did not.
// Below the 250 ms cap, so the timer's first payment (the gap plus up to one 40 ms timer period) is never clipped.
export const RAF_STARVED_MS = 200;
export const rafStarved = (now, rafAt, visible) => visible && now - rafAt >= RAF_STARVED_MS;
