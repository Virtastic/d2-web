// D2Client's Common #11090 mode-change path initializes an ordinary S1 CUnit at frame zero.
// Its mode-8 update policy is 2: clamp when the next increment reaches frameCountPrecise.
// For class 406, local 1.13c setup yields client CUnit +0x4C = 120; `animSpeed` carries that same
// initial rate from the engine snapshot. A live scene comparison remains pending.
export function advanceIzualGhostS1Clock(actor, previous = null) {
  if (Number(actor.classId) !== 406 || Number(actor.mode) !== 8 || actor.hasAnimSequence) return null;
  const gameFrame = Number(actor.gameFrame);
  const frameCountPrecise = Math.max(0, Math.trunc(Number(actor.frameCountPrecise) || 0));
  if (!Number.isSafeInteger(gameFrame) || !frameCountPrecise) return null;

  if (!previous || previous.mode !== 8 || gameFrame < previous.gameFrame) {
    return { mode: 8, gameFrame, framePrecise: 0 };
  }

  let framePrecise = previous.framePrecise;
  const animSpeed = Math.trunc(Number(actor.animSpeed) || 0);
  if (animSpeed <= 0) return { mode: 8, gameFrame, framePrecise };
  for (let frame = previous.gameFrame; frame < gameFrame; frame++) {
    if (framePrecise + animSpeed >= frameCountPrecise) {
      framePrecise = Math.max(0, frameCountPrecise - 0x100);
      break;
    }
    framePrecise += animSpeed;
  }
  return { mode: 8, gameFrame, framePrecise };
}

// Leaving S1 resets D2Client's CUnit animation record if the unit later re-enters S1.
export function resetIzualGhostS1Clock(clocks) {
  clocks.clear();
}
