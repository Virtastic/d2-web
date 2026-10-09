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
