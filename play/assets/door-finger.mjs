// A finger's aim on the front door's title screen (byo-door.mjs). The door draws on its own canvas, not
// the game's #game canvas, so input-touch.mjs's snap never reaches it and a finger there got raw taps:
// a thumb that landed a few CSS px beside ENTER SANCTUARY pressed nothing. This module is the door's own
// nearest-control snap, with the same reach input-touch.mjs gives the game's controls.
//
// The snap is for TOUCH only (the caller passes the pointer's type). A mouse keeps the exact hit test it
// always had, and an exact hit always wins over a snap, so a finger on a button presses that button.
// Everything here is in the door's 800x600 frame; the caller converts the reach from CSS px.

// One finger's reach, in CSS px: the same value input-touch.mjs uses (FINGER_REACH_CSS). Kept as a
// copy, not an import: input-touch.mjs pulls in the game's controls, and the door must not load them.
export const FINGER_REACH_CSS = 16;

/** The distance from a point to a rectangle's nearest edge or corner (0 inside it). */
export function distanceTo(p, r) {
  const dx = Math.max(r.x - p.x, 0, p.x - (r.x + r.w));
  const dy = Math.max(r.y - p.y, 0, p.y - (r.y + r.h));
  return Math.hypot(dx, dy);
}

/**
 * The control a press at p lands on. `controls` is an ordered list of { key, x, y, w, h }.
 * An exact hit is the first control (in list order) whose rectangle holds p, as before. With a
 * positive `reach` (game px), a point outside every rectangle snaps to the nearest control whose
 * edge is within reach; a tie goes to the control whose centre is nearer. Returns null otherwise.
 */
export function fingerHit(p, controls, reach = 0) {
  for (const c of controls) if (p.x >= c.x && p.y >= c.y && p.x < c.x + c.w && p.y < c.y + c.h) return c.key;
  if (!(reach > 0)) return null;
  let best = null, bestD = Infinity, bestC = Infinity;
  for (const c of controls) {
    const d = distanceTo(p, c);
    if (d > reach) continue;
    const cc = Math.hypot(p.x - (c.x + c.w / 2), p.y - (c.y + c.h / 2));
    if (d < bestD || (d === bestD && cc < bestC)) { best = c.key; bestD = d; bestC = cc; }
  }
  return best;
}

/**
 * The reach in game px for a finger on a canvas: FINGER_REACH_CSS CSS px through the canvas's drawn box
 * (canvasPerCss = canvas.width / its CSS width) and the frame's scale (frameScale = canvas px per game px).
 * Zero when the drawn box is not measurable, so a stub or an off-page canvas gets no snap.
 */
export function fingerReachGame(canvasPerCss, frameScale) {
  if (!(canvasPerCss > 0) || !(frameScale > 0)) return 0;
  return FINGER_REACH_CSS * canvasPerCss / frameScale;
}

/**
 * The commands a sequence of finger samples sends the door: a press on a control is remembered, and a
 * lift on that same control acts (the door's onUp rule). `events` are { type: 'down'|'up', key } where key
 * is the control the sample resolved to (null for none). Returns the ordered list of 'act:<key>' commands.
 */
export function pressCommands(events) {
  const out = [];
  let down = null;
  for (const { type, key } of events) {
    if (type === 'down') down = key;
    else if (type === 'up') { if (key && key === down) out.push('act:' + key); down = null; }
  }
  return out;
}
