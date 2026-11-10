// When the character is written: every `interval` ticks and at once on an act change, but a game played through
// packets only after its join has finished (before that the view's act is a placeholder, not the game's).
export function autosaveDue({ session, named, inFlight, fromPackets, joined, ticks, interval, actChanged }) {
  if (!session || !named || inFlight || (fromPackets && !joined)) return { count: ticks, due: false };
  const count = ticks + 1;
  return { count, due: actChanged || count >= interval };
}
