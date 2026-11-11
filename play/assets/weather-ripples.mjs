// The rain's ground ripples, as the 1.13c client runs them.  A ripple is not a permanent decoration:
//   * once every 4th game tick (the pass where tick > last + 3), every visible water floor tile rolls
//     `step % 1000` and spawns a ripple when the roll is below ftol(intensity * 1000), intensity being
//     the weather's 0..1 level (here the 0..255 cycle value / 256).  A dry weather spawns none;
//   * a spawn takes the next free slot of a 512-slot pool (a full pool spawns nothing and draws no
//     numbers), then three numbers: row, column (see weather-ripple-offset.mjs) and the cel,
//     Rain3 or Rain4 by the low bit of the third step;
//   * each game tick a ripple's countdown (2) drops by one; at zero it moves to the next frame and
//     the countdown restarts, and when the frame reaches the cel's frame count the slot is freed --
//     a ripple therefore plays every frame of its cel for two ticks each, once.
export const RIPPLE_POOL = 0x200;
export const SPAWN_EVERY = 3;   // spawn pass when tick > last + 3

export function spawnLimit(intensity255) {
  return Math.floor(Math.max(0, intensity255) * 1000 / 256);
}

export function createRipplePool(rng, frameCounts = [10, 10]) {
  const slots = new Array(RIPPLE_POOL).fill(null);
  let cursor = 0, last = 0;
  const alloc = () => {
    let i = cursor;
    while (i < RIPPLE_POOL && slots[i]) i++;
    if (i >= RIPPLE_POOL) return -1;
    cursor = i + 1;
    return i;
  };
  const free = (i) => { slots[i] = null; if (i < cursor) cursor = i; };
  return {
    slots,
    get live() { return slots.reduce((n, s) => n + (s ? 1 : 0), 0); },
    /** One render pass.  `anchors` are the water tiles in walk order; `place(anchor, rng)` returns the
     *  ripple's world position (two rng steps).  Returns the number spawned. */
    spawnPass(tick, intensity255, anchors, place) {
      if (!(last + SPAWN_EVERY < tick)) return 0;
      last = tick;
      const limit = spawnLimit(intensity255);
      if (!limit) return 0;
      let n = 0;
      for (const anchor of anchors) {
        if (rng.step() % 1000 >= limit) continue;
        const i = alloc();
        if (i < 0) continue;
        const pos = place(anchor, rng);
        slots[i] = { ...pos, layer: rng.step() & 1, frame: 0, timer: 2 };
        n++;
      }
      return n;
    },
    /** One game tick of animation. */
    tick() {
      for (let i = 0; i < RIPPLE_POOL; i++) {
        const r = slots[i];
        if (!r) continue;
        if (--r.timer < 1) {
          r.timer = 2;
          if (++r.frame >= frameCounts[r.layer]) free(i);
        }
      }
    },
    reset(tick = 0) { slots.fill(null); cursor = 0; last = tick; },
  };
}
