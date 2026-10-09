#pragma once

#include <cstdint>

namespace d2::realm {

// D2Client's missile update calls its terminal hit handler on a collision or
// lifetime expiry. Keep the browser edge limited to those native terminal
// events and to Fire Ball's known 1.13c missile row until other classes have
// their own recovered playback contract.
constexpr int32_t kFireballMissileClass = 62;
constexpr uint32_t kMissileEventCollision = 2;
constexpr uint32_t kMissileEventTargetHit = 3;
constexpr uint32_t kMissileEventExpired = 4;

constexpr int32_t fireballImpactSoundRow(uint32_t missileClass, uint32_t event,
        int32_t hitSoundRow) {
    const bool terminal = event == kMissileEventCollision ||
        event == kMissileEventTargetHit || event == kMissileEventExpired;
    return missileClass == kFireballMissileClass && terminal && hitSoundRow >= 0
        ? hitSoundRow : -1;
}

} // namespace d2::realm
