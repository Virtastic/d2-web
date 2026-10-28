// Closing the inventory while the stash paired it with must use the same retail
// TRADEBTN_CLOSESTASH command as Esc. The client changes presentation only after
// the engine accepts that command; it never treats the pointer hit as authority.
export const TRADEBTN_CLOSESTASH = 18;

export function closeStashFromInventory({ stashOpen, commandButton, onAccepted }) {
  if (!stashOpen) return false;
  const accepted = Boolean(commandButton(TRADEBTN_CLOSESTASH, 0));
  if (accepted) onAccepted();
  return accepted;
}

// The stash's own close cel (buysellbtn, 32x32 at x=352, bottom row 476). Retail draws every list button
// at frame base + held (D2Client+0x95C60); the rect is the cel -- ponytail: the stash entry's exact
// hit bounds are not recovered, widen to the inclusive rect if a retail trace shows one.
export function stashCloseAt(px, py, width, height, visible) {
  if (!visible || width <= 0 || height <= 0) return false;
  const scale = Math.min(1, width / 800, height / 600);
  const x = (px - (width - 800 * scale) / 2) / scale;
  const y = (py - (height - 600 * scale) / 2) / scale;
  return x >= 352 && x < 384 && y >= 445 && y < 477;
}
