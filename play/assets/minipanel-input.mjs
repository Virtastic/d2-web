// D2Client's MiniPanel down and up callbacks both reject a non-null inventory cursor item.
// The action is selected from the release hit test, independently of the pressed slot.
export function miniPanelReleaseAction({ button, pressed, pointerMatches, releaseHit, cursorItem }) {
  if (button !== 0 || pressed < 0 || !pointerMatches || cursorItem) return -1;
  return Number.isInteger(releaseHit) && releaseHit >= 0 ? releaseHit : -1;
}

export function miniPanelPressAction(hit, cursorItem) {
  return !cursorItem && Number.isInteger(hit) && hit >= 0 ? hit : -1;
}
