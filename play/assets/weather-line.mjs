// Rain streaks have at most 10 vertical pixels and a four-pixel horizontal span.
// Precompute the exact integer DDA offsets for that range; keep the generic
// interpolation as a fallback if a caller supplies a wider segment.
const RAIN_LINE_OFFSETS = Array.from({ length: 11 }, (_, dy) =>
  Array.from({ length: 9 }, (_, dxIndex) => {
    const dx = dxIndex - 4, steps = Math.max(Math.abs(dx), Math.abs(dy)), offsets = [];
    for (let i = 0; i <= steps; i++) {
      const fraction = steps ? i / steps : 0;
      offsets.push(Math.round(dx * fraction), Math.round(dy * fraction));
    }
    return offsets;
  }));

/** Rasterize a rain line using the compositor's original rounded DDA pixel sequence. */
export function plotRainLine(ctx, x0, y0, x1, y1) {
  const dx = x1 - x0, dy = y1 - y0;
  if (dy >= 0 && dy <= 10 && dx >= -4 && dx <= 4) {
    const offsets = RAIN_LINE_OFFSETS[dy][dx + 4];
    for (let i = 0; i < offsets.length; i += 2) ctx.fillRect(x0 + offsets[i], y0 + offsets[i + 1], 1, 1);
    return;
  }
  const steps = Math.max(Math.abs(dx), Math.abs(dy));
  for (let i = 0; i <= steps; i++) {
    const fraction = steps ? i / steps : 0;
    ctx.fillRect(Math.round(x0 + dx * fraction), Math.round(y0 + dy * fraction), 1, 1);
  }
}
