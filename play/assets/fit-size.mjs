// The game view's size for a box of the screen (gameplay-shell.mjs fitView).
//
// The view is the box's size in DEVICE pixels divided by one scale S, so the bitmap has the box's own
// aspect: no bars and no stretch. The view is never smaller than retail's 800x600 in either direction,
// because the control panel and every panel are laid out for 800 columns and 600 rows.
//
// Touch: S = min(deviceH/600, deviceW/800), a fractional scale that is smoothed. A landscape phone gets
// exactly 600 rows and a wider view. A box narrower than 4:3 gets exactly 800 columns and a taller view:
// a portrait tablet, a foldable's inner screen, or split screen. A whole-number S on touch would either
// drop under 800x600 or double it and halve the text.
// Desktop: S = the largest whole number that keeps 800x600, at least 1, so the upscale is exact. A window
// smaller than 800x600 keeps S = 1.
//
// cssW/cssH are the box in CSS px (the canvas's own rect), dpr is devicePixelRatio. Returns null for an
// empty box (not laid out yet), and the caller keeps the size it has.
export function fitSize({ cssW, cssH, dpr = 1, touch = false }) {
  const d = dpr > 0 ? dpr : 1, dw = Math.round(cssW * d), dh = Math.round(cssH * d);
  if (!(dw > 0 && dh > 0)) return null;
  if (touch) {
    const S = Math.min(dh / 600, dw / 800);
    return { w: Math.max(800, Math.round(dw / S)), h: Math.max(600, Math.round(dh / S)), S };
  }
  const S = Math.max(1, Math.floor(Math.min(dh / 600, dw / 800)));
  return { w: Math.max(1, Math.floor(dw / S)), h: Math.max(1, Math.floor(dh / S)), S };
}
