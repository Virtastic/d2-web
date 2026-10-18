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

// PERF: the rounded DDA above walks one PIXEL at a time, so a streak is 1..11 fillRect(1, 1)
// calls and a rain pass is ~1,700 of them a frame. The pixels it walks are a monotone staircase
// (x moves one way, y never decreases), so consecutive pixels that share a column or a row are a
// single rectangle: the same pixels, written by the same colour, in the same order -- only the
// number of canvas operations changes. Merged once here, per shape, so the draw path is a handful
// of fillRects per streak (the drawn pixel sequence is proved identical by
// tools/bench-weather-line.mjs, which expands both streams back to pixels).
const RAIN_LINE_RECTS = RAIN_LINE_OFFSETS.map((row) => row.map((offsets) => {
  const rects = [];
  let x = offsets[0], y = offsets[1], w = 1, h = 1;
  for (let i = 2; i < offsets.length; i += 2) {
    const px = offsets[i], py = offsets[i + 1];
    if (px === x && py === y + h) h++;                       // same column, next row
    else if (py === y && px === x + w) w++;                  // same row, next column (dx > 0)
    else if (py === y && px === x - 1) { x = px; w++; }      // same row, previous column (dx < 0)
    else { rects.push(x, y, w, h); x = px; y = py; w = 1; h = 1; }
  }
  rects.push(x, y, w, h);
  return Int8Array.from(rects);
}));

/** Rasterize a rain line using the compositor's original rounded DDA pixel sequence. */
export function plotRainLine(ctx, x0, y0, x1, y1) {
  const dx = x1 - x0, dy = y1 - y0;
  if (dy >= 0 && dy <= 10 && dx >= -4 && dx <= 4) {
    const rects = RAIN_LINE_RECTS[dy][dx + 4];
    for (let i = 0; i < rects.length; i += 4) ctx.fillRect(x0 + rects[i], y0 + rects[i + 1], rects[i + 2], rects[i + 3]);
    return;
  }
  const steps = Math.max(Math.abs(dx), Math.abs(dy));
  for (let i = 0; i <= steps; i++) {
    const fraction = steps ? i / steps : 0;
    ctx.fillRect(Math.round(x0 + dx * fraction), Math.round(y0 + dy * fraction), 1, 1);
  }
}
