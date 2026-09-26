// Touch is an input layer: every gesture here ends as one of
// the SAME DOM pointer or key events a mouse and a keyboard produce, dispatched through
// input-common.mjs's proven helpers, so play/gameplay-shell.mjs's own handlers decide what happens
// -- nothing about the game is re-implemented here.
//
// A raw tap already reaches the shell's canvas listener unchanged: Pointer Events carry pointerType
// through, and that listener never reads it, so M3 is real once touch-action:none keeps the browser
// from turning the gesture into a scroll or a zoom first. Long press (M5) is the one thing retail's
// mouse has no equivalent of turning up unasked, so a touch's pointerdown on the canvas is held back
// from the shell (a capture-phase listener above the canvas, stopping the real event before the
// shell's own bubble-phase listener ever sees it) for up to LONG_PRESS_MS, then REPLAYED as exactly
// the click the shell already knows how to read -- a left tap if released early, a right click
// (button 2) if the hold matures where it started. The virtual stick and the button bar (M1/M2/M4)
// are new DOM elements this module creates and owns in #touch-overlay; each just dispatches the same
// primitives. Everything synthetic this module dispatches goes through input-common's own
// click()/tapKey() -- the exact calls a mouse and a keyboard already produce -- so a row comparing a
// touch path against a mouse or keyboard path is comparing two runs of the SAME code, not two
// implementations of the same idea.

import { click, tapKey, toCanvas, playerOnCanvas, stickTarget } from './input-common.mjs';

export const LONG_PRESS_MS = 500;
export const MOVE_CANCEL_PX = 12;      // client px of drift that gives up on a long press (still taps)
export const STICK_TICK_MS = 150;      // how often a held stick re-aims the walk, as a mouse re-clicked
export const STICK_RADIUS = 160;       // input-common.stickTarget's own default, named for the CSS geometry

const BAR_KEYS = [
  { code: 'KeyI', label: 'I' },
  { code: 'KeyC', label: 'C' },
  { code: 'KeyT', label: 'T' },
  { code: 'Escape', label: 'Esc' },
];

function injectStyle(documentObject) {
  if (documentObject.getElementById('d2-touch-style')) return;
  const style = documentObject.createElement('style');
  style.id = 'd2-touch-style';
  // The overlay is a transparent pass-through layer (pointer-events:none) so a mouse player's clicks
  // reach the canvas beneath it untouched; only .d2-touch-hit children opt in, and only once revealed
  // (coarse pointer, or the first real touch -- "The overlay appears only on touch
  // devices... never over a mouse player's screen").
  style.textContent =
    '#touch-overlay{position:absolute;inset:0;opacity:0;pointer-events:none;transition:opacity .15s;font:14px monospace}' +
    '#touch-overlay.d2-touch-on{opacity:1}' +
    '#touch-overlay .d2-touch-hit{pointer-events:none}' +
    '#touch-overlay.d2-touch-on .d2-touch-hit{pointer-events:auto}' +
    '.d2-touch-stick{position:absolute;left:5%;bottom:7%;width:26%;max-width:150px;aspect-ratio:1;' +
      'border-radius:50%;background:rgba(201,173,104,.12);border:2px solid rgba(201,173,104,.45);touch-action:none}' +
    '.d2-touch-knob{position:absolute;left:50%;top:50%;width:42%;height:42%;border-radius:50%;' +
      'background:rgba(201,173,104,.55);transform:translate(-50%,-50%);pointer-events:none}' +
    '.d2-touch-bar{position:absolute;right:4%;bottom:7%;display:flex;gap:10px}' +
    '.d2-touch-btn{width:44px;height:44px;border-radius:8px;background:rgba(20,18,12,.65);' +
      'border:2px solid rgba(201,173,104,.45);color:#c9ad68;font:15px monospace;touch-action:none;' +
      'display:flex;align-items:center;justify-content:center;user-select:none}' +
    '#d2-touch-name{position:absolute;left:39.75%;top:82.2%;width:27.5%;height:5.2%;box-sizing:border-box;' +
      'font:16px monospace;padding:2px 6px;background:#1a170f;color:#e8d9ab;border:1px solid #493d27;touch-action:none}' +
    // A wide phone held sideways letterboxes the 4:3 game with black bars at both sides: the stick and
    // the key bar go there, off the orbs, belt and skill buttons they covered when drawn over the game.
    // (A 4:3 tablet has no bars; they stay over the game there.) Last, so it overrides the rules above.
    '@media (pointer: coarse) and (min-aspect-ratio: 16/9){' +
      '.d2-touch-stick{position:fixed;left:calc((100vw - 100dvh * 4 / 3) / 4);bottom:12%;transform:translateX(-50%);' +
        'width:min(150px, calc((100vw - 100dvh * 4 / 3) / 2 - 16px))}' +
      '.d2-touch-bar{position:fixed;right:calc((100vw - 100dvh * 4 / 3) / 4);bottom:12%;transform:translateX(50%);flex-direction:column}' +
    '}';
  documentObject.head.appendChild(style);
}

