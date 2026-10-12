// D2Client's 0x15 handler starts a 500 ms black draw fade when the selected player is
// reassigned with a nonzero final byte. The client calls the draw routine each frame.
export const REASSIGN_FADE_MS = 500;
const BOTTOM_EXCLUSION = 47;

export function createRealmPlayerReassignFade() {
  const seen = new Map();
  let deadline = null;
  let queued = false;

  function installRoster(players) {
    seen.clear();
    deadline = null;
    queued = false;
    for (const player of players ?? []) {
      if (player?.active) seen.set(player.guid >>> 0, player.reassignments >>> 0);
    }
  }

  function observe(players, selected, nowMs) {
    const selectedGuid = selected?.active ? selected.guid >>> 0 : null;
    for (const player of players ?? []) {
      if (!player?.active) continue;
      const guid = player.guid >>> 0;
      const reassignments = player.reassignments >>> 0;
      const previous = seen.get(guid);
      if (guid === selectedGuid && previous != null && reassignments > previous && (player.reassignValue & 0xff) !== 0)
        deadline = nowMs + REASSIGN_FADE_MS;
      seen.set(guid, reassignments);
    }
  }

  function drawState(nowMs, width, height) {
    if (queued && Number.isFinite(nowMs) && width > 0 && height > 0) {
      deadline = nowMs + REASSIGN_FADE_MS;
      queued = false;
    }
    if (deadline == null || nowMs >= deadline || width <= 0 || height <= 0) return null;
    const alpha = Math.max(0, Math.min(255, Math.trunc(((deadline - nowMs) * 255) / REASSIGN_FADE_MS)));
    return alpha === 0 ? null : { x: 0, y: 0, width, height: Math.max(0, height - BOTTOM_EXCLUSION), color: '#000', alpha };
  }

  function reset() {
    seen.clear();
    deadline = null;
    queued = false;
  }

  function queue() {
    queued = true;
  }

  return { installRoster, observe, drawState, queue, reset };
}
