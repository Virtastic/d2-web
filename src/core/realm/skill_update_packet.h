// The two skill-update server packets (0x21 assign, 0x22 quantity). 0x21 names its unit by type and id, so the
// unit can be a player, a mercenary or any other pet; 0x22 always names a player and is dropped when its last
// byte is set. `ops` resolves a unit and applies the skill; the return is 1 applied, 0 ignored, -1 malformed.
#pragma once
#include <cstdint>

namespace d2::realm {

template <class Ops>
int applySkillUpdatePacket(const uint8_t* p, int length, Ops& ops) {
    if (length != 12) return -1;
    const uint8_t type = p[0] == 0x21 ? p[1] : 0;
    if (p[0] == 0x22 && p[11]) return 0;
    const uint32_t guid = uint32_t(p[3]) | (uint32_t(p[4]) << 8) | (uint32_t(p[5]) << 16) | (uint32_t(p[6]) << 24);
    const int id = p[7] | (int(p[8]) << 8);
    auto* unit = ops.resolveUnit(type, guid);
    if (!unit) return 0;
    if (p[0] == 0x21) {
        if (!ops.knownSkill(id)) return 0;
        ops.assign(unit, id, p[9], p[2]);
    } else {
        if (!ops.setQuantity(unit, id, p[9])) return 0;
    }
    return 1;
}

}  // namespace d2::realm
