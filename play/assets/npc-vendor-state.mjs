// The vendor window follows the engine's accepted 0x38 counter state. Its stock may be empty after
// purchases, so item count is presentation data rather than an open/closed signal.
export function npcVendorIsOpen(state) {
  if (!state) return false;
  const { npcId, vendorNpcId, action, result } = state;
  return Number.isInteger(npcId) && npcId > 0 && vendorNpcId === npcId &&
    (action === 1 || action === 2) && result === 1;
}

// The inventory X beside an open trade window closes the whole trade (retail's close-all-panels ends
// the interaction with 0x30), not just the inventory; the vendor panel then goes with its state.
export function closeInventoryPanel({ shopOpen, endNpc, hide }) {
  if (shopOpen) endNpc();
  hide();
}
