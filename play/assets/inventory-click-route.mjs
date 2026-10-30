// The player's own inventory under a left click with an empty cursor: which action retail takes.
// The rule is the engine's (src/core/d2client/inv_click_route.cpp, exported as
// d2_client_inv_click_route, 1.13c D2Client+0x95FE0/+0x96280/+0x96690/+0x96AA0); this module only
// names the shop mode the shell is in and turns the engine's answer into the shell's command.
// Retail's shop mode DAT_6fbcbc34: 1 neutral, 3 Sell (the Sell button), 4 Repair (the Repair button).
export const SHOP_MODE = Object.freeze({ neutral: 1, sell: 3, repair: 4 });
// Retail's cursor state DAT_6fbcb864: 6 is an identify scroll or tome armed over the inventory.
const CURSOR_NORMAL = 1, CURSOR_TARGETING = 6;

export function shopModeOf({ shopOpen, sell, repair }) {
  if (!shopOpen) return SHOP_MODE.neutral;
  return repair ? SHOP_MODE.repair : sell ? SHOP_MODE.sell : SHOP_MODE.neutral;
}

/** route(cursorState, shopMode, bodyLoc, hasItem) is the engine export. bodyLoc 0 = the grid.
 *  Returns 'identify' | 'sell' | 'repair' | 'pickup' | 'none'. */
export function inventoryClickAction(route, { itemId, bodyLoc = 0, targeting = false, mode = SHOP_MODE.neutral, ctrl = false, npcOpen = false }) {
  const has = Number.isInteger(itemId) && itemId >= 0 ? 1 : 0;
  const r = route(targeting ? CURSOR_TARGETING : CURSOR_NORMAL, mode, bodyLoc | 0, has, ctrl ? 1 : 0, npcOpen ? 1 : 0);
  if (r === 3) return 'identify'; if (r === 4) return 'sell-direct';
  if (r === 1) return 'pickup';
  if (r === 2) return mode === SHOP_MODE.sell ? 'sell' : mode === SHOP_MODE.repair ? 'repair' : 'none';
  return 'none';
}
