// Retail 1.13c D2Client: the Sell button (FUN_6fb47450, the up handler at RVA 0x97450) stores its action
// in the persistent mode DAT_6fbcbc34 (0x6FBCBC34); only that mode turns an inventory click into a trade.
// A plain left click on a carried item is the ordinary pick-up (FUN_6fb46690 at 0x96690 -> 0x19), and an
// item held on the cursor is sold by dropping it on the shop panel.
let sellMode = false;
export const toggleSellMode = () => (sellMode = !sellMode);
export const resetSellMode = () => { sellMode = false; };
/** What a left click does with a shop open. where: 'mine' (carried item) | 'panel' (shop window).
 *  A held item dropped on the shop is a sale on ANY cell, stocked or not: the stock hit must not win. */
export function vendorClickAction(where, cursorId, stockHit = false) {
  if (where === 'panel') return cursorId ? 'sell-cursor' : stockHit ? 'buy' : 'none';
  return sellMode && !cursorId ? 'sell' : 'pickup';
}
