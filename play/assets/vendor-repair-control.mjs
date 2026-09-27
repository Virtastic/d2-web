// The Repair button is a mode, not a second price calculator. OpenDiablo2's
// ButtonTypeRepair is toggleable (d2core/d2ui/button.go), so the next carried
// item click names the item for the already-wired retail 0x35 command.
//
// The callback is deliberately supplied by the shell: this module owns only
// panel interaction; the engine validates the id, calculates the cost, moves
// gold, and restores durability.
let armed = false;

/** Consume a shop click when it belongs to Repair mode. */
export function handleVendorRepairClick(button, itemId, repair) {
  if (button === 'repair') { armed = true; return true; }
  if (!armed) return false;
  // A repair-mode click stays inside the shop. Only a real carried item is
  // sent to the engine; stock and empty-panel clicks cannot become purchases.
  if (Number.isInteger(itemId) && itemId >= 0) {
    armed = false;
    repair(itemId);
  }
  return true;
}
