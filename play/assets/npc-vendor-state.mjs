// The vendor window follows the engine's accepted 0x38 counter state. Its stock may be empty after
// purchases, so item count is presentation data rather than an open/closed signal.
export function npcVendorIsOpen(state) {
  if (!state) return false;
  const { npcId, vendorNpcId, action, result } = state;
  return Number.isInteger(npcId) && npcId > 0 && vendorNpcId === npcId &&
    (action === 1 || action === 2) && result === 1;
}
