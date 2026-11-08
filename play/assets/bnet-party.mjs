// Party and player-relation packets over the game server connection. Pure of the engine and the DOM.
//
// Relation toggle sent by the client: [0x5d][action][on][player id u32 LE], 7 bytes.
//   action 1 = loot, 2 = hear, 3 = squelch, 4 = hostile.
// Party request sent by the client: [0x5e][action][player id u32 LE], 6 bytes.
// Relation flags and the roster of players live in bnet-roster.mjs.
export { RELATION, PARTY_NONE, Roster } from './bnet-roster.mjs';
const ACTION = { loot: 1, hear: 2, squelch: 3, hostile: 4 };

const u32 = (n) => [n & 0xff, (n >>> 8) & 0xff, (n >>> 16) & 0xff, (n >>> 24) & 0xff];

export function relationPacket(kind, on, playerId) {
  const action = ACTION[kind];
  if (!action) throw new RangeError(`unknown relation ${kind}`);
  return Uint8Array.from([0x5d, action, on ? 1 : 0, ...u32(playerId >>> 0)]);
}

export function partyRequestPacket(action, playerId) {
  return Uint8Array.from([0x5e, action & 0xff, ...u32(playerId >>> 0)]);
}

// Hostility needs both players at level 9 or above, and the target in town.
export function canBeHostile(me, other) {
  return !!me && !!other && me.level >= 9 && other.level >= 9 && !!other.inTown;
}

// A toggle of `kind` for a player in the roster: the flag flips at once and the packet carries its new state
// (the server answers with the relation, which sets it again). Null when the player is not in the game, is
// the local player, or hostility is not allowed yet.
const FLAG = { loot: 1, hear: 2, squelch: 4, hostile: 8 };
export function relationRequest(roster, me = roster.get(roster.myId), kind, playerId) {
  const p = roster.get(playerId);
  if (!p || playerId === me?.id || !FLAG[kind]) return null;
  if (kind === 'hostile' && !canBeHostile(me, p)) return null;
  return relationPacket(kind, roster.toggle(playerId, FLAG[kind]), playerId);
}
