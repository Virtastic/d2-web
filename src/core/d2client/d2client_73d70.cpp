// 1.13c D2Client+0x73D70: ECX = unit, EDX = skill id, stack = arg (ret 4). Resolves the skill's missile through 0x40550, marks the unit
// (flag 0x40 at +0xC4) and launches through 0xA15C0; the truth of that launch, 0 for a bad skill or missile id.
#include "d2c_seam.h"
extern "C" uint32_t d2c_73d70(const D2cSkillTbl* t, int32_t nMissiles, const D2cSeam* s, uint8_t* unit, int32_t id, uint32_t arg) {
    if (!d2cSkillRec(*t, id)) return 0;
    int32_t m = (int32_t)(*s)(0x40550, (uint64_t)(int64_t)id, 0, 0, 0, 0, (uintptr_t)unit);
    if (m < 0 || m >= nMissiles) return 0;
    if (unit) { uint32_t f; std::memcpy(&f, unit + 0xc4, 4); f |= 0x40u; std::memcpy(unit + 0xc4, &f, 4); }
    return (uint32_t)(*s)(0xa15c0, (uintptr_t)unit, 0, 0, 0, 0, (uint64_t)(int64_t)m, (uint64_t)(int64_t)id, arg) != 0;
}
