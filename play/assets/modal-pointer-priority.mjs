// Pointer ownership for overlays that obscure the always-visible HUD.
// OpenDiablo2's EscapeMenu and MoveGoldPanel are separate UI owners; this
// policy makes their input precedence explicit without duplicating game state.
export const MODAL_POINTER_OWNER = Object.freeze({
  GAME_MENU: 'game-menu',
  STASH_GOLD_DIALOG: 'stash-gold-dialog',
});

/** Return the foremost input-owning overlay, or null when HUD input may run. */
export function modalPointerOwner({ gameMenuOpen = false, stashGoldDialogOpen = false } = {}) {
  if (gameMenuOpen) return MODAL_POINTER_OWNER.GAME_MENU;
  if (stashGoldDialogOpen) return MODAL_POINTER_OWNER.STASH_GOLD_DIALOG;
  return null;
}
