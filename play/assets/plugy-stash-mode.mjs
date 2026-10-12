// The local simulation is bootstrapped from SCREEN.GAME while no realm scene
// or realm transport owns the page. Every non-idle realm phase disables BigStash.
export function plugyStashMode({ enabled, screen, gameScreen, realmState, realmSceneActive }) {
  const singlePlayer = screen === gameScreen && realmState === 'idle' && !realmSceneActive;
  return { activeBigStash: Boolean(enabled && singlePlayer), singlePlayer };
}
