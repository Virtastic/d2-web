// Load-complete (S2C 0x04) before the player assignment (S2C 0x59). Retail's 0x04 handler needs a
// current player and exits the process without one, so the client never acts on 0x04 early: the
// packet is held until the assignment has been applied, then delivered right after it.
const LOAD_COMPLETE = 0x04, ASSIGN_PLAYER = 0x59;

export function createLoadCompleteOrder(sizeOf) {
  let held = 0;
  return {
    /** Returns the bytes to feed. `active` is whether a player is already assigned. */
    filter(bytes, active) {
      let at = 0, assigned = !!active, out = [], changed = false;
      while (at < bytes.length) {
        const n = sizeOf(bytes.subarray(at));
        if (!(n > 0) || at + n > bytes.length) { out.push(bytes.subarray(at)); at = bytes.length; break; }
        const pkt = bytes.subarray(at, at + n); at += n;
        if (pkt[0] === LOAD_COMPLETE && !assigned) { held++; changed = true; continue; }
        out.push(pkt);
        if (pkt[0] === ASSIGN_PLAYER) { assigned = true; }
      }
      if (assigned && held) { changed = true; for (; held; held--) out.push(Uint8Array.of(LOAD_COMPLETE)); }
      if (!changed) return bytes;
      const total = out.reduce((s, p) => s + p.length, 0), merged = new Uint8Array(total);
      let o = 0; for (const p of out) { merged.set(p, o); o += p.length; }
      return merged;
    },
    reset() { held = 0; },
    get held() { return held; },
  };
}
