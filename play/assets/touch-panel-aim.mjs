// The touch aim for the panels a finger has to hit on the stash and the Horadric Cube: PlugY's page
// buttons and the stash close, the cube's Transmute button, and the cube's grid cells.
//
// The controls are small. Every stash control is a 32x32 game-px cel (plugy-stash-ui.mjs BTN: the
// page buttons and the close at 352,445), and the Transmute button is the cube grid's width by 24 game
// px (stash-panel.mjs transmuteRect). A finger's own aim error is 16 CSS px (input-touch.mjs fingerReach),
// so a near miss of a few pixels is common, and a miss spends the world instead of the control.
//
// The rules are the ones input-touch.mjs already applies to the stash grid and the statAim buttons:
//   * a point ON a control (or inside the cube's grid) is delivered raw, so the shell's own hit test decides;
//   * a point off every control, within the finger's reach of one, snaps to the NEAREST control's centre
//     (the reach is Chebyshev, the statAim pattern);
//   * otherwise a point just outside the cube's grid snaps to the cell the ring belongs to, bounded by the
//     cube panel's own rectangle (gridAim's rule), so the aim never reaches a point the panel does not own.
// The shell hands over game-px layout (touchPanelLayout); panelSpec converts it to canvas px through the
// same origin the renderers draw with. The reach is passed in, so this module stays free of the DOM.
import { BTN } from './plugy-stash-ui.mjs';
import { transmuteRect } from './stash-panel.mjs';

export const CONTROL_CEL = 32;
// PlugY's page buttons the stash draws while the shared-stash art is loaded (plugy-stash-ui.mjs LIVE, minus the gold ones).
const PAGE_BUTTONS = ['prev', 'next', 'shared', 'prevIndex', 'nextIndex'];

// The retail 800x600 frame's origin on a canvas of width x height (the same centred scale the renderers use).
export function panelOrigin(width, height) {
  const scale = Math.min(1, width / 800, height / 600);
  return { scale, x: (width - 800 * scale) / 2, y: (height - 600 * scale) / 2 };
}

const toCanvasRect = (r, o) => ({ x: o.x + r.x * o.scale, y: o.y + r.y * o.scale, w: r.w * o.scale, h: r.h * o.scale });
const inside = (x, y, r) => x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;

// The game-px layout the shell reports -> the canvas-px spec the aim reads, or null when no panel is open.
//   layout.stash: { plugy } while the stash is open (plugy: PlugY's page buttons are drawn);
//   layout.cube:  { geometry, panel } while the cube is open -- geometry is the cube's own record (the
//                 renderer's grid numbers), panel its drawn rectangle { left, top, width, height }.
export function panelSpec(layout, width, height) {
  if (!layout || !(width > 0) || !(height > 0)) return null;
  const o = panelOrigin(width, height);
  const controls = [];
  let cube = null;
  if (layout.stash) {
    if (layout.stash.plugy) for (const n of PAGE_BUTTONS) controls.push(toCanvasRect({ x: BTN[n].x, y: BTN[n].y, w: CONTROL_CEL, h: CONTROL_CEL }, o));
    // The stash's close cel is drawn whether or not PlugY is on (stash-close.mjs stashCloseAt).
    controls.push(toCanvasRect({ x: BTN.close.x, y: BTN.close.y, w: CONTROL_CEL, h: CONTROL_CEL }, o));
  }
  if (layout.cube) {
    const g = layout.cube.geometry, p = layout.cube.panel;
    if (g) {
      controls.push(toCanvasRect(transmuteRect(g), o));
      cube = {
        grid: { left: o.x + g.gridLeft * o.scale, top: o.y + g.gridTop * o.scale, columns: g.columns, rows: g.rows,
          cellWidth: g.boxW * o.scale, cellHeight: g.boxH * o.scale },
        panel: p ? toCanvasRect({ x: p.left, y: p.top, w: p.width, h: p.height }, o) : null,
      };
    }
  }
  if (!controls.length && !cube) return null;
  return { controls, cube };
}

