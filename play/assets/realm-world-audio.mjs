// Online (Battle.net) music and ambience per area. The single-player loop reads the level from the
// engine session; online there is none, so the level comes from the client's own room tracking.
import { levelMusicDue } from './act-music.mjs';

/** The area the player is in: the active room's level, else the level of the last map reveal. */
export function realmAreaLevel(playerView, worldView) {
  for (const v of [playerView?.clientRoomLevel, worldView?.mapLevel]) {
    if (Number.isInteger(v) && v > 0) return v;
  }
  return -1;
}

/**
 * Per-tick decision. Stamps the tick on an area change (retail adopts the new song only after more
 * than 0x4a ticks, or at once when nothing plays) and drives the ambience on the change or a cycle flip.
 * `deps`: { music: {activeLevel}, playMusic(level), ambience: { change(level), sync(level, cycle) }, cycle() }
 */
export function createRealmAreaAudio(deps) {
  let seen = -1, stamp = 0, ambLevel = -1, ambCycle = 0;
  return {
    step(level, tick) {
      if (!Number.isInteger(level) || level < 0) return;
      if (level !== seen) { seen = level; stamp = tick; }
      if (deps.ambience) {
        const cycle = deps.cycle ? deps.cycle() : 0;
        if (level !== ambLevel) deps.ambience.change(level);
        if (level !== ambLevel || cycle !== ambCycle) { ambLevel = level; ambCycle = cycle; deps.ambience.sync(level, cycle); }
      }
      if (deps.music && levelMusicDue(level, deps.music.activeLevel, stamp, tick)) deps.playMusic(level);
    },
    reset() { seen = -1; stamp = 0; ambLevel = -1; ambCycle = 0; },
    get level() { return seen; },
  };
}
