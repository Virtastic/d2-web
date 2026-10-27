// Pointer routing for the waypoint menu.  The menu's state belongs to the shell (the engine owns
// opening and 0x49); this module only converts canvas coordinates through the SAME 800x600 layout
// that draws the menu and identifies which control received the click.
//
// The rectangles are RETAIL'S, recovered from D2Client 1.13c:
//   mouse-down handler FUN_6FB5A4E0 works in panel coordinates px = x - 80, py = y - 60;
//   tabs   FUN_6FB5A290 (expansion): py <= 30 (no lower bound), px < 64 -> tab 0, < 128 -> 1, < 192 -> 2,
//          < 256 -> 3, else 4 (px <= 320); the zone is a 64 px column, NOT the 62 px cel pitch;
//   close  px 273..308, py 387..420 -> screen x 353..388, y 447..480 (36x34, not buysellbtn's cell);
//   rows   FUN_6FB5A240: px 18..296 and py in (hityRow, hityRow + 30) with the hit-y table
//          60,96,132,168,205,241,277,313,349 (irregular 36/37 pitch) -> screen x 98..376.
// A click on tab zone k selects the highest act whose preceding completion flags are all set
// (FUN_6FB5A090). This is independent of which tab cels the waypoint table happened to draw.
import { WAYPOINT_LAYOUT } from './waypoint-panel.mjs';

const PANEL_X = 80, PANEL_Y = 60;
const TAB_ZONE_W = 64, TAB_PX_MAX = 0x140, TAB_PY_MAX = 0x1e;
const CLOSE = { x: PANEL_X + 0x111, y: PANEL_Y + 0x183, w: 0x134 - 0x111 + 1, h: 0x1a4 - 0x183 + 1 };

function menuPoint(px, py, width, height) {
  const scale = Math.min(1, width / 800, height / 600);
  return { x: (px - (width - 800 * scale) / 2) / scale, y: (py - (height - 600 * scale) / 2) / scale };
}

function contains(point, rect) {
  return point.x >= rect.x && point.x < rect.x + rect.w && point.y >= rect.y && point.y < rect.y + rect.h;
}

export function createWaypointMenuInput() {
  const L = WAYPOINT_LAYOUT;
  // The tab zone of tab index act-1: a 64 px column from the panel edge, from the top of the screen
  // down to py = 30 (screen y 90).  The last zone ends at px 320 (the panel's right edge, x 400).
  const tabRect = (act) => {
    const x = PANEL_X + TAB_ZONE_W * (act - 1), right = act === 5 ? PANEL_X + TAB_PX_MAX + 1 : x + TAB_ZONE_W;
    return { x: act === 1 ? 0 : x, y: 0, w: right - (act === 1 ? 0 : x), h: PANEL_Y + TAB_PY_MAX + 1 };
  };
  // A point inside a tab's zone AND on its drawn cel (cel top 63 = anchor 94 - 31), for pad/test targets.
  const tabTarget = (act) => ({ x: PANEL_X + TAB_ZONE_W * (act - 1), y: L.tabY - 31, w: TAB_ZONE_W, h: 28 });
  const closeRect = () => CLOSE;
  const canvasPoint = (rect, width, height) => {
    const scale = Math.min(1, width / 800, height / 600);
    return [Math.round((width - 800 * scale) / 2 + (rect.x + rect.w / 2) * scale), Math.round((height - 600 * scale) / 2 + (rect.y + rect.h / 2) * scale)];
  };

  return {
    hitTest(px, py, width, height, rows, rowAt, completedActs = null, hereLevelId = null) {
      const point = menuPoint(px, py, width, height);
      if (contains(point, closeRect())) return { action: 'close' };
      for (let zone = 1; zone <= 5; zone++) {
        if (!contains(point, tabRect(zone))) continue;
        // Native FUN_6FB5A090 checks reward-granted bit 0 for completed quest records
        // [7, 0xF, 0x17, 0x1C], lowering the selected tab at the first missing act. The visible
        // tab set comes from active waypoints and is not a substitute for these completion flags.
        if (Array.isArray(completedActs) && completedActs.length === 4 && completedActs.every((v) => typeof v === 'boolean')) {
          let act = 1;
          for (let candidate = 2; candidate <= zone; candidate++) {
            if (!completedActs[candidate - 2]) break;
            act = candidate;
          }
          return { action: 'tab', act };
        }
        // A waypoint menu cannot be open without an active session. If its native record is
        // unavailable, fail closed rather than deriving progression from the rendered tab set.
        return null;
      }
      // Outside: mouse down at x > 400 or y > 551 closes the panel.
      if (point.x > 400 || point.y > 551) return { action: 'close' };
      // FUN_6FB59F00 sets the row flag only for active waypoints other than the one the player stands at.
      const row = rowAt(px, py, width, height, rows);
      return row && row.active && row.levelId !== hereLevelId ? { action: 'row', row } : null;
    },
    targets(width, height, drawnTabs = []) {
      const acts = [...new Set(drawnTabs.filter((tab) => tab?.drawn && Number.isInteger(tab.act) && tab.act >= 1 && tab.act <= 5)
        .map((tab) => tab.act))].sort((a, b) => a - b);
      return { tabs: Object.fromEntries(acts.map((act) => [act, canvasPoint(tabTarget(act), width, height)])),
        close: canvasPoint(closeRect(), width, height) };
    },
  };
}
