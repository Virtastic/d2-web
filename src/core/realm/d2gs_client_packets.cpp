// D2GS server->client packet readers. Keep packet validation and typed wire extraction in the
// engine boundary; the browser transports bytes and presents the resulting engine-owned state.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2gs_client_owned_item.h"

struct D2RealmPlayerAssign {
    uint32_t guid;
    uint8_t classId;
    uint8_t name[16];
    uint16_t x;
    uint16_t y;
};

// Exact 0x15 ReassignPlayer wire fields, exposed through a fixed 12-byte ABI record.
struct D2RealmPlayerReassign {
    uint32_t guid;
    uint16_t x;
    uint16_t y;
    uint8_t unitType;
    uint8_t value;
    uint8_t reserved[2];
};

static_assert(offsetof(D2RealmOwnedItemPacket, itemGuid) == 0);
static_assert(offsetof(D2RealmOwnedItemPacket, ownerGuid) == 4);
static_assert(offsetof(D2RealmOwnedItemPacket, bitstreamOffset) == 8);
static_assert(offsetof(D2RealmOwnedItemPacket, bitstreamLength) == 12);
static_assert(offsetof(D2RealmOwnedItemPacket, action) == 16);
static_assert(offsetof(D2RealmOwnedItemPacket, packetSize) == 17);
static_assert(offsetof(D2RealmOwnedItemPacket, category) == 18);
static_assert(offsetof(D2RealmOwnedItemPacket, ownerType) == 19);
static_assert(sizeof(D2RealmOwnedItemPacket) == 20);

// D2GS_GAMECHAT (0x26) preserves fields whose semantics are still unknown. Strings are kept in
// bounded, NUL-terminated buffers matching the native handler's destinations (16 and 0x100 bytes).
struct D2RealmGameChat {
    uint32_t version;
    uint32_t size;
    uint32_t messageType;
    uint32_t unknown2;
    uint32_t subType;
    uint32_t unitGuid;
    uint32_t unknown8;
    uint32_t color;
    uint32_t nameLength;
    uint32_t messageLength;
    uint8_t name[16];
    uint8_t message[256];
};

static_assert(offsetof(D2RealmPlayerAssign, guid) == 0);
static_assert(offsetof(D2RealmPlayerAssign, classId) == 4);
static_assert(offsetof(D2RealmPlayerAssign, name) == 5);
static_assert(offsetof(D2RealmPlayerAssign, x) == 22);
static_assert(offsetof(D2RealmPlayerAssign, y) == 24);
// The C++ record has three bytes of ABI padding: one before x and two after y. The returned 26 is
// the wire packet length, not sizeof the in-memory record.
static_assert(sizeof(D2RealmPlayerAssign) == 28);
static_assert(offsetof(D2RealmPlayerReassign, guid) == 0);
static_assert(offsetof(D2RealmPlayerReassign, x) == 4);
static_assert(offsetof(D2RealmPlayerReassign, y) == 6);
static_assert(offsetof(D2RealmPlayerReassign, unitType) == 8);
static_assert(offsetof(D2RealmPlayerReassign, value) == 9);
static_assert(offsetof(D2RealmPlayerReassign, reserved) == 10);
static_assert(sizeof(D2RealmPlayerReassign) == 12);
static_assert(offsetof(D2RealmGameChat, name) == 40);
static_assert(offsetof(D2RealmGameChat, message) == 56);
static_assert(sizeof(D2RealmGameChat) == 312);

