#pragma once
// The pure rules of the joined game's combat records: the hit action of the unit request (0x0C),
// the life a monster assignment (0xAC) carries, and the two unit-word records (0x11 overlay,
// 0x2C unit sound). What they cause is queued on the one unit-effect channel
// (d2gs_client_unit_fx.h); nothing here keeps state.
#include <cstdint>

namespace d2::realm {

// The unit request's action byte for "struck": no mode change, the unit keeps what it is doing.
inline constexpr uint32_t kHitAction = 0x13;

// A struck monster: word0 is the hit class; word1 carries the life in its low seven bits (bit 7
// is a flag), stored one higher above 1. A zero life leaves the unit's own.
struct MonsterHit { uint32_t hitClass; uint32_t life; bool setLife; };
inline MonsterHit monsterHit(uint32_t word0, uint32_t word1) {
    uint32_t life = word1 & 0x7fu;
    if (life > 1) ++life;
    return { word0, life, life != 0 };
}

// A struck player takes its hit class from the request's third word, which the 9-byte record
// never fills.
inline uint32_t playerHitClass(uint32_t word2) { return word2; }

// An assigned monster's maximum life is fixed and its life is the record's byte, both in 1/256.
inline constexpr uint32_t kAssignedMaxLife = 0x8000;
inline uint32_t assignedLife(uint8_t lifeByte) { return static_cast<uint32_t>(lifeByte) << 8; }

// {opcode, unit type, guid u32, word}: the overlay (0x11) and unit sound (0x2C) records.
struct UnitWordPacket { uint8_t unitType; uint32_t guid; uint16_t value; };
inline bool decodeUnitWordPacket(uint8_t opcode, const uint8_t* p, int length, UnitWordPacket* out) {
    if (!p || !out || length != 8 || p[0] != opcode) return false;
    out->unitType = p[1];
    out->guid = static_cast<uint32_t>(p[2]) | (static_cast<uint32_t>(p[3]) << 8) |
        (static_cast<uint32_t>(p[4]) << 16) | (static_cast<uint32_t>(p[5]) << 24);
    out->value = static_cast<uint16_t>(p[6] | (p[7] << 8));
    return true;
}

// The overlay record is taken only for a unit the client holds and a row inside the overlay table.
inline bool unitOverlayAccepted(bool haveUnit, uint32_t overlay, int overlayRows) {
    return haveUnit && static_cast<int>(overlay) < overlayRows;
}

// Two overlay rows of the overlay record also play a sound row.
inline int unitOverlaySound(uint32_t overlay) {
    return overlay == 0x97 ? 0x18c : overlay == 0x98 ? 0x18d : -1;
}

} // namespace d2::realm
