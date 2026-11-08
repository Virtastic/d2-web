// The life of a Battle.net game on the client: why it ends, how long the reason stays up, the Save and Exit packet.
// Retail records the FIRST exit reason only, shows its text in the game for 600 game ticks (25 per second), then leaves.
import { REASON_TIMEOUT, REASON_DISCONNECTED, REASON_HOST_LEFT, reasonString, d2gsDenyReason } from './d2gs-deny.mjs';

export const EXIT_TICKS = 600;
export const TICK_MS = 40;
export const JOIN_TIMEOUT_MS = 30000;      // no game entered 30 s after the game started: reason 6
export const LOADING_TIMEOUT_MS = 180000;  // still on the loading screen after 3 minutes: reason 7
export const SAVE_AND_EXIT_PACKET = Uint8Array.of(0x69);

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

export function createGameLife({ now = () => Date.now(), expansion = false, onNotice = () => {}, onLeave = () => {} } = {}) {
  let phase = 'joining';   // joining, then loading, then ingame
  let phaseAt = now();
  const startedAt = phaseAt;
  let exiting = false, reason = 0, remaining = 0, left = false, saveSent = false;
  const fail = (code) => {
    if (exiting || left) return false;           // a pending exit is never overwritten
    exiting = true; reason = code > 0x1c ? 9 : code; remaining = EXIT_TICKS;
    onNotice(reasonString(reason, expansion), reason);
    return true;
  };
  return {
    loading() { phase = 'loading'; phaseAt = now(); },
    inGame() { phase = 'ingame'; phaseAt = now(); },
    deny(wireCode) { const r = d2gsDenyReason(wireCode); return r === null ? false : fail(r); },
    fail,
    connectionLost() { return fail(REASON_DISCONNECTED); },
    hostLeft() { return fail(REASON_HOST_LEFT); },
    // Save and Exit: one 0x69 byte, then the game loop ends at once with no reason shown.
    saveAndExit() { if (exiting || left || saveSent) return null; saveSent = true; left = true; return SAVE_AND_EXIT_PACKET; },
    // The per-frame timers: join timeout, loading timeout.
    checkTimers() {
      if (exiting || left) return;
      const t = now();
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
    get reason() { return reason; },
    get remaining() { return remaining; },
    get phase() { return phase; },
  };
}
