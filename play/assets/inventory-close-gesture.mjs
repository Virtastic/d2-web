// The right inventory close is a custom D2Client control. Its pointer bounds are inclusive
// (x=418..450, y=444..476 at 800x600), while the 32x32 cel starts at (418,445).
export function inventoryCloseAt(px, py, width, height, visible) {
  if (!visible || width <= 0 || height <= 0) return false;
  const scale = Math.min(1, width / 800, height / 600);
  const x = (px - (width - 800 * scale) / 2) / scale;
  const y = (py - (height - 600 * scale) / 2) / scale;
  return x >= 418 && x <= 450 && y >= 444 && y <= 476;
}

export function beginInventoryClosePress(pointerId, visible, hit) {
  if (pointerId == null || !visible || !hit) return null;
  return { pointerId };
}

export function finishInventoryClosePress(press, pointerId, leftButton, visible, releasedOverClose) {
  const matched = Boolean(press && press.pointerId === pointerId);
  return { matched, activate: matched && Boolean(leftButton && visible && releasedOverClose) };
}

export function cancelInventoryClosePress(press, pointerId) {
  return Boolean(press && (pointerId == null || press.pointerId === pointerId));
}

export function inventoryCloseFrame(pressed) {
  return pressed ? 11 : 10;
}
