// Position resync: the 5-byte client packet 0x5F {x u16, y u16} that retail's D2Client sends when the
// server's placement puts the local player outside its radius (D2Client 0x2A1B0 -> 0x149E0). The
// bytes are built from the player's own unit position; the send goes through the wrapper 0x143E0,
// which drops an identical packet sent again within 200 ms.
//
// `take` returns the next pending 5-byte packet or null (the engine's one-slot take); `send` writes
// bytes to the D2GS socket and returns truthy when it was accepted.

export const RESYNC_OPCODE = 0x5f;
export const RESYNC_LENGTH = 5;
export const RESYNC_THROTTLE_MS = 200;

export function createRealmResync({ take, send, now = () => performance.now() }) {
  let last = null;
  let lastAt = -Infinity;
  return {
    // Sends the pending resync, if any. Returns the bytes sent, or null when nothing was sent.
    drain() {
      const bytes = take();
      if (!bytes || bytes.length !== RESYNC_LENGTH || bytes[0] !== RESYNC_OPCODE) return null;
      const t = now();
      const same = last && last.every((b, i) => b === bytes[i]);
      if (same && t - lastAt < RESYNC_THROTTLE_MS) return null;
      if (!send(bytes)) return null;
      last = Array.from(bytes);
      lastAt = t;
      return last;
    },
  };
}
