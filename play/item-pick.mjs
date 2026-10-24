// Ground-item click picking, pure. The shell feeds it snapshot items.

// Alt: the topmost drawn label under the pointer owns the click. Evidence is in draw order, so
// the LAST hit is on top; a label whose item left the ground (picked up since the draw) is skipped.
export function labelItemAt(evidence, items, x, y) {
  for (let i = evidence.length - 1; i >= 0; i--) {
    const { id, box } = evidence[i];
    if (!box || x < box.x || x >= box.x + box.width || y < box.y || y >= box.y + box.height) continue;
    const item = (items || []).find((it) => it.id === id && it.mode === 3 && it.invPage === 255);
    if (item) return item;
  }
  return null;
}

// Nearest ground item to the pointer's world point among those in the 2-subtile disc or whose
// drawn rect holds the cursor (`drawn`); ties go to the lower id. Returns {item, distance} or null.
export function nearestGroundItem(items, at, drawn) {
  const d2 = (c) => (c.x - at.x) ** 2 + (c.y - at.y) ** 2;
  return (items || []).filter((c) => c.mode === 3 && c.invPage === 255)
    .map((item) => ({ item, distance: d2(item) }))
    .filter((c) => c.distance <= 4 || drawn.has(c.item))
    .sort((a, b) => a.distance - b.distance || a.item.id - b.item.id)[0] ?? null;
}

// session.pickup result: 1 in inventory/cursor, 2 engine started the walk, 0 refused (out of reach
// of the engine's own path). Only a refusal needs the shell's hop walker, then pickup on arrival.
export const pickupNeedsWalk = (result) => result === 0;

// 1.13c D2Client+0x427C0 / +0x446B0: the modifier byte a click carries -- 8 when either run-hold
// flag (DAT 6fbcc3e8/3ec) is set, | 4 when Show Items (DAT 6fbcc3f0, polled from the binding for fn 0x24 by +0x43310) is held.
export const clickModifiers = (run, showItems) => (run ? 8 : 0) | (showItems ? 4 : 0);
