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

// The one label to highlight: the label of the item the click would pick (`pick`, the shell's
// resolvePointerTarget(...).item -- label hit first, else the item's own sprite), so highlight and click agree.
export const hoveredLabel = (evidence, pick) =>
  pick ? evidence.findLast((e) => e.id === pick.id) ?? null : null;

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

// A click event carries its own Alt state; a lost keydown/keyup (focus change, OS menu grab) must not
// leave Show Items stale. Returns the held set's desired Alt membership: true add, false remove, null leave.
export const altSync = (altBound, altKey, heldAlt) => (!altBound || altKey === heldAlt ? null : altKey);
