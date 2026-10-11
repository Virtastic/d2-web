#pragma once

#include <cstddef>
#include <cstdint>

struct D2UnitStrc;

// Typed view of the fixed 1.13c D2GS 0x0F server-to-client PlayerMove record.
// targetX/Y are read by the local handler and passed to its path target/route calls;
// currentX/Y are consumed by a separate guarded position-correction helper.
struct D2RealmPlayerMove {
    uint32_t guid;
    uint16_t targetX;
    uint16_t targetY;
    uint16_t currentX;
    uint16_t currentY;
    uint8_t unitType;
    uint8_t action;
    uint8_t opaque;
    uint8_t reserved;
};

static_assert(offsetof(D2RealmPlayerMove, guid) == 0);
static_assert(offsetof(D2RealmPlayerMove, targetX) == 4);
static_assert(offsetof(D2RealmPlayerMove, targetY) == 6);
static_assert(offsetof(D2RealmPlayerMove, currentX) == 8);
static_assert(offsetof(D2RealmPlayerMove, currentY) == 10);
static_assert(offsetof(D2RealmPlayerMove, unitType) == 12);
static_assert(offsetof(D2RealmPlayerMove, action) == 13);
static_assert(offsetof(D2RealmPlayerMove, opaque) == 14);
static_assert(sizeof(D2RealmPlayerMove) == 16);

// Returns 16 on success. On every failure, output is zeroed when non-null.
extern "C" int d2_realm_d2gs_parse_player_move(
    const uint8_t* packet, int length, D2RealmPlayerMove* output);

// Validates a decoded packet against its already-resolved canonical player unit, then applies
// only the recovered action 0x01/0x17 target+route operation. Current-position correction is
// intentionally separate because the native helper has coordinate, mode, and room guards.
extern "C" int d2_realm_client_apply_player_move(
    D2UnitStrc* unit, const D2RealmPlayerMove* move);

// 0x4D is a coordinate skill start; 0x4C is its unit-target counterpart in 1.13c.
struct D2RealmPlayerCast {
    uint32_t guid;
    uint32_t skill;
    uint32_t targetGuid;
    int32_t owner;
    uint16_t targetX;
    uint16_t targetY;
    uint16_t rawTail;
    uint8_t unitType;
    uint8_t skillLevel;
    uint8_t targetType;
    uint8_t opcode;
    uint8_t reserved[2];
};
static_assert(sizeof(D2RealmPlayerCast) == 28);
static_assert(offsetof(D2RealmPlayerCast, targetX) == 16);
static_assert(offsetof(D2RealmPlayerCast, unitType) == 22);
extern "C" int d2_realm_d2gs_parse_player_cast(const uint8_t*, int, D2RealmPlayerCast*);
