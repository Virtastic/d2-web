// Input from a controller or a touch screen drives the game through the SAME DOM events a mouse and a
// keyboard produce: the shell's own pointer and key handlers turn
// them into the same engine commands, so nothing about the game is re-implemented here -- the engine
// decides, the client draws. The dispatch mirrors wasm-build/scenarios/lib/player.mjs's
// click(), which the s000 play-throughs have proved against the engine for every kind of click.

/** Client coordinates of the CENTRE of canvas pixel (x, y), however the page scales the canvas.
 *
 *  The +0.5 is load-bearing, and it is measured, not a preference. gameplay-shell.mjs decodes a client
 *  point with canvasPoint() (no rounding) and the hit tests truncate, so aiming at a pixel's CORNER
 *  round-trips to p-epsilon and lands on pixel p-1: with a canvas scaled to 481 of its 800 backing
 *  pixels, 58 of those 800 pixels decode into their left neighbour (4 -> 3, 7 -> 6, 8 -> 7 ...). The
 *  instrument that sees it end to end is s000a driven by the pad: with the corner, the pad run's step-9
 *  NPC click lands on a different subtile and forks from the mouse run (frame 116 vs 112, world hash
 *  different, `tools/s000-compare.mjs`); with the centre, the same pad run's `steps.json` is
 *  BYTE-IDENTICAL to the mouse run's through step 18 (frame, level, x, y, hp, gold, clvl, world hash)
 *  and first parts at step 19 -- so the corner parts at step 9 and the centre at step 19.
 *
 *  The other half of the pair belongs to a real finger: input-touch.mjs decodes a physical touch with
 *  toCanvas() (the same formula, shifted back half a pixel) and replays it through click() -> toClient,
 *  so the finger has to land on the pixel it touched. Centre here and the same 0.5 undone there are
 *  what make a mouse click, a scripted tap and a real finger at the same pixel all reach the shell as
 *  that pixel -- s315 sweeps the canvas for it, and s000a's touch run without that half parted from its
 *  mouse run at step 5, "walk by left-clicking the ground".
 *
 *  A regression to check before "fixing" either half: dropping the +0.5 makes the value look like the
 *  shell's own canvasPoint() formula, but it makes every scripted touch tap land one pixel left and up
 *  of the mouse click at the same canvas point, which is the miss this convention exists to prevent. */
export function toClient(canvas, x, y) {
  const r = canvas.getBoundingClientRect();
  return [r.left + (x + 0.5) * r.width / canvas.width, r.top + (y + 0.5) * r.height / canvas.height];
}

/** A client-space point (event.clientX/Y) as canvas pixel space (800x600, however the page scales it):
 *  play/gameplay-shell.mjs's own canvasPoint() formula, half a canvas pixel back, so the value is the
 *  pixel the shell itself would read from those client coordinates. `toCanvas(v) + 0.5` is literally
 *  canvasPoint(v); the half is undone here because this is a real finger's position, not a pixel
 *  centre, and the seam replays it through click(), which re-centres. */
export function toCanvas(canvas, clientX, clientY) {
  const r = canvas.getBoundingClientRect();
  // toClient centers on the pixel (x + 0.5); undoing that same 0.5 here is what makes a real touch
  // exact. Without it, every real touch round-tripped through pointerdown -> toCanvas -> (stored) ->
  // replay() -> click() -> toClient lands half a canvas pixel from where the finger actually touched --
  // enough to register a different subtile on a walk click (s000a's touch run parted from its mouse run
  // at step 5, "walk by left-clicking the ground", by exactly one frame).
  return [(clientX - r.left) * canvas.width / r.width - 0.5, (clientY - r.top) * canvas.height / r.height - 0.5];
}

/** Diagnostic only (G6): when a scenario sets window.__d2InputLog to an array, every event this module
 *  dispatches is appended with the engine tick it lands on (window.__d2Shell.snapshot.tick) -- s000's
 *  own per-step fingerprint (frame, world hash) proves parity at the step level; this exists to find
 *  the first differing EVENT inside a step when two runs of the identical build still part (the
 *  question two S000_INPUT=pad runs of the same commit raised). No-op, and no behaviour change, unless
 *  a scenario opts in; never enabled by default. */
function logInput(rec) { globalThis.__d2InputLog?.push({ ...rec, tick: globalThis.__d2Shell?.snapshot?.tick ?? null }); }

/** One pointer event on the canvas at canvas pixel (x, y). */
export function pointer(canvas, type, x, y, { button = 0, pointerType = 'mouse', pointerId = 1, shiftKey = false } = {}) {
  const [clientX, clientY] = toClient(canvas, x, y);
  const buttons = type === 'pointerup' ? 0 : button === 2 ? 2 : 1;
  logInput({ t: type, x, y, button, pointerType });
  canvas.dispatchEvent(new PointerEvent(type, {
    clientX, clientY, button, buttons, pointerType, pointerId, isPrimary: true, bubbles: true, cancelable: true, shiftKey,
  }));
}

/** A whole click -- move, down, up -- exactly the sequence a mouse click produces. A real right click
 *  also fires the browser's own 'contextmenu' event after pointerup; gameplay-shell.mjs's inventory
 *  right-click-to-use (read a scroll, drink a potion -- retail's 0x20) listens ONLY to that event, not
 *  to pointerdown's button, so a button:2 click that skipped it (as this one did until S000_INPUT=pad's
 *  s000a caught it: a scroll never got read) left every touch and controller right click silently
 *  inert on the inventory grid, wrong art or not -- nothing raised. A synthetic pointer event gets no
 *  contextmenu for free, so every caller that asks for button 2 -- mouse, touch long press, the
 *  controller's B button -- gets it here, once, carrying the button a real right click's event has. */
export function click(canvas, x, y, opts = {}) {
  pointer(canvas, 'pointermove', x, y, opts);
  pointer(canvas, 'pointerdown', x, y, opts);
  pointer(canvas, 'pointerup', x, y, opts);
  if (opts.button === 2) {
    const [clientX, clientY] = toClient(canvas, x, y);
    canvas.dispatchEvent(new MouseEvent('contextmenu', { clientX, clientY, button: 2, bubbles: true, cancelable: true }));
  }
}

/** A key press or release on the window, where the shell listens for the keyboard. */
export function key(code, keyName = '', down = true) {
  logInput({ t: down ? 'keydown' : 'keyup', code });
  globalThis.dispatchEvent(new KeyboardEvent(down ? 'keydown' : 'keyup', { code, key: keyName, bubbles: true, cancelable: true }));
}
export const tapKey = (code, keyName = '') => { key(code, keyName, true); key(code, keyName, false); };

/** The player's position on the canvas, through the shell's own projection (never re-derived here). */
export function playerOnCanvas(shell = globalThis.__d2Shell) {
  const p = shell?.snapshot?.player;
  if (!shell?.project || !p || !(p.hpMax > 0)) return null;   // traps.md: the snapshot is truthy before boot
  return shell.project(p.x, p.y);
}

/**
 * Where a stick at (ax, ay) in [-1, 1] aims a walk: a point `radius` canvas pixels from `origin` in the
 * stick's direction, the way a mouse user clicks ahead of the hero on screen. null inside the dead zone.
 * Screen space, not world space: up on the stick is up on the screen, as it is for the mouse.
 */
export function stickTarget(origin, ax, ay, { radius = 160, deadzone = 0.25 } = {}) {
  const mag = Math.hypot(ax, ay);
  if (!origin || !(mag > deadzone)) return null;
  return [Math.round(origin[0] + (ax / mag) * radius), Math.round(origin[1] + (ay / mag) * radius)];
}