/**
 * Mount the touch input layer on the page: the canvas long-press/tap handling, the virtual stick,
 * the button bar, and the name-entry bridge. Safe to call once per page; a page with none of
 * the expected markup (#game, #touch-overlay) is a no-op, so this can be the page's whole script.
 */
export function installTouchInput({ documentObject = globalThis.document, windowObject = globalThis.window } = {}) {
  const canvas = documentObject.getElementById('game');
  const container = documentObject.getElementById('touch-overlay');
  if (!canvas || !container) return null;

  injectStyle(documentObject);
  canvas.style.touchAction = 'none';   // M3: a touch drag on the canvas walks the hero, never the page

  let revealed = false;
  function reveal() { if (!revealed) { revealed = true; container.classList.add('d2-touch-on'); } }
  if (windowObject.matchMedia?.('(pointer: coarse)')?.matches) reveal();

  // Events THIS module redispatches at the canvas (a confirmed tap, a matured long press, a stick
  // tick) must not be read back as a new gesture. element.dispatchEvent() is synchronous -- every
  // listener runs before it returns -- so reserving the pointerId only around that call is enough.
  const replaying = new Set();
  function replay(x, y, opts) {
    const id = opts.pointerId ?? 1;
    replaying.add(id);
    try { click(canvas, x, y, opts); } finally { replaying.delete(id); }
  }

  // ---- M3 / M5: a tap is a click, a long press is a right click, tracked per pointerId (M4) -----
  const holds = new Map();   // pointerId -> { x, y, startClientX, startClientY, timer, fired }

  documentObject.addEventListener('pointerdown', (e) => {
    if (e.pointerType !== 'touch' || replaying.has(e.pointerId)) return;
    reveal();   // "the first touch", wherever it lands
    if (e.target !== canvas) return;
    e.stopPropagation();
    if (e.cancelable) e.preventDefault();
    const [x, y] = toCanvas(canvas, e.clientX, e.clientY);
    const h = { x, y, startClientX: e.clientX, startClientY: e.clientY, fired: false, timer: null };
    h.timer = setTimeout(() => {
      h.fired = true;
      replay(h.x, h.y, { button: 2, pointerType: 'touch', pointerId: e.pointerId });
    }, LONG_PRESS_MS);
    holds.set(e.pointerId, h);
  }, { capture: true });

  documentObject.addEventListener('pointermove', (e) => {
    if (e.pointerType !== 'touch' || replaying.has(e.pointerId)) return;
    const h = holds.get(e.pointerId);
    if (!h) return;
    e.stopPropagation();
    const [x, y] = toCanvas(canvas, e.clientX, e.clientY);
    h.x = x; h.y = y;
    if (h.timer && Math.hypot(e.clientX - h.startClientX, e.clientY - h.startClientY) > MOVE_CANCEL_PX) {
      clearTimeout(h.timer); h.timer = null;   // drifted too far to be a long press; still taps on release
    }
  }, { capture: true });

  documentObject.addEventListener('pointerup', (e) => {
    if (e.pointerType !== 'touch' || replaying.has(e.pointerId)) return;
    const h = holds.get(e.pointerId);
    if (!h) return;
    e.stopPropagation();
    if (h.timer) clearTimeout(h.timer);
    if (!h.fired) replay(h.x, h.y, { button: 0, pointerType: 'touch', pointerId: e.pointerId });
    holds.delete(e.pointerId);
  }, { capture: true });

  documentObject.addEventListener('pointercancel', (e) => {
    if (e.pointerType !== 'touch') return;
    const h = holds.get(e.pointerId);
    if (!h) return;
    e.stopPropagation();
    if (h.timer) clearTimeout(h.timer);
    holds.delete(e.pointerId);
  }, { capture: true });

  // ---------------------------- M1 / M4: the virtual stick -----------------------------------
  const stickBase = documentObject.createElement('div');
  stickBase.id = 'd2-touch-stick'; stickBase.className = 'd2-touch-stick d2-touch-hit';
  const knob = documentObject.createElement('div'); knob.className = 'd2-touch-knob';
  stickBase.appendChild(knob); container.appendChild(stickBase);

  let stick = null;   // { pointerId, ax, ay, timer }
  function stickAt(clientX, clientY) {
    const r = stickBase.getBoundingClientRect();
    const cx = r.left + r.width / 2, cy = r.top + r.height / 2, radius = Math.max(1, r.width / 2);
    const dx = clientX - cx, dy = clientY - cy, mag = Math.hypot(dx, dy) || 1;
    const clamp = Math.min(1, mag / radius);
    knob.style.transform = `translate(${-50 + (dx / mag) * clamp * 50}%, ${-50 + (dy / mag) * clamp * 50}%)`;
    return { ax: dx / radius, ay: dy / radius };
  }
  function stickTick() {
    if (!stick) return;
    const target = stickTarget(playerOnCanvas(), stick.ax, stick.ay, { radius: STICK_RADIUS });
    if (target) replay(target[0], target[1], { button: 0, pointerType: 'touch', pointerId: stick.pointerId });
  }
  stickBase.addEventListener('pointerdown', (e) => {
    if (stick) return;   // one thumb on the stick at a time
    // Pointer capture keeps the drag reporting to this element even if the finger slides off it; a
    // synthetic PointerEvent (a scenario's dispatch, not a real OS-tracked pointer) has no "active
    // pointer" to capture and throws NotFoundError here, which must not stop the gesture itself.
    try { stickBase.setPointerCapture?.(e.pointerId); } catch {}
    const aim = stickAt(e.clientX, e.clientY);
    stick = { pointerId: e.pointerId, ax: aim.ax, ay: aim.ay, timer: setInterval(stickTick, STICK_TICK_MS) };
    stickTick();
    if (e.cancelable) e.preventDefault();
  });
  stickBase.addEventListener('pointermove', (e) => {
    if (!stick || e.pointerId !== stick.pointerId) return;
    const aim = stickAt(e.clientX, e.clientY);
    stick.ax = aim.ax; stick.ay = aim.ay;
  });
  const releaseStick = (e) => {
    if (!stick || e.pointerId !== stick.pointerId) return;
    clearInterval(stick.timer); stick = null;
    knob.style.transform = 'translate(-50%,-50%)';
  };
  stickBase.addEventListener('pointerup', releaseStick);
  stickBase.addEventListener('pointercancel', releaseStick);

  // -------------------------------- M2: the button bar ---------------------------------------
  const bar = documentObject.createElement('div'); bar.id = 'd2-touch-bar'; bar.className = 'd2-touch-bar';
  for (const { code, label } of BAR_KEYS) {
    const btn = documentObject.createElement('div');
    btn.className = 'd2-touch-btn d2-touch-hit'; btn.dataset.touchKey = code; btn.textContent = label;
    // The same primitive the keyboard's own keydown/keyup produce (input-common.tapKey): retail's
    // handler cannot tell this apart from a physical key, because it is not.
    btn.addEventListener('pointerdown', (e) => { tapKey(code, code === 'Escape' ? 'Escape' : ''); if (e.cancelable) e.preventDefault(); });
    bar.appendChild(btn);
  }
  container.appendChild(bar);

  // ------------- issue 8: a focused input that opens the phone keyboard, forwarding to typeKey ----
  const nameInput = documentObject.createElement('input');
  nameInput.type = 'text'; nameInput.id = 'd2-touch-name'; nameInput.className = 'd2-touch-hit';
  nameInput.autocomplete = 'off'; nameInput.autocapitalize = 'off'; nameInput.spellcheck = false;
  nameInput.maxLength = 15; nameInput.style.display = 'none';
  container.appendChild(nameInput);

  // A phone keyboard's own keydown is unreliable (predictive text and IME composition report
  // "Unidentified" on many devices), so what is forwarded is the INPUT's value, diffed against what
  // it held a moment ago and replayed a character at a time through the SAME tapKey a physical key
  // uses -- charcreate.mjs's typeKey does the validating and the 15-character cap, once, either way.
  let prevValue = '';
  function forwardNameDiff(oldValue, newValue) {
    if (newValue.startsWith(oldValue)) { for (const ch of newValue.slice(oldValue.length)) tapKey('', ch); }
    else if (oldValue.startsWith(newValue)) { for (let i = oldValue.length; i > newValue.length; i--) tapKey('Backspace', 'Backspace'); }
    else { for (let i = 0; i < oldValue.length; i++) tapKey('Backspace', 'Backspace'); for (const ch of newValue) tapKey('', ch); }
  }
  nameInput.addEventListener('input', () => { const value = nameInput.value; forwardNameDiff(prevValue, value); prevValue = value; });
  nameInput.addEventListener('keydown', (e) => { if (e.key === 'Enter') { tapKey('Enter', 'Enter'); e.preventDefault(); } });

  // The shell already exposes exactly the signal retail's own name box uses to appear (a class
  // picked): polled, so this needs no new hook into the hot shared shell file.
  setInterval(() => {
    const shell = windowObject.__d2Shell;
    const show = revealed && shell?.screen === 'create' && Boolean(shell?.charCreateEvidence?.chosen);
    // The stick and the key bar are for play: on the title, select and create screens they sat over
    // retail's own buttons (CREDITS, CINEMATICS, EXIT on a landscape phone) and took their taps.
    const inGame = shell?.screen === 'game' ? '' : 'none';
    if (stickBase.style.display !== inGame) { stickBase.style.display = inGame; bar.style.display = inGame; }
    if (show && nameInput.style.display === 'none') { nameInput.style.display = 'block'; prevValue = nameInput.value = ''; nameInput.focus(); }
    else if (!show && nameInput.style.display !== 'none') { nameInput.style.display = 'none'; nameInput.blur(); }
  }, 200);

  return { canvas, container, stickBase, bar, nameInput };
}

installTouchInput();
