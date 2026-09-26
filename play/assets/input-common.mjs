// Input from a controller or a touch screen drives the game through the SAME DOM events a mouse and a
// keyboard produce: the shell's own pointer and key handlers turn
// them into the same engine commands, so nothing about the game is re-implemented here -- the engine
// decides, the client draws. The dispatch mirrors wasm-build/scenarios/lib/player.mjs's
// click(), which the s000 play-throughs have proved against the engine for every kind of click.

/** Client coordinates of a point in the canvas's own pixel space (800x600, however the page scales it). */
export function toClient(canvas, x, y) {
  const r = canvas.getBoundingClientRect();
  return [r.left + (x + 0.5) * r.width / canvas.width, r.top + (y + 0.5) * r.height / canvas.height];
}

/** The inverse of toClient: a client-space point (event.clientX/Y) in the canvas's own pixel space --
 *  the same formula play/gameplay-shell.mjs's own canvasPoint() uses, so a touch or controller cursor
 *  lands on the exact pixel the shell itself would read from the same client coordinates. */
export function toCanvas(canvas, clientX, clientY) {
  const r = canvas.getBoundingClientRect();
  return [(clientX - r.left) * canvas.width / r.width, (clientY - r.top) * canvas.height / r.height];
}

/** One pointer event on the canvas at canvas pixel (x, y). */
export function pointer(canvas, type, x, y, { button = 0, pointerType = 'mouse', pointerId = 1, shiftKey = false } = {}) {
  const [clientX, clientY] = toClient(canvas, x, y);
  const buttons = type === 'pointerup' ? 0 : button === 2 ? 2 : 1;
  canvas.dispatchEvent(new PointerEvent(type, {
    clientX, clientY, button, buttons, pointerType, pointerId, isPrimary: true, bubbles: true, cancelable: true, shiftKey,
  }));
}

/** A whole click -- move, down, up -- exactly the sequence a mouse click produces. */
export function click(canvas, x, y, opts = {}) {
  pointer(canvas, 'pointermove', x, y, opts);
  pointer(canvas, 'pointerdown', x, y, opts);
  pointer(canvas, 'pointerup', x, y, opts);
}

/** A key press or release on the window, where the shell listens for the keyboard. */
export function key(code, keyName = '', down = true) {
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
