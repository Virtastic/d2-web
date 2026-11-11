// The single-item Repair confirmation, retail D2Client 0x47500 in state 3 (DAT_6FBC9729). It is NOT a
// stash-gold amount box: it builds an NPC-style text menu (0x6FB53E40, the same box, boxpieces frame and
// font16 lines as the NPC menu, anchored at the NPC like the trade menu 0x4B420) with these lines
// (0x6FB536F0: first line 21px, then 15px pitch; colour in eax):
//   string 3351 "Repair"                       gold (4)
//   the item's name, first line of 0x914F0     gold
//   string 3346 "Gold:"                        gold
//   the amount, "%d" of 0x46230's out value    gold  (repair cursor state: transaction type 3, repair)
//   string 3344 "Yes"                          white (0), selectable -> 0x6FAF7480: closes, 0x46EE0 sends 0x35
//   string 3345 "No"                           white, selectable -> 0x6FAF61D0, the menu's own close (also its outside-click)
// Nothing is sent from the item click itself: this module holds the pending item until Yes.
import { createNpcMenu } from './npc-menu.mjs';

export const REPAIR_TITLE_STRING = 3351;   // 0xd17 "Repair"
export const BUY_TITLE_STRING = 3348;      // 0xd14 "Buy" (0x47500 state 1, the same box)
export const GOLD_STRING = 3346;           // 0xd12 "Gold:"
export const YES_STRING = 3344;            // 0xd10 "Yes"
export const NO_STRING = 3345;             // 0xd11 "No"
export const CONTINUE_STRING = 3403;       // 0xd4b "Continue"

// D2Client 0x4B5D0: a 0x2A trade reply with a result above 6 (both the single-item and the Repair All 0x35, which
// 0x46EE0 sends with 0x6FBC9663 = 5) closes the trade and opens the NPC text menu: the message in colour 1 (red)
// over a selectable white "Continue". Codes 7..16 index this table; any other code above 6 is 0xd21. Code 9 (the
// server refused: nothing to repair, ethereal, wrong NPC) is 3361 "I cannot complete that request."; code 12 (not
// enough gold) is 3362 "You do not have enough gold for that.". The engine's d2_client_4b5d0_refusal_string is the
// same table.
export const REFUSAL_STRINGS = [0xd1e, 0xd1e, 0xd21, 0xd1c, 0xd20, 0xd22, 0xd21, 0xd21, 0xd23, 0xd1d];
/** D2Client 0x4B420 (the hire window's reply): 3, 5 and 6 leave the box down; every other code opens it, the table for 7..16 and 0xD21 otherwise. */
export const hireRefusalString = (code) => (code === 3 || code === 5 || code === 6 ? 0 : code >= 7 && code <= 16 ? REFUSAL_STRINGS[code - 7] : 0xd21);
export const refusalString = (code) => (!(code > 6) ? 0 : code <= 16 ? REFUSAL_STRINGS[code - 7] : 0xd21);

/**
 * Pending-item state machine (an item that does not need repair, per 0x45B40, never opens it): request() opens,
 * yes() hands the item to send(), no() drops it. refuse(stringId) opens the 0x4B5D0 refusal box after a reply;
 * its only entry, Continue, is yes() or no(), which close it.
 */
// `end` runs when the box closes without a repair being sent: No (0x461D0, then 0x8D300) and the refusal's
// Continue (0x4AAF0, 0x8F630, 0x8D300) both drop the cursor to mode 1, ending Repair mode. Yes keeps it:
// the success reply re-presses the Repair button (0x8F660 then 0x94970).
export function createRepairConfirm(send, needsRepair = () => true, end = () => {}) {
  let pending = -1, refusal = 0;
  return {
    get open() { return pending >= 0 || refusal > 0; },
    get itemId() { return pending; },
    get refusal() { return refusal; },
    request(itemId) { if (!(itemId >= 0) || pending >= 0 || refusal || !needsRepair(itemId)) return false; pending = itemId; return true; },
    yes() { if (refusal) { refusal = 0; end(); return true; } if (pending < 0) return false; const id = pending; pending = -1; send(id); return true; },
    no() { const was = pending >= 0 || refusal > 0; pending = -1; refusal = 0; if (was) end(); return was; },
    refuse(stringId) { if (stringId > 0) { pending = -1; refusal = stringId; } return refusal > 0; },
  };
}

/**
 * The menu 0x47500 builds for `title` (the item's top-down name text, lines split by '\n') and `cost`.
 * 0x914F0 writes a multi-line name bottom-up (D2Win draws the last line on top), and 0x47500 keeps the text
 * before its first '\n': the bottom line, i.e. the LAST line of a top-down title.
 */
export function repairConfirmMenu(title, cost, string, titleString = REPAIR_TITLE_STRING) {
  const t = (id, d) => string(id) || d;
  const name = String(title ?? '').split('\n').filter(Boolean).at(-1) ?? '';
  return {
    heads: [t(titleString, titleString === BUY_TITLE_STRING ? 'Buy' : 'Repair'), name, t(GOLD_STRING, 'Gold:'), String(Math.trunc(cost) | 0)],
    entries: [{ stringId: YES_STRING, action: 'yes' }, { stringId: NO_STRING, action: 'no' }],
  };
}

/** The confirm drawn through the NPC menu renderer; hitAt answers 'yes' / 'no' / 'modal'. */
export function createRepairConfirmRenderer(deps) {
  const menu = createNpcMenu(deps);
  return {
    get open() { return menu.open; },
    show(npcId, title, cost, titleString = REPAIR_TITLE_STRING) { const m = repairConfirmMenu(title, cost, deps.string, titleString); menu.show(npcId, 0, m.heads, m.entries); },
    /** 0x4B5D0's box: the refusal line in red (colour 1), then "Continue". */
    showRefusal(npcId, stringId) { menu.show(npcId, 0, [deps.string(stringId) || String(stringId)], [{ stringId: CONTINUE_STRING, action: 'no' }], 1); },
    close() { menu.close(); },
    hover(x, y) { return menu.hover(x, y); },
    targets() { return menu.targets(); },
    hitAt(x, y) {
      const e = menu.entryAt(x, y);
      if (e) return e.action;
      return menu.outside(x, y) ? 'no' : 'modal';
    },
    draw(ctx, feetX, feetY, act = 1) { return menu.draw(ctx, feetX, feetY, act); },
  };
}
