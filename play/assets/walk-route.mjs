// F658 (the client half of F633): aim the ENGINE's own path when the shell's local walk fan cannot
// round a wall TO AN NPC.
//
// The shell's click-walk (`gameplay-shell.mjs` `walkStep`) is a LOCAL fan: it proposes cells around
// the hero and keeps one that is standable AND chebyshev-closer to the goal. That is enough in the
// open, but a goal the aim sees PAST a wall -- an NPC across one -- has no closer standable cell on
// the hero's side, so the fan proposes nothing and the walk stops short. The goal's own cell cannot
// be the fallback either: a unit OCCUPIES it, so `session.move(goal)` reports MOVE_NO_PATH.
//
// The fix is not a JS pathfinder. This only picks a cell to AIM AT -- a standable cell within the
// walk's own arrival tolerance of the goal -- and the ENGINE routes to it, exactly as it does for
// every other leg (`d2_session_command_move` runs retail's PATH_Straight/PATH_AStar). Walkability is
// the engine's answer (`d2_session_walkable`), never the client's guess, so the engine still decides
// and the client only draws.
//
// The caller fires this ONLY for a goal the CLICK resolved to an NPC/unit (its `npcId`), never for a
// generic unreachable ground goal: gating a ground goal on geometry alone was F698 -- at s000e step 17
// it shuffled the hero around `route()`'s one-point leg forever and the room ahead never activated.
//
// `reach` is the caller's arrival tolerance (`within`): a cell farther than that from the goal would
// not end the walk even if the hero reached it. The search is a ring walk out from the goal, and the
// cell CLOSEST TO THE HERO wins among those at the first radius that has any -- so the returned aim is
// the one the engine's shortest route reaches first. Null when nothing standable lies within reach
// (the goal is a wall or a sealed pocket), which leaves the caller's give-up exactly as it was.
export function nearestStandableNear(goal, reach, isWalkable, player) {
  const within = Math.max(1, Math.floor(reach));
  for (let d = 1; d <= within; d++) {
    let best = null, bestD = Infinity;
    for (let dx = -d; dx <= d; dx++) for (let dy = -d; dy <= d; dy++) {
      if (Math.max(Math.abs(dx), Math.abs(dy)) !== d) continue;
      const x = goal.x + dx, y = goal.y + dy;
      // The 1.13c ground-move handler rejects coordinates more than 50
      // subtiles away on either axis before pathfinding sees them.
      if (Math.max(Math.abs(x - player.x), Math.abs(y - player.y)) > 50) continue;
      if (!isWalkable(x, y)) continue;
      const toPlayer = Math.max(Math.abs(x - player.x), Math.abs(y - player.y));
      if (toPlayer < bestD) { best = { x, y }; bestD = toPlayer; }
    }
    if (best) return best;
  }
  return null;
}