namespace {
// NET_D2GS_CLIENT_INCOMING_SIZE for 1.13c. libd2 derives these entries from D2Net.dll 1.13c
// file offset 0xa900 (182 entries, 0x00..0xb5); positive values include the opcode, zero is an
// invalid/stub opcode, and -1 means the packet size comes from its body.
constexpr int16_t kServerPacketSize113c[0xb6] = {
    1, 8, 1, 12, 1, 1, 1, 6, 6, 11, 6, 6, 9, 13, 12, 16,
    16, 8, 26, 14, 18, 11, -1, 0, 15, 2, 2, 3, 5, 3, 4, 6,
    10, 12, 12, 13, 90, 90, -1, 40, 103, 97, 15, 0, 8, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 8,
    13, 0, 6, 0, 0, 13, 0, 11, 11, 0, 0, 0, 16, 17, 7, 1,
    15, 14, 42, 10, 3, 0, 0, 14, 7, 26, 40, -1, 5, 6, 38, 5,
    7, 2, 7, 21, 0, 7, 7, 16, 21, 12, 12, 16, 16, 10, 1, 1,
    1, 1, 1, 32, 10, 13, 6, 2, 21, 6, 13, 8, 6, 18, 5, 10,
    0, 20, 29, 0, 0, 0, 0, 0, 0, 2, 6, 6, 11, 7, 10, 33,
    13, 26, 6, 8, -1, 13, 9, 1, 7, 16, 17, 7, -1, -1, 7, 8,
    10, 7, 8, 24, 3, 8, -1, 7, -1, 7, -1, 7, -1, 0, -1, -1,
    1, 0, 53, -1, 5, 0,
};

inline uint16_t ru16(const uint8_t* p) {
    return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}
inline uint32_t ru32(const uint8_t* p) {
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8)
        | (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
}

// Variable-size rules recovered from the local 1.13c D2Net.dll packet-size helper. -2 means the
// packet's native availability gate is not met. A raw declared size is returned without extra
// minimum checks; a table variable with no native sizing case returns zero.
int variablePacketSize(uint8_t opcode, const uint8_t* packet, int length) {
    switch (opcode) {
        case 0x16:
            if (length < 13) return -2;
            return ru16(packet + 1);
        case 0x26: {
            if (length < 10) return -2;
            int pos = 10;
            while (pos < length && packet[pos] != 0) ++pos;
            if (pos == length) return -2;
            const int nameEnd = pos++;
            while (pos < length && packet[pos] != 0) ++pos;
            if (pos == length) return -2;
            const int messageEnd = pos;
            const int size = (nameEnd - 10) + (messageEnd - nameEnd - 1) + 12;
            return length >= size ? size : -2;
        }
        case 0x3e:
            if (length < 2) return -2;
            return packet[1];
        case 0x5b: {
            // The native helper checks 0x21 < availableBytes before reading u16@+1.
            if (length < 34) return -2;
            return ru16(packet + 1);
        }
        case 0x94:
            if (length < 9) return -2;
            return (static_cast<int>(packet[1]) + 2) * 3;
        case 0x9c:
        case 0x9d:
            if (length < 3) return -2;
            return packet[2];
        case 0xa6:
            if (length < 4) return -2;
            return ru16(packet + 2);
        case 0xa8:
        case 0xaa:
            if (length < 7) return -2;
            return packet[6];
        case 0xac:
            if (length < 13) return -2;
            return packet[12];
        case 0xae: {
            if (length < 3) return -2;
            int payload = ru16(packet + 1);
            if (payload > 0x1fd) payload = 0;  // Native replaces oversized values with zero.
            return payload + 3;
        }
        case 0xaf:
            if (length < 2) return -2;
            return packet[1] == 0 ? 2 : static_cast<int>(packet[1]) + 1;
        case 0xb3:
            if (length < 8) return -2;
            return static_cast<int>(packet[1]) + 7;
        default:
            return 0;
    }
}

int d2gsPacketSize113c(const uint8_t* packet, int length) {
    if (!packet || length < 0) return -1;
    if (length == 0) return 0;
    const uint8_t opcode = packet[0];
    if (opcode >= sizeof(kServerPacketSize113c) / sizeof(kServerPacketSize113c[0])) return -1;
    const int entry = kServerPacketSize113c[opcode];
    if (entry == 0) return -1;
    const int size = entry > 0 ? entry : variablePacketSize(opcode, packet, length);
    if (size == -2 || size == 0) return 0;
    if (size < 0) return -1;
    return length >= size ? size : 0;
}
}  // namespace

extern "C" {

// D2GS_GAMECHAT follows the native byte layout: message type, unknown byte, subtype, unit GUID,
// unknown byte, color, then bounded sender name and message strings.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_parse_gamechat(
    const uint8_t* packet, int length, D2RealmGameChat* out) {
    if (!out) return 0;
    std::memset(out, 0, sizeof(*out));
    out->version = 1;
    out->size = sizeof(*out);
    if (!packet || length < 12 || packet[0] != 0x26) return 0;
    int nameEnd = 10;
    while (nameEnd < length && packet[nameEnd]) ++nameEnd;
    if (nameEnd == length || nameEnd - 10 >= static_cast<int>(sizeof(out->name))) return 0;
    const int messageStart = nameEnd + 1;
    int messageEnd = messageStart;
    while (messageEnd < length && packet[messageEnd]) ++messageEnd;
    if (messageEnd != length - 1 || messageEnd - messageStart >= static_cast<int>(sizeof(out->message))) return 0;
    out->messageType = packet[1];
    out->unknown2 = packet[2];
    out->subType = packet[3];
    out->unitGuid = ru32(packet + 4);
    out->unknown8 = packet[8];
    out->color = packet[9];
    out->nameLength = static_cast<uint32_t>(nameEnd - 10);
    out->messageLength = static_cast<uint32_t>(messageEnd - messageStart);
    std::memcpy(out->name, packet + 10, out->nameLength);
    std::memcpy(out->message, packet + messageStart, out->messageLength);
    return length;
}

