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
  return { session: null, bootstrapStarted: false, currentFixture: null };
}
