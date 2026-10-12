#include "d2gs_client_player_motion.h"

#include <cstring>
#include <emscripten.h>

namespace {
uint16_t read_u16(const uint8_t* bytes) {
    return static_cast<uint16_t>(bytes[0]) | (static_cast<uint16_t>(bytes[1]) << 8);
}

uint32_t read_u32(const uint8_t* bytes) {
    return static_cast<uint32_t>(bytes[0]) |
        (static_cast<uint32_t>(bytes[1]) << 8) |
        (static_cast<uint32_t>(bytes[2]) << 16) |
        (static_cast<uint32_t>(bytes[3]) << 24);
}
}

extern "C" EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_parse_player_cast(
    const uint8_t* packet, int length, D2RealmPlayerCast* output) {
    if (output) std::memset(output, 0, sizeof(*output));
    if (!packet || !output || (length != 16 && length != 17)) return 0;
    if ((packet[0] != 0x4c || length != 16) && (packet[0] != 0x4d || length != 17)) return 0;
    output->guid = read_u32(packet + 2);
    output->unitType = packet[1];
    output->owner = -1; // Owner identity is resolved from the leading unit type/GUID.
    output->opcode = packet[0];
    if (packet[0] == 0x4d) {
        // The native callback reads this dword as a packed action argument. Community schemas
        // identify its low word as skill; preserve all four bytes in `skill` for ABI stability.
        output->skill = read_u32(packet + 6);
        output->skillHand = packet[10];
        output->targetX = read_u16(packet + 11);
        output->targetY = read_u16(packet + 13);
        output->rawTail = read_u16(packet + 15);
    } else {
        output->skill = read_u16(packet + 6);
        output->skillLevel = packet[8];
        output->targetType = packet[9];
        output->targetGuid = read_u32(packet + 10);
        output->rawTail = read_u16(packet + 14);
    }
    return length;
}
