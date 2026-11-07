// Gamepad policy that is not a button map: the L3 run toggle's accident guard, and (below) the stick
// settings. Pure functions and a small store, so a test drives them with a clock and no DOM.

/** L3 is D2R's Run/Walk toggle: one press (the rising edge) toggles, a hold does not repeat.
 *  fire(pressed, nowMs) is called every poll; it returns true on the poll that should send the key. */
export function createRunToggle() {
  let was = false;
  return {
    fire(pressed) {
      const go = pressed && !was;
      was = pressed;
      return go;
    },
  };
}

// ---------------- stick settings, persisted ----------------

export const SETTINGS_KEY = 'd2.pad.settings';
export const DEFAULTS = { deadzone: 0.25, sensitivity: 1, swapSticks: false, latchItems: false, autoGold: false };
// Row order is the overlay's order. Numbers step by `step` inside [min, max]; booleans flip.
export const ROWS = [
  { key: 'deadzone', label: 'Stick deadzone', min: 0.05, max: 0.5, step: 0.05 },
  { key: 'sensitivity', label: 'Stick sensitivity', min: 0.5, max: 2, step: 0.25 },
  { key: 'swapSticks', label: 'Swap sticks', bool: true },
  { key: 'latchItems', label: 'Show items: press to latch', bool: true },
  { key: 'autoGold', label: 'Pick up nearby gold', bool: true },
];
const round = (v) => Math.round(v * 100) / 100;
function clean(raw) {
  const out = { ...DEFAULTS };
  for (const row of ROWS) {
    const v = raw?.[row.key];
    if (row.bool) { if (typeof v === 'boolean') out[row.key] = v; }
    else if (typeof v === 'number' && Number.isFinite(v)) out[row.key] = round(Math.min(row.max, Math.max(row.min, v)));
  }
  return out;
}

/** The settings, read once from `storage` (localStorage, or anything with getItem/setItem) and written on every change. */
export function createSettings(storage) {
  let value;
  try { value = clean(JSON.parse(storage?.getItem(SETTINGS_KEY))); } catch { value = clean(null); }
  return {
    get: () => value,
    set(patch) {
      value = clean({ ...value, ...patch });
      try { storage?.setItem(SETTINGS_KEY, JSON.stringify(value)); } catch { /* private mode: the session keeps it */ }
      return value;
    },
    /** Step a row by direction (-1 / +1); a boolean flips. */
    nudge(key, dir) {
      const row = ROWS.find((r) => r.key === key);
      return this.set({ [key]: row.bool ? !value[key] : value[key] + dir * row.step });
    },
  };
}

/** Raw axes [lx, ly, rx, ry] to the axes the controller reads: sticks swapped if asked, each stick's
 *  inside-the-deadzone zeroed, the rest scaled by sensitivity and held to unit length. */
export function adjustAxes(axes, s) {
  const [a, b, c, d] = [0, 1, 2, 3].map((i) => axes[i] || 0);
  const sticks = s.swapSticks ? [[c, d], [a, b]] : [[a, b], [c, d]];
  return sticks.flatMap(([x, y]) => {
    const m = Math.hypot(x, y);
    if (m < s.deadzone) return [0, 0];
    const k = Math.min(1, m * s.sensitivity) / m;
    return [x * k, y * k];
  });
}

/** A pad that reads like the browser's, with the adjusted axes. */
export const adjustedPad = (pad, s) => ({ id: pad.id, index: pad.index, mapping: pad.mapping, buttons: pad.buttons, axes: adjustAxes(pad.axes, s) });

const B = { A: 0, B: 1, START: 9, UP: 12, DOWN: 13, LEFT: 14, RIGHT: 15 };
const fmt = (row, v) => (row.bool ? (v ? 'On' : 'Off') : String(v));
/** A text overlay for the settings. Back+Start opens it; d-pad up/down picks a row, left/right changes it,
 *  B or Start closes it. update(buttons) is called every poll and returns true while it owns the pad. */
export function createSettingsPanel(viewport, settings, fontFamily = 'monospace') {
  const el = document.createElement('div');
  Object.assign(el.style, { position: 'absolute', left: '50%', top: '50%', transform: 'translate(-50%,-50%)', display: 'none', padding: '12px 18px',
    background: 'rgba(0,0,0,0.85)', color: '#c8b88a', border: '1px solid #6c5a2e', font: `16px ${fontFamily}`, whiteSpace: 'pre', zIndex: 30, pointerEvents: 'none' });
  viewport.appendChild(el);
  let open = false, row = 0;
  const was = {};
  const render = () => {
    const v = settings.get();
    el.textContent = ['Controller settings', '', ...ROWS.map((r, i) => `${i === row ? '> ' : '  '}${r.label}: ${fmt(r, v[r.key])}`), '', 'Up/Down row   Left/Right change   B close'].join('\n');
    el.style.display = open ? 'block' : 'none';
  };
  const api = {
    get open() { return open; },
    get row() { return row; },
    toggle(on = !open) { open = on; row = 0; render(); },
    update(buttons) {
      const edge = (i) => { const now = Boolean(buttons[i]?.pressed), r = now && !was[i]; was[i] = now; return r; };
      const pressed = [B.A, B.B, B.START, B.UP, B.DOWN, B.LEFT, B.RIGHT].filter(edge);
      if (!open) return false;
      for (const b of pressed) {
        if (b === B.B || b === B.START) { open = false; render(); return true; }
        if (b === B.UP) row = (row + ROWS.length - 1) % ROWS.length;
        else if (b === B.DOWN) row = (row + 1) % ROWS.length;
        else if (b === B.LEFT) settings.nudge(ROWS[row].key, -1);
        else if (b === B.RIGHT || b === B.A) settings.nudge(ROWS[row].key, 1);
      }
      render();
      return true;
    },
  };
  return api;
}
