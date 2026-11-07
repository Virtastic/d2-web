// Which entry of navigator.getGamepads() the controller reads.
//
// A pad the browser reports with mapping 'standard' is taken first. Without one, a pad with an empty
// mapping is taken too: Chrome on Android reports a built-in or unrecognised controller (a handheld's
// own pad, such as the Retroid Pocket's) with mapping '' even though its Android mapping puts every
// button, the d-pad and both sticks at the standard indices. Requiring 'standard' left those
// devices with no controller at all. A pad too small to be a game controller (fewer than four axes or
// twelve buttons) is never taken, so a stray HID device cannot walk the hero.
//
// Of several candidates the one already in use stays; otherwise the one with the newest input
// (Gamepad.timestamp) wins, so the pad the player last touched is the one that drives.

const MIN_AXES = 4, MIN_BUTTONS = 12;

const usable = (p) => p && p.connected !== false && (p.axes?.length ?? 0) >= MIN_AXES && (p.buttons?.length ?? 0) >= MIN_BUTTONS;

/** The pad to read this poll, or null. `current` is the index read last poll (or -1). */
export function pickPad(pads, current = -1) {
  const list = Array.prototype.filter.call(pads || [], usable);
  if (!list.length) return null;
  const standard = list.filter((p) => p.mapping === 'standard');
  const pool = standard.length ? standard : list.filter((p) => !p.mapping);
  if (!pool.length) return null;
  const kept = pool.find((p) => p.index === current);
  if (kept) return kept;
  return pool.reduce((best, p) => ((p.timestamp || 0) > (best.timestamp || 0) ? p : best));
}
