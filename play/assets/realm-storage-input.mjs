// The stash and the Horadric Cube in a joined game, driven by the same panels a local game draws. A click on
// either panel becomes the retail item packets: lift (0x19), put in a cell (0x18) or swap with the item under
// it (0x1F) with the page of the grid (stash 4, cube 3), and the cube's Transmute button is 0x4F button 24.
// A shift+click on a carried item sends it to the belt (0x63). Everything else (the server's answer, the new
// item positions) comes back as item packets; nothing here moves an item locally.
import { itemPackets, BUTTON } from './realm-item-packets.mjs';
import { INV_PAGE } from './realm-input.mjs';
import { decideGridPlace } from './grid-place.mjs';

/**
 * `ctx`: { width, height, items, cursorId, input (createRealmInput), send(bytes), inventory: {visible, renderer},
 * stash: {open, renderer, layout}, cube: {open, renderer, layout} }. Returns true when the click was the panels'.
 */
export function realmStorageClick(ctx, x, y, { button = 0, shift = false } = {}) {
  if (button !== 0) return false;
  const { width: W, height: H, items, cursorId, input: ri } = ctx;
  const held = () => items.find((i) => i.id === cursorId) || { id: cursorId };
  const grid = (panel, layout, page) => {
    if (!panel.renderer || !layout || !panel.renderer.contains(x, y, W, H, layout)) return false;
    if (!cursorId) { const item = panel.renderer.itemAt(x, y, W, H, layout, items); if (item) ri.lift(item.id); return true; }
    const cell = panel.renderer.cellAt(x, y, W, H, layout, held());
    if (!cell) return true;
    const d = decideGridPlace(items, page, cell.x, cell.y, held());
    if (d.action === 'swap') ri.swapGrid(cursorId, d.item.id, cell.x, cell.y);
    else if (d.action !== 'refuse') ri.putInGrid(cursorId, cell.x, cell.y, page);
    return true;
  };
  const cube = ctx.cube;
  if (cube.open && ctx.inventory.visible && cube.renderer && cube.layout && cube.renderer.contains(x, y, W, H, cube.layout)) {
    if (cube.renderer.transmuteAt(x, y, W, H, cube.layout)) { ctx.send(itemPackets.button(BUTTON.TRANSMUTE)); return true; }
    return grid(cube, cube.layout, INV_PAGE.CUBE);
  }
  if (ctx.stash.open && grid(ctx.stash, ctx.stash.layout, INV_PAGE.STASH)) return true;
  const inv = ctx.inventory;
  if (shift && !cursorId && inv.visible && inv.renderer?.contains(x, y, W, H)) {
    const item = inv.renderer.itemAt(x, y, W, H, ctx.snapshot);
    if (item) { ctx.send(itemPackets.shiftToBelt(item.id)); return true; }
  }
  return false;
}
