/** Whether a repeated held-button command currently belongs to the world. */
export function heldPointerWorldAllowed({
  screenIsGame = false,
  sessionAvailable = false,
  modalOwner = null,
  panelOwner = null,
  overlayOwnsPointer = false,
} = {}) {
  return Boolean(screenIsGame && sessionAvailable && !modalOwner && !panelOwner && !overlayOwnsPointer);
}
