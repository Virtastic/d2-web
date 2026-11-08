// The D2GS client packets a joined game's stash and cube need beyond the inventory moves in realm-input.mjs
// (1.13c client-to-server, little endian): the 0x4F panel buttons and the belt moves.
const make = (op, ...u32) => { const out = new Uint8Array(1 + u32.length * 4), v = new DataView(out.buffer); out[0] = op;
  u32.forEach((n, i) => v.setUint32(1 + i * 4, n >>> 0, true)); return out; };
const button = (id, hi = 0, lo = 0) => Uint8Array.of(0x4f, id & 255, id >> 8, hi & 255, hi >> 8, lo & 255, lo >> 8);
export const BUTTON = Object.freeze({ CLOSE_STASH: 18, WITHDRAW_GOLD: 19, DEPOSIT_GOLD: 20, TRANSMUTE: 24 });
export const itemPackets = Object.freeze({
  button,                                                    // 7: {u16 button, u16 high, u16 low}
  stashButton: (id, amount = 0) => button(id, amount >>> 16, amount & 0xffff),
  shiftToBelt: (guid) => make(0x63, guid),                   // 5
  toBelt: (guid, slot) => make(0x23, guid, slot),            // 9
  fromBelt: (guid) => make(0x24, guid),                      // 5
});
