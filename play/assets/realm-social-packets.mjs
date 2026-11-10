// The social packets of a joined Battle.net game: chat, the roster, the event lines and trade. The bytes
// the transport delivers can hold several packets, and the join burst holds the players already in the
// game; `sizeOf(bytes)` (the client's size table) splits them and each packet goes to the handler its
// opcode names, in stream order. Nothing here decides a game rule.
//   0x26                       a chat line (own handler: the message line and the overhead bubble)
//   0x5a                       an event line (joined, left, slain, party notices)
//   0x5b 0x5c 0x8b 0x8c 0x8d   the roster
//   0x77 0x78 0x79             trade

// One 0x26 packet: [1] type, [2] unknown, [3] sub type, [4..7] unit guid, [8] unknown, [9] colour, then
// the name and the message, each NUL-terminated. The same fields the wasm client keeps of its last chat.
export function parseGameChat(packet) {
  if (!packet || packet[0] !== 0x26 || packet.length < 12) return null;
  const nameEnd = packet.indexOf(0, 10);
  if (nameEnd < 0 || nameEnd - 10 > 15) return null;
  const messageEnd = packet.indexOf(0, nameEnd + 1);
  if (messageEnd < 0 || messageEnd - nameEnd - 1 > 255) return null;
  return { messageType: packet[1], unknown2: packet[2], subType: packet[3],
    unitGuid: (packet[4] | packet[5] << 8 | packet[6] << 16 | packet[7] << 24) >>> 0,
    unknown8: packet[8], color: packet[9],
    name: Array.from(packet.subarray(10, nameEnd)), message: Array.from(packet.subarray(nameEnd + 1, messageEnd)) };
}

export function routeSocialPackets(bytes, sizeOf, { roster = null, chat = null, event = null, trade = null } = {}) {
  let routed = 0;
  for (let at = 0; at < bytes.length;) {
    const n = sizeOf(bytes.subarray(at));
    if (!(n > 0) || at + n > bytes.length) break;
    const packet = bytes.subarray(at, at + n), op = packet[0];
    if (op === 0x26) { if (chat) { const parsed = parseGameChat(packet); if (parsed) { chat(parsed); routed++; } } }
    else if (op === 0x5a) { if (event) { event(packet); routed++; } }
    else if (op === 0x5b || op === 0x5c || (op >= 0x8b && op <= 0x8d)) { if (roster) { roster(packet); routed++; } }
    else if (op >= 0x77 && op <= 0x79) { if (trade) { trade(packet); routed++; } }
    at += n;
  }
  return routed;
}
