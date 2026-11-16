// 1.13c D2Common+0x50AF0 (caller 0x52340, the skill-use check): EAX = skill, ECX = unit, plain ret. Can the unit pay the skill's
// cost. A charged skill (+0x34 != -1) answers its charges left (+0x38 > 0). Otherwise the level is the skill's base level (+0x28)
// plus 0x4FCB0's bonus, floored at 0 and capped by the max-level global (0 without a unit); cost =
// (txt+0x18C * max(level - 1, 0) + txt+0x18A) << (txt+0x188 & 31). A player, monster or missile pays it from stat 6 when the
// stat table has stat 0x72 and 0x37DB0 accepts it; a skill whose txt+0x2E is 0x74 on a unit with bit 3 of +0xC8 is free;
// everything else pays from stat 8. 32-bit retail layout through raw reads; a null skill or record is retail-fatal.
#include "d2common_50af0.h"
#include <cstring>

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 4); return v; }
int32_t rd16s(uint32_t a) { int16_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 2); return v; }
uint32_t rd8(uint32_t a) { return *reinterpret_cast<const uint8_t*>(uintptr_t(a)); }
}

// 1.13c D2Common+0x50AF0
extern "C" uint32_t D2Common_50AF0_CanPayCost(uint32_t pSkill, uint32_t pUnit, const D2Common50AF0Env* e)
{
    if (!pSkill) __builtin_trap();   // retail: fatal assertion (line 0x638)
    if (static_cast<int32_t>(rd32(pSkill + 0x34)) != -1) return static_cast<int32_t>(rd32(pSkill + 0x38)) > 0;
    const uint32_t txt = rd32(pSkill);
    int32_t lvl = 0;
    if (pUnit) {
        const int32_t n = static_cast<int32_t>(rd32(pSkill + 0x28)) + e->bonus(pSkill, pUnit);
        const int32_t f = n < 1 ? 0 : n;
        const int32_t cap = static_cast<int32_t>(rd32(e->maxLevelPtr));
        lvl = f < cap ? f : cap;
    }
    if (!txt) __builtin_trap();   // retail: fatal assertion (line 0x6ef)
    int32_t m = lvl - 1;
    if (m < 0) m = 0;
    const int32_t cost = static_cast<int32_t>(static_cast<uint32_t>(rd16s(txt + 0x18C) * m + rd16s(txt + 0x18A)) << (rd8(txt + 0x188) & 0x1F));
    if (pUnit) {
        const int32_t type = static_cast<int32_t>(rd32(pUnit));
        if (type >= 0 && (type < 2 || type == 3) && static_cast<int32_t>(rd32(e->dataTables + 0xC4)) > 0x72 && e->statValid(pUnit, 0x72))
            return cost <= e->getStat(pUnit, 6, 0);
    }
    if (rd16s(txt + 0x2E) == 0x74 && pUnit && (rd32(pUnit + 0xC8) >> 3 & 1)) return 1;
    return cost <= e->getStat(pUnit, 8, 0);
}
