// The life of a Battle.net game on the client: why it ends, how long the reason stays up, and the explicit leave.
// Retail records the FIRST exit reason only, shows its text in the game for 600 game ticks (25 per second), then leaves.
//
// Save and Exit: the menu's exit row and the window's close both arrive as WM_CLOSE. The client's close handler sends the
// Save and Exit byte (0x69) at once and swallows the message; no 4F packet goes out first and nothing waits. The socket is
// closed only after that byte.
//
// The 4F 02 / 30 s wait is a different path: it is the trade window's CANCEL button (4F 02 00 ..., 7 bytes), which enters
// state 7 and stamps GetTickCount. The game keeps running while the wait is armed. When GetTickCount() is more than
// 30000 ms past the stamp (strictly greater), the poll sends 0x69 and runs the teardown.
// Cancelling the wait: an item action while state 7 is armed sends 4F 07 (7 bytes), drops to state 3 and refuses the
// action. The 30 s teardown then no longer fires. The expiry is checked at that poll too: an expired wait tears down
// there instead of cancelling. Only the armed wait is cancelled; the reason countdown is not state 7.
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
export function joinDeadline({ expansion = false, ms = JOIN_TIMEOUT_MS, timers = globalThis, now = () => Date.now() } = {}) {
  // The clock starts when the client's game starts (before the game server is dialled) and the check is strict: it fires
  // only once more than `ms` has passed, so a timer that wakes at exactly `ms` waits one more millisecond.
  let handle = null, expired = false;
  const startedAt = now();
  const promise = new Promise((_, reject) => {
    const fire = () => {
      if (now() - startedAt <= ms) { handle = timers.setTimeout(fire, 1); return; }
      expired = true;
      const error = new Error('the game did not start in time');
      error.noticeString = reasonString(REASON_TIMEOUT, expansion);
      reject(error);
    };
    handle = timers.setTimeout(fire, ms);
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
    // A dead connection is reason 7 only once the game has been entered; before that the join clock decides (reason 6).
    connectionLost() { return phase === 'ingame' ? fail(REASON_DISCONNECTED) : false; },
    hostLeft() { return fail(REASON_HOST_LEFT); },
    // The menu's Save and Exit (and the window's close): 0x69 goes out at once and the game is torn down. It does not
    // wait and sends no 4F packet, even when a trade-cancel wait is armed. Returns false when the game is already
    // leaving or a reason countdown is running, so the caller can tear down at once instead.
    saveAndExit() {
      if (exiting || left) return false;
      left = true; onSend(SAVE_AND_EXIT_PACKET); onLeave(0);
      return true;
    },
    // The trade window's CANCEL: 4F 02 00 goes out now and state 7 starts. A second press during the wait sends the bytes
    // again and starts the 30 s over; it never tears down on its own. Returns false when a reason countdown is running.
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
    // The page is closing, which retail sees as the window's close: 0x69 alone, once. Returns the packets to send; the
    // teardown is the browser's.
    unload() {
      if (exiting || left) return [];
      left = true;
      return [SAVE_AND_EXIT_PACKET];
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

// The server's 0x05 (unload complete) starts the loading wait and its 0x04 (load complete) enters the game. Each packet
// overwrites the flags the previous one set, so the LAST of either in a decoded frame decides; it is returned as 'loading'
// or 'ingame'. Other packets are skipped: 0x59 (create the player unit), 0x0B (pick the local player from the unit table)
// and 0x11 (set a unit's mode) never touch the entered / loading flags or the clocks.
// Settled by the client's handlers: 0x04 sets entered, clears loading (and ends the process if no local player unit
// exists yet, so a sane server sends 0x59 and 0x0B before it); 0x05 clears entered, sets loading and re-stamps the loading
// clock, leaving the join clock alone. The join timeout (30 s) fires only while neither flag is set, the loading timeout
// (3 minutes) only while loading is set, the lost-connection exit only once entered.
// Entry order, measured: 0x59 creates the unit, 0x0B makes it the local player, then 0x04 enters the game. 0x04 first ends
// the process; 0x0B first does nothing and the next stat update halts.
// Leaving: the menu exit sends 0x69 first and the socket closes after it; the join timeout and a dropped connection send no
// 0x69, they only set the exit flag and the teardown closes the socket.
// GUESS: how long after 0x59 the real server sends 0x0B (the measured order is from a hand-built join, not the server).
export function lifePhaseFromFrame(bytes, sizeOf) {
  let seen = null;
  for (let at = 0; at < bytes.length;) {
    if (bytes[at] === 0x05) seen = 'loading'; else if (bytes[at] === 0x04) seen = 'ingame';
    const n = sizeOf(bytes.subarray(at));
    if (!(n > 0)) break;
    at += n;
  }
  return seen;
}

// The page is closing (pagehide): send the unload packets synchronously on the game socket, then close it from this
// side. WebSocket.close() queues the close frame behind the bytes already sent, so the bridge sees both and ends the
// realm side with a FIN, which is the orderly close the game server saves the character on. Returns what was sent.
export function flushUnloadLeave(life, transport) {
  if (!life || !transport) return [];
  const sent = [];
  for (const bytes of life.unload()) {
    try { if (transport.send(bytes) !== false) sent.push(bytes); } catch { /* socket already closed */ }
  }
  if (sent.length) { try { transport.close(); } catch { /* already closed */ } }
  return sent;
}
