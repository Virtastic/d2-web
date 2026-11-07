// Where a ground ripple lands inside a water tile's diamond. The game's own generator is a 64-bit
// state stepped as lo' = low32(lo * 0x6AC690C5 + hi), hi' = high32(...); a ripple takes two steps: a
// row (step % 80) and then a column across that row's half-width w = 80 - 2|row - 40|, uniform over
// [-w, w). The tile anchor is the diamond's top vertex, so the point is (anchor.x + dx, anchor.y + row).
export function createRippleRng(lo = 0x2545f491, hi = 0x9e3779b9) {
  let l = BigInt(lo >>> 0), h = BigInt(hi >>> 0);
  const step = () => {
    const v = l * 0x6ac690c5n + h;
    l = v & 0xffffffffn; h = (v >> 32n) & 0xffffffffn;
    return Number(l);
  };
  return {
    step,
    offset() {
      const row = step() % 80;
      const w = 80 - 2 * Math.abs(row - 40);
      const n = 2 * w;
      const dx = (n < 1 ? 0 : step() % n) - w;
      return { dx, dy: row };
    },
  };
}
