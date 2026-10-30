// Retail's item-spell targeting, the client half. A right click on a usable inventory item sends 0x20 and
// the ENGINE decides what the item does (D2Game Rcv0x20 -> sub_6FC47D30). An Identify scroll or tome runs
// SKILLITEM_pSpell01_Initializer, which sets IFLAG_TARGETING (0x4) on the item and sends 0x3F: from then
// the pointer is the identify ring (buysell.dc6 frame 8) and the next left click on an unidentified item
// sends 0x27 {target, use} -- the engine identifies it, spends the tome charge / scroll, and clears the flag.
// The same id twice (or no target) in 0x27 is the cancel (sub_6FC47470 clears every IFLAG_TARGETING item and
// sends 0x3F back); any other item action does the same server-side, so the armed state is READ from the
// engine's flag every time it is asked, never trusted from here.
export const IFLAG_TARGETING = 0x4;
export const IFLAG_IDENTIFIED = 0x10;
export const IDENTIFY_CURSOR_FRAME = 8;

/**
 * io.use(id) -> true if the engine accepted the 0x20; io.flags(id) -> the item's dwItemFlags (0 if gone);
 * io.action(targetId, useId) -> true if the engine accepted the 0x27.
 */
export function createItemTargeting(io) {
  let armed = 0;
  const targeting = (id) => id > 0 && (io.flags(id) & IFLAG_TARGETING) !== 0;
  const sync = () => { if (armed && !targeting(armed)) armed = 0; return armed; };
  return {
    /** The item whose spell is waiting for a target, or 0. */
    get armed() { return sync(); },
    /** Right click on an inventory item (id) or on nothing (0/null). */
    rightClick(id) {
      if (sync() && (!id || id === armed)) { io.action(armed, armed); armed = 0; sync(); return true; }
      if (!id) return false;
      const accepted = io.use(id);
      armed = targeting(id) ? id : 0;
      sync();
      return accepted;
    },
    /** Left click on an item: true if the click was the targeting's (the caller then does nothing else). */
    leftClick(targetId) {
      if (!sync() || !targetId || (io.flags(targetId) & IFLAG_IDENTIFIED)) return false;
      io.action(targetId, armed);
      sync();
      return true;
    },
    /** Escape: the client's cancel. */
    cancel() { if (!sync()) return false; io.action(armed, armed); armed = 0; return true; },
    /** The pointer context the cursor draws. */
    cursorContext() { return sync() ? { kind: 'identify' } : null; },
  };
}
