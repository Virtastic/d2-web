// 1.13c D2Client+0x406F0: ECX = unit, EDX = skill id, stack = level (ret 4). The first parameter of the unit's used skill becomes 0
// when the unit has no record, else the skill calc at record +0x138; 0 for a bad id or without a used skill.
#include "d2c_seam.h"
extern "C" uint32_t d2c_406f0(const D2cSkillTbl* t, const D2cSeam* s, const uint8_t* unit, int32_t id, uint32_t level) {
    const uint8_t* rec = d2cSkillRec(*t, id);
    if (!rec) return 0;
    uint64_t res = d2cUnitRes(*s, unit);
    uint64_t used = (*s)(0xc310, 0, 0, 0, 0, 0, (uintptr_t)unit);
    if (!(uint32_t)used) return 0;
    if (!(uint32_t)res) { (*s)(0xcb50, 0, 0, 0, 0, 0, used, 0); return 1; }
    uint32_t calc; std::memcpy(&calc, rec + 0x138, 4);
    uint64_t v = (*s)(0xc316, 0, 0, 0, 0, 0, (uintptr_t)unit, calc, (uint64_t)(int64_t)id, level);
    (*s)(0xcb50, 0, 0, 0, 0, 0, used, v);
    return 1;
}
