// F66: which renderer entry a wall-list record goes through -- the wall entry (39, opaque) or the roof entry
// (40, the transparent one) -- is D2Client's per-record "does this wall hide the player" test, not DRLG data.
// Recovered from D2Client.dll 1.13c: Wall2.cpp's gather
// (6fb2a5d0) sends every tile whose type is not the roof/lower-wall block 15..19 through 6fb2a340, which asks
// 6fb29f20 and, when the answer is yes, fades the tile from alpha 0xff to 0x80 over 500 ms (retail's
// transparent entry; the fade is time-based state this rule does not model). 6fb29f20, with the coord-list
// mode that is always on (DAT_6fb924f4 = 1, never written):
//   hide  <=>  the tile has a coord list, the list is not the one under the player (nIndex differs), and the
//              player's tile lies left of or above the list's first box:  px < box0.x0 || py < box0.y0
// Tiles carrying dwFlags & 4 skip the test (they are never faded).
// `player` is { tileX, tileY, list } -- the player's position / 5 and D2Common #10095's coord-list index there.
export function wallHidesPlayer({ orientation, flags }, coord, player) {
  if (flags & 4) return false;
  if (orientation >= 15 && orientation <= 19) return false;
  if (!coord || coord.nIndex < 0) return false;
  if (coord.nIndex === player.list) return false;
  return player.tileX < coord.x0 || player.tileY < coord.y0;
}
