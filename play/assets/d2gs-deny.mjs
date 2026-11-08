// The game server's refusal packet: [0xB4][int32 code]. The code picks the notice shown on the way back to the lobby.
const WIRE_TO_REASON = [0, 1, 2, 3, 4, 5, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 9, 25, 26, 27, 28];
const REASON_STRING = [5365, 5366, 5367, 5368, 5369, 5371, 5370, 5357, 5358, 5372, 5373, 5374, 5375, 5376, 5377, 5378,
  5379, 5380, 5381, 5360, 5364, 5363, 5362, 5361, 5359, 10101, 10102, 5370, 5371];
// Expansion characters see the expansion wording for the two reasons the classic text names a classic-only limit.
const EXPANSION_STRING = { 20: 21794, 21: 21793 };
export const REASON_DISCONNECTED = 7;
export const REASON_HOST_LEFT = 8;
export const REASON_TIMEOUT = 6;
export function reasonString(reason, expansion = false) {
  if (expansion && EXPANSION_STRING[reason]) return EXPANSION_STRING[reason];
  return REASON_STRING[reason] ?? null;
}
export function d2gsDenyReason(wireCode) { return WIRE_TO_REASON[wireCode - 1] ?? null; }
export function d2gsDenyString(wireCode, expansion = false) {
  // Retail: a code outside the 26-entry jump table takes the default reason 9.
  const reason = WIRE_TO_REASON[wireCode - 1] ?? 9;
  return reasonString(reason, expansion);
}
// Returns { code } when the packet at `at` is a refusal packet, else null.
export function readD2gsDeny(bytes, at = 0) {
  if (bytes[at] !== 0xb4 || at + 5 > bytes.length) return null;
  const code = (bytes[at + 1] | (bytes[at + 2] << 8) | (bytes[at + 3] << 16) | (bytes[at + 4] << 24)) >>> 0;
  return { code };
}
export function d2gsDenyError(code, expansion = false) {
  const noticeString = d2gsDenyString(code, expansion);
  const error = new Error(`game server refused the join (reason ${code})`);
  if (noticeString) error.noticeString = noticeString;
  error.d2gsDeny = code;
  return error;
}
// Walk a decoded frame packet by packet (sizeOf = the engine's packet-size export) and return the first refusal code.
export function findD2gsDeny(bytes, sizeOf) {
  for (let at = 0; at < bytes.length;) {
    const deny = readD2gsDeny(bytes, at);
    if (deny) return deny.code;
    const n = sizeOf(bytes.subarray(at));
    if (!(n > 0)) return null;
    at += n;
  }
  return null;
}
