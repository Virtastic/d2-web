// Pointer routing for the waypoint menu.  The menu's state belongs to the shell (the engine owns
// opening and 0x49); this module only converts canvas coordinates through the SAME 800x600 layout
// that draws the menu and identifies which rendered control received the click.
//
// The tab frames and anchors are now MEASURED, not declared (s492; retail's own Act II and Act IV
// waypoint-menu captures): a tab stands at tabX + 62*(act-1) with frame 2*(act-1) if it is the shown
// act's and 2*(act-1)+1 otherwise, and the cels are 63 wide on that 62 px pitch, so consecutive tabs
// overlap by one column.  A tab's click band is the 62 px PITCH, not the 63 px cel: the overlapping
// column is drawn by the later act's cel and belongs to it.  Retail's rects themselves are still not
// captured -- the bands remain our approximation and stay declared in P6 --
// and the close uses buysellbtn frame 10's 96x32 rendered-cell band.
import { WAYPOINT_LAYOUT } from './waypoint-panel.mjs';

const TAB_HEIGHT = 32;
const CLOSE_WIDTH = 96;
const CLOSE_HEIGHT = 32;

function menuPoint(px, py, width, height) {
  const scale = Math.min(1, width / 800, height / 600);
  return { x: (px - (width - 800 * scale) / 2) / scale, y: (py - (height - 600 * scale) / 2) / scale };
}

function contains(point, rect) {
  return point.x >= rect.x && point.x < rect.x + rect.w && point.y >= rect.y && point.y < rect.y + rect.h;
}

export function createWaypointMenuInput() {
  const L = WAYPOINT_LAYOUT;
  const tabRect = (act) => ({ x: L.tabX + L.tabPitch * (act - 1), y: L.tabY - TAB_HEIGHT, w: L.tabPitch, h: TAB_HEIGHT });
  const closeRect = () => ({ x: L.closeX, y: L.closeY - CLOSE_HEIGHT + 1, w: CLOSE_WIDTH, h: CLOSE_HEIGHT });
  const canvasPoint = (rect, width, height) => {
    const scale = Math.min(1, width / 800, height / 600);
    return [Math.round((width - 800 * scale) / 2 + (rect.x + rect.w / 2) * scale), Math.round((height - 600 * scale) / 2 + (rect.y + rect.h / 2) * scale)];
  };

  return {
    hitTest(px, py, width, height, rows, rowAt, drawnTabs = []) {
      const point = menuPoint(px, py, width, height);
      if (contains(point, closeRect())) return { action: 'close' };
      // Route tabs only when their cel actually reached the canvas. The engine's waypoint table
      // decides which tabs exist; `drawnTabs` carries the renderer's per-blit evidence for that set.
      // This prevents empty, undisplayed act slots from swallowing clicks or becoming phantom tabs.
      for (const tab of drawnTabs) {
        if (!tab?.drawn || !Number.isInteger(tab.act) || tab.act < 1 || tab.act > 5) continue;
        if (contains(point, tabRect(tab.act))) return { action: 'tab', act: tab.act };
      }
      const row = rowAt(px, py, width, height, rows);
      return row ? { action: 'row', row } : null;
    },
    targets(width, height, drawnTabs = []) {
      const acts = [...new Set(drawnTabs.filter((tab) => tab?.drawn && Number.isInteger(tab.act) && tab.act >= 1 && tab.act <= 5)
        .map((tab) => tab.act))].sort((a, b) => a - b);
      return { tabs: Object.fromEntries(acts.map((act) => [act, canvasPoint(tabRect(act), width, height)])),
        close: canvasPoint(closeRect(), width, height) };
    },
  };
}
