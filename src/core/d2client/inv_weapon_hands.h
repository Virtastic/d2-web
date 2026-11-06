// The inventory panel's two weapon boxes: which empty-arm silhouettes retail draws and which hand
// gets the "ghost" of a two-handed weapon.
//   1.13c D2Client+0x99440 (the panel draw): the inv_weapons silhouette over an empty hand, unless the
//     OTHER hand holds an item and D2Common #10051 (INVENTORY_GetWieldType) is 2.
//   1.13c D2Client+0x953B0 (the worn-item draw): while the wield type is 2 and neither hand's item has
//     a quiver type (D2Common #11144, ItemTypes wQuiver: a bow or crossbow), a hand's item is drawn a
//     second time, centred in the other hand's box, with a red box over that box.
#pragma once
#include <stdint.h>

namespace d2client_weapon_hands {
enum : int32_t {
  SILHOUETTE_RARM = 1,   // bodyLoc 4 shows the empty-arm silhouette
  SILHOUETTE_LARM = 2,   // bodyLoc 5 shows it
  GHOST_RARM_IN_LARM = 4,  // bodyLoc 4's item is drawn again in bodyLoc 5's box
  GHOST_LARM_IN_RARM = 8,  // bodyLoc 5's item is drawn again in bodyLoc 4's box
};
// rarm/larm: the hand holds an item; wield: #10051's result; quiverR/quiverL: #11144 of each hand's
// item (0 for an empty hand).
inline int32_t rule(bool rarm, bool larm, int32_t wield, int32_t quiverR, int32_t quiverL) {
  const bool two = wield == 2, noQuiver = quiverR == 0 && quiverL == 0;
  int32_t out = 0;
  if (!rarm && !(larm && two)) out |= SILHOUETTE_RARM;
  if (!larm && !(rarm && two)) out |= SILHOUETTE_LARM;
  if (rarm && two && noQuiver) out |= GHOST_RARM_IN_LARM;
  if (larm && two && noQuiver) out |= GHOST_LARM_IN_RARM;
  return out;
}
}  // namespace d2client_weapon_hands
