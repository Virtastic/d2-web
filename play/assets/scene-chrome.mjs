// Which game-screen chrome is drawn. A Battle.net game shows the same HUD, belt and pointer as a
// single-player one; the difference is where the numbers come from. `engineLive` is a local engine
// session (its state is read through the frame snapshot); `realmSceneActive` is a joined realm game,
// whose HUD is fed by the realm clientView (assets/realm-client-view.mjs) because there is no local session to read.
export function chromeGates({ engineLive, realmSceneActive, hasControlPanel, hasPointerLayer, hasBelt, hasSkillSelect, clearScreen, screenIsGame }) {
  const shown = Boolean(engineLive) || Boolean(realmSceneActive);
  return { hud: shown && Boolean(hasControlPanel) && !clearScreen,
    pointerLayer: shown && Boolean(hasPointerLayer) && Boolean(screenIsGame),
    belt: shown && Boolean(hasBelt) && !clearScreen,
    // the skill buttons beside the orbs: without them the bar between each orb and the console is empty
    skillSelect: shown && Boolean(hasSkillSelect) && !clearScreen,
    // only a local session has an engine to read the frame's state from
    engineReads: Boolean(engineLive) && !realmSceneActive };
}
