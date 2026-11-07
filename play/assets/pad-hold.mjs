// Held face buttons, pure state machines (no DOM): the controller feeds one sample per poll and applies
// the returned pointer actions through its normal pointer(). D2R's controller repeats an attack while
// its button is held and stands still to do it (a Hydra-style cast keeps spawning); on the keyboard and
// mouse that is Shift + a held mouse button, so a held pad button is a held Shift-click on the target.
export const HOLD_MS = 250;   // a tap is still a plain click; past this the press becomes a held one

export const STAT_HOLD_FIRST_MS = 400;   // character panel: hold A on a "+" this long, then ...
export const STAT_HOLD_EVERY_MS = 150;   // ... one more point per this interval

/** One button's hold. `locate(key)` returns {screen:[x,y]} for the target the press began on, or null if gone.
 *  step({ edge, pressed, now, key, locate }) returns [] or [{ type: 'down'|'move'|'up', x, y }].
 *  A press begins only with a `key` (the target it began on). If the target goes while the button is
 *  still down the hold ends and stays inert until the button is let go. */
export function makeTargetHold(ms = HOLD_MS) {
  let phase = 'idle', since = 0, key = null, at = null;
  const end = (next = 'idle') => { const out = phase === 'held' ? [{ type: 'up', x: at[0], y: at[1] }] : []; phase = next; key = null; return out; };
  return {
    step({ edge, pressed, now, key: startKey = null, locate }) {
      if (edge) { const out = end(); if (startKey) { phase = 'pending'; since = now; key = startKey; } return out; }
      if (phase === 'idle') return [];
      if (!pressed) return end();
      if (phase === 'dead') return [];
      const t = locate(key);
      if (!t) return end('dead');
      if (phase === 'pending' && now - since >= ms) {
        phase = 'held'; at = t.screen;
        return [{ type: 'down', x: at[0], y: at[1] }];
      }
      if (phase === 'held' && (t.screen[0] !== at[0] || t.screen[1] !== at[1])) {
        at = t.screen;
        return [{ type: 'move', x: at[0], y: at[1] }];
      }
      return [];
    },
    /** Let go at once (the pad left the world: a panel opened, the pad was unplugged). */
    release() { return end(); },
    get phase() { return phase; },
  };
}
