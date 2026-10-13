import { nativeAutomapMode } from './automap-mode.mjs';

const DEFAULT_FULL_MAP = 'data\\global\\ui\\automap\\maximap.dc6';

// D2Client 1.13c applies full-map Fade to mode 0's default MaxiMap cells.
// The browser's Full option uses mode 0; its other full-sized native mode (2)
// is accepted by the overlay, but is not exposed by the current options page.
export function fullMapFadeDrawMode({ mode, fade, source, x, y, width, height, targetX = width / 2 }) {
  if (nativeAutomapMode(mode) !== 0 || fade !== 'on' || String(source).toLowerCase() !== DEFAULT_FULL_MAP) return 5;

  // Native tests the projected cell frame center after applying cell scale and scroll.
  const px = x + 8, py = y + 16;
  const targetY = height / 2 - 10;
  const dx = Math.abs(px - targetX), dy = Math.abs(py - targetY);
  if (dx > 140 || py < targetY - 150 || py > targetY + 130) return 5;

  const distance = (2 * Math.max(dx, dy) + Math.min(dx, dy)) >> 1;
  if (distance < 50) return 0;
  if (distance < 100) return 1;
  if (distance < 150) return 2;
  return 5;
}
