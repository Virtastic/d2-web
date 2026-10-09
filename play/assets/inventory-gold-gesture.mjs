/** Pointer-owned release gesture for the inventory's carried-gold coin. */
export function beginInventoryGoldPress(pointerId, enabled) {
  return enabled && Number.isInteger(pointerId) ? { pointerId } : null;
}

export function finishInventoryGoldPress(press, pointerId, leftButton, enabled, releasedOverCoin) {
  if (!press || press.pointerId !== pointerId) return { matched: false, activate: false };
  return { matched: true, activate: Boolean(leftButton && enabled && releasedOverCoin) };
}

export function cancelInventoryGoldPress(press, pointerId) {
  return Boolean(press && press.pointerId === pointerId);
}
