#pragma once
// Wire forms of the fixed-size server packets for objects, portals and item side effects. Each writes the packet's
// bytes little-endian and returns its length; the senders in the server pass them to the client's packet list.
#include <cstdint>
#include <cstring>

namespace scmd {
inline void put16(uint8_t* at, uint32_t v) { at[0] = static_cast<uint8_t>(v); at[1] = static_cast<uint8_t>(v >> 8); }
inline void put32(uint8_t* at, uint32_t v) { put16(at, v); put16(at + 2, v >> 16); }

// 0x42: the unit whose cursor item is cleared; no unit is type 6, GUID -1.
inline int encode42(uint8_t out[6], bool hasUnit, uint32_t unitType, uint32_t unitGuid) {
    out[0] = 0x42; out[1] = hasUnit ? static_cast<uint8_t>(unitType) : 6; put32(out + 2, hasUnit ? unitGuid : 0xffffffffu);
    return 6;
}
// 0x51: an object's class, place, mode and interact type.
inline int encode51(uint8_t out[14], uint8_t header, uint8_t unitType, uint32_t guid, uint32_t classId, uint32_t x, uint32_t y,
        uint8_t mode, uint8_t interact) {
    out[0] = header; out[1] = unitType; put32(out + 2, guid); put16(out + 6, classId); put16(out + 8, x); put16(out + 10, y);
    out[12] = mode; out[13] = interact;
    return 14;
}
// 0x60: a town portal's flags and destination level.
inline int encode60(uint8_t out[7], uint8_t flags, uint8_t level, uint32_t guid) {
    out[0] = 0x60; out[1] = flags; out[2] = level; put32(out + 3, guid);
    return 7;
}
// 0x7D: one item flag set or cleared on a player's item; a missing unit is type 6 or GUID -1.
inline int encode7D(uint8_t out[18], bool hasPlayer, uint32_t playerType, uint32_t playerGuid, bool hasItem, uint32_t itemGuid,
        uint32_t flag, uint32_t set) {
    out[0] = 0x7d; out[1] = hasPlayer ? static_cast<uint8_t>(playerType) : 6; put32(out + 2, hasPlayer ? playerGuid : 0xffffffffu);
    put32(out + 6, hasItem ? itemGuid : 0xffffffffu); put32(out + 10, flag); put32(out + 14, set);
    return 18;
}
// 0x82: a portal pair and its owner's name (at most 15 characters and a terminator).
inline int encode82(uint8_t out[29], uint32_t ownerGuid, const char* ownerName, uint32_t portalGuid, uint32_t destPortalGuid) {
    std::memset(out, 0, 29);
    out[0] = 0x82; put32(out + 1, ownerGuid);
    for (int i = 0; i < 15 && ownerName && ownerName[i]; ++i) out[5 + i] = static_cast<uint8_t>(ownerName[i]);
    put32(out + 21, portalGuid); put32(out + 25, destPortalGuid);
    return 29;
}
// 0xA5: a unit and a skill.
inline int encodeA5(uint8_t out[8], uint8_t unitType, uint32_t guid, uint32_t skill) {
    out[0] = 0xa5; out[1] = unitType; put32(out + 2, guid); put16(out + 6, skill);
    return 8;
}
}
