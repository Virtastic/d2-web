// A click on an offer in an open store, as retail decides it. The rule is the engine's
// (src/core/d2client/store_offer_click.cpp, exported as d2_client_store_offer_click, 1.13c
// D2Client+0x9BED0 / +0x9C830 / +0x95F80 / +0x47D60): a left press opens the Buy confirm box
// (+0x47500 state 1: "Buy", the name, "Gold:", the cost, Yes / No) and sends nothing; a right press buys
// at once, at most once per 500 ms of the last trade send (DAT_6fbd2309, set by +0x46EE0). This module
// only names the inputs, keeps the send tick and turns the answer into the shell's action.
import { createRepairConfirm } from './vendor-repair-confirm.mjs';

const ACTIONS = ['none', 'confirm', 'buy', 'end'];

/** rule(button, shopMode, cursorState, cursorItem, hasOffer, nowMs, lastSendMs) is the engine export. */
export function createStoreOfferClick(rule, now = () => Date.now() >>> 0) {
  let lastSend = 0;
  return {
    get lastSend() { return lastSend; },
    /** Every trade packet +0x46EE0 sends stamps the tick. */
    sent() { lastSend = now() >>> 0; },
    action({ button, mode = 1, targeting = false, cursorItem = 0, offer = false }) {
      const r = rule(button | 0, mode | 0, targeting ? 6 : 1, cursorItem ? 1 : 0, offer ? 1 : 0, now() >>> 0, lastSend >>> 0);
      return ACTIONS[r] ?? 'none';
    },
  };
}

/**
 * The one 0x47500 box for both transactions it confirms here: the Repair state machine of
 * vendor-repair-confirm.mjs, with requestBuy() holding a store offer for Yes to buy. No (0x461D0 then
 * 0x8D300) ends with `end` in either state.
 */
export function createTradeConfirm(sendRepair, needsRepair, end, sendBuy) {
  let buy = false;
  const m = createRepairConfirm((id) => (buy ? sendBuy(id) : sendRepair(id)), (id) => buy || needsRepair(id), end);
  return {
    get open() { return m.open; },
    get itemId() { return m.itemId; },
    get refusal() { return m.refusal; },
    get kind() { return buy ? 'buy' : 'repair'; },
    request(id) { if (m.open) return false; buy = false; return m.request(id); },
    requestBuy(id) { if (m.open) return false; buy = true; const ok = m.request(id); if (!ok) buy = false; return ok; },
    yes() { return m.yes(); },
    no() { return m.no(); },
    refuse(stringId) { return m.refuse(stringId); },
  };
}
