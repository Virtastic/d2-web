// WHERE THE WORLD SITS WHEN A SIDE PANEL IS OPEN (F264).
//
// Retail 1.13c moves the whole view when a "big" panel opens, and it does it at its own 800x600:
// with a panel on one half the world is offset a QUARTER OF THE SCREEN WIDTH to the other side, so
// the hero stands at the middle of the FREE half and the panel covers the vacated one -- the same
// rule the WIDESCREEN view already used (`hud-origin.mjs` panelShift). MEASURED off retail's own
// frame buffers of one static scene (the Pandemonium Fortress, F258's `cov-a4-*` catches, F209 row
// `s871`, re-measured by `s878`): aligning the no-panel town frame to its own state frames over
// the world right of the panels gives exactly +200 px for the quest log and the character panel (a
// LEFT panel) and exactly -200 px for the inventory (a RIGHT panel) -- 0.000% of the compared
// pixels differ by more than 8/255 at that shift (1.8% for the inventory, its torch alight), and
// 65-90% at every other shift. The help overlay and the automap occupy neither half and shift
// nothing. The client already shifted the world for the wide view; that shift was gated to it by
// accident of the cycle that introduced it (`wide-hud`), and retail applies it at 800x600 too -- so
// it is a property of the OPEN PANEL, not of the canvas size.
import { panelShift } from './hud-origin.mjs';

// The panels that occupy a half, by side. charpanel and waypoint-menu are LEFT, skilltree is RIGHT,
// and stash/vendor are one per side, all from the HUD mini-panel's own moves (control-panel.mjs
// HUD_LAYOUT.miniPanelShift, measured off retail's draw lists); the quest log is added here on the
// `cov-a4-questlog` frame buffer, which draws the panel on the left half and the world shifted +200.
export function panelSides({ character, quest, stash, cube, waypoint, skills, inventory, vendor }) {
  return [Boolean(character || quest || stash || cube || waypoint || vendor),
          Boolean(skills || inventory)];
}

// The world's centre column on a `width`-wide canvas, given its side panels.
export function viewCentreX(width, sides) {
  const [left, right] = sides;
  return width / 2 + panelShift(width, left, right);
}
