// Device reports for a pad the browser does not map to the standard layout (mapping '', Chrome on Android).
//
// Such a pad still has its buttons and sticks somewhere, but two things commonly differ from the standard
// layout of a handheld's built-in pad (the Retroid Pocket class, unverified on the device itself):
//   * the d-pad arrives as a HAT: one axis per direction pair, reading exactly -1, 0 or +1, and no buttons
//     12-15 at all;
//   * the triggers arrive as ANALOG AXES (0..1, rising as the trigger is pulled) with no buttons 6 and 7.
// Neither is guessed from the id. Each axis (index 4 and up; 0-3 are the sticks and are never touched) is
// classified from the samples it has produced since the pad connected:
//   * a HAT axis has only ever read -1, 0 or +1, and has moved off 0 at least once. Before it has read -1, an axis
//     in a trigger slot (4 or 5) is a digital trigger (0 and 1 only), not a hat; A constant 0 axis is
//     not a hat; neither is a stick, which reads fractions as it moves;
//   * a TRIGGER axis has never read below 0, and has read a fraction strictly between 0 and 1.
// Hat axes, in index order, are the X pair then the Y pair; trigger axes, in index order, are LT (button 6)
// then RT (button 7). The buttons are only ever ADDED to: a button the pad itself reports pressed stays
// pressed, so a pad that already sends the standard buttons is read exactly as before.
//
// Standard-mapping pads are returned untouched. Hot-plug: a pad that disconnects, or a new pad index, starts
// the classification over (reset()), so one device's history never steers another.

const HAT_FIRST_AXIS = 4;   // axes 0-3 are the two sticks
const TRIGGER_AXES_END = 6; // axes 4 and 5 are trigger slots; a never-negative digital axis there is a trigger
const HAT_HIT = 0.5;        // a hat or trigger reads as pressed past this
const EXACT = 0.02;         // a hat reads -1, 0 or +1; anything else is an analog axis

const isExact = (v) => Math.abs(v) < EXACT || Math.abs(Math.abs(v) - 1) < EXACT;

export function createReportReader() {
  let stats = new Map();   // axis index -> { moved, neg, analog, exact }
  let index = null;

  function classify(axes) {
    const hats = [], triggers = [];
    for (let i = HAT_FIRST_AXIS; i < axes.length; i++) {
      const s = stats.get(i);
      if (!s) continue;
      const digital = s.exact && s.moved;
      // An axis that has never read -1 and sits in a trigger slot (4, 5) is a digital trigger, not a hat.
      if (digital && (s.neg || i >= TRIGGER_AXES_END)) hats.push(i);
      else if (!s.neg && (s.analog || digital)) triggers.push(i);
    }
    return { hats, triggers };
  }

  return {
    /** Start over: a pad disconnected, or the browser handed back a different pad index. */
    reset() { stats = new Map(); index = null; },

    /** The pad to read this poll: the same pad, with the buttons the device reports under other names filled in. */
    read(pad) {
      if (!pad) { this.reset(); return pad; }
      if (pad.mapping === 'standard') return pad;
      if (pad.index !== index) { stats = new Map(); index = pad.index; }
      const axes = pad.axes || [];
      for (let i = HAT_FIRST_AXIS; i < axes.length; i++) {
        const v = Number(axes[i]) || 0;
        const s = stats.get(i) || { moved: false, neg: false, analog: false, exact: true };
        if (v !== 0) s.moved = true;
        if (v < 0) s.neg = true;
        if (v > EXACT && v < 1 - EXACT) s.analog = true;
        if (!isExact(v)) s.exact = false;
        stats.set(i, s);
      }
      const { hats, triggers } = classify(axes);
      const buttons = (pad.buttons || []).map((b) => ({ pressed: Boolean(b?.pressed), value: Number(b?.value) || 0 }));
      const press = (button, on, value = on ? 1 : 0) => {
        if (!buttons[button]) return;
        if (on) { buttons[button].pressed = true; buttons[button].value = Math.max(buttons[button].value, value); }
      };
      if (hats.length >= 2) {
        const x = Number(axes[hats[0]]) || 0, y = Number(axes[hats[1]]) || 0;
        press(14, x < -HAT_HIT);    // left
        press(15, x > HAT_HIT);     // right
        press(12, y < -HAT_HIT);    // up
        press(13, y > HAT_HIT);     // down
      }
      triggers.slice(0, 2).forEach((axis, n) => {
        const v = Math.max(0, Math.min(1, Number(axes[axis]) || 0));
        press(6 + n, v > HAT_HIT, v);
      });
      // A browser Gamepad keeps id, index, mapping and axes as prototype getters, so a spread would drop them.
      return { id: pad.id, index: pad.index, mapping: pad.mapping, connected: pad.connected, timestamp: pad.timestamp, axes: pad.axes, buttons };
    },
  };
}
