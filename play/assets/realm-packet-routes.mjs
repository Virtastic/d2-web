// Which server packets of a joined Battle.net game reach the client's own handlers. The bytes the transport
// delivers can hold several packets; `sizeOf(bytes)` (the wasm client's size table) splits them, and each
// packet goes to the handler its opcode names. Nothing here decides a game rule.
//   0x28  the server's answer to a talk: the input opens or ends the conversation
//   0x7b  one slot of the hotkey table
//   0x81  a pet or hireling listed under its owner
//   0x97  the weapon-set flip
export function routeRealmPackets(bytes, sizeOf, { npcInteraction = null, applyHotkey = null, applyWeaponSwitch = null, applyPet = null } = {}) {
  const done = [];
  for (let at = 0; at < bytes.length;) {
    const n = sizeOf(bytes.subarray(at));
    if (!(n > 0) || at + n > bytes.length) break;
    const packet = bytes.subarray(at, at + n);
    if (packet[0] === 0x28 && npcInteraction) done.push({ op: 0x28, result: npcInteraction(packet) });
    else if (packet[0] === 0x7b && applyHotkey) done.push({ op: 0x7b, result: applyHotkey(packet) });
    else if (packet[0] === 0x81 && applyPet) done.push({ op: 0x81, result: applyPet(packet) });
    else if (packet[0] === 0x97 && applyWeaponSwitch) done.push({ op: 0x97, result: applyWeaponSwitch(packet) });
    at += n;
  }
  return done;
}
