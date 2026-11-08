// A pointer event's position in the canvas's own pixels: the CSS box the canvas is drawn in (which the
// safe-area insets and a fractional upscale make differ from the bitmap) scaled to the bitmap. A screen
// then maps that to its 800x600 scene (the letterbox offset and scale are its own `view`).
export function canvasPoint(canvas, event) {
  const r = canvas.getBoundingClientRect();
  return [(event.clientX - r.left) * canvas.width / r.width, (event.clientY - r.top) * canvas.height / r.height];
}
