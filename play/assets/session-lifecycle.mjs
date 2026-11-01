/** Reset renderer-local actor state before replacing the native session it describes. */
export function replaceNativeSession(session, monsterRenderer, create) {
  destroyNativeSession(session, monsterRenderer);
  return create();
}

/** Reset renderer-local actor state before the native session it describes is destroyed. */
export function destroyNativeSession(session, monsterRenderer) {
  monsterRenderer?.resetSession?.();
  session.destroy();
}

/**
 * Leave a game (Save and Exit): the native session is destroyed and the shell's hold on it is
 * released, so the next game started from character select builds a fresh session. Without the
 * release the shell keeps its "already started" flag and the next pick re-enters the old game.
 */
export function leaveNativeSession(session, monsterRenderer) {
  if (session) destroyNativeSession(session, monsterRenderer);
  return { session: null, bootstrapStarted: false, currentFixture: null, left: true };
}

/**
 * The developer notice the game frame draws while it has no native session. Retail has no such
 * text; it is kept only for a game that never connected (no session adapter, a failed create). A
 * game the player LEFT (Save and Exit) has no session for the frames until character select's
 * profile read lands, and retail shows nothing there, so the notice is not drawn.
 */
export function noSessionNotice({ session, left }) {
  if (session || left) return [];
  return ['Gameplay simulation is not connected', 'The renderer and controls are ready for d2_session_*'];
}
