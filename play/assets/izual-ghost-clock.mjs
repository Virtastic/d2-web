// D2Client's Common #11090 mode-change path initializes an ordinary S1 CUnit at frame zero.
// Class 406 uses the monster fallback mode row: policy 1 skips the step that would reach the
// sequence length. Its local 1.13c setup yields CUnit +0x4C = 120; `animSpeed` carries that same
// initial rate from the engine snapshot. D2Game requests neutral mode after its terminal event, but
// the renderer waits for that actual mode snapshot instead of predicting the client callback result.
// A live scene comparison remains pending.
export function advanceIzualGhostS1Clock(actor, previous = null) {
  if (Number(actor.classId) !== 406 || actor.hasAnimSequence) return null;
  const gameFrame = Number(actor.gameFrame);
  const frameCountPrecise = Math.max(0, Math.trunc(Number(actor.frameCountPrecise) || 0));
  if (!Number.isSafeInteger(gameFrame) || !frameCountPrecise) return null;

  if (Number(actor.mode) !== 8) return null;
  const nativeFramePrecise = Number(actor.framePrecise);
  const hasNativeFrame = Number.isSafeInteger(nativeFramePrecise) && nativeFramePrecise >= 0;
  const nativeFrameState = hasNativeFrame ? { nativeFramePrecise } : {};
  const frameReset = hasNativeFrame && Number.isSafeInteger(previous?.nativeFramePrecise) &&
    nativeFramePrecise < previous.nativeFramePrecise;
  if (frameReset) {
    return { mode: 8, gameFrame, framePrecise: 0, nativeFramePrecise };
  }

  if (!previous || previous.mode !== 8 || gameFrame < previous.gameFrame) {
    return { mode: 8, gameFrame, framePrecise: 0,
      ...nativeFrameState };
  }

  let framePrecise = previous.framePrecise;
  const animSpeed = Math.trunc(Number(actor.animSpeed) || 0);
  for (let frame = previous.gameFrame; frame < gameFrame; frame++) {
    if (animSpeed <= 0) break;
    if (frameCountPrecise <= framePrecise + animSpeed) break;
    framePrecise += animSpeed;
  }
  return { mode: 8, gameFrame, framePrecise,
    ...nativeFrameState };
}

// Leaving S1 resets D2Client's CUnit animation record if the unit later re-enters S1.
export function resetIzualGhostS1Clock(clocks) {
  clocks.clear();
}