// D2GS_CREATECLIENTPLAYER (server->client opcode 0x59), from the 1.13 packet schema:
// guid:u32, class:u8, name[16], x:u16, y:u16, all little-endian. This parser consumes exactly
// one packet; it does not apply state or infer a simulation unit. Returns the wire size (26) on
// success and 0 on any invalid input. A failed parse leaves a zero record, never stale
// fields from an earlier packet.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_parse_player_assign(
    const uint8_t* packet, int length, D2RealmPlayerAssign* out) {
    if (!out) return 0;
    std::memset(out, 0, sizeof(*out));
    if (!packet || length != 26 || packet[0] != 0x59) return 0;

    out->guid = ru32(packet + 1);
    out->classId = packet[5];
    std::memcpy(out->name, packet + 6, sizeof out->name);
    out->x = ru16(packet + 22);
    out->y = ru16(packet + 24);
    return 26;
}

// D2GS_REASSIGNPLAYER (server->client opcode 0x15), 1.13c wire size 11. The schema is
// `[opcode,type,guid:u32,x:u16,y:u16,value:u8]`; the final byte is preserved without assigning
// semantics. Returns 11 on success and 0 on malformed input; a failed parse leaves a zero record.
// Applying the coordinates is a separate engine state operation.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_parse_player_reassign(
    const uint8_t* packet, int length, D2RealmPlayerReassign* out) {
    if (!out) return 0;
    std::memset(out, 0, sizeof(*out));
    if (!packet || length != 11 || packet[0] != 0x15) return 0;

    out->unitType = packet[1];
    out->guid = ru32(packet + 2);
    out->x = ru16(packet + 6);
    out->y = ru16(packet + 8);
    out->value = packet[10];
    return 11;
}

// Parse exactly one length-prefixed D2GS_ITEM_OWNED packet. 0x9d lays out
// `[op,action,size,category,itemGuid:u32,ownerType,ownerGuid:u32,itemBits...]`.
// A malformed packet zeroes the output and returns 0; otherwise the return is the
// declared wire length. Item-bitstream interpretation is a separate operation.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_parse_owned_item(
    const uint8_t* packet, int length, D2RealmOwnedItemPacket* out) {
    if (!out) return 0;
    std::memset(out, 0, sizeof(*out));
    if (!packet || length < 13 || packet[0] != 0x9d || packet[2] != length) return 0;

    out->action = packet[1];
    out->packetSize = packet[2];
    out->category = packet[3];
    out->itemGuid = ru32(packet + 4);
    out->ownerType = packet[8];
    out->ownerGuid = ru32(packet + 9);
    out->bitstreamOffset = 13;
    out->bitstreamLength = static_cast<uint32_t>(length - 13);
    return length;
}

// Frame one decompressed server-to-client stream packet with the measured 1.13c D2Net size table.
// Returns its native-reported wire size, 0 when the native reader would leave the suffix buffered,
// and -1 when this wrapper rejects invalid arguments/opcodes. The native helper reports zero for
// an invalid table opcode; this wrapper keeps that distinct from an incomplete packet so callers
// can fail closed. Unknown data is never skipped in an attempt to resynchronize.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_packet_size(const uint8_t* packet, int length) {
    return d2gsPacketSize113c(packet, length);
}

// Walk complete packets in one or more concatenated decompressed Huffman payloads and locate the
// first exactly framed PlayerAssign (0x59), wherever it occurs in packet order. Return 1 and write
// its byte offset when found; 0 when the complete prefix contains no assignment; -1 when the tail
// is incomplete; -2 when framing is invalid/unknown. The caller appends another decoded frame for
// either nonterminal status, and fails closed on -2.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_find_player_assign(
    const uint8_t* stream, int length, int* outOffset) {
    if (!outOffset || !stream || length < 0) return -2;
    *outOffset = -1;
    int offset = 0;
    while (offset < length) {
        const int packetLength = d2gsPacketSize113c(stream + offset, length - offset);
        if (packetLength == 0) return -1;
        if (packetLength < 0) return -2;
        if (stream[offset] == 0x59) {
            D2RealmPlayerAssign parsed{};
            if (d2_realm_d2gs_parse_player_assign(stream + offset, packetLength, &parsed) != 26)
                return -2;
            *outOffset = offset;
            return 1;
        }
        offset += packetLength;
    }
    return 0;
}

}  // extern "C"
