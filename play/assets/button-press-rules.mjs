// The held (down) cel of every momentary D2Client button, in one table the renderers and the
// instrument (test/button-press-instrument.test.mjs) both read. Each rule names the DC6 and the
// frame drawn up and held; a button is held from its pointer-down until the matching pointer-up,
// and draws the down frame only while it is the armed one. `activate` is when the action runs.
export const BUTTON_PRESS_RULES = Object.freeze({
  'stat-plus':  { cel: 'data\\global\\ui\\PANEL\\level.DC6',       up: 0, down: 1, disabled: 2, activate: 'release' },
  'level-hud':  { cel: 'data\\global\\ui\\PANEL\\level.DC6',       up: 0, down: 1, disabled: 2, activate: 'release' },
  'run-walk':   { cel: 'data\\global\\ui\\PANEL\\runbutton.dc6',   up: 0, down: 1, activate: 'release' },
  'run-run':    { cel: 'data\\global\\ui\\PANEL\\runbutton.dc6',   up: 2, down: 3, activate: 'release' },
  'menu-open':  { cel: 'data\\global\\ui\\PANEL\\menubutton.DC6',  up: 2, down: 3, activate: 'release' },
  'menu-closed':{ cel: 'data\\global\\ui\\PANEL\\menubutton.DC6',  up: 0, down: 1, activate: 'release' },
  'panel-close':{ cel: 'data\\global\\ui\\PANEL\\buysellbtn.dc6',  up: 10, down: 11, activate: 'release' },
});

// The frame a rule draws given the armed token and this button's own token.
export const pressedFrameOf = (rule, armedToken, ownToken) => (armedToken != null && armedToken === ownToken ? rule.down : rule.up);

// Pure gesture: feed [{type:'down'|'up'|'move', over:<token|null>}] and read the frame sequence for `own`.
export function simulateButton(rule, own, events) {
  let armed = null; const frames = [], activated = [];
  for (const e of events) {
    if (e.type === 'down') armed = e.over === own ? own : null;
    else if (e.type === 'up') { if (armed === own && e.over === own) activated.push(frames.length); armed = null; }
    // a held button shows its down cel only while the pointer is still over it (D2Win FUN_6F8EFA80)
    frames.push(armed === own && e.over === own ? rule.down : rule.up);
  }
  return { frames, activated };
}
