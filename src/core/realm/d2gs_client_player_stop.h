#pragma once

#include <cstdint>

namespace d2::realm {

// Decoded 1.13c D2GS 0x0D server-to-client PlayerStop record. The D2Net length
// table fixes the record at 13 bytes including the opcode, and D2Client+0xAF590
// reads unitType at +1, the unit GUID at +2, the action selector at +6, x at +7,
// y at +9, the hit class at +11 and the life percentage at +12. The fields are
// packed inside the record, so this is a decode result and not an overlay.
struct D2RealmPlayerStop {
    uint8_t unitType = 0;
    uint32_t guid = 0;
    uint8_t action = 0;
    uint16_t x = 0;
    uint16_t y = 0;
    uint8_t hitClass = 0;
    uint8_t lifePct = 0;
};

inline uint32_t playerStopU32(const uint8_t* p) {
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
        (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
}

inline uint16_t playerStopU16(const uint8_t* p) {
    return static_cast<uint16_t>(static_cast<uint16_t>(p[0]) |
        static_cast<uint16_t>(static_cast<uint16_t>(p[1]) << 8));
}

// Returns 1 for a well-formed record, -1 for a wrong opcode or length. Every
// action value is well formed: the action column is a selector, and D2Client
// passes it to FUN_6FADA750 unchanged.
inline int decodePlayerStopPacket(const uint8_t* packet, int length, D2RealmPlayerStop* out) {
    if (!packet || !out || length != 13 || packet[0] != 0x0d) return -1;
    out->unitType = packet[1];
    out->guid = playerStopU32(packet + 2);
    out->action = packet[6];
    out->x = playerStopU16(packet + 7);
    out->y = playerStopU16(packet + 9);
    out->hitClass = packet[11];
    out->lifePct = packet[12];
    return 1;
}

// D2Client+0x19830 (FUN_6FAC9830) is the player branch of the type dispatcher
// FUN_6FADA750; it indexes its action selector through the 26-entry byte/dword
// selector table (D2Client+0x19D58 / D2Client+0x19D18). Selector 6 is the
// GETHIT column: its handler body loads EDI = 4 and calls FUN_6FADA6C0, the
// same mode-setter wrapper the 0x69/0x6A monster family reaches. The D2Game
// player-mode broadcast confirms the wire identity: PLRMODE_GETHIT is row 4 of
// the 20-row mode sender table, whose field_4 action byte is 6.
// Returns the PLRMODE for a selector this projection grounds, -1 otherwise.
inline int playerStopModeFromAction(uint8_t action) {
    return action == 6 ? 4 : -1;
}

} // namespace d2::realm
