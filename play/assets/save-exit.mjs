// Leave the game only after persistence confirms a saved character. A failed save must leave the
// player in the session so they can retry instead of losing unsaved progress.
export async function saveBeforeExit(persist, exit) {
  let saved;
  try { saved = await persist(); }
  catch { return false; }
  if (!saved) return false;
  await exit(saved);
  return true;
}
