// Gamepad menu behaviour that needs no DOM: held-direction repeat, the "back" action per menu, the chat
// field's key grid, and the button glyphs by controller family. input-controller.mjs calls these; each is
// pure so test/pad-menus.test.mjs feeds it a sequence and reads the exact answers back.

// A held direction fires once on press, again after FIRST_MS, then every EVERY_MS (devilutionX's menu
// repeater uses a 150 ms floor). A tap therefore moves one step; the controller polls every 50 ms.
export const REPEAT_FIRST_MS = 400, REPEAT_EVERY_MS = 150;
export function createRepeater(first = REPEAT_FIRST_MS, every = REPEAT_EVERY_MS) {
  const held = new Map();
  return {
    /** True when `id` should act this poll. `down` is whether it is held; `now` is ms. */
    fire(id, down, now) {
      if (!down) { held.delete(id); return false; }
      if (!held.has(id)) { held.set(id, now + first); return true; }
      if (now >= held.get(id)) { held.set(id, now + every); return true; }
      return false;
    },
    reset() { held.clear(); },
  };
}

// What B does on a menu screen, given the snap panel's current key prefix and the shell's targets:
// { key } taps that key, { click: [x, y] } clicks there, null leaves B as the right click it is over item
// grids. Every answer is a route the mouse or keyboard already has (EXIT button, Escape).
const ESCAPE_PANELS = new Set(['gamemenu', 'npcmenu', 'cinematics', 'credits', 'otherMultiplayer', 'titleGateways', 'char', 'waypoint', 'quest']);
export function backAction(targets, panelKey) {
  const prefix = String(panelKey || '').split(':')[0];
  if (ESCAPE_PANELS.has(prefix)) return { key: 'Escape' };
  if (prefix === 'select') {
    const no = targets?.select?.DELETE_NO, exit = targets?.select?.exit;
    return no ? { click: no } : exit ? { click: exit } : null;
  }
  if (prefix === 'create') return targets?.create?.exit ? { click: targets.create.exit } : null;
  return null;
}

// The chat bar's text: every printable key the shell's chat handler appends (e.key of length 1).
export const CHAT_ROWS = ['abcdefghij', 'klmnopqrst', 'uvwxyz0123', "456789.,'!", '?-:;()/ @#', ['DEL', 'OK']];
const CHAT_NAMES = { ' ': 'Space', '.': 'Period', ',': 'Comma', "'": 'Quote', '-': 'Minus', ';': 'Semicolon', '/': 'Slash' };
export function chatCellCode(label) {
  if (label === 'DEL') return ['Backspace', 'Backspace'];
  if (label === 'OK') return ['Enter', 'Enter'];
  if (/^[a-z]$/.test(label)) return [`Key${label.toUpperCase()}`, label];
  if (/^[0-9]$/.test(label)) return [`Digit${label}`, label];
  return [CHAT_NAMES[label] || 'Unidentified', label];
}

// Glyphs by controller family, from Gamepad.id. Each list is indexed by the standard mapping's button.
export function padLayout(id) {
  const s = String(id || '').toLowerCase();
  if (/054c|sony|playstation|dualshock|dualsense|ps[345]/.test(s)) return 'playstation';
  if (/057e|nintendo|switch|joy-con|pro controller/.test(s)) return 'switch';
  return 'xbox';   // the standard mapping's own names are Xbox's
}
const DPAD = ['D-pad Up', 'D-pad Down', 'D-pad Left', 'D-pad Right'];
const GLYPHS = {
  xbox: ['A', 'B', 'X', 'Y', 'LB', 'RB', 'LT', 'RT', 'View', 'Menu', 'LS', 'RS', ...DPAD],
  playstation: ['Cross', 'Circle', 'Square', 'Triangle', 'L1', 'R1', 'L2', 'R2', 'Create', 'Options', 'L3', 'R3', ...DPAD],
  // Nintendo's A/B and X/Y sit swapped against the standard mapping's positions.
  switch: ['B', 'A', 'Y', 'X', 'L', 'R', 'ZL', 'ZR', '-', '+', 'L-Stick', 'R-Stick', ...DPAD],
};
export const glyph = (layout, button) => (GLYPHS[layout] || GLYPHS.xbox)[button] ?? `Button ${button}`;
/** "A Select   B Delete" for the given [button, verb] pairs. */
export const hintLine = (layout, pairs) => pairs.map(([b, verb]) => `${glyph(layout, b)} ${verb}`).join('   ');
