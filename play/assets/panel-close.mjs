import { CHROME_ART } from './panel-chrome.mjs';

// Retail close controls shared by the inventory and skill tree.  Positions are the
// 800x600 widget coordinates recovered by OpenDiablo2, not pixels fitted to our canvas:
// inventory.go: invCloseButtonX/Y; skilltree.go: skillCloseButtonX*/Y and getTab().
// Keep the exact asset identity the panels already drew: their frame rows are exact
// comparisons, so this is deliberately an alias rather than a duplicate path string.
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
  if (panel === 'inventory') return { x: 419, y: 449, width: 32, height: 32 };
  if (panel === 'skills') {
    const xs = SKILL_CLOSE_X[playerClass] || SKILL_CLOSE_X[0];
    return { x: xs[Math.max(0, Math.min(2, (tab | 0) - 1))], y: 449, width: 32, height: 32 };
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
