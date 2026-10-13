// Player-side D2GS movement packet decoding and application boundary.
#include "d2gs_client_player_motion.h"

#include <cstring>
#include <emscripten.h>

#include <Units/Units.h>
#include "d2gs_client_motion.h"

// Retail x86 writes these values at CUnit +0x8c/+0x8e. The wasm64 C++
// structure preserves field order, but widened pointers move these members.
static_assert(offsetof(D2UnitStrc, nLastClickX) == 0xbc);
static_assert(offsetof(D2UnitStrc, nLastClickY) == 0xbe);

namespace {
inline uint16_t read_u16(const uint8_t* bytes) {
    return static_cast<uint16_t>(bytes[0]) | (static_cast<uint16_t>(bytes[1]) << 8);
}

inline uint32_t read_u32(const uint8_t* bytes) {
    return static_cast<uint32_t>(bytes[0]) |
        (static_cast<uint32_t>(bytes[1]) << 8) |
        (static_cast<uint32_t>(bytes[2]) << 16) |
        (static_cast<uint32_t>(bytes[3]) << 24);
}
}

extern "C" {

static void apply_server_position_hint(D2UnitStrc* unit, const D2RealmPlayerMove* move) {
    // FUN_6FADA1B0 runs before action dispatch. In the local 1.13c body, both words must be
    // nonzero, flag 0x10000 must be clear, and player modes 0/0x11 return before these stores.
    // The writes are the unit's client words at +0x8c/+0x8e; later room/path reconciliation is
    // separate and is not approximated here.
    if (!unit || !move || move->currentX == 0 || move->currentY == 0 ||
        (unit->dwFlags & 0x10000u) != 0 || unit->dwUnitType != UNIT_PLAYER ||
        unit->dwAnimMode == 0 || unit->dwAnimMode == 0x11) return;
    unit->nLastClickX = move->currentX;
    unit->nLastClickY = move->currentY;
}

// The state owner resolves the packet to its canonical player unit. The helper routes only the
// two actions the local 1.13c dispatcher sends through the coordinate target/route branch.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_apply_player_move(
    D2UnitStrc* unit, const D2RealmPlayerMove* move) {
    if (!unit || !move || move->unitType != UNIT_PLAYER ||
        unit->dwUnitType != UNIT_PLAYER || unit->dwUnitId != move->guid) return -1;
    apply_server_position_hint(unit, move);
    return d2_realm_client_motion_apply_player_move(
        unit, move->action, move->opaque, move->targetX, move->targetY);
}

EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_parse_player_move(
    const uint8_t* packet, int length, D2RealmPlayerMove* output) {
    if (output) std::memset(output, 0, sizeof(*output));
    if (!packet || !output || length != 16 || packet[0] != 0x0f) return 0;

    output->guid = read_u32(packet + 2);
    output->targetX = read_u16(packet + 7);
    output->targetY = read_u16(packet + 9);
    output->currentX = read_u16(packet + 12);
    output->currentY = read_u16(packet + 14);
    output->unitType = packet[1];
    output->action = packet[6];
    output->opaque = packet[11];
    output->reserved = 0;
    return 16;
}

}
