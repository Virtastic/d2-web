#pragma once

#include <cstdint>
#include <vector>

namespace d2::realm {

// Death-state packets are 12 bytes including their opcode in the local 1.13c
// D2Net size table. The state byte at +5 is the D2Game mode-table selector
// column (1.13c base 0x6FD1A718, 24-byte rows; the 0x67 header reads row
// offset +0x10, the 0x68 header +0x14), not the anim mode itself. D2Client
// maps selector 6 to GH (MONMODE_GETHIT, mode 3), 8 to DT (mode 0) and 9 to
// DD (mode 12). The remaining selectors reach the client through the 0x67/0x68
// or 0x6B/0x6C families and stay out of this focused projection.
struct MonsterDeathModePacket {
    uint32_t guid = 0;
    uint32_t mode = 0;
    uint8_t opcode = 0;
    uint8_t wireState = 0;
};

// Common #11090 initializes a changed animation mode, but returns early for a
// same-mode update. A zero frame count still needs initialization before the
// first render, matching the local CUnit construction path.
inline bool monsterAnimationModeNeedsSetup(uint32_t currentMode, uint32_t requestedMode,
        uint32_t frameCountPrecise) {
    return currentMode != requestedMode || frameCountPrecise == 0;
}

// Returns 1 for a projected mode state, 0 for a valid unrelated mode, -1 for
// a malformed packet. Both packet families carry the state byte at +5.
inline int decodeMonsterDeathModePacket(const uint8_t* packet, int length,
        MonsterDeathModePacket* out) {
    if (!packet || !out || length != 12 || (packet[0] != 0x69 && packet[0] != 0x6a)) return -1;
    const uint8_t state = packet[5];
    uint32_t mode = 0;
    if (state == 6) mode = 3u;        // GETHIT selector -> MONMODE_GETHIT
    else if (state == 8) mode = 0u;   // DEATH selector  -> MONMODE_DEATH
    else if (state == 9) mode = 12u;  // DEAD selector   -> MONMODE_DEAD
    else return 0;
    out->guid = static_cast<uint32_t>(packet[1]) |
        (static_cast<uint32_t>(packet[2]) << 8) |
        (static_cast<uint32_t>(packet[3]) << 16) |
        (static_cast<uint32_t>(packet[4]) << 24);
    out->mode = mode;
    out->opcode = packet[0];
    out->wireState = state;
    return 1;
}

struct MonsterDeathModeEvent {
    uint32_t version = 1;
    uint32_t size = 32;
    uint32_t sequence = 0;
    uint32_t guid = 0;
    uint32_t packetOpcode = 0;
    uint32_t wireState = 0;
    uint32_t fromMode = 0;
    uint32_t toMode = 0;
};
static_assert(sizeof(MonsterDeathModeEvent) == 32);

inline bool appendMonsterDeathModeEvent(std::vector<MonsterDeathModeEvent>& queue,
        uint32_t sequence, uint32_t guid, uint32_t packetOpcode, uint32_t wireState,
        uint32_t fromMode, uint32_t toMode) {
    if (fromMode == toMode) return false;
    if (queue.size() == 128) queue.erase(queue.begin());
    queue.push_back({1, 32, sequence, guid, packetOpcode, wireState, fromMode, toMode});
    return true;
}

} // namespace d2::realm
