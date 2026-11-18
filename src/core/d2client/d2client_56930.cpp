// 1.13c D2Client+0x56930: ECX = unit, EDX = skill id, stack = level (ret 4). Stores the skill calc at record +0x114 as the first
// parameter of the unit's used skill; 0 for a bad id or without a used skill.
#include "d2c_seam.h"
extern "C" uint32_t d2c_56930(const D2cSkillTbl* t, const D2cSeam* s, const uint8_t* unit, int32_t id, uint32_t level) {
    const uint8_t* rec = d2cSkillRec(*t, id);
    if (!rec) return 0;
    uint64_t used = (*s)(0xc310, 0, 0, 0, 0, 0, (uintptr_t)unit);
    if (!(uint32_t)used) return 0;
    uint32_t calc; std::memcpy(&calc, rec + 0x114, 4);
    uint64_t v = (*s)(0xc316, 0, 0, 0, 0, 0, (uintptr_t)unit, calc, (uint64_t)(int64_t)id, level);
    (*s)(0xcb50, 0, 0, 0, 0, 0, used, v);
    return 1;
}
