#pragma once

#include <cstdint>
#include <unordered_map>

namespace d2::drop_flip {

// Retail's 0x9C "dropped to ground" builds the client item in IMODE_DROPPING and runs the flippy at
// wAnimSpeed 256: 17 frames, one per game frame (UNITS_SetAnimStartFrame), then the item rests.
inline constexpr uint32_t kFlipFrames = 17;
inline constexpr uint32_t kFrameCountPrecise = 17u << 8;

// The server creates a dropped item already on the ground, so the client view replays the flip from the
// frame it first sees the item there. An item seen off the ground (cursor, inventory, belt) forgets
// that frame: picking an item up and dropping it again (same unit id) flips again.
struct Clock {
    uint32_t prime = 0;
    std::unordered_map<uint32_t, uint32_t> first;

    void reset(uint32_t gameFrame) { first.clear(); prime = gameFrame; }

    // True while the item is flipping; `precise` is then its clock (frames since first seen << 8).
    bool step(uint32_t id, bool onGround, uint32_t now, bool inTick, uint32_t& precise) {
        if (!onGround) { first.erase(id); return false; }
        const uint32_t at = first.emplace(id, inTick ? now - 1 : now).first->second;
        if (at <= prime || now - at >= kFlipFrames) return false;
        precise = (now - at) << 8;
        return true;
    }
};

}  // namespace d2::drop_flip
