import { CHROME_ART } from './panel-chrome.mjs';

// Retail close controls shared by the inventory and skill tree. `panelCloseLayout` is now the ONE
// place that holds the button's position: it returns the TOP-LEFT rect that matches retail's own
// draw-list anchor (buysellbtn frame 10, 32x32, bottom-row y), so the rect that DRAWS the button and
// the rect that HIT-TESTS it can never drift apart again -- which is the bug F-button-align fixed:
// the two read from separate numbers (the draw call used a retail anchor, the hit rect used
// OpenDiablo2's inventory.go/skilltree.go widget coordinates unadjusted) and disagreed by 1-4 px, so
// a click near the button's own edge missed a control that was visibly right there.
//   inventory: the retail captures and vendor-malah-act5.txt both draw it at x=418 y=476
//   (bottom row) regardless of which panel pairs with it on the left -- inventory's own screen
//   position is fixed. skills: the retail captures and skilltree-act5.txt draw the
//   Barbarian's tab-1 close at x=571 y=477 -- one less than SKILL_CLOSE_X's Barbarian entry (572),
//   which is why the table's x is used as SKILL_CLOSE_X - 1: OpenDiablo2's skilltree.go table names
//   the class/tab x for every other class (unmeasured beyond Barbarian) and retail's own capture
//   confirms the -1, fixed y here rather than per class/tab.
export const PANEL_CLOSE_ART = Object.freeze({ path: CHROME_ART.close, frame: 10 });

const SKILL_CLOSE_X = Object.freeze([
  [572, 501, 416], // Amazon
  [416, 416, 572], // Sorceress
  [416, 572, 416], // Necromancer
  [416, 501, 416], // Paladin
  [572, 416, 572], // Barbarian
  [572, 572, 572], // Druid
  [501, 572, 416], // Assassin
]);

export function panelCloseLayout(panel, { playerClass = 0, tab = 1 } = {}) {
  if (panel === 'inventory') return { x: 418, y: 445, width: 32, height: 32 };
  if (panel === 'skills') {
    const xs = SKILL_CLOSE_X[playerClass] || SKILL_CLOSE_X[0];
    return { x: xs[Math.max(0, Math.min(2, (tab | 0) - 1))] - 1, y: 446, width: 32, height: 32 };
  }
  return null;
}

/** The visible panel close hit target at a canvas point, or null. */
export function panelCloseAt(px, py, width, height, state) {
  const scale = Math.min(1, width / 800, height / 600);
  const x = (px - (width - 800 * scale) / 2) / scale;
  const y = (py - (height - 600 * scale) / 2) / scale;
  for (const panel of ['inventory', 'skills']) {
    if (!state?.[panel]) continue;
    const r = panelCloseLayout(panel, state[panel]);
    if (x >= r.x && x < r.x + r.width && y >= r.y && y < r.y + r.height) return { panel, ...r };
  }
  return null;
}
