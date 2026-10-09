#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>

#include "cast_target_relation.h"

// D2GS player skill-start records used by the retail 1.13c client receive path.
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
    uint8_t skillHand;
    uint8_t reserved;
};
static_assert(sizeof(D2RealmPlayerCast) == 28);
static_assert(offsetof(D2RealmPlayerCast, targetX) == 16);
static_assert(offsetof(D2RealmPlayerCast, unitType) == 22);

namespace d2::realm {

inline int decodeRemotePlayerCast(const uint8_t* packet, int length, D2RealmPlayerCast* output) {
    if (output) std::memset(output, 0, sizeof(*output));
    if (!packet || !output || (length != 16 && length != 17)) return 0;
    if ((packet[0] != 0x4c || length != 16) && (packet[0] != 0x4d || length != 17)) return 0;
    const auto u16 = [](const uint8_t* p) {
        return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
    };
    const auto u32 = [](const uint8_t* p) {
        return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
            (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
    };
    output->guid = u32(packet + 2);
    output->unitType = packet[1];
    output->owner = -1;
    output->opcode = packet[0];
    if (packet[0] == 0x4d) {
        output->skill = u32(packet + 6);
        output->skillHand = packet[10];
        output->targetX = u16(packet + 11);
        output->targetY = u16(packet + 13);
        output->rawTail = u16(packet + 15);
    } else {
        output->skill = u16(packet + 6);
        output->skillLevel = packet[8];
        output->targetType = packet[9];
        output->targetGuid = u32(packet + 10);
        output->rawTail = u16(packet + 14);
    }
    return length;
}

// The production D2GS receive handler uses this ordered path for Fire Bolt/Fireball
// cast starts. Ops supplies the live realm/CUnit lookups and the final native skill start;
// this function owns packet decode, target semantics and dynamic-path relation updates.
template <typename Unit, typename Ops>
int applyRemoteProjectileCastPacket(const uint8_t* packet, int length, Ops&& ops) {
    D2RealmPlayerCast cast{};
    if (!decodeRemotePlayerCast(packet, length, &cast)) return -1;
    if ((cast.opcode != 0x4d && cast.opcode != 0x4c) || !ops.isPlayerType(cast.unitType) ||
        !ops.isNaturalProjectile(cast.skill)) return 0;

    Unit* caster = ops.resolveRemoteCaster(cast.guid);
    if (!caster || !ops.casterCanStart(caster)) return 0;

    Unit* target = nullptr;
    int targetX = 0, targetY = 0;
    if (!resolveRemoteCastTarget<Unit>(cast.opcode, cast.targetType, cast.targetGuid,
            cast.targetX, cast.targetY,
            [&ops](uint8_t type, uint32_t guid) { return ops.resolveMonster(type, guid); },
            [&ops](Unit* resolved) { return ops.x(resolved); },
            [&ops](Unit* resolved) { return ops.y(resolved); }, target, targetX, targetY)) return 0;
    if (!ops.skillCanStart(caster, cast)) return 0;
    if (!ops.prepareStart(caster)) return -1;
    if (!applyCastTargetRelation(cast.opcode, target, targetX, targetY,
            [&ops, caster](int x, int y) { ops.setAim(caster, x, y); },
            [&ops, caster](Unit* relation) { ops.setTarget(caster, relation); })) return 0;
    return ops.start(caster, cast, target, targetX, targetY) ? 1 : 0;
}

} // namespace d2::realm
