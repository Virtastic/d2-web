#pragma once

#include <cstdint>
#include <cstring>

// The bridge's NPC-dialog record mirrors the last accepted 0x27 response; it is not a
// live client window handle. A successful travel action changes the player's level and
// leaves that dialog behind, so the presentation record must be retired at that boundary.
template <typename Dialog>
bool clearNpcDialogAfterActTravel(uint32_t action, int32_t oldLevel, int32_t newLevel,
                                  Dialog& dialog) {
    if (action != 0 || oldLevel == newLevel || dialog.result == 0) return false;
    std::memset(&dialog, 0, sizeof dialog);
    return true;
}
