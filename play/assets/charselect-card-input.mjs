const TICK_MASK = 0xFFFFFFFF;
const elapsed32 = (now, then) => ((Math.trunc(now) & TICK_MASK) - then) >>> 0;

// D2Launch's always-registered type-4 card controls occupy two columns and four rows.
// Retail also registers a conditional 72 px extension; its predicate is not represented here.
export function characterCardHitBox(index) {
  return {
    x: 37 + (index % 2) * 272,
    y: 178 + Math.floor(index / 2) * 93,
    w: 200,
    h: 92,
  };
}

/** Track D2Launch's second-down activation for one already-selected character card. */
export function createCharacterSelectCardInput() {
  let lastDownAt = null;

  return {
    /** The card selection itself is written by the caller on this down edge. */
    down(index, previouslySelected, nowMs) {
      const now = Math.trunc(nowMs) & TICK_MASK;
      const activate = previouslySelected === index && lastDownAt !== null && elapsed32(now, lastDownAt) < 500;
      // Retail leaves the timestamp untouched on its confirming second down.
      if (!activate) lastDownAt = now;
      return activate;
    },
  };
}
