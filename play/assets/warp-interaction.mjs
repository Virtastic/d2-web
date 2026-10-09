// Retail's 0x13 UNIT_TILE handler switches to the warp path only after its
// D2Common_10399 distance check is at most four subtiles.
export function withinNativeWarpInteractionRange(distance) {
  return Number.isInteger(distance) && distance >= 0 && distance <= 4;
}

// Fixtures may use the direct warp command as a remote transition. A player
// click first asks native code to apply retail's UNIT_TILE interaction range.
export function interactWithWarpTile(Module, destinationLevel, warpId) {
  if (!Module?.ccall) return { inRange: false, result: 0 };
  const inRange = Boolean(Module.ccall('d2_session_warp_tile_in_interact_range', 'number',
    ['number', 'number'], [destinationLevel, warpId]));
  const result = inRange
    ? Module.ccall('d2_session_command_warp', 'number', ['number', 'number'], [destinationLevel, warpId])
    : 0;
  return { inRange, result };
}

// Keep the shell snapshot in sync with PlrMsg's accepted approach and warp outcomes. Status 2 is
// the local command's >50-subtile preflight; status 3 hands off to main's existing transition path
// when an exit crosses acts or its generated source tile cannot be resolved.
export function runWarpInteraction({ warpHit, snapshot, motion, session, invoke, onActChange }) {
  const result = invoke(warpHit.destinationLevel, warpHit.warpId);
  if (result !== 1) {
    return { result, tooFar: result === 2, legacy: result === 3, snapshot, motion };
  }

  const previousAct = snapshot.act;
  const nextSnapshot = session.snapshot();
  const nextMotion = session.motion();
  if (nextSnapshot.act !== previousAct) onActChange?.(nextSnapshot.act);
  return { result, tooFar: false, legacy: false, snapshot: nextSnapshot, motion: nextMotion };
}

// D2Common_10399's far metric is max(dx + 2*dy, dy + 2*dx) after unit-size adjustment. A
// 16-subtile Chebyshev radius bounds that metric at 48, inside the packet handler's 50 limit.
export function warpApproachGoal(warpHit, interact) {
  return {
    x: warpHit.x,
    y: warpHit.y,
    within: 16,
    onArrive: () => interact(false),
    onStuck: () => interact(true),
  };
}
