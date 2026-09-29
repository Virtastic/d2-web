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
