// 1.13c D2Common+0x52340 (#10787) -- stdcall(unit, skill), ret 8: why a skill cannot be used, a reason code 0..8 (0 = usable).
// Written from the disassembly. Offsets (1.13c, 32-bit): skill +0 skills.txt record, +0x28 base level, +0x34 charge owner
// (-1 = learned, the bonus applies); record byte +4 flags (0x10, 0x20), byte +5 flags (4). Retail reads the bits through Fog's
// mask table (entries 2, 4, 5 = 4, 0x10, 0x20). A null skill is retail-fatal; we return 3.
#include <cstdint>
#include <cstring>
#include "d2common_52340.h"

namespace {
inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 4); return v; }
inline uint8_t rd8(uint32_t a) { return *reinterpret_cast<const uint8_t*>(uintptr_t(a)); }
}

// 1.13c D2Common+0x52340
extern "C" uint32_t D2Common_52340_SkillUseCheck(uint32_t unit, uint32_t skill, const D2Common52340Env* e)
{
    if (!skill) return 3;
    const uint32_t txt = rd32(skill);
    if (!txt || !(rd8(txt + 5) & 4)) return 3;
    if (!unit) return 7;
    int32_t lvl = (int32_t)rd32(skill + 0x28);
    if ((int32_t)rd32(skill + 0x34) == -1) lvl += e->bonus(skill, unit);
    if (lvl < 0) lvl = 0;
    const int32_t cap = (int32_t)rd32(e->maxLevel);
    if (lvl > cap) lvl = cap;
    if (lvl == 0) return 7;
    const uint8_t f = rd8(txt + 4);
    if (f & 0x20) return 6;
    if (f & 0x10) return 5;
    if (!e->c50930(skill, unit)) return 2;
    if (!e->c50f00(skill, unit)) return 2;
    if (!e->c50af0(skill, unit)) return 1;
    if (!e->c4f590(skill, unit)) return 4;
    if (!e->c4fdc0(unit, skill)) return 1;
    if (!e->c50870(skill, unit)) return 2;
    return e->c52210(unit, skill) ? 0u : 8u;
}