// The cube grid's ring: the half-cell outside the grid, inside the panel. Interior points are raw.
function cubeCellAim(x, y, cube, reachOf) {
  const g = cube.grid;
  const right = g.left + g.columns * g.cellWidth, bottom = g.top + g.rows * g.cellHeight;
  if (x >= g.left && x < right && y >= g.top && y < bottom) return null;
  const halfW = reachOf(g.cellWidth / 2), halfH = reachOf(g.cellHeight / 2);
  if (x < g.left - halfW || x > right + halfW || y < g.top - halfH || y > bottom + halfH) return null;
  const p = cube.panel;
  if (!p || x < p.x || x >= p.x + p.w || y < p.y || y >= p.y + p.h) return null;
  const column = Math.min(g.columns - 1, Math.max(0, Math.round((x - g.left) / g.cellWidth - 0.5)));
  const row = Math.min(g.rows - 1, Math.max(0, Math.round((y - g.top) / g.cellHeight - 0.5)));
  return [g.left + (column + 0.5) * g.cellWidth, g.top + (row + 0.5) * g.cellHeight];
}

// The aim for a finger at (x, y): the nearest control's centre within reach, else the cube grid's ring cell,
// else null (deliver the point as it landed). reachOf(legacyCanvasPx) is the finger's reach in canvas px.
export function panelControlAim(x, y, spec, reachOf) {
  if (!spec) return null;
  for (const r of spec.controls) if (inside(x, y, r)) return null;
  let best = null, bestDistance = Infinity;
  // Every control is reached by the finger's own reach, whatever its size: a wide, short Transmute button is
  // not reached further than a 32x32 cel (the statAim pattern's half-width would reach 58 px out from it).
  const reach = reachOf(CONTROL_CEL / 2);
  for (const r of spec.controls) {
    if (x < r.x - reach || x > r.x + r.w + reach || y < r.y - reach || y > r.y + r.h + reach) continue;
    const cx = r.x + r.w / 2, cy = r.y + r.h / 2, d = (cx - x) ** 2 + (cy - y) ** 2;
    if (d < bestDistance) { bestDistance = d; best = [cx, cy]; }
  }
  if (best) return best;
  return spec.cube ? cubeCellAim(x, y, spec.cube, reachOf) : null;
}

// ---- the small targets the shell reports (vendor, waypoint, NPC menu, skill popup) ----
// The near-miss snap for the small targets a finger aims at: the vendor window's tabs and buttons, the
// waypoint menu's tabs, close and rows, the NPC dialogue menu's rows, and the skill-select popup's icons.
// Pure geometry: the shell supplies each target's canvas-pixel centre (the same hit tests the clicks run),
// and this module answers which one a finger that landed beside them means. Where two targets are within
// reach, the nearer one wins, so a tap between two rows presses the row it is closer to.

// The nearest point within `reach` canvas px of (x, y), or null. Ties keep the earlier point.
export function nearestPointAim(x, y, points, reach) {
  let best = null, bd = Infinity;
  const limit = reach * reach;
  for (const p of points) {
    if (!Array.isArray(p) || typeof p[0] !== 'number' || typeof p[1] !== 'number') continue;
    const d = (p[0] - x) ** 2 + (p[1] - y) ** 2;
    if (d <= limit && d < bd) { bd = d; best = p; }
  }
  return best;
}

// A dialog with no point list (repair yes/no, the stash gold dialog): probe rings of spokes around the
// finger, nearest ring first; the first spoke on a control is where the press goes. `on(px, py)` is the
// dialog's own hit test. Returns [px, py] or null.
export const RING_SPOKES = 16;
export function nearestControlAim(x, y, on, reach, spokes = RING_SPOKES) {
  for (const r of [reach / 4, reach / 2, (3 * reach) / 4, reach]) {
    for (let i = 0; i < spokes; i += 1) {
      const a = (i / spokes) * 2 * Math.PI, px = x + r * Math.cos(a), py = y + r * Math.sin(a);
      if (on(px, py)) return [px, py];
    }
  }
  return null;
}

// The flat list of canvas centres of every panel target the shell reports (touchPanelTargets). Each
// group is optional: null when that panel is not up.
export function panelControlPoints(t) {
  const pts = [];
  const add = (group) => { for (const p of Object.values(group || {})) if (Array.isArray(p)) pts.push(p); };
  if (t?.shop) { add(t.shop.tabs); add(t.shop.buttons); }
  if (t?.waypoint) { add(t.waypoint.tabs); if (Array.isArray(t.waypoint.close)) pts.push(t.waypoint.close); add(t.waypoint.rows); }
  if (t?.skills) add(t.skills);
  if (t?.npc) add(t.npc);
  return pts;
}
