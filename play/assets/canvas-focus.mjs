// A canvas is focusable for keyboard controls, but browser focus is also allowed to scroll the
// document. Install this in capture phase so coordinates read by the shell's pointerdown handler
// keep the same canvas origin for the whole press/release gesture.
export function installScrollSafeCanvasFocus(canvas) {
  canvas.addEventListener('pointerdown', () => canvas.focus({ preventScroll: true }), { capture: true });
}
