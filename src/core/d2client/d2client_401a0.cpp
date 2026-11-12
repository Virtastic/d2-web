// 1.13c D2Client+0x401A0: ECX = unit, EDX = skill id, one ignored stack word (ret 4). True when the skill's state (record +0x80) is a
// valid state index and the unit does not have it.
#include "d2c_seam.h"
extern "C" uint32_t d2c_401a0(const D2cSkillTbl* t, const D2cSeam* s, const uint8_t* unit, int32_t id) {
    const uint8_t* rec = d2cSkillRec(*t, id);
    if (!rec) return 0;
    int32_t st = d2cS16(rec, 0x80);
    if (st < 0 || st >= t->nStates) return 0;
    return (uint32_t)(*s)(0xc1fc, 0, 0, 0, 0, 0, (uintptr_t)unit, (uint64_t)(int64_t)st) == 0;
}
