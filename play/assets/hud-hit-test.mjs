// Dispatch only a skill control the renderer has actually placed on the canvas.
//
// `skill-select.mjs` owns both the socket and popup rectangles.  Keeping this
// decision beside the input boundary prevents a HUD overlay from turning an
// otherwise ordinary world click into a consumed UI click.
export function dispatchSkillSelectHit(renderer, session, x, y, width, height, choices, select) {
  if (!renderer || !session) return false;
  const hit = renderer.hitTest(x, y, width, height, choices);
  if (hit?.button) {
    renderer.open = renderer.open === hit.button ? null : hit.button;
    return true;
  }
  if (hit?.skill) {
    select(hit.skill.id, hit.side);
    renderer.open = null;
    return true;
  }
  return false;
}
