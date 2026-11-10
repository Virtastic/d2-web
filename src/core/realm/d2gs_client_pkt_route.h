#pragma once
// The native seam behind the retail client's movement and mode packet handlers. The handlers
// themselves are the D2Client ports (d2c_af5f0, d2c_af510, d2c_af4c0, d2c_af460 .. d2c_af7c0,
// d2c_af2b0); this header holds the pure decisions the state owner's adapter makes when those
// handlers call back into the unit-placement and action-dispatch routines.
#include <cstdint>

namespace d2::realm {

// Which handler a server opcode runs, and where its unit GUID sits.
enum class PktHandler : uint8_t { None, AF5F0, AF510, AF4C0, AF460, AF3F0, AF390, AF340, AF7C0, AF750, AF2B0 };

struct PktRoute {
    PktHandler handler = PktHandler::None;
    uint8_t length = 0;      // the framed record size
    uint8_t guidOffset = 0;  // the unit GUID
    bool typed = false;      // unit type byte at +1 (else the unit is a monster)
    bool unitTarget = false; // the request's first two words are a target unit (type, guid), not a point
    uint8_t block = 0;       // dwords the handler fills in its request block (the rest is never read)
};

inline PktRoute pktRoute(uint8_t opcode) {
    switch (opcode) {
    case 0x0c: return { PktHandler::AF5F0, 9, 2, true, false, 7 };
    case 0x0f: return { PktHandler::AF510, 16, 2, true, false, 3 };
    case 0x10: return { PktHandler::AF4C0, 16, 2, true, true, 2 };
    case 0x67: return { PktHandler::AF460, 16, 1, false, false, 7 };
    case 0x68: return { PktHandler::AF3F0, 21, 1, false, true, 7 };   // 21 bytes, the size table's entry: the handler reads its last byte at index 20
    case 0x69: return { PktHandler::AF390, 12, 1, false, false, 7 };
    case 0x6a: return { PktHandler::AF340, 12, 1, false, true, 6 };
    case 0x6b: return { PktHandler::AF7C0, 16, 1, false, false, 7 };
    case 0x6c: return { PktHandler::AF750, 16, 1, false, true, 6 };
    case 0x6d: return { PktHandler::AF2B0, 10, 1, false, false, 7 };
    default: return {};
    }
}

// The action codes of the monster branch of D2Client+0x2A750 (D2Client+0x4DC70) that the adapter
// carries out. Codes it does not carry out yet are Unhandled and leave the unit as it is.
// AttackAt / AttackOn are the arms that aim at a spot or at a unit and then set the mode the
// action table (monsterActionMode) gives the code: both attacks, block, cast and the four skills.
enum class MonsterAction : uint8_t { Unhandled, Move, GetHit, Stop, Death, Dead, AttackAt, AttackOn, Hit };

inline MonsterAction monsterAction(uint32_t code) {
    switch (code) {
    case 1: case 0x17: return MonsterAction::Move;
    case 6: return MonsterAction::GetHit;
    case 7: return MonsterAction::Stop;
    case 8: return MonsterAction::Death;
    case 9: return MonsterAction::Dead;
    case 0x13: return MonsterAction::Hit;   // struck in place: no mode change
    case 4: case 0xb: case 0xc: case 0xe: case 0x11: case 0x1a: case 0x1c: return MonsterAction::AttackAt;
    case 5: case 0xa: case 0xd: case 0xf: case 0x10: case 0x1b: case 0x1d: return MonsterAction::AttackOn;
    default: return MonsterAction::Unhandled;
    }
}

// The placement tolerance. Selector 1 gives 10 and selector 2 gives 0 for every unit.
// Otherwise the local player gets 3 / 7 / 5 (mode 1 / mode 3 / any other) plus
// ((a global dword + 0x32) >> 7), a monster is by mode (3..5 gives 5, 6..0xB gives 7, else 15),
// and every other unit gets 15. The mode table is the monster's only: a remote player is 15.
inline int placeRadius(bool isMonster, bool isLocalPlayer, uint32_t mode, uint32_t s1, uint32_t localTerm) {
    if (s1 == 1) return 10;
    if (s1 == 2) return 0;
    if (isLocalPlayer) return static_cast<int>(((localTerm + 0x32u) >> 7) + (mode == 1 ? 3u : mode == 3 ? 7u : 5u));
    if (!isMonster) return 15;
    if (mode >= 3 && mode <= 5) return 5;
    if (mode >= 6 && mode <= 0xb) return 7;
    return 15;
}

// The placement early returns: a zero coordinate or unit flag 0x10000 for any unit; then, by unit
// type only, a player (type 0) in mode 0 or 0x11 and a monster (type 1) in mode 0 or 0xC. Every other
// unit type (objects, missiles, items) is not returned on by its mode.
inline bool placeIgnored(uint32_t unitType, uint32_t mode, uint32_t flags, uint32_t x, uint32_t y) {
    if (!x || !y || (flags & 0x10000u)) return true;
    if (unitType == 0) return mode == 0 || mode == 0x11;
    if (unitType == 1) return mode == 0 || mode == 0xc;
    return false;
}

// The 0x4DC70 GETHIT arm's life value: the request's hit byte & 0x7F, plus one above 1.
inline uint32_t getHitLife(uint32_t hit) {
    hit &= 0x7f;
    return hit > 1 ? hit + 1 : hit;
}

} // namespace d2::realm
