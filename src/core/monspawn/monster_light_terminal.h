#pragma once

#include <cstdint>

namespace d2::monspawn {

// D2Common 1.13c ordinal 10426, called by D2Client's CUnit updater before it advances the
// current frame. Sequence-backed animations terminate when their frame count is non-positive;
// ordinary animations terminate when the next signed-speed step reaches the frame count.
constexpr bool clientAnimationAtEnd(bool hasSequence, int32_t frameCountPrecise,
                                   int32_t framePrecise, int16_t animSpeed) {
    return hasSequence
        ? frameCountPrecise <= 0
        : frameCountPrecise <= framePrecise + static_cast<int32_t>(animSpeed);
}

// D2Client FUN_6FAFEE40 dispatches mode-end handler 9, which unlinks this monster's light,
// only for death mode 0, SplEndDeath exactly 1, and the terminal frame predicate above.
constexpr bool terminalDeathUnlinksLight(uint32_t mode, uint8_t splEndDeath, bool atEnd) {
    return mode == 0 && splEndDeath == 1 && atEnd;
}

// Once handler 9 has unlinked the dying CUnit's source, keep that decision across the session's
// wrapped frame clock and the following dead mode. A later living mode denotes a reinitialization
// and may enroll the replacement class/source through the ordinary updater.
constexpr bool terminalLightTombstoneHeld(uint32_t mode) {
    return mode == 0 || mode == 12;
}

} // namespace d2::monspawn
