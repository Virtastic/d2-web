// The life of a Battle.net game on the client: why it ends, how long the reason stays up, and the explicit leave.
// Retail records the FIRST exit reason only, shows its text in the game for 600 game ticks (25 per second), then leaves.
//
// Explicit leave (1.13c D2Client 0x59650 -> 0x59830 -> 0x14660): the menu's exit sends the trade-window CANCEL button
// (4F 02 00 ..., 7 bytes), enters state 7 and stamps GetTickCount. The game keeps running and the server's traffic keeps
// flowing while the wait is armed. When GetTickCount() is more than 30000 ms past the stamp (strictly greater), the client
// sends 0x69 (Save and Exit) and runs the teardown. The socket is closed only after that byte.
// Cancelling the wait (1.13c D2Client 0x923F0 -> 0x595C0): an item action while state 7 is armed sends 4F 07 (7 bytes),
// drops to state 3 and refuses the action. The 30 s teardown then no longer fires. The expiry is checked at that poll too
// (0x59830): an expired wait tears down there instead of cancelling. Only the armed wait is cancelled; the reason countdown is not state 7.
import { REASON_TIMEOUT, REASON_DISCONNECTED, REASON_HOST_LEFT, reasonString, d2gsDenyReason } from './d2gs-deny.mjs';
import { TRADE_BTN, tradeButtonPacket } from './bnet-game-trade.mjs';

export const EXIT_TICKS = 600;
export const TICK_MS = 40;
export const JOIN_TIMEOUT_MS = 30000;      // no game entered 30 s after the game started: reason 6
export const LOADING_TIMEOUT_MS = 180000;  // still on the loading screen after 3 minutes: reason 7
export const LEAVE_WAIT_MS = 30000;        // state 7: the Save and Exit byte goes out once this much time has passed
export const SAVE_AND_EXIT_PACKET = Uint8Array.of(0x69);
export const LEAVE_CANCEL_PACKET = tradeButtonPacket(TRADE_BTN.CANCEL);   // 4F 02 00 00 00 00 00, retail's 0x59650 bytes
export const LEAVE_WAIT_CANCEL_PACKET = Uint8Array.of(0x4f, 0x07, 0, 0, 0, 0, 0);   // retail's 0x595C0 bytes: the wait's cancel

// The 30 s join timeout before the game exists: a game server that has not let the player in by then ends the join with
// the timeout notice (exit reason 6), shown in the lobby. `expired` is true once it has fired.
export function joinDeadline({ expansion = false, ms = JOIN_TIMEOUT_MS, timers = globalThis } = {}) {
  let handle = null, expired = false;
  const promise = new Promise((_, reject) => {
    handle = timers.setTimeout(() => {
      expired = true;
      const error = new Error('the game did not start in time');
      error.noticeString = reasonString(REASON_TIMEOUT, expansion);
      reject(error);
    }, ms);
  });
  promise.catch(() => {});   // a deadline nobody is racing any more is not an unhandled rejection
  return { promise, get expired() { return expired; }, cancel() { timers.clearTimeout(handle); } };
}

// `onSend(bytes)` carries the client's packets to the game server; `onLeave(reason)` is the teardown (reason 0 for an
// explicit leave, which shows no notice).
export function createGameLife({ now = () => Date.now(), expansion = false, onNotice = () => {}, onLeave = () => {}, onSend = () => {} } = {}) {
  let phase = 'joining';   // joining, then loading, then ingame
  let phaseAt = now();
  const startedAt = phaseAt;
  let exiting = false, reason = 0, remaining = 0, left = false, leaving = false, leaveAt = 0;
  const fail = (code) => {
    if (exiting || left || leaving) return false;   // a pending exit is never overwritten; state 7 owns the leave
    exiting = true; reason = code > 0x1c ? 9 : code; remaining = EXIT_TICKS;
    onNotice(reasonString(reason, expansion), reason);
    return true;
  };
  // The 0x59830 step: 0x69 then the teardown. Runs once.
  const finishLeave = () => { left = true; onSend(SAVE_AND_EXIT_PACKET); onLeave(0); };
  return {
    loading() { phase = 'loading'; phaseAt = now(); },
    inGame() { phase = 'ingame'; phaseAt = now(); },
    deny(wireCode) { const r = d2gsDenyReason(wireCode); return r === null ? false : fail(r); },
    fail,
    connectionLost() { return fail(REASON_DISCONNECTED); },
    hostLeft() { return fail(REASON_HOST_LEFT); },
    // The menu's exit: 4F 02 00 goes out now and state 7 starts. An exit during the wait sends the bytes again and
    // starts the 30 s over; it never tears down on its own. Returns false when a reason countdown is running, so the
    // caller can tear down at once instead.
    leaveGame() {
      if (exiting || left) return false;
      leaving = true; leaveAt = now();
      onSend(LEAVE_CANCEL_PACKET);
      return true;
    },
    // An item action (0x923F0): true when it may go ahead. While the leave wait is armed it is refused and the wait
    // is cancelled with 4F 07; an expired wait is refused and tears down instead, as 0x59830 does at this poll.
    itemAction() {
      if (!leaving || left || exiting) return true;
      if (now() - leaveAt > LEAVE_WAIT_MS) { finishLeave(); return false; }
      leaving = false;
      onSend(LEAVE_WAIT_CANCEL_PACKET);
      return false;
    },
    // The page is closing: no wait is possible, so the packets go out back to back (4F first unless the menu's leave
    // already sent it). Returns the packets to send, in order; the teardown is the browser's.
    unload() {
      if (exiting || left) return [];
      const out = leaving ? [] : [LEAVE_CANCEL_PACKET];
      leaving = true; left = true;
      out.push(SAVE_AND_EXIT_PACKET);
      return out;
    },
    // The per-frame timers: join timeout, loading timeout, and the state-7 wait.
    checkTimers() {
      if (left) return;
      const t = now();
      if (leaving) { if (t - leaveAt > LEAVE_WAIT_MS) finishLeave(); return; }
      if (exiting) return;
      if (phase === 'loading' && t - phaseAt > LOADING_TIMEOUT_MS) fail(REASON_DISCONNECTED);
      else if (phase === 'joining' && t - startedAt > JOIN_TIMEOUT_MS) fail(REASON_TIMEOUT);
    },
    // One game tick of the exit countdown; returns true on the tick the game is left.
    tick() {
      if (!exiting || left) return false;
      if (--remaining > 0) return false;
      left = true; onLeave(reason); return true;
    },
    get exiting() { return exiting; },
    get leaving() { return leaving; },
    get reason() { return reason; },
    get remaining() { return remaining; },
    get phase() { return phase; },
  };
}
