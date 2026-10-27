// Centred retail-bitmap-font text (D2Win font draw): left edge = cx - width/2, whole pixels.
// Returns false when no font is loaded so the caller keeps its canvas-text fallback.
export function drawCentredBitmap(ctx, font, text, cx, top) {
  if (!font) return false;
  font.draw(ctx, text, Math.trunc(cx - font.measure(text) / 2), top);
  return true;
}

// Left-aligned retail-bitmap-font text at whole pixels; false when no font so the caller falls back.
export function drawLeftBitmap(ctx, font, text, x, top) {
  if (!font) return false;
  font.draw(ctx, text, Math.trunc(x), Math.trunc(top));
  return true;
}

// Centred message in the retail bitmap font; with no font loaded yet it draws nothing (no canvas-text stand-in).
export function drawCentredMessage(ctx, font, text, w, h) {
  drawCentredBitmap(ctx, font, text, w / 2, h / 2 - 8);
}

// Several centred bitmap lines, `step` px apart from `top`; false when no font so the caller falls back.
export function drawCentredLines(ctx, font, lines, cx, top, step = 16) {
  if (!font) return false;
  lines.forEach((text, i) => drawCentredBitmap(ctx, font, text, cx, top + i * step));
  return true;
}

// The retail font for screens drawn before a session exists: keep `current`, else try `make`
// (the MPQ may not be mounted yet, so a throw means "not yet", not an error).
export function ensureFont(current, make) {
  if (current) return current;
  try { return make() || null; } catch { return null; }
}
